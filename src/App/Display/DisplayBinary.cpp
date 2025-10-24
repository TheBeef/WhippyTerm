/*******************************************************************************
 * FILENAME: DisplayBinary.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the display for binary in it.  This handles the back buffer,
 *    scroll bars, word wrap, etc in it.
 *
 * COPYRIGHT:
 *    Copyright 2023 Paul Hutchinson.
 *
 *    This program is free software: you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation, either version 3 of the License, or (at your
 *    option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program. If not, see https://www.gnu.org/licenses/.
 *
 * CREATED BY:
 *    Paul Hutchinson (06 Aug 2023)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Settings.h"
#include "DisplayBinary.h"
#include "UI/UIDebug.h"
#include <stdint.h>
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/
//#define DEBUG_SHOW_BUFFER_POS               1       // Show the pointers to parts of the screen

#define HEX_BYTES_PER_LINE                      16
#define HEX_MIN_LINES                           24
#define HEX_SPACE_BETWEEN_HEX_AND_ASCII         3

#define END_OF_HEX_PX           (HEX_BYTES_PER_LINE*3*CharWidthPx)
#define START_OF_ASCII_PX       (END_OF_HEX_PX+HEX_SPACE_BETWEEN_HEX_AND_ASCII*CharWidthPx)
#define END_OF_ASCII_PX         (START_OF_ASCII_PX+HEX_BYTES_PER_LINE*CharWidthPx)

#define END_OF_HEX_CHAR         (HEX_BYTES_PER_LINE*3)
#define START_OF_ASCII_CHAR     (END_OF_HEX_CHAR+HEX_SPACE_BETWEEN_HEX_AND_ASCII)
#define END_OF_ASCII_CHAR       (START_OF_ASCII_CHAR+HEX_BYTES_PER_LINE)

#define SELECTION_SCROLL_SPEED_TIMER            50 // ms

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct DisBin_PointPair
{
    uint8_t *Line;
    int Offset;
};

struct DisBin_Block
{
    struct DisBin_PointPair Start;
    struct DisBin_PointPair End;
};

struct BinaryPointMarker
{
    bool Valid;
    uint8_t *Line;
    int Offset;
    struct BinaryPointMarker *Prev;
    struct BinaryPointMarker *Next;
};

/*** FUNCTION PROTOTYPES      ***/
bool DisplayBin_EventHandlerCB(const struct TextDisplayEvent *Event);
void DisplayBin_ScrollTimer_Timeout(uintptr_t UserData);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    DisplayBinary_EventHandlerCB
 *
 * SYNOPSIS:
 *    static bool DisplayBinary_EventHandlerCB(
 *              const struct TextDisplayEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the TextDisplay widget.
 *
 * FUNCTION:
 *    This function takes an event from the TextDisplay widget and figures
 *    out the DisplayBin class it belongs with and then calls the class
 *    handler method.
 *
 * RETURNS:
 *    true -- Let the event continue
 *    false -- Cancel the event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBinary_EventHandlerCB(const struct TextDisplayEvent *Event)
{
    class DisplayBinary *DBin=(class DisplayBinary *)Event->ID;

    return DBin->DoTextDisplayCtrlEvent(Event);
}

/*******************************************************************************
 * NAME:
 *    DisplayBin_ScrollTimer_Timeout
 *
 * SYNOPSIS:
 *    void DisplayBin_ScrollTimer_Timeout(uintptr_t UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- A pointer to our display bin class.
 *
 * FUNCTION:
 *    This is a callback from the scroll timer.  It just calls the class
 *    DoScrollTimerTimeout() function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBin_ScrollTimer_Timeout(uintptr_t UserData)
{
    class DisplayBinary *DB=(class DisplayBinary *)UserData;

    DB->DoScrollTimerTimeout();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::DisplayBinary
 *
 * SYNOPSIS:
 *    DisplayBinary::DisplayBinary();
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This is the constructor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
DisplayBinary::DisplayBinary()
{
    TextDisplayCtrl=NULL;
    MarkerList=NULL;
    MarkerGetStrBuffer=NULL;
    MarkerGetStrBufferSize=0;

    HexBufferSize=0;
    HexBuffer=NULL;
    EndOfHexBuffer=NULL;
    BottomOfBufferLine=NULL;
    TopOfBufferLine=NULL;
    TopLine=NULL;

    ColorBuffer=NULL;
    ColorBottomOfBufferLine=NULL;
    ColorTopOfBufferLine=NULL;
    ColorTopLine=NULL;

    ScreenWidthPx=0;
    ScreenHeightPx=0;
    DisplayLines=0;
    CharWidthPx=1;
    CharHeightPx=1;
    WindowXOffsetPx=0;

    SelectionActive=false;
    SelectionInAscII=false;
    SelectionLine=NULL;
    SelectionAnchorLine=NULL;

    CursorStyle=e_TextCursorStyle_Block;

    ScrollTimer=NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::Init
 *
 * SYNOPSIS:
 *    bool DisplayBinary::Init(void *ParentWidget,class ConSettings *SettingsPtr,
 *          bool (*EventCallback)(const struct DBEvent *Event),
 *          uintptr_t UserData);
 *
 * PARAMETERS:
 *    ParentWidget [I] -- The parent UI widget that we will add our widgets to
 *    SettingsPtr [I] -- The setting to use for this display.  NULL for the
 *                       global settings.
 *    EventCallback [I] -- See DisplayBase::InitBase()
 *    UserData [I] -- The user data to send to the 'EventCallback' callback.
 *
 * FUNCTION:
 *    This function init's the binary display.
 *
 * RETURNS:
 *    true -- Things worked
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBinary::Init(void *ParentWidget,class ConSettings *SettingsPtr,
        bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData)
{
    TextDisplayCtrl=NULL;
    try
    {
        if(!InitBase(EventCallback,UserData))
            throw(0);

        Settings=SettingsPtr;
        if(Settings==NULL)
            Settings=&g_Settings.DefaultConSettings;

        /* Allocate the text canvas */
        TextDisplayCtrl=UITC_AllocTextDisplay(ParentWidget,
                DisplayBinary_EventHandlerCB,(uintptr_t)this);
        if(TextDisplayCtrl==NULL)
            throw(0);

        SetupHexInput(UITC_GetSendHexDisplayContainerFrameCtrlHandle(TextDisplayCtrl));

        /* We show hex input because we are binary */
        Block_SetHexOrTextMode(false);

        HexBufferSize=Settings->ScrollBufferLines*HEX_BYTES_PER_LINE;
        if(HexBufferSize<HEX_MIN_LINES*HEX_BYTES_PER_LINE)
            HexBufferSize=HEX_MIN_LINES*HEX_BYTES_PER_LINE;

        /* Hex buffer can't handle part lines, all lines must be full */
        if(HexBufferSize%HEX_BYTES_PER_LINE!=0)
            HexBufferSize+=HEX_BYTES_PER_LINE-HexBufferSize%HEX_BYTES_PER_LINE;

        HexBuffer=(uint8_t *)malloc(HexBufferSize);
        if(HexBuffer==NULL)
            throw(0);

        EndOfHexBuffer=HexBuffer+HexBufferSize;
        BottomOfBufferLine=HexBuffer;
        InsertPoint=0;
        TopOfBufferLine=HexBuffer;
        TopLine=TopOfBufferLine;

        ColorBuffer=(struct CharStyling *)malloc(HexBufferSize*
                sizeof(struct CharStyling));
        if(ColorBuffer==NULL)
            throw(0);

        ColorBottomOfBufferLine=ColorBuffer;
        ColorTopOfBufferLine=ColorBuffer;
        ColorTopLine=ColorTopOfBufferLine;

        /* Current Style */
        CurrentStyle.FGColor=Settings->DefaultColors[e_DefaultColors_FG];
        CurrentStyle.BGColor=Settings->DefaultColors[e_DefaultColors_BG];
        CurrentStyle.Attribs=0;
        CurrentStyle.ULineColor=CurrentStyle.FGColor;

        FontName=Settings->FontName;
        FontSize=Settings->FontSize;
        FontBold=Settings->FontBold;
        FontItalic=Settings->FontItalic;

        ScreenWidthPx=0;
        ScreenHeightPx=0;
        DisplayLines=0;

        SetupCanvas();

        ScrollTimer=AllocUITimer();
        if(ScrollTimer==NULL)
            throw(0);

        SetupUITimer(ScrollTimer,DisplayBin_ScrollTimer_Timeout,(uintptr_t)this,
                true);

        UITimerSetTimeout(ScrollTimer,SELECTION_SCROLL_SPEED_TIMER);

        ApplySettings();

        InitCalled=true;
    }
    catch(...)
    {
        if(HexBuffer!=NULL)
            free(HexBuffer);
        if(ColorBuffer!=NULL)
            free(ColorBuffer);
        if(TextDisplayCtrl!=NULL)
        {
            UITC_FreeTextDisplay(TextDisplayCtrl);
            TextDisplayCtrl=NULL;
        }
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::~DisplayBinary
 *
 * SYNOPSIS:
 *    DisplayBinary::~DisplayBinary();
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This is the destructor
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
DisplayBinary::~DisplayBinary()
{
    struct BinaryPointMarker *Marker;

    InitCalled=false;

    FreeHexInput();

    /* Free the marker list */
    while(MarkerList!=NULL)
    {
        Marker=MarkerList->Next;
        delete MarkerList;
        MarkerList=Marker;
    }

    if(MarkerGetStrBuffer!=NULL)
        free(MarkerGetStrBuffer);

    if(ScrollTimer!=NULL)
        FreeUITimer(ScrollTimer);

    if(HexBuffer!=NULL)
        free(HexBuffer);
    if(ColorBuffer!=NULL)
        free(ColorBuffer);

    if(TextDisplayCtrl!=NULL)
        UITC_FreeTextDisplay(TextDisplayCtrl);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::Reparent
 *
 * SYNOPSIS:
 *    void DisplayBinary::Reparent(void *NewParentWidget);
 *
 * PARAMETERS:
 *    NewParentWidget [I] -- The new parent
 *
 * FUNCTION:
 *    This function changes the widget that is the parent of the UI elements
 *    for this display text.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::Reparent(void *NewParentWidget)
{
    UITC_Reparent(TextDisplayCtrl,NewParentWidget);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::WriteChar
 *
 * SYNOPSIS:
 *    void DisplayBinary::WriteChar(uint8_t *Chr);
 *
 * PARAMETERS:
 *    Chr [I] -- The char to add.  This is a UTF8 char, and is 0 term'ed.
 *
 * FUNCTION:
 *    This function adds a char to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::WriteChar(uint8_t *Chr)
{
    bool WasAtBottom;
    t_UIScrollBarCtrl *VertScroll;
    bool NeedRedraw;

    BottomOfBufferLine[InsertPoint]=*Chr;
    ColorBottomOfBufferLine[InsertPoint]=CurrentStyle;
    InsertPoint++;

    RedrawCurrentLine();

    if(InsertPoint>=HEX_BYTES_PER_LINE)
    {
        NeedRedraw=false;

        InsertPoint=0;

        /* If we are scrolled all the way at the bottom then keep it that way */
        WasAtBottom=ScrollBarAtBottom();

        /* Shift the buffer */
        BottomOfBufferLine+=HEX_BYTES_PER_LINE;
        ColorBottomOfBufferLine+=HEX_BYTES_PER_LINE;
        if(BottomOfBufferLine>=EndOfHexBuffer)
        {
            BottomOfBufferLine=HexBuffer;
            ColorBottomOfBufferLine=ColorBuffer;
        }

        /* See if we need to move the start of buffer */
        if(BottomOfBufferLine==TopOfBufferLine)
        {
            /* Move selection down */
            if(SelectionLine==SelectionAnchorLine)
            {
                SelectionActive=false;
                SelectionLine=NULL;
                SelectionAnchorLine=NULL;
            }

            if(SelectionLine==TopOfBufferLine)
                SelectionLine+=HEX_BYTES_PER_LINE;
            if(SelectionAnchorLine==TopOfBufferLine)
                SelectionAnchorLine+=HEX_BYTES_PER_LINE;

            /* Handle marks */
            InvalidateMarksOnScroll();

            /* Handle topline */
            if(TopLine==TopOfBufferLine || WasAtBottom)
            {
                /* Ok, we ran out of data, move topline too */
                TopLine+=HEX_BYTES_PER_LINE;
                ColorTopLine+=HEX_BYTES_PER_LINE;
                if(TopLine>=EndOfHexBuffer)
                {
                    TopLine=HexBuffer;
                    ColorTopLine=ColorBuffer;
                }
                NeedRedraw=true;
            }

            /* Move the start of the buffer */
            TopOfBufferLine+=HEX_BYTES_PER_LINE;
            ColorTopOfBufferLine+=HEX_BYTES_PER_LINE;
            if(TopOfBufferLine>=EndOfHexBuffer)
            {
                TopOfBufferLine=HexBuffer;
                ColorTopOfBufferLine=ColorBuffer;
            }
        }

        RethinkYScrollBar();

        if(WasAtBottom)
        {
            int TotalLines;

            VertScroll=UITC_GetVertSlider(TextDisplayCtrl);
            TotalLines=UIGetScrollBarTotalSize(VertScroll);

            if(TotalLines>=DisplayLines)
            {
                UISetScrollBarPos(VertScroll,TotalLines-DisplayLines);
            }
        }

        if(NeedRedraw)
            RedrawScreen();
    }

    RethinkCursor();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::DoTextDisplayCtrlEvent
 *
 * SYNOPSIS:
 *    bool DisplayBinary::DoTextDisplayCtrlEvent(
 *              const struct TextDisplayEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the text display control.
 *
 * FUNCTION:
 *    This function is called when an event from our text display control
 *    sends an event.
 *
 * RETURNS:
 *    true -- Let the event continue
 *    false -- Cancel the event
 *
 * NOTES:
 *    Called from DisplayBinary_EventHandlerCB()
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBinary::DoTextDisplayCtrlEvent(const struct TextDisplayEvent *Event)
{
    union DBEventData Info;
    int Offset;
    int TotalLines;
    int Delta;

    if(!InitCalled)
        return false;

    switch(Event->EventType)
    {
        case e_TextDisplayEvent_DisplayFrameScrollX:
            if(TextDisplayCtrl==NULL)
                break;
            WindowXOffsetPx=Event->Info.Scroll.Amount*CharWidthPx;
            UITC_SetXOffset(TextDisplayCtrl,WindowXOffsetPx);
            UITC_RedrawScreen(TextDisplayCtrl);
        break;
        case e_TextDisplayEvent_DisplayFrameScrollY:
            TopLine=TopOfBufferLine+Event->Info.Scroll.Amount*HEX_BYTES_PER_LINE;
            ColorTopLine=ColorTopOfBufferLine+
                    (Event->Info.Scroll.Amount*HEX_BYTES_PER_LINE);
            if(TopLine>EndOfHexBuffer)
            {
                Offset=TopLine-EndOfHexBuffer;
                TopLine=HexBuffer+Offset;
                ColorTopLine=ColorBuffer+Offset;
            }
            RedrawScreen();
            RethinkCursor();
        break;
        case e_TextDisplayEvent_MouseTripleClick:
        case e_TextDisplayEvent_MouseDoubleClick:
        case e_TextDisplayEvent_MouseDown:
            Info.Mouse.x=Event->Info.Mouse.x;
            Info.Mouse.y=Event->Info.Mouse.y;
            SendEvent(e_DBEvent_LeftMousePress,&Info);

            HandleLeftMousePress(true,Event->Info.Mouse.x,
                    Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_MouseUp:
            HandleLeftMousePress(false,Event->Info.Mouse.x,
                    Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_MouseRightDown:
        break;
        case e_TextDisplayEvent_MouseRightUp:
        break;
        case e_TextDisplayEvent_MouseMiddleDown:
        break;
        case e_TextDisplayEvent_MouseMiddleUp:
        break;
        case e_TextDisplayEvent_MouseWheel:
            if(TextDisplayCtrl==NULL)
                return false;

            if(Event->Info.MouseWheel.Mods)
            {
                Info.MouseWheel.Mods=Event->Info.MouseWheel.Mods;
                Info.MouseWheel.Steps=Event->Info.MouseWheel.Steps;
                SendEvent(e_DBEvent_MouseMouseWheel,&Info);
            }
            else
            {
                Delta=-Event->Info.MouseWheel.Steps;

                if(Delta!=0)
                {
                    /* Add to the scroll bar */
                    t_UIScrollBarCtrl *VertScroll;
                    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);
                    Offset=UIGetScrollBarPos(VertScroll);
                    TotalLines=UIGetScrollBarTotalSize(VertScroll);
                    Offset+=Delta;
                    if(Offset<0)
                        Offset=0;
                    if(Offset>=TotalLines)
                        Offset=TotalLines;
                    UISetScrollBarPos(VertScroll,Offset);
                    RethinkCursor();
                }
            }
        break;
        case e_TextDisplayEvent_MouseMove:
            HandleMouseMove(Event->Info.Mouse.x,Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_Resize:
            ScreenResize();
            RethinkWindowSize();
            RethinkYScrollBar();
            RedrawScreen();
            RethinkCursor();
        break;
        case e_TextDisplayEvent_LostFocus:
            HasFocus=false;
            Info.Focus.HasFocus=HasFocus;
            SendEvent(e_DBEvent_FocusChange,&Info);
        break;
        case e_TextDisplayEvent_GotFocus:
            HasFocus=true;
            Info.Focus.HasFocus=HasFocus;
            SendEvent(e_DBEvent_FocusChange,&Info);
        break;
        case e_TextDisplayEvent_KeyEvent:
            Info.Key.Mods=Event->Info.Key.Mods;
            Info.Key.Key=Event->Info.Key.Key;
            Info.Key.TextPtr=Event->Info.Key.TextPtr;
            Info.Key.TextLen=Event->Info.Key.TextLen;
            SendEvent(e_DBEvent_KeyEvent,&Info);
        break;
        case e_TextDisplayEvent_ContextMenu:
            Info.Context.Menu=(e_UITD_ContextMenuType)Event->Info.Context.Menu;
            SendEvent(e_DBEvent_ContextMenu,&Info);
        break;
        case e_TextDisplayEvent_ButtonPress:
            switch(Event->Info.ButtonPress.Bttn)
            {
                case e_UITC_Bttn_Send:
                    DoBlock_Send();
                break;
                case e_UITC_Bttn_HexEdit:
                    DoBlock_EditHex();
                break;
                case e_UITC_Bttn_Clear:
                    DoBlock_ClearHexInput();
                break;
                case e_UITC_BttnMAX:
                default:
                break;
            }
        break;
        case e_TextDisplayEvent_RadioButtonPress:
            switch(Event->Info.RadioButton.BttnID)
            {
                case e_UITC_RadioButton_Text:
                case e_UITC_RadioButton_Hex:
                    DoBlock_RadioBttnChange();
                break;
                case e_UITC_RadioButtonMAX:
                default:
                break;
            }
        break;
        case e_TextDisplayEvent_ComboxChange:
        case e_TextDisplayEventMAX:
        default:
            return true;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ScreenResize
 *
 * SYNOPSIS:
 *    void DisplayBinary::ScreenResize(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This is called when the screen is resized.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::ScreenResize(void)
{
    t_UIScrollBarCtrl *HozScroll;
    int ScreenChars;
    int TotalChars;

    if(TextDisplayCtrl==NULL)
        return;

    HozScroll=UITC_GetHorzSlider(TextDisplayCtrl);

    ScreenWidthPx=UITC_GetWidgetWidth(TextDisplayCtrl);
    ScreenHeightPx=UITC_GetWidgetHeight(TextDisplayCtrl);

    if(Settings->TermSizeFixedHeight)
        DisplayLines=Settings->TermSizeHeight;
    else
        DisplayLines=ScreenHeightPx/CharHeightPx;

    /* Horizontal */
    ScreenChars=ScreenWidthPx/CharWidthPx;
    TotalChars=END_OF_ASCII_CHAR;
    UISetScrollBarPageSizeAndMax(HozScroll,ScreenChars,TotalChars);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::RethinkYScrollBar
 *
 * SYNOPSIS:
 *    void DisplayBinary::RethinkYScrollBar(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the size of the scroll bars.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::RethinkYScrollBar(void)
{
    int TotalLines;
    int Bytes;
    int TopLineY;
    t_UIScrollBarCtrl *VertScroll;

    if(TextDisplayCtrl==NULL)
        return;

    /* Find the total number of lines */
    if(TopOfBufferLine>BottomOfBufferLine)
        Bytes=(EndOfHexBuffer-TopOfBufferLine)+(BottomOfBufferLine-HexBuffer);
    else
        Bytes=BottomOfBufferLine-TopOfBufferLine;
    TotalLines=Bytes/HEX_BYTES_PER_LINE;
    TotalLines++;

    /* Figure out the current topline in lines */
    if(TopLine>=TopOfBufferLine)
        Bytes=TopLine-TopOfBufferLine;
    else
        Bytes=(EndOfHexBuffer-TopOfBufferLine)+(TopLine-HexBuffer);
    TopLineY=(Bytes+HEX_BYTES_PER_LINE-1)/HEX_BYTES_PER_LINE;

    /* Vert */
    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);
    UISetScrollBarPageSizeAndMax(VertScroll,DisplayLines,TotalLines);
    UISetScrollBarPos(VertScroll,TopLineY);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::RethinkWindowSize
 *
 * SYNOPSIS:
 *    void DisplayBinary::RethinkWindowSize(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the size of the window area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::RethinkWindowSize(void)
{
    int LeftEdge;
    int TopEdge;
    int Width;
    int Height;

    /* Set the clipping and offset from the edge of the widget */
    if(TextDisplayCtrl==NULL)
        return;

    LeftEdge=0;
    TopEdge=0;
    Width=ScreenWidthPx;
    Height=ScreenHeightPx;

    if(Settings->TermSizeFixedWidth)
    {
        Width=Settings->TermSizeWidth*CharWidthPx;
        LeftEdge=ScreenWidthPx/2-Width/2;
        if(LeftEdge<0)
            LeftEdge=0;

        if(!Settings->CenterTextInWindow)
            LeftEdge=0;
    }

    if(Settings->TermSizeFixedHeight)
    {
        Height=Settings->TermSizeHeight*CharHeightPx;
        TopEdge=ScreenHeightPx/2-Height/2;
        if(TopEdge<0)
            TopEdge=0;

        if(!Settings->CenterTextInWindow)
            TopEdge=0;
    }

    UITC_SetClippingWindow(TextDisplayCtrl,LeftEdge,TopEdge,Width,Height);
    UITC_SetMaxLines(TextDisplayCtrl,DisplayLines,
            Settings->DefaultColors[e_DefaultColors_BG]);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::SetupCanvas
 *
 * SYNOPSIS:
 *    void DisplayBinary::SetupCanvas(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sets up the display canvas with the correct font and
 *    other settings.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DisplayBuffer::ChangeCanvasID()
 ******************************************************************************/
void DisplayBinary::SetupCanvas(void)
{
    UITC_SetFont(TextDisplayCtrl,FontName.c_str(),FontSize,FontBold,FontItalic);

    UITC_SetCursorColor(TextDisplayCtrl,Settings->CursorColor);

    UITC_SetTextAreaBackgroundColor(TextDisplayCtrl,Settings->
            DefaultColors[e_DefaultColors_BG]);
    UITC_SetTextDefaultColor(TextDisplayCtrl,Settings->
            DefaultColors[e_DefaultColors_FG]);

    CharWidthPx=UITC_GetCharPxWidth(TextDisplayCtrl);
    CharHeightPx=UITC_GetCharPxHeight(TextDisplayCtrl);
    if(CharWidthPx<1)
        CharWidthPx=1;
    if(CharHeightPx<1)
        CharHeightPx=1;

    if(Settings->TermSizeFixedHeight)
        DisplayLines=Settings->TermSizeHeight;
    else
        DisplayLines=ScreenHeightPx/CharHeightPx;

    ScreenResize();
    RethinkWindowSize();
    RethinkYScrollBar();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::RedrawCurrentLine
 *
 * SYNOPSIS:
 *    void DisplayBinary::RedrawCurrentLine(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function redraw the current line.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::RedrawCurrentLine(void)
{
    int y;
    int Bytes;

    if(TopLine>BottomOfBufferLine)
        Bytes=(EndOfHexBuffer-TopLine)+(BottomOfBufferLine-HexBuffer);
    else
        Bytes=BottomOfBufferLine-TopLine;
    y=Bytes/HEX_BYTES_PER_LINE;

    /* Don't do anything if the insert line is not on the screen */
    if(y>=DisplayLines)
        return;

    DrawLine(BottomOfBufferLine,ColorBottomOfBufferLine,y,InsertPoint);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::RedrawScreen
 *
 * SYNOPSIS:
 *    void DisplayBinary::RedrawScreen(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function redraws the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::RedrawScreen(void)
{
    const uint8_t *StartOfLine;
    const struct CharStyling *ColorStartOfLine;
    int y;
    int Bytes2Draw;

    StartOfLine=TopLine;
    ColorStartOfLine=ColorTopLine;
    for(y=0;y<DisplayLines;y++)
    {
        if(StartOfLine>=EndOfHexBuffer)
        {
            StartOfLine=HexBuffer;
            ColorStartOfLine=ColorBuffer;
        }
        Bytes2Draw=HEX_BYTES_PER_LINE;
        if(StartOfLine==BottomOfBufferLine)
            Bytes2Draw=InsertPoint;

        DrawLine(StartOfLine,ColorStartOfLine,y,Bytes2Draw);
        if(Bytes2Draw!=HEX_BYTES_PER_LINE)
            break;

        StartOfLine+=HEX_BYTES_PER_LINE;
        ColorStartOfLine+=HEX_BYTES_PER_LINE;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::DrawLine
 *
 * SYNOPSIS:
 *    void DisplayBinary::DrawLine(const uint8_t *Line,
 *          const struct CharStyling *ColorLine,int ScreenLine,
 *          unsigned int Bytes);
 *
 * PARAMETERS:
 *    Line [I] -- The start of the line to draw
 *    ColorLine [I] -- The color info for this line
 *    ScreenLine [I] -- Where on the screen are we going to put this line.
 *                      (0 = top of the screen)
 *    Bytes [I] -- The number of bytes to draw from this line.
 *
 * FUNCTION:
 *    This function draws a line.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::DrawLine(const uint8_t *Line,
        const struct CharStyling *ColorLine,int ScreenLine,unsigned int Bytes)
{
    struct TextCanvasFrag DisplayFrag;
    char LineBuff[MAX_BINARY_HEX_BYTES_PER_LINE*3+HEX_SPACE_BETWEEN_HEX_AND_ASCII+MAX_BINARY_HEX_BYTES_PER_LINE+1];
    struct CharStyling ColorBuff[MAX_BINARY_HEX_BYTES_PER_LINE*3+HEX_SPACE_BETWEEN_HEX_AND_ASCII+MAX_BINARY_HEX_BYTES_PER_LINE+1];
    uint8_t c;
    unsigned int x;
    unsigned int r;
    unsigned int s;
    unsigned int tmp;
    unsigned int Offset;
    struct CharStyling SpaceStyle;
    int StartOfStr;
    unsigned int HighLightStart;
    unsigned int HighLightEnd;
    unsigned int HighLightMaxSize;
    unsigned int HighLightLineSize;
    struct DisBin_Block SelBlocks[2];
    struct CharStyling NextColor;
    struct CharStyling ApplyColor;
    uint16_t NextAttrib;

    /* Make sure the padding is all 0x00's */
    memset(ColorBuff,0x00,sizeof(ColorBuff));
    memset(&SpaceStyle,0x00,sizeof(SpaceStyle));

    SpaceStyle.FGColor=Settings->DefaultColors[e_DefaultColors_FG];
    SpaceStyle.BGColor=Settings->DefaultColors[e_DefaultColors_BG];
    SpaceStyle.Attribs=0;
    SpaceStyle.ULineColor=SpaceStyle.FGColor;

    GetNormalizedSelectionBlocks(SelBlocks);

    for(x=0;x<Bytes;x++)
    {
        c=Line[x];

        sprintf(&LineBuff[x*3],"%02X ",c);
        /* If you change HEX_SPACE_BETWEEN_HEX_AND_ASCII you need to update
           this as well */
        tmp=x*3+0;
        ColorBuff[tmp++]=ColorLine[x];
        ColorBuff[tmp++]=ColorLine[x];

        /* Figure out what to do to the space between bytes */
        ApplyColor=ColorLine[x];

        /* Sneak a peek at the next char's attribs */
        NextColor=ColorLine[x];
        if(x+1<Bytes)
            NextColor=ColorLine[x+1];

        NextAttrib=NextColor.Attribs;
        if(ColorLine[x].Attribs!=NextAttrib)
        {
            /* Ok, the attribs are changing apply a number of rules because it
               looks really odd having underline or strike hang off the end. */
            NextAttrib&=~(TXT_ATTRIB_BOLD|TXT_ATTRIB_ITALIC);  // Ignore Bold and Italic
            if(NextAttrib==0)
                ApplyColor.Attribs=0;
        }
        if(NextColor.BGColor==SpaceStyle.BGColor)
        {
            /* Ok, the next char is using the space bg color, apply the
               BG color to the space */
            ApplyColor.BGColor=SpaceStyle.BGColor;
        }

        ColorBuff[tmp]=ApplyColor;

        tmp++;

        if(c<32 || c>127)
            c='.';

        LineBuff[START_OF_ASCII_CHAR+x]=c;
        ColorBuff[START_OF_ASCII_CHAR+x]=ColorLine[x];
    }
    /* x=Bytes at this point */
    memset(&LineBuff[x*3],' ',START_OF_ASCII_CHAR-(x*3));
    for(r=0;r<START_OF_ASCII_CHAR-(x*3);r++)
        ColorBuff[x*3+r]=SpaceStyle;

    /* Kill of the trailing space after the last byte */
    if(x>0)
        ColorBuff[x*3-1]=SpaceStyle;

    memset(&LineBuff[START_OF_ASCII_CHAR+x],' ',HEX_BYTES_PER_LINE-x);
    for(r=0;r<HEX_BYTES_PER_LINE-x;r++)
        ColorBuff[START_OF_ASCII_CHAR+x+r]=SpaceStyle;

    LineBuff[END_OF_ASCII_CHAR]=0;

    /* Handle selection */
    if(SelectionActive)
    {
        for(s=0;s<2;s++)
        {
            if(Line>=SelBlocks[s].Start.Line && Line<=SelBlocks[s].End.Line)
            {
                if(SelectionInAscII)
                {
                    Offset=START_OF_ASCII_CHAR;
                    HighLightStart=SelBlocks[s].Start.Offset;
                    HighLightEnd=SelBlocks[s].End.Offset+1;
                    HighLightMaxSize=HEX_BYTES_PER_LINE;
                    HighLightLineSize=SelBlocks[s].End.Offset-
                            SelBlocks[s].Start.Offset+1;
                }
                else
                {
                    /* Times 3 because we use 3 chars per hex value, and -1
                       because we don't highlight the last char */
                    Offset=0;
                    HighLightStart=SelBlocks[s].Start.Offset*3;
                    HighLightEnd=(SelBlocks[s].End.Offset+1)*3-1;
                    HighLightMaxSize=HEX_BYTES_PER_LINE*3-1;
                    HighLightLineSize=(SelBlocks[s].End.Offset-
                            SelBlocks[s].Start.Offset+1)*3-1;
                }

                if(Line==SelBlocks[s].Start.Line && Line==SelBlocks[s].End.Line)
                {
                    for(r=0;r<HighLightLineSize;r++)
                    {
                        ColorBuff[Offset+HighLightStart+r].FGColor=
                                Settings->SelectionColors[e_Color_FG];
                        ColorBuff[Offset+HighLightStart+r].BGColor=
                                Settings->SelectionColors[e_Color_BG];
                        ColorBuff[Offset+HighLightStart+r].Attribs=
                                TXT_ATTRIB_FORCE;
                    }
                }
                else if(Line==SelBlocks[s].Start.Line)
                {
                    for(r=HighLightStart;r<HighLightMaxSize;r++)
                    {
                        ColorBuff[Offset+r].FGColor=Settings->
                                SelectionColors[e_Color_FG];
                        ColorBuff[Offset+r].BGColor=Settings->
                                SelectionColors[e_Color_BG];
                        ColorBuff[Offset+r].Attribs=TXT_ATTRIB_FORCE;
                    }
                }
                else if(Line==SelBlocks[s].End.Line)
                {
                    for(r=0;r<HighLightEnd;r++)
                    {
                        ColorBuff[Offset+r].FGColor=Settings->
                                SelectionColors[e_Color_FG];
                        ColorBuff[Offset+r].BGColor=Settings->
                                SelectionColors[e_Color_BG];
                        ColorBuff[Offset+r].Attribs=TXT_ATTRIB_FORCE;
                    }
                }
                else
                {
                    for(r=0;r<HighLightMaxSize;r++)
                    {
                        ColorBuff[Offset+r].FGColor=Settings->
                                SelectionColors[e_Color_FG];
                        ColorBuff[Offset+r].BGColor=Settings->
                                SelectionColors[e_Color_BG];
                        ColorBuff[Offset+r].Attribs=TXT_ATTRIB_FORCE;
                    }
                }
            }
        }
    }

#ifdef DEBUG_SHOW_BUFFER_POS
    if(Line==TopOfBufferLine)
    {
        ColorBuff[HEX_BYTES_PER_LINE*3+1].BGColor=0xFF0000; // Red
        LineBuff[HEX_BYTES_PER_LINE*3+1]='^';
    }
    if(Line==BottomOfBufferLine)
    {
        ColorBuff[HEX_BYTES_PER_LINE*3+1].BGColor=0x00FF00; // Green
        LineBuff[HEX_BYTES_PER_LINE*3+1]='B';
    }
    if(Line==TopLine)
    {
        ColorBuff[HEX_BYTES_PER_LINE*3+0].BGColor=0x0000FF; // Blue
        LineBuff[HEX_BYTES_PER_LINE*3+0]='T';
    }
    if(Line==HexBuffer)
    {
        ColorBuff[HEX_BYTES_PER_LINE*3+1].BGColor=0xFFFF00; // Yellow
        LineBuff[HEX_BYTES_PER_LINE*3+1]='H';
    }
    if(Line==SelectionAnchorLine)
    {
        ColorBuff[HEX_BYTES_PER_LINE*3+2].BGColor=0xFFFFFF; // White
        LineBuff[HEX_BYTES_PER_LINE*3+2]='A';
    }
    if(Line==SelectionLine)
    {
        ColorBuff[HEX_BYTES_PER_LINE*3+2].BGColor=0xFF00FF; // Perp
        LineBuff[HEX_BYTES_PER_LINE*3+2]='S';
    }

    /* Add the address to the end of the buffer */
    sprintf(&LineBuff[END_OF_ASCII_CHAR]," %lX",Line-HexBuffer);
#endif

    DisplayFrag.FragType=e_TextCanvasFrag_String;
    DisplayFrag.Text=LineBuff;
    DisplayFrag.Styling=ColorBuff[0];
    DisplayFrag.Value=0;
    DisplayFrag.Data=0;

    UITC_Begin(TextDisplayCtrl,ScreenLine);
    UITC_ClearLine(TextDisplayCtrl,Settings->DefaultColors[e_DefaultColors_BG]);

    /* Add the different fragments based on styling */
    StartOfStr=0;
    for(r=0;r<END_OF_ASCII_CHAR;r++)
    {
        if(memcmp(&ColorBuff[StartOfStr],&ColorBuff[r],
                sizeof(struct CharStyling))!=0)
        {
            /* Style changed, output the last string and start again */
            c=LineBuff[r];
            LineBuff[r]=0;  // Make it a smaller string
            DisplayFrag.Text=&LineBuff[StartOfStr];
            DisplayFrag.Styling=ColorBuff[StartOfStr];
            UITC_AddFragment(TextDisplayCtrl,&DisplayFrag);

            LineBuff[r]=c;  // Restore the string

            /* Mark the start of the next fragment */
            StartOfStr=r;
        }
    }
    /* Add the last fragment */
    DisplayFrag.Text=&LineBuff[StartOfStr];
    DisplayFrag.Styling=ColorBuff[StartOfStr];
    UITC_AddFragment(TextDisplayCtrl,&DisplayFrag);

    UITC_End(TextDisplayCtrl);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ScrollBarAtBottom
 *
 * SYNOPSIS:
 *    bool DisplayBinary::ScrollBarAtBottom(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function figures out if the vert scroll bar is at the bottom (can't
 *    scroll any farther down).
 *
 * RETURNS:
 *    true -- Scroll bar is at the bottom
 *    false -- User could scroll down more
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBinary::ScrollBarAtBottom(void)
{
    t_UIScrollBarCtrl *VertScroll;
    int CurrentPos;
    int TotalLines;

    if(TextDisplayCtrl==NULL)
        return false;

    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);

    CurrentPos=UIGetScrollBarPos(VertScroll);
    TotalLines=UIGetScrollBarTotalSize(VertScroll);

    if(CurrentPos>=TotalLines-DisplayLines)
        return true;
    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ClearScreen
 *
 * SYNOPSIS:
 *    void DisplayBinary::ClearScreen(e_ScreenClearType Type);
 *
 * PARAMETERS:
 *    Type [I] -- The type of clearing we want to do.  Supported types:
 *                  e_ScreenClear_Clear -- Normal clearing.  Throw away
 *                          anything that's on the screen area.
 *                  e_ScreenClear_Scroll -- Move any non blank lines to the
 *                          scroll back buffer before clearing.
 *                  e_ScreenClear_ScrollAll -- Move all the screen area lines
 *                          to the scroll back buffer.
 *                  e_ScreenClear_ScrollWithHR -- Move any non blank lines to
 *                          the scroll back buffer and then add a marker to
 *                          show that's where the new screen starts.
 *
 * FUNCTION:
 *    This function clears the screen area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::ClearScreen(e_ScreenClearType Type)
{
    TopOfBufferLine=HexBuffer;
    BottomOfBufferLine=HexBuffer;
    TopLine=TopOfBufferLine;

    ColorTopOfBufferLine=ColorBuffer;
    ColorBottomOfBufferLine=ColorBuffer;
    ColorTopLine=ColorTopOfBufferLine;

    InsertPoint=0;

    SelectionActive=false;
    SelectionLine=NULL;
    SelectionAnchorLine=NULL;

    InvalidateAllMarks();

    UITC_ClearAllLines(TextDisplayCtrl);
    RethinkYScrollBar();
    RethinkCursor();
    RedrawScreen();
    UITC_RedrawScreen(TextDisplayCtrl);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::RethinkCursor
 *
 * SYNOPSIS:
 *    void DisplayBinary::RethinkCursor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the cursor to the insert point.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::RethinkCursor(void)
{
    unsigned int x;
    unsigned int y;
    int TotalLines;
    int Bytes;
    int TopLineY;

    if(TextDisplayCtrl==NULL)
        return;

    /* Find the total number of lines */
    if(TopOfBufferLine>BottomOfBufferLine)
        Bytes=(EndOfHexBuffer-TopOfBufferLine)+(BottomOfBufferLine-HexBuffer);
    else
        Bytes=BottomOfBufferLine-TopOfBufferLine;
    TotalLines=Bytes/HEX_BYTES_PER_LINE;
    TotalLines++;

    /* Figure out the current topline in lines */
    if(TopLine>=TopOfBufferLine)
        Bytes=TopLine-TopOfBufferLine;
    else
        Bytes=(EndOfHexBuffer-TopOfBufferLine)+(TopLine-HexBuffer);
    TopLineY=Bytes/HEX_BYTES_PER_LINE;
    TopLineY++;

    if(TotalLines<DisplayLines)
    {
        /* Simple cal's all line are visible */
        y=(BottomOfBufferLine-TopOfBufferLine)/HEX_BYTES_PER_LINE;
    }
    else
    {
        /* More lines than are visible, we need to see if the cursor should
           be shown */
        y=DisplayLines-1;
        if(!ScrollBarAtBottom())
        {
            /* Bottom line is not on screen, turn cursor off and return */
            UITC_SetCursorStyle(TextDisplayCtrl,e_TextCursorStyle_Hidden);
            return;
        }
    }

    /* Now figure out the X */
    x=InsertPoint;
    x*=3;   // 3 because a hex number is 2 digits + a space

    UITC_SetCursorPos(TextDisplayCtrl,x,y);
    UITC_SetCursorStyle(TextDisplayCtrl,CursorStyle);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::SetCursorStyle
 *
 * SYNOPSIS:
 *    void DisplayBinary::SetCursorStyle(e_TextCursorStyleType Style)
 *
 * PARAMETERS:
 *    Style [I] -- What style to draw the cursor in
 *
 * FUNCTION:
 *    This function changes what style the cursor is drawen as in this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::SetCursorStyle(e_TextCursorStyleType Style)
{
    CursorStyle=Style;
    RethinkCursor();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::SetInFocus
 *
 * SYNOPSIS:
 *    void DisplayBinary::SetInFocus(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gives focus to this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    GetInFocus()
 ******************************************************************************/
void DisplayBinary::SetInFocus(void)
{
    union DBEventData Info;

    HasFocus=true;

    UITC_SetFocus(TextDisplayCtrl,e_UITCSetFocus_Main);

    Info.Focus.HasFocus=HasFocus;
    SendEvent(e_DBEvent_FocusChange,&Info);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ResetTerm
 *
 * SYNOPSIS:
 *    void DisplayBinary::ResetTerm(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does an reset term on the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::ResetTerm(void)
{
    ClearScreen(e_ScreenClear_Clear);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ClearScrollBackBuffer
 *
 * SYNOPSIS:
 *    void DisplayBinary::ClearScrollBackBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the scroll back buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::ClearScrollBackBuffer(void)
{
    ClearScreen(e_ScreenClear_Clear);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::SetDrawMask
 *
 * SYNOPSIS:
 *    void DisplayBinary::SetDrawMask(uint16_t Mask);
 *
 * PARAMETERS:
 *    Mask [I] -- What mask to apply to drawing attributes.  If the bit is
 *                set then this is drawen.  These are the bits from
 *                UITC_SetDrawMask() UI control.
 *
 * FUNCTION:
 *    This function sets the draw mask for this display.  The draw mask is
 *    what attributes / styles can be drawn (IE bold, color, etc).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::SetDrawMask(uint16_t Mask)
{
    if(TextDisplayCtrl==NULL)
        return;

    UITC_SetDrawMask(TextDisplayCtrl,Mask);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetContextMenuHandle
 *
 * SYNOPSIS:
 *    t_UIContextMenuCtrl *DisplayBinary::GetContextMenuHandle(
 *              e_UITD_ContextMenuType UIObj)
 *
 * PARAMETERS:
 *    UIObj [I] -- The context menu item to get the handle for.
 *
 * FUNCTION:
 *    This function gets a context menu item's handle.
 *
 * RETURNS:
 *    The context menu item's handle or NULL if it was not found.  If this
 *    is not supported (because there is no menu for example) then this will
 *    return NULL.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContextMenuCtrl *DisplayBinary::GetContextMenuHandle(e_UITD_ContextMenuType UIObj)
{
    if(TextDisplayCtrl==NULL)
        return NULL;
    return UITC_GetContextMenuHandle(TextDisplayCtrl,UIObj);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetContextSubMenuHandle
 *
 * SYNOPSIS:
 *    t_UIContextMenuCtrl *DisplayBinary::GetContextSubMenuHandle(
 *              e_UITD_ContextMenuType UIObj)
 *
 * PARAMETERS:
 *    UIObj [I] -- The context menu item to get the handle for.
 *
 * FUNCTION:
 *    This function gets a context menu item's handle.
 *
 * RETURNS:
 *    The context sub menu item's handle or NULL if it was not found.  If this
 *    is not supported (because there is no menu for example) then this will
 *    return NULL.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContextSubMenuCtrl *DisplayBinary::GetContextSubMenuHandle(e_UITD_ContextSubMenuType UIObj)
{
    if(TextDisplayCtrl==NULL)
        return NULL;

    return UITC_GetContextSubMenuHandle(TextDisplayCtrl,UIObj);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::DoScrollTimerTimeout
 *
 * SYNOPSIS:
 *    void DisplayBinary::DoScrollTimerTimeout(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the scroll timer goes off.  It scroll the
 *    screen if needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::DoScrollTimerTimeout(void)
{
    if(SelectionLine==NULL)
    {
        /* Hu? this is for scrolling when doing a selection.  No selection
           means we shouldn't be called */
        UITimerStop(ScrollTimer);
        return;
    }

    ScrollScreen(AutoSelectionScrolldx*CharWidthPx,AutoSelectionScrolldy);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::HandleLeftMousePress
 *
 * SYNOPSIS:
 *    void DisplayBinary::HandleLeftMousePress(bool Down,int x,int y);
 *
 * PARAMETERS:
 *    Down [I] -- The mouse button was pressed (true) or released (false).
 *    x [I] -- The x pos of the mouse
 *    y [I] -- The y pos of the mouse
 *
 * FUNCTION:
 *    This function is called when the left mouse button is pressed or released.
 *
 *    It used to track if we are moving the selection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::HandleLeftMousePress(bool Down,int x,int y)
{
    LeftMouseDown=Down;
    if(Down)
    {
        /* Clear the selection, but note where the user clicked */
        SelectionActive=false;
        SelectionLine=NULL;
        SelectionAnchorLine=NULL;

        ConvertScreenXY2BufferLinePtr(x,y,&SelectionAnchorLine,
                &SelectionLineAnchorOffset,&SelectionInAscII);

        SelectionLine=SelectionAnchorLine;
        SelectionLineOffset=SelectionLineAnchorOffset;

        RedrawScreen();
        SendEvent(e_DBEvent_SelectionChanged,NULL);
    }
    else
    {
        /* If the mouse is coming up, cancel any selection scroll timers */
        if(UITimerRunning(ScrollTimer))
            UITimerStop(ScrollTimer);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::HandleMouseMove
 *
 * SYNOPSIS:
 *    void DisplayBinary::HandleMouseMove(int x,int y);
 *
 * PARAMETERS:
 *    x [I] -- The x pos of the mouse
 *    y [I] -- The y pos of the mouse
 *
 * FUNCTION:
 *    This function is called to inform us that the mouse has moved.  The
 *    event may only come if the mouse button is down.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::HandleMouseMove(int x,int y)
{
    uint8_t *TmpSelectionLine;
    int TmpSelectionLineOffset;
    bool TmpSelectionInAscII;

    if(TextDisplayCtrl==NULL)
        return;

    if(LeftMouseDown)
    {
        AutoSelectionScrolldx=0;
        if(x<0)
            AutoSelectionScrolldx=-1;
        else if(x>UITC_GetWidgetWidth(TextDisplayCtrl))
            AutoSelectionScrolldx=1;

        AutoSelectionScrolldy=0;
        if(y<0)
            AutoSelectionScrolldy=-1;
        else if(y>UITC_GetWidgetHeight(TextDisplayCtrl))
            AutoSelectionScrolldy=1;

        if(AutoSelectionScrolldx!=0 || AutoSelectionScrolldy!=0)
        {
            /* Ok, we are scrolling the display */
            ScrollScreen(AutoSelectionScrolldx*CharWidthPx,
                    AutoSelectionScrolldy);

            /* Start the auto scroll timer */
            UITimerStart(ScrollTimer);
        }
        else
        {
            /* We don't need the timer */
            if(UITimerRunning(ScrollTimer))
                UITimerStop(ScrollTimer);
        }

        ConvertScreenXY2BufferLinePtr(x,y,&TmpSelectionLine,
                &TmpSelectionLineOffset,&TmpSelectionInAscII);

        /* Only change the selection if it was in the same zone we started in */
        if(TmpSelectionInAscII==SelectionInAscII && TmpSelectionLine!=NULL)
        {
            SelectionLine=TmpSelectionLine;
            SelectionLineOffset=TmpSelectionLineOffset;
        }

        if(SelectionLine!=SelectionAnchorLine ||
                SelectionLineOffset!=SelectionLineAnchorOffset)
        {
            SelectionActive=true;
        }

        RedrawScreen();
        SendEvent(e_DBEvent_SelectionChanged,NULL);
    }
    else
    {
        /* If we have a scroll timer running kill it */
        if(UITimerRunning(ScrollTimer))
            UITimerStop(ScrollTimer);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ConvertScreenXY2BufferLinePtr
 *
 * SYNOPSIS:
 *    bool DisplayBinary::ConvertScreenXY2BufferLinePtr(int x,int y,
 *              uint8_t **LinePtr,int *Offset,bool *InAscII);
 *
 * PARAMETERS:
 *    x [I] -- The x pos on the screen
 *    y [I] -- The y pos on the screen
 *    LinePtr [O] -- The pointer into 'HexBuffer' that this x,y is.
 *    Offset [O] -- The offset from 'LinePtr' that this x,y is.
 *    InAscII [O] -- Set to true if this is in the AscII area or false if
 *                   it's in the hex area.
 *
 * FUNCTION:
 *    This function takes a screen x,y and converts it to a pointer into the
 *    'HexBuffer' buffer.
 *
 * RETURNS:
 *    true -- The x,y is in the text
 *    false -- The x,y is out of bounds.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBinary::ConvertScreenXY2BufferLinePtr(int x,int y,uint8_t **Ptr,
        int *Offset,bool *InAscII)
{
    int XOffset;
    int YOffset;
    uint8_t *SelLine;

    *Ptr=NULL;
    *Offset=0;

    if(x<0 || y<0)
        return false;

    /* Add the offset */
    x+=WindowXOffsetPx;

    /* Check if we are clicking a "dead" space */
    if((x>=END_OF_HEX_PX && x<START_OF_ASCII_PX) || x>=END_OF_ASCII_PX)
        return false;

    YOffset=y/CharHeightPx;
    SelLine=TopLine+YOffset*HEX_BYTES_PER_LINE;
    if(SelLine>=EndOfHexBuffer)
    {
        /* Wrap */
        SelLine=HexBuffer+(SelLine-EndOfHexBuffer);
    }
    if(y>=DisplayLines*CharHeightPx)
        return false;

    XOffset=x/(CharWidthPx*3);    // We have 3 chars for a hex number (2 numbers + a space)
    if(x>=START_OF_ASCII_PX)
    {
        /* Ok, we are in the ascII display */
        XOffset=(x-START_OF_ASCII_PX)/CharWidthPx;
        *InAscII=true;
    }
    else
    {
        *InAscII=false;
    }

    if(SelLine==BottomOfBufferLine && XOffset>=InsertPoint)
        return false;

    *Ptr=SelLine;
    *Offset=XOffset;

    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ConvertPoint2Offset
 *
 * SYNOPSIS:
 *    uint32_t DisplayBinary::ConvertPoint2Offset(struct DisBin_PointPair *Point);
 *
 * PARAMETERS:
 *    Point [I] -- The point to convert
 *
 * FUNCTION:
 *    This function converts a point to an offset from 'TopOfBufferLine'
 *
 * RETURNS:
 *    The offset from 'TopOfBufferLine'
 *
 * SEE ALSO:
 *    ConvertOffset2Point()
 ******************************************************************************/
uint32_t DisplayBinary::ConvertPoint2Offset(struct DisBin_PointPair *Point)
{
    uint32_t Offset;

    if(Point->Line>=TopOfBufferLine)
        Offset=Point->Line-TopOfBufferLine;
    else
        Offset=(EndOfHexBuffer-TopOfBufferLine)+(Point->Line-HexBuffer);

    Offset+=Point->Offset;

    return Offset;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ConvertOffset2Point
 *
 * SYNOPSIS:
 *    void DisplayBinary::ConvertOffset2Point(uint32_t Offset,
 *              struct DisBin_PointPair *Point);
 *
 * PARAMETERS:
 *    Offset [I] -- The offset to convert
 *    Point [O] -- The point this offset is at
 *
 * FUNCTION:
 *    This function converts from an offset to a point.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ConvertPoint2Offset()
 ******************************************************************************/
void DisplayBinary::ConvertOffset2Point(uint32_t Offset,
        struct DisBin_PointPair *Point)
{
    uint32_t Lines;
    uint8_t *Ptr;

    Lines=Offset/HEX_BYTES_PER_LINE;
    Ptr=TopOfBufferLine+Lines*HEX_BYTES_PER_LINE;
    if(Ptr>=EndOfHexBuffer)
        Ptr-=HexBufferSize;

    Point->Line=Ptr;
    Point->Offset=Offset%HEX_BYTES_PER_LINE;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ScrollScreen
 *
 * SYNOPSIS:
 *    void DisplayBinary::ScrollScreen(int dxpx,int dy);
 *
 * PARAMETERS:
 *    dxpx [I] -- The amount to scroll the screen in the x dir.  This is pixels
 *    dy [I] -- The amount to scroll the screen in the y dir.  This is chars
 *
 * FUNCTION:
 *    This function moves the scroll bar(s) by an amount.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::ScrollScreen(int dxpx,int dy)
{
    t_UIScrollBarCtrl *HorzScroll;
    t_UIScrollBarCtrl *VertScroll;
    int MaxPos;
    int NewPos;
    int ScreenChars;
    int TotalChars;
    int TotalLines;
    int CurrentPos;

    if(TextDisplayCtrl==NULL)
        return;

    HorzScroll=UITC_GetHorzSlider(TextDisplayCtrl);
    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);

    /* Hozr */
    ScreenChars=ScreenWidthPx/CharWidthPx;
    TotalChars=END_OF_ASCII_CHAR;

    if(TotalChars>=ScreenChars)
        MaxPos=(TotalChars-ScreenChars)*CharWidthPx;
    else
        MaxPos=0;
    WindowXOffsetPx+=dxpx;
    if(WindowXOffsetPx<0)
        WindowXOffsetPx=0;
    if(WindowXOffsetPx>MaxPos)
        WindowXOffsetPx=MaxPos;

    UISetScrollBarPos(HorzScroll,WindowXOffsetPx/CharWidthPx);

    /* Vert */
    CurrentPos=UIGetScrollBarPos(VertScroll);
    TotalLines=UIGetScrollBarTotalSize(VertScroll);
    MaxPos=TotalLines-DisplayLines;

    NewPos=CurrentPos+dy;
    if(NewPos>MaxPos)
        NewPos=MaxPos;
    if(NewPos<0)
        NewPos=0;

    UISetScrollBarPos(VertScroll,NewPos);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetSelectionString
 *
 * SYNOPSIS:
 *    bool DisplayBinary::GetSelectionString(std::string &Clip);
 *
 * PARAMETERS:
 *    Clip [O] -- The text from the selection
 *
 * FUNCTION:
 *    This function gets a copy of the selection text.
 *
 * RETURNS:
 *    true -- The selection was valid and 'Clip' has been set
 *    false --- There was no selection and 'Clip' has been set to ""
 *
 * SEE ALSO:
 *    IsThereASelection(), SelectAll(), ClearSelection()
 ******************************************************************************/
bool DisplayBinary::GetSelectionString(std::string &Clip)
{
    uint8_t *Start;
    uint8_t *End;
    int StartOffset;
    int EndOffset;
    uint8_t *Line;
    string TmpStr;
    unsigned int s;
    struct DisBin_Block SelBlocks[2];

    if(!SelectionActive || SelectionLine==NULL || SelectionAnchorLine==NULL)
        return false;

    GetNormalizedSelectionBlocks(SelBlocks);

    Clip="";
    for(s=0;s<2;s++)
    {
        Start=SelBlocks[s].Start.Line;
        End=SelBlocks[s].End.Line;
        StartOffset=SelBlocks[s].Start.Offset;
        EndOffset=SelBlocks[s].End.Offset;

        if(Start==NULL || End==NULL)
            continue;

        Line=Start;
        if(Start!=End)
        {
            /* The first line (partal line) */
            BuildSelOutputAndAppendData(Clip,&Line[StartOffset],
                    HEX_BYTES_PER_LINE-StartOffset,SelectionInAscII);
            Line+=HEX_BYTES_PER_LINE;

            /* Full lines */
            while(Line!=End)
            {
                BuildSelOutputAndAppendData(Clip,Line,HEX_BYTES_PER_LINE,
                        SelectionInAscII);
                Line+=HEX_BYTES_PER_LINE;
            }

            /* The last line (partal line) */
            if(End<EndOfHexBuffer)
            {
                BuildSelOutputAndAppendData(Clip,Line,EndOffset+1,
                        SelectionInAscII);
            }
        }
        else
        {
            /* Just part of one line */
            if(EndOffset>StartOffset)
            {
                BuildSelOutputAndAppendData(Clip,&Line[StartOffset],
                        EndOffset-StartOffset+1,SelectionInAscII);
            }
            else
            {
                BuildSelOutputAndAppendData(Clip,&Line[EndOffset],
                        StartOffset-EndOffset+1,SelectionInAscII);
            }
        }
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::BuildSelOutputAndAppendData
 *
 * SYNOPSIS:
 *    void DisplayBinary::BuildSelOutputAndAppendData(std::string &Dest,
 *              const uint8_t *Src,int Bytes,bool AscII);
 *
 * PARAMETERS:
 *    Dest [I/O] -- The is the string to append to
 *    Src [I] -- The hex data to append
 *    Bytes [I] -- The number of bytes to copy
 *    AscII [I] -- Output the hex data as or as AscII data.
 *
 * FUNCTION:
 *    This function is a helper function for building a block of the hex data
 *    as AscII or hex codes and append it to a string.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::BuildSelOutputAndAppendData(std::string &Dest,
        const uint8_t *Src,int Bytes,bool AscII)
{
    int r;
    char buff[HEX_BYTES_PER_LINE*3+1];

    if((unsigned)Bytes*3>=sizeof(buff))
        return;

    if(AscII)
    {
        Dest.append((const char *)Src,Bytes);
    }
    else
    {
        for(r=0;r<Bytes;r++)
        {
            sprintf(&buff[r*3],"%02X ",Src[r]);
        }
        Dest.append(buff);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::IsThereASelection
 *
 * SYNOPSIS:
 *    bool DisplayBinary::IsThereASelection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function checks to see if there is selected text.
 *
 * RETURNS:
 *    true -- There is text that can be copied to the clip board
 *    false -- There is not selection (nothing to copy).
 *
 * SEE ALSO:
 *    DisplayBase::GetSelectionString()
 ******************************************************************************/
bool DisplayBinary::IsThereASelection(void)
{
    if(!SelectionActive)
        return false;
    if(SelectionLine==NULL || SelectionAnchorLine==NULL)
        return false;
    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::SelectAll
 *
 * SYNOPSIS:
 *    void DisplayBinary::SelectAll(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sets the selection to select everything.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    GetSelectionString()
 ******************************************************************************/
void DisplayBinary::SelectAll(void)
{
    SelectionInAscII=false;
    SelectionActive=true;
    SelectionLine=TopOfBufferLine;
    SelectionLineOffset=0;
    SelectionAnchorLine=BottomOfBufferLine;
    SelectionLineAnchorOffset=InsertPoint;

    RedrawScreen();

    SendEvent(e_DBEvent_SelectionChanged,NULL);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ClearSelection
 *
 * SYNOPSIS:
 *    void DisplayBinary::ClearSelection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sets the selection to nothing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    GetSelectionString()
 ******************************************************************************/
void DisplayBinary::ClearSelection(void)
{
    SelectionInAscII=false;
    SelectionActive=false;
    SelectionLine=NULL;
    SelectionLineOffset=0;
    SelectionAnchorLine=NULL;
    SelectionLineAnchorOffset=0;

    RedrawScreen();

    SendEvent(e_DBEvent_SelectionChanged,NULL);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetNormalizedSelectionBlocks
 *
 * SYNOPSIS:
 *    bool DisplayBinary::GetNormalizedSelectionBlocks(
 *              struct DisBin_Block *Blocks);
 *
 * PARAMETERS:
 *    Blocks [O] -- This is a 2 dim array of selection data for the two blocks
 *                  that make up the selection.
 *
 * FUNCTION:
 *    This function takes the selection and normalizes it (makes sure select
 *    is before anchor) and split the selection into blocks.
 *
 *    The blocks are needed because the hex buffer is circular and loops.  This
 *    means that you can end up with one block that goes to the end of the
 *    buffer and another that is at the top with a hole in the middle.
 *
 * RETURNS:
 *    true -- We where able to get the selection
 *    false -- There wan't a valid selection.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBinary::GetNormalizedSelectionBlocks(struct DisBin_Block *Blocks)
{
    struct DisBin_PointPair P1;
    struct DisBin_PointPair P2;

    if(!SelectionActive || SelectionLine==NULL || SelectionAnchorLine==NULL)
        return false;

    P1.Line=SelectionLine;
    P1.Offset=SelectionLineOffset;
    P2.Line=SelectionAnchorLine;
    P2.Offset=SelectionLineAnchorOffset;

    GetNormalizedPoints(&P1,&P2,Blocks);

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetNormalizedPoints
 *
 * SYNOPSIS:
 *    void DisplayBinary::GetNormalizedPoints(struct DisBin_PointPair *P1,
 *              struct DisBin_PointPair *P2,struct DisBin_Block *Blocks);
 *
 * PARAMETERS:
 *    P1 [I] -- The first point
 *    P2 [I] -- The second point
 *    Blocks [O] -- This is a 2 dim array of for the two points
 *                  that make up the selection.
 *
 * FUNCTION:
 *    This function takes 2 points and normalizes it (makes sure start point
 *    is before end point) and split the selection into blocks.
 *
 *    The blocks are needed because the hex buffer is circular and loops.  This
 *    means that you can end up with one block that goes to the end of the
 *    buffer and another that is at the top with a hole in the middle.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::GetNormalizedPoints(struct DisBin_PointPair *P1,
        struct DisBin_PointPair *P2,struct DisBin_Block *Blocks)
{
    unsigned int s;

    /* Setup the return values */
    for(s=0;s<2;s++)
    {
        Blocks[s].Start.Line=NULL;
        Blocks[s].Start.Offset=0;
        Blocks[s].End.Line=NULL;
        Blocks[s].End.Offset=0;
    }

    if(P1->Line==P2->Line)
    {
        /* They are on the same line */
        Blocks[0].Start.Line=P1->Line;
        Blocks[0].End.Line=P2->Line;
        if(P2->Offset>P1->Offset)
        {
            Blocks[0].Start.Offset=P1->Offset;
            Blocks[0].End.Offset=P2->Offset;
        }
        else
        {
            Blocks[0].Start.Offset=P2->Offset;
            Blocks[0].End.Offset=P1->Offset;
        }
    }
    else if((P1->Line<TopOfBufferLine &&
            P2->Line<TopOfBufferLine) ||
            (P1->Line>=TopOfBufferLine &&
            P2->Line>=TopOfBufferLine))
    {
        /* Both are above or below the start of the data, just setup one */
        if(P2->Line>P1->Line)
        {
            Blocks[0].Start.Line=P1->Line;
            Blocks[0].Start.Offset=P1->Offset;
            Blocks[0].End.Line=P2->Line;
            Blocks[0].End.Offset=P2->Offset;
        }
        else
        {
            Blocks[0].Start.Line=P2->Line;
            Blocks[0].Start.Offset=P2->Offset;
            Blocks[0].End.Line=P1->Line;
            Blocks[0].End.Offset=P1->Offset;
        }
    }
    else
    {
        /* One is above and one below, we need to highlight blocks */
        if(P1->Line>TopOfBufferLine)
        {
            Blocks[0].Start.Line=P1->Line;
            Blocks[0].Start.Offset=P1->Offset;
            Blocks[0].End.Line=EndOfHexBuffer-HEX_BYTES_PER_LINE;
            Blocks[0].End.Offset=HEX_BYTES_PER_LINE-1;

            Blocks[1].Start.Line=HexBuffer;
            Blocks[1].Start.Offset=0;
            Blocks[1].End.Line=P2->Line;
            Blocks[1].End.Offset=P2->Offset;
        }
        else
        {
            Blocks[0].Start.Line=P2->Line;
            Blocks[0].Start.Offset=P2->Offset;
            Blocks[0].End.Line=EndOfHexBuffer-HEX_BYTES_PER_LINE;
            Blocks[0].End.Offset=HEX_BYTES_PER_LINE-1;

            Blocks[1].Start.Line=HexBuffer;
            Blocks[1].Start.Offset=0;
            Blocks[1].End.Line=P1->Line;
            Blocks[1].End.Offset=P1->Offset;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ApplySettings
 *
 * SYNOPSIS:
 *    void DisplayBinary::ApplySettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function needs to be called after there is a change to the con
 *    settings that this display has been connected to.
 *
 *    It will look things again and apply them to this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::ApplySettings(void)
{
    if(TextDisplayCtrl==NULL)
        return;

    if(g_Settings.MouseCursorIBeam)
        UITC_SetMouseCursor(TextDisplayCtrl,e_UIMouse_Cursor_IBeam);
    else
        UITC_SetMouseCursor(TextDisplayCtrl,e_UIMouse_Cursor_Default);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::SetCursorBlinking
 *
 * SYNOPSIS:
 *    void DisplayBinary::SetCursorBlinking(bool Blinking);
 *
 * PARAMETERS:
 *    Blinking [I] -- Is the cursor blinking (true), or on solid (false)
 *
 * FUNCTION:
 *    This function changes if the cursor is blinking or not.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::SetCursorBlinking(bool Blinking)
{
    if(TextDisplayCtrl==NULL)
        return;

    UITC_SetCursorBlinking(TextDisplayCtrl,Blinking);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetSendPanel_HexPosInput
 *
 * SYNOPSIS:
 *    t_UITextInputCtrl *DisplayBinary::GetSendPanel_HexPosInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the text input for cursor position.
 *
 * RETURNS:
 *    A handle to the hex cursor position text input in the send panel or NULL
 *    if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UITextInputCtrl *DisplayBinary::GetSendPanel_HexPosInput(void)
{
    return UITC_GetTextInputHandle(TextDisplayCtrl,e_UITC_Txt_Pos);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetSendPanel_HexRadioBttn
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *DisplayBinary::GetSendPanel_HexRadioBttn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the hex mode radio button.
 *
 * RETURNS:
 *    A handle to the widget or NULL if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIRadioBttnCtrl *DisplayBinary::GetSendPanel_HexRadioBttn(void)
{
    return UITC_GetRadioButton(TextDisplayCtrl,e_UITC_RadioButton_Hex);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetSendPanel_TextRadioBttn
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *DisplayBinary::GetSendPanel_TextRadioBttn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the text mode radio button.
 *
 * RETURNS:
 *    A handle to the widget or NULL if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIRadioBttnCtrl *DisplayBinary::GetSendPanel_TextRadioBttn(void)
{
    return UITC_GetRadioButton(TextDisplayCtrl,e_UITC_RadioButton_Text);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetSendPanel_TextInput
 *
 * SYNOPSIS:
 *    t_UIMuliLineTextInputCtrl *DisplayBinary::GetSendPanel_TextInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the muliline text to send input.
 *
 * RETURNS:
 *    A handle to the widget or NULL if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIMuliLineTextInputCtrl *DisplayBinary::GetSendPanel_TextInput(void)
{
    return UITC_GetMuliLineTextInputHandle(TextDisplayCtrl,e_UITC_MuliTxt_TextInput);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetSendPanel_LineEndInput
 *
 * SYNOPSIS:
 *    t_UIComboBoxCtrl *DisplayBinary::GetSendPanel_LineEndInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the line end input.
 *
 * RETURNS:
 *    A handle to the widget or NULL if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIComboBoxCtrl *DisplayBinary::GetSendPanel_LineEndInput(void)
{
    return UITC_GetComboBoxHandle(TextDisplayCtrl,e_UITC_Combox_LineEnd);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::SendPanel_ShowHexOrText
 *
 * SYNOPSIS:
 *    void DisplayBinary::SendPanel_ShowHexOrText(bool Text);
 *
 * PARAMETERS:
 *    Text [I] -- Show the text input (true) or the hex input (false).
 *
 * FUNCTION:
 *    This function tells the GUI to show the text input or the hex input in
 *    the send panel.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::SendPanel_ShowHexOrText(bool Text)
{
    UITC_SendPanelShowHexOrTextInput(TextDisplayCtrl,Text);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ToggleAttribs2Selection
 *
 * SYNOPSIS:
 *    void DisplayBinary::ToggleAttribs2Selection(uint32_t Attribs);
 *
 * PARAMETERS:
 *    Attribs [I] -- The TXT_ATTRIB_ attribs to toggle.
 *
 * FUNCTION:
 *    This function takes the current selection and check if any of the
 *    selected bytes has these attribs turned on.  If they do it removes that
 *    attrib, if not it turns the attribs on.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::ToggleAttribs2Selection(uint32_t Attribs)
{
    struct DisBin_Block SelBlocks[2];
    bool IsSet;

    if(!SelectionActive)
        return;

    if(!GetNormalizedSelectionBlocks(SelBlocks))
        return;

    IsSet=false;
    if(CheckIfAttribSet(&SelBlocks[0],Attribs))
        IsSet=true;
    if(SelBlocks[1].Start.Line!=NULL)
        if(CheckIfAttribSet(&SelBlocks[0],Attribs))
            IsSet=true;

    FillAttrib(&SelBlocks[0],Attribs,!IsSet);

    /* We need to do it again for block 2 */
    if(SelBlocks[1].Start.Line!=NULL)
        FillAttrib(&SelBlocks[1],Attribs,!IsSet);

    RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::FillAttrib
 *
 * SYNOPSIS:
 *    void DisplayBinary::FillAttrib(struct DisBin_Block *SelBlock,
 *          uint32_t Attribs,bool Set);
 *
 * PARAMETERS:
 *    SelBlock [I] -- The selection block to fill the attrib on
 *    Attribs [I] -- The attribs to set or clear (bits)
 *    Set [I] -- If this is true then we set the attrib, else we clear
 *
 * FUNCTION:
 *    This function is a helper function that fills a selection with an
 *    attribute.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DisplayBinary::ToggleAttribs2Selection()
 ******************************************************************************/
void DisplayBinary::FillAttrib(struct DisBin_Block *SelBlock,
        uint32_t Attribs,bool Set)
{
    struct CharStyling *ColStart;
    struct CharStyling *ColEnd;
    struct CharStyling *FillPos;

    /* Get the pointers into the color buffers for this selection */
    ColStart=GetColorPtrFromLinePtr(&SelBlock->Start.Line[SelBlock->Start.Offset]);
    ColEnd=GetColorPtrFromLinePtr(&SelBlock->End.Line[SelBlock->End.Offset]);

    FillPos=ColStart;
    while(FillPos<=ColEnd)
    {
        if(Set)
            FillPos->Attribs|=Attribs;
        else
            FillPos->Attribs&=~Attribs;
        FillPos++;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::CheckIfAttribSet
 *
 * SYNOPSIS:
 *    bool DisplayBinary::CheckIfAttribSet(struct DisBin_Block *SelBlock,
 *          uint32_t Attribs);
 *
 * PARAMETERS:
 *    SelBlock [I] -- The selection block to check the attrib on
 *    Attribs [I] -- The attribs to check
 *
 * FUNCTION:
 *    This function checks if attribs are send in a block
 *
 * RETURNS:
 *    true -- The attrib is set somewhere in the block.
 *    false -- The attrib is not in the block.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBinary::CheckIfAttribSet(struct DisBin_Block *SelBlock,
        uint32_t Attribs)
{
    struct CharStyling *ColStart;
    struct CharStyling *ColEnd;
    struct CharStyling *FillPos;

    /* Get the pointers into the color buffers for this selection */
    ColStart=GetColorPtrFromLinePtr(&SelBlock->Start.Line[SelBlock->Start.Offset]);
    ColEnd=GetColorPtrFromLinePtr(&SelBlock->End.Line[SelBlock->End.Offset]);

    FillPos=ColStart;
    while(FillPos<=ColEnd)
    {
        if(FillPos->Attribs&Attribs)
            return true;
        FillPos++;
    }
    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetColorPtrFromLinePtr
 *
 * SYNOPSIS:
 *    struct CharStyling *DisplayBinary::GetColorPtrFromLinePtr(
 *          const uint8_t *Line);
 *
 * PARAMETERS:
 *    Line [I] -- A pointer to the line we are interested in finding the
 *                matching spot in 'ColorBuffer'.
 *
 * FUNCTION:
 *    This function finds a matching position from 'HexBuffer' in 'ColorBuffer'.
 *    You can use this function to find the start of the screen or selection
 *    positions in the color buffer.
 *
 * RETURNS:
 *    A pointer that matches 'Line' in the color buffer.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct CharStyling *DisplayBinary::GetColorPtrFromLinePtr(const uint8_t *Line)
{
    unsigned int Offset;

    /* Step 1, find the offset from the 'HexBuffer' */
    Offset=Line-HexBuffer;

    /* Step 2, find this offset in the color buffer */
    return &ColorBuffer[Offset];
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ApplyBGColor2Selection
 *
 * SYNOPSIS:
 *    void DisplayBinary::ApplyBGColor2Selection(uint32_t RGB);
 *
 * PARAMETERS:
 *    RGB [I] -- The color to apply
 *
 * FUNCTION:
 *    This function takes the current selection and changes the background
 *    color for all the text selected.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::ApplyBGColor2Selection(uint32_t RGB)
{
    struct DisBin_Block SelBlocks[2];
    struct CharStyling *ColStart;
    struct CharStyling *ColEnd;
    struct CharStyling *FillPos;
    int s;

    if(!SelectionActive)
        return;

    if(!GetNormalizedSelectionBlocks(SelBlocks))
        return;

    for(s=0;s<2;s++)
    {
        if(SelBlocks[s].Start.Line==NULL || SelBlocks[s].End.Line==NULL)
            break;

        /* Get the pointers into the color buffers for this selection */
        ColStart=GetColorPtrFromLinePtr(&SelBlocks[s].Start.Line[SelBlocks[s].Start.Offset]);
        ColEnd=GetColorPtrFromLinePtr(&SelBlocks[s].End.Line[SelBlocks[s].End.Offset]);

        FillPos=ColStart;
        while(FillPos<=ColEnd)
        {
            FillPos->BGColor=RGB;
            FillPos++;
        }
    }

    RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::IsAttribSetInSelection
 *
 * SYNOPSIS:
 *    bool DisplayBinary::IsAttribSetInSelection(uint32_t Attribs);
 *
 * PARAMETERS:
 *    Attribs [I] -- The attrib to check
 *
 * FUNCTION:
 *    This function checks the bytes in the selection and return true if
 *    any of them have this attrib set.
 *
 * RETURNS:
 *    true -- At least 1 byte of the selection has this attrib
 *    false -- There is no selection or the selection does not have this
 *             attrib in it.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBinary::IsAttribSetInSelection(uint32_t Attribs)
{
    struct DisBin_Block SelBlocks[2];
    bool IsSet;

    if(!SelectionActive)
        return false;

    if(!GetNormalizedSelectionBlocks(SelBlocks))
        return false;

    IsSet=false;
    if(CheckIfAttribSet(&SelBlocks[0],Attribs))
        IsSet=true;
    if(SelBlocks[1].Start.Line!=NULL)
        if(CheckIfAttribSet(&SelBlocks[0],Attribs))
            IsSet=true;

    return IsSet;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetSelectionRAW
 *
 * SYNOPSIS:
 *    uint8_t *DisplayBinary::GetSelectionRAW(unsigned int *Bytes);
 *
 * PARAMETERS:
 *    Bytes [O] -- The number of bytes in the selection.
 *
 * FUNCTION:
 *    This function gets a copy of the selection as RAW bytes.
 *
 * RETURNS:
 *    A buffer with the selection copied into it or NULL if there was an error.
 *    You must free this buffer with free() (it will be allocated with
 *    malloc()).
 *
 * SEE ALSO:
 *    IsThereASelection(), SelectAll(), ClearSelection()
 ******************************************************************************/
uint8_t *DisplayBinary::GetSelectionRAW(unsigned int *Bytes)
{
    uint8_t *Start;
    uint8_t *End;
    int StartOffset;
    int EndOffset;
    uint8_t *Line;
    string TmpStr;
    unsigned int s;
    struct DisBin_Block SelBlocks[2];
    unsigned int TotalBytes;
    uint8_t *RetBuff;
    uint8_t *RetBuffInsertPos;

    if(!SelectionActive || SelectionLine==NULL || SelectionAnchorLine==NULL)
        return NULL;

    GetNormalizedSelectionBlocks(SelBlocks);

    /* See how many bytes we will be copying */
    TotalBytes=0;
    for(s=0;s<2;s++)
    {
        Start=SelBlocks[s].Start.Line;
        End=SelBlocks[s].End.Line;
        StartOffset=SelBlocks[s].Start.Offset;
        EndOffset=SelBlocks[s].End.Offset;

        if(Start==NULL || End==NULL)
            continue;

        Line=Start;
        if(Start!=End)
        {
            /* The first line (partal line) */
            TotalBytes+=HEX_BYTES_PER_LINE-StartOffset;
            Line+=HEX_BYTES_PER_LINE;

            /* Full lines */
            TotalBytes+=End-Line;

            /* The last line (partal line) */
            if(End<EndOfHexBuffer)
                TotalBytes+=EndOffset+1;
        }
        else
        {
            /* All on the same line */
            if(EndOffset>StartOffset)
                TotalBytes+=EndOffset-StartOffset+1;
            else
                TotalBytes+=StartOffset-EndOffset+1;
        }
    }

    if(TotalBytes==0)
    {
        /* A blank selection */
        return NULL;
    }

    *Bytes=TotalBytes;

    RetBuff=(uint8_t *)malloc(TotalBytes);
    if(RetBuff==NULL)
        return NULL;

    RetBuffInsertPos=RetBuff;
    for(s=0;s<2;s++)
    {
        Start=SelBlocks[s].Start.Line;
        End=SelBlocks[s].End.Line;
        StartOffset=SelBlocks[s].Start.Offset;
        EndOffset=SelBlocks[s].End.Offset;

        if(Start==NULL || End==NULL)
            continue;

        Line=Start;
        if(Start!=End)
        {
            /* The first line (partal line) */
            memcpy(RetBuffInsertPos,&Line[StartOffset],
                    HEX_BYTES_PER_LINE-StartOffset);
            RetBuffInsertPos+=HEX_BYTES_PER_LINE-StartOffset;
            Line+=HEX_BYTES_PER_LINE;

            /* Full lines */
            memcpy(RetBuffInsertPos,Line,End-Line);
            RetBuffInsertPos+=End-Line;
            Line+=End-Line;

            /* The last line (partal line) */
            if(End<EndOfHexBuffer)
            {
                memcpy(RetBuffInsertPos,Line,EndOffset+1);
                RetBuffInsertPos+=EndOffset+1;
            }
        }
        else
        {
            /* Just part of one line */
            if(EndOffset>StartOffset)
            {
                memcpy(RetBuffInsertPos,&Line[StartOffset],
                        EndOffset-StartOffset+1);
                RetBuffInsertPos+=EndOffset-StartOffset+1;
            }
            else
            {
                memcpy(RetBuffInsertPos,&Line[EndOffset],
                        StartOffset-EndOffset+1);
                RetBuffInsertPos+=StartOffset-EndOffset+1;
            }
        }
    }

    return RetBuff;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::InvalidateAllMarks
 *
 * SYNOPSIS:
 *    void DisplayBinary::InvalidateAllMarks(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes all the allocated marks and marks them as invalid.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::InvalidateAllMarks(void)
{
    struct BinaryPointMarker *Marker;

    for(Marker=MarkerList;Marker!=NULL;Marker=Marker->Next)
        Marker->Valid=false;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::InvalidateMarksOnScroll
 *
 * SYNOPSIS:
 *    void DisplayBinary::InvalidateMarksOnScroll(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function goes thought all the valid marks and sees if they will
 *    be invalid after a scroll.  If they will become invalid then they are
 *    marked as invalid.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::InvalidateMarksOnScroll(void)
{
    struct BinaryPointMarker *Marker;

    for(Marker=MarkerList;Marker!=NULL;Marker=Marker->Next)
    {
        if(Marker->Valid)
        {
            if(Marker->Line==TopOfBufferLine)
                Marker->Valid=false;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::AdvancePoint
 *
 * SYNOPSIS:
 *    void DisplayBinary::AdvancePoint(struct DisBin_PointPair *Point,int Amount);
 *
 * PARAMETERS:
 *    Point [I/O] -- The point to move
 *    Amount [I] -- The amount to move the point
 *
 * FUNCTION:
 *    This function moves a point backward or forward on the line.  It will not
 *    to past the start of the buffer or the current insert point.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::AdvancePoint(struct DisBin_PointPair *Point,int Amount)
{
    struct DisBin_PointPair BottomPoint;
    uint32_t Offset;
    uint32_t BottomOffset;
    int UseAmount;

    UseAmount=Amount;

    BottomPoint.Line=BottomOfBufferLine;
    BottomPoint.Offset=InsertPoint;
    BottomOffset=ConvertPoint2Offset(&BottomPoint);

    Offset=ConvertPoint2Offset(Point);

    if(UseAmount<0)
    {
        /* Check if we will overflow */
        if(-UseAmount>(int)Offset)
        {
            /* We can't go neg so 0 it */
            UseAmount=-Offset;
        }
    }

    Offset+=UseAmount;

    if(Offset>BottomOffset)
    {
        /* Clip to bottom offset */
        Point->Line=BottomOfBufferLine;
        Point->Offset=InsertPoint;
    }
    else
    {
        ConvertOffset2Point(Offset,Point);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::AllocateMark
 *
 * SYNOPSIS:
 *    t_DataProMark *DisplayBinary::AllocateMark(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates a mark for this display.  A mark is a point in
 *    the display that we are currently adding chars to.  This mark is
 *    used to reference this insert point again in the future.  The mark
 *    can become invalid if the insert point goes away.  The mark remains
 *    allocated and can be used but will not do anything.
 *
 *    The display keeps a list of all the marks that have been allocated.
 *    This is so it can go through the list and invalid any that become
 *    invalid because of a change to the display.
 *
 * RETURNS:
 *    A pointer to the mark.
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
t_DataProMark *DisplayBinary::AllocateMark(void)
{
    struct BinaryPointMarker *NewMarker;

    try
    {
        NewMarker=new struct BinaryPointMarker;
        NewMarker->Valid=false;

        /* Add it to the list of markers */
        NewMarker->Prev=NULL;
        NewMarker->Next=MarkerList;
        if(MarkerList!=NULL)
            MarkerList->Prev=NewMarker;
        MarkerList=NewMarker;

        /* Move this marker to the current cursor pos */
        SetMark2CursorPos((t_DataProMark *)NewMarker);
    }
    catch(...)
    {
        NewMarker=NULL;
    }

    return (t_DataProMark *)NewMarker;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::FreeMark
 *
 * SYNOPSIS:
 *    void DisplayBinary::FreeMark(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to free
 *
 * FUNCTION:
 *    This function frees a mark that was allocated with AllocateMark()
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    When a display text is free'ed the users of Mark's can't call FreeMark()
 *    because the connection is no longer connected.  There for when a
 *    display text is free'ed it must free the list of marks.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::FreeMark(t_DataProMark *Mark)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;

    /* Unlink this item and free it */
    if(MarkerList==Marker)
        MarkerList=Marker->Next;

    if(Marker->Next!=NULL)
        Marker->Next->Prev=Marker->Prev;

    if(Marker->Prev!=NULL)
        Marker->Prev->Next=Marker->Next;

    delete Marker;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::IsMarkValid
 *
 * SYNOPSIS:
 *    bool DisplayBinary::IsMarkValid(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *
 * FUNCTION:
 *    This function checks if a marker is valid or not.
 *
 * RETURNS:
 *    true -- Mark is still valid
 *    false -- Mark is invalid
 *
 * SEE ALSO:
 *    DPS_IsMarkValid()
 ******************************************************************************/
bool DisplayBinary::IsMarkValid(t_DataProMark *Mark)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;

    return Marker->Valid;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::SetMark2CursorPos
 *
 * SYNOPSIS:
 *    void DisplayBinary::SetMark2CursorPos(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *
 * FUNCTION:
 *    This function will take a mark and move it to the current cursor position.
 *    It will also set this mark to valid.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_SetMark2CursorPos()
 ******************************************************************************/
void DisplayBinary::SetMark2CursorPos(t_DataProMark *Mark)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;

    Marker->Line=BottomOfBufferLine;
    Marker->Offset=InsertPoint;
    Marker->Valid=true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ApplyAttrib2Mark
 *
 * SYNOPSIS:
 *    void DisplayBinary::ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Attrib [I] -- The new attrib(s) to set
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the attribs.
 *    Len [I] -- The number of chars to apply these new attributes to.
 *
 * FUNCTION:
 *    This function does the DPS_ApplyAttrib2Mark() function to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ApplyAttrib2Mark()
 ******************************************************************************/
void DisplayBinary::ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,
        uint32_t Offset,uint32_t Len)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;
    struct DisBin_PointPair P1;
    struct DisBin_PointPair P2;
    struct DisBin_Block Blocks[2];
    struct CharStyling *ColStart;
    struct CharStyling *ColEnd;
    struct CharStyling *FillPos;
    int s;

    if(!Marker->Valid)
        return;

    P1.Line=Marker->Line;
    P1.Offset=Marker->Offset;
    AdvancePoint(&P1,Offset);

    if(Len==0)
    {
        P2.Line=BottomOfBufferLine;
        P2.Offset=InsertPoint;
    }
    else
    {
        P2.Line=P1.Line;
        P2.Offset=P1.Offset;
        AdvancePoint(&P2,Len);
    }

    GetNormalizedPoints(&P1,&P2,Blocks);

    for(s=0;s<2;s++)
    {
        if(Blocks[s].Start.Line==NULL || Blocks[s].End.Line==NULL)
            break;

        /* Get the pointers into the color buffers for this selection */
        ColStart=GetColorPtrFromLinePtr(&Blocks[s].Start.Line[
                Blocks[s].Start.Offset]);
        ColEnd=GetColorPtrFromLinePtr(&Blocks[s].End.Line[
                Blocks[s].End.Offset]);

        FillPos=ColStart;
        while(FillPos<=ColEnd)
        {
            FillPos->Attribs|=Attrib;
            FillPos++;
        }
    }

    RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::RemoveAttribFromMark
 *
 * SYNOPSIS:
 *    void DisplayBinary::RemoveAttribFromMark(t_DataProMark *Mark,
 *          uint32_t Attrib,uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Attrib [I] -- The new attrib(s) to clear
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to remove the attribs.
 *    Len [I] -- The number of chars to remove these new attributes from.
 *
 * FUNCTION:
 *    This function does the DPS_RemoveAttribFromMark() function to the active
 *    connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_RemoveAttribFromMark()
 ******************************************************************************/
void DisplayBinary::RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,
        uint32_t Offset,uint32_t Len)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;
    struct DisBin_PointPair P1;
    struct DisBin_PointPair P2;
    struct DisBin_Block Blocks[2];
    struct CharStyling *ColStart;
    struct CharStyling *ColEnd;
    struct CharStyling *FillPos;
    int s;

    if(!Marker->Valid)
        return;

    P1.Line=Marker->Line;
    P1.Offset=Marker->Offset;
    AdvancePoint(&P1,Offset);

    if(Len==0)
    {
        P2.Line=BottomOfBufferLine;
        P2.Offset=InsertPoint;
    }
    else
    {
        P2.Line=P1.Line;
        P2.Offset=P1.Offset;
        AdvancePoint(&P2,Len);
    }

    GetNormalizedPoints(&P1,&P2,Blocks);

    for(s=0;s<2;s++)
    {
        if(Blocks[s].Start.Line==NULL || Blocks[s].End.Line==NULL)
            break;

        /* Get the pointers into the color buffers for this selection */
        ColStart=GetColorPtrFromLinePtr(&Blocks[s].Start.Line[
                Blocks[s].Start.Offset]);
        ColEnd=GetColorPtrFromLinePtr(&Blocks[s].End.Line[
                Blocks[s].End.Offset]);

        FillPos=ColStart;
        while(FillPos<=ColEnd)
        {
            FillPos->Attribs&=~Attrib;
            FillPos++;
        }
    }

    RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ApplyFGColor2Mark
 *
 * SYNOPSIS:
 *    void DPS_ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    FGColor [I] -- The colors to apply
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the color.
 *    Len [I] -- The number of chars to apply this color to
 *
 * FUNCTION:
 *    This function does the DPS_ApplyFGColor2Mark() function to the active
 *    connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ApplyFGColor2Mark()
 ******************************************************************************/
void DisplayBinary::ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,
        uint32_t Offset,uint32_t Len)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;
    struct DisBin_PointPair P1;
    struct DisBin_PointPair P2;
    struct DisBin_Block Blocks[2];
    struct CharStyling *ColStart;
    struct CharStyling *ColEnd;
    struct CharStyling *FillPos;
    int s;

    if(!Marker->Valid)
        return;

    P1.Line=Marker->Line;
    P1.Offset=Marker->Offset;
    AdvancePoint(&P1,Offset);

    if(Len==0)
    {
        P2.Line=BottomOfBufferLine;
        P2.Offset=InsertPoint;
    }
    else
    {
        P2.Line=P1.Line;
        P2.Offset=P1.Offset;
        AdvancePoint(&P2,Len);
    }

    GetNormalizedPoints(&P1,&P2,Blocks);

    for(s=0;s<2;s++)
    {
        if(Blocks[s].Start.Line==NULL || Blocks[s].End.Line==NULL)
            break;

        /* Get the pointers into the color buffers for this selection */
        ColStart=GetColorPtrFromLinePtr(&Blocks[s].Start.Line[
                Blocks[s].Start.Offset]);
        ColEnd=GetColorPtrFromLinePtr(&Blocks[s].End.Line[
                Blocks[s].End.Offset]);

        FillPos=ColStart;
        while(FillPos<=ColEnd)
        {
            FillPos->FGColor=FGColor;
            FillPos->ULineColor=FGColor;
            FillPos++;
        }
    }

    RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::ApplyBGColor2Mark
 *
 * SYNOPSIS:
 *    void DisplayBinary::ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    BGColor [I] -- The colors to apply
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the color.
 *    Len [I] -- The number of chars to apply this color to
 *
 * FUNCTION:
 *    This function does the DPS_ApplyBGColor2Mark() function to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ApplyBGColor2Mark()
 ******************************************************************************/
void DisplayBinary::ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,
        uint32_t Offset,uint32_t Len)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;
    struct DisBin_PointPair P1;
    struct DisBin_PointPair P2;
    struct DisBin_Block Blocks[2];
    struct CharStyling *ColStart;
    struct CharStyling *ColEnd;
    struct CharStyling *FillPos;
    int s;

    if(!Marker->Valid)
        return;

    P1.Line=Marker->Line;
    P1.Offset=Marker->Offset;
    AdvancePoint(&P1,Offset);

    if(Len==0)
    {
        P2.Line=BottomOfBufferLine;
        P2.Offset=InsertPoint;
    }
    else
    {
        P2.Line=P1.Line;
        P2.Offset=P1.Offset;
        AdvancePoint(&P2,Len);
    }

    GetNormalizedPoints(&P1,&P2,Blocks);

    for(s=0;s<2;s++)
    {
        if(Blocks[s].Start.Line==NULL || Blocks[s].End.Line==NULL)
            break;

        /* Get the pointers into the color buffers for this selection */
        ColStart=GetColorPtrFromLinePtr(&Blocks[s].Start.Line[
                Blocks[s].Start.Offset]);
        ColEnd=GetColorPtrFromLinePtr(&Blocks[s].End.Line[
                Blocks[s].End.Offset]);

        FillPos=ColStart;
        while(FillPos<=ColEnd)
        {
            FillPos->BGColor=BGColor;
            FillPos++;
        }
    }

    RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MoveMark
 *
 * SYNOPSIS:
 *    void DisplayBinary::MoveMark(t_DataProMark *Mark,int Amount);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Amount [I] -- How much to move the mark by (plus for toward the cursor,
 *                  neg to move toward the start of the buffer).
 *
 * FUNCTION:
 *    This function does the DPS_MoveMark() function to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_MoveMark()
 ******************************************************************************/
void DisplayBinary::MoveMark(t_DataProMark *Mark,int Amount)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;
    struct DisBin_PointPair P1;

    if(!Marker->Valid)
        return;

    P1.Line=Marker->Line;
    P1.Offset=Marker->Offset;
    AdvancePoint(&P1,Amount);
    Marker->Line=P1.Line;
    Marker->Offset=P1.Offset;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::GetMarkString
 *
 * SYNOPSIS:
 *    const uint8_t *DisplayBinary::GetMarkString(t_DataProMark *Mark,
 *              uint32_t *Size,uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Size [O] -- The number of bytes in the returned buffer.
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to copy the string.
 *    Len [I] -- The number of chars to copy from this string.
 *
 * FUNCTION:
 *    This function does the DPS_GetMarkString() function to the display.
 *
 * RETURNS:
 *    A pointer to a buffer with the string in it.
 *
 * SEE ALSO:
 *    DPS_GetMarkString()
 ******************************************************************************/
const uint8_t *DisplayBinary::GetMarkString(t_DataProMark *Mark,uint32_t *Size,
        uint32_t Offset,uint32_t Len)
{
    struct BinaryPointMarker *Marker=(struct BinaryPointMarker *)Mark;
    struct DisBin_PointPair P1;
    struct DisBin_PointPair P2;
    struct DisBin_Block Blocks[2];
    const uint8_t *StartPtr;
    const uint8_t *EndPtr;
    int s;
    uint32_t TotalNeededSize;
    uint8_t *InsertPos;

    if(!Marker->Valid)
    {
        *Size=0;
        return NULL;
    }

    P1.Line=Marker->Line;
    P1.Offset=Marker->Offset;
    AdvancePoint(&P1,Offset);

    if(Len==0)
    {
        P2.Line=BottomOfBufferLine;
        P2.Offset=InsertPoint;
    }
    else
    {
        P2.Line=P1.Line;
        P2.Offset=P1.Offset;
        AdvancePoint(&P2,Len);
    }

    GetNormalizedPoints(&P1,&P2,Blocks);

    /* Figure out how many bytes we will be returning */
    TotalNeededSize=0;
    for(s=0;s<2;s++)
    {
        if(Blocks[s].Start.Line==NULL || Blocks[s].End.Line==NULL)
            break;

        StartPtr=&Blocks[s].Start.Line[Blocks[s].Start.Offset];
        EndPtr=&Blocks[s].End.Line[Blocks[s].End.Offset];
        TotalNeededSize+=EndPtr-StartPtr;
    }
    TotalNeededSize+=1; // A spot for the \0 we add (the doc's say will will add it so text processors's life is a little easier)

    /* Resize the buffer as needed */
    if(MarkerGetStrBufferSize<TotalNeededSize)
    {
        /* We need to resize the buffer */
        free(MarkerGetStrBuffer);
        MarkerGetStrBuffer=(uint8_t *)malloc(TotalNeededSize);
        if(MarkerGetStrBuffer==NULL)
            return NULL;
    }

    /* Make the return "string" (really a byte buffer) */
    InsertPos=MarkerGetStrBuffer;
    for(s=0;s<2;s++)
    {
        if(Blocks[s].Start.Line==NULL || Blocks[s].End.Line==NULL)
            break;

        StartPtr=&Blocks[s].Start.Line[Blocks[s].Start.Offset];
        EndPtr=&Blocks[s].End.Line[Blocks[s].End.Offset];

        memcpy(InsertPos,StartPtr,EndPtr-StartPtr);
        InsertPos+=EndPtr-StartPtr;
    }
    *InsertPos++=0; // Add the forced \0

    *Size=TotalNeededSize-1;
    return MarkerGetStrBuffer;
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MoveViewTop
 *
 * SYNOPSIS:
 *    void DisplayBinary::MoveViewTop(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function move the display to the top of the scroll back buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::MoveViewTop(void)
{
    t_UIScrollBarCtrl *VertScroll;

    if(TextDisplayCtrl==NULL)
        return;

    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);

    /* Vert */
    UISetScrollBarPos(VertScroll,0);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MoveViewBottom
 *
 * SYNOPSIS:
 *    void DisplayBinary::MoveViewBottom(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function move the display to the bottom of the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::MoveViewBottom(void)
{
    t_UIScrollBarCtrl *VertScroll;
    int TotalLines;
    int MaxPos;

    if(TextDisplayCtrl==NULL)
        return;

    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);

    /* Vert */
    TotalLines=UIGetScrollBarTotalSize(VertScroll);
    MaxPos=TotalLines-DisplayLines;

    UISetScrollBarPos(VertScroll,MaxPos);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MoveViewUpDown
 *
 * SYNOPSIS:
 *    void DisplayBinary::MoveViewUpDown(int Delta);
 *
 * PARAMETERS:
 *    Delta [I] -- How mucht to move up (neg) or down (positive)
 *
 * FUNCTION:
 *    This function moves the display to up or down (scrolls)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::MoveViewUpDown(int Delta)
{
    ScrollScreen(0,Delta);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MoveViewLeftRight
 *
 * SYNOPSIS:
 *    void DisplayBinary::MoveViewLeftRight(int Delta);
 *
 * PARAMETERS:
 *    Delta [I] -- How much to move left (neg) or right (positive)
 *
 * FUNCTION:
 *    This function moves the display to left or right (scrolls)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::MoveViewLeftRight(int Delta)
{
    ScrollScreen(Delta*CharWidthPx,0);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MoveViewHome
 *
 * SYNOPSIS:
 *    void DisplayBinary::MoveViewHome(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the display to the start of the left side.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::MoveViewHome(void)
{
    ScrollScreen(-WindowXOffsetPx,0);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MoveViewEnd
 *
 * SYNOPSIS:
 *    void DisplayBinary::MoveViewEnd(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the display to the end of the right side.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::MoveViewEnd(void)
{
    ScrollScreen(ScreenHeightPx-WindowXOffsetPx,0);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MovePageUp
 *
 * SYNOPSIS:
 *    void DisplayBinary::MovePageUp(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the display 1 page up
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::MovePageUp(void)
{
    ScrollScreen(0,-ScreenHeightPx/CharHeightPx);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::MovePageDown
 *
 * SYNOPSIS:
 *    void DisplayBinary::MovePageDown(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the display 1 page down
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::MovePageDown(void)
{
    ScrollScreen(0,ScreenHeightPx/CharHeightPx);
}
