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
#include <stdint.h>
#include <string.h>

/*** DEFINES                  ***/
#define DEBUG_SHOW_BUFFER_POS               1       // Show the pointers to parts of the screen

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
    InitCalled=false;

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
        case e_TextDisplayEvent_MouseDown:
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
RethinkYScrollBar();
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
        case e_TextDisplayEvent_SendBttn:
            Info.BlockSend.Buffer=Event->Info.SendBttn.Buffer;
            Info.BlockSend.Len=Event->Info.SendBttn.Len;
            SendEvent(e_DBEvent_SendBlockData,&Info);
        break;
        case e_TextDisplayEvent_ContextMenu:
            Info.Context.Menu=(e_UITD_ContextMenuType)Event->Info.Context.Menu;
            SendEvent(e_DBEvent_ContextMenu,&Info);
        break;
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
    uint8_t *SelWindowStart[2];
    int SelWindowStartOffset[2];
    uint8_t *SelWindowEnd[2];
    int SelWindowEndOffset[2];
    unsigned int HighLightStart;
    unsigned int HighLightEnd;
    unsigned int HighLightMaxSize;
    unsigned int HighLightLineSize;

    /* Make sure the padding is all 0x00's */
    memset(ColorBuff,0x00,sizeof(ColorBuff));
    memset(&SpaceStyle,0x00,sizeof(SpaceStyle));

    SpaceStyle.FGColor=Settings->DefaultColors[e_DefaultColors_FG];
    SpaceStyle.BGColor=Settings->DefaultColors[e_DefaultColors_BG];
    SpaceStyle.Attribs=0;
    SpaceStyle.ULineColor=SpaceStyle.FGColor;

    /* Setup the selection */
    for(s=0;s<2;s++)
    {
        SelWindowStart[s]=NULL;
        SelWindowEnd[s]=NULL;
    }

    if(SelectionActive && SelectionLine!=NULL && SelectionAnchorLine!=NULL)
    {
        if(SelectionLine==SelectionAnchorLine)
        {
            /* They are on the same line */
            SelWindowStart[0]=SelectionLine;
            SelWindowEnd[0]=SelectionAnchorLine;
            if(SelectionLineAnchorOffset>SelectionLineOffset)
            {
                SelWindowStartOffset[0]=SelectionLineOffset;
                SelWindowEndOffset[0]=SelectionLineAnchorOffset;
            }
            else
            {
                SelWindowStartOffset[0]=SelectionLineAnchorOffset;
                SelWindowEndOffset[0]=SelectionLineOffset;
            }
        }
        else if((SelectionLine<TopOfBufferLine &&
                SelectionAnchorLine<TopOfBufferLine) ||
                (SelectionLine>=TopOfBufferLine &&
                SelectionAnchorLine>=TopOfBufferLine))
        {
            /* Both are above or below the start of the data, just setup one */
            if(SelectionAnchorLine>SelectionLine)
            {
                SelWindowStart[0]=SelectionLine;
                SelWindowStartOffset[0]=SelectionLineOffset;
                SelWindowEnd[0]=SelectionAnchorLine;
                SelWindowEndOffset[0]=SelectionLineAnchorOffset;
            }
            else
            {
                SelWindowStart[0]=SelectionAnchorLine;
                SelWindowStartOffset[0]=SelectionLineAnchorOffset;
                SelWindowEnd[0]=SelectionLine;
                SelWindowEndOffset[0]=SelectionLineOffset;
            }
        }
        else
        {
            /* One is above and one below, we need to highlight blocks */
            if(SelectionLine>TopOfBufferLine)
            {
                SelWindowStart[0]=SelectionLine;
                SelWindowStartOffset[0]=SelectionLineOffset;
                SelWindowEnd[0]=EndOfHexBuffer;
                SelWindowEndOffset[0]=0;

                SelWindowStart[1]=HexBuffer;
                SelWindowStartOffset[1]=0;
                SelWindowEnd[1]=SelectionAnchorLine;
                SelWindowEndOffset[1]=SelectionLineAnchorOffset;
            }
            else
            {
                SelWindowStart[0]=SelectionAnchorLine;
                SelWindowStartOffset[0]=SelectionLineAnchorOffset;
                SelWindowEnd[0]=EndOfHexBuffer;
                SelWindowEndOffset[0]=0;

                SelWindowStart[1]=HexBuffer;
                SelWindowStartOffset[1]=0;
                SelWindowEnd[1]=SelectionLine;
                SelWindowEndOffset[1]=SelectionLineOffset;
            }
        }
    }

    for(x=0;x<Bytes;x++)
    {
        c=Line[x];

        sprintf(&LineBuff[x*3],"%02X ",c);
        /* If you change HEX_SPACE_BETWEEN_HEX_AND_ASCII you need to update
           this as well */
        tmp=x*3+0;
        ColorBuff[tmp++]=ColorLine[x];
        ColorBuff[tmp++]=ColorLine[x];
        ColorBuff[tmp++]=ColorLine[x];

        if(c<32 || c>127)
            c='.';

        LineBuff[START_OF_ASCII_CHAR+x]=c;
        ColorBuff[START_OF_ASCII_CHAR+x]=ColorLine[x];
    }
    /* x=Bytes at this point */
    memset(&LineBuff[x*3],' ',START_OF_ASCII_CHAR-(x*3));
    for(r=0;r<START_OF_ASCII_CHAR-(x*3);r++)
        ColorBuff[x*3+r]=SpaceStyle;

    memset(&LineBuff[START_OF_ASCII_CHAR+x],' ',HEX_BYTES_PER_LINE-x);
    for(r=0;r<HEX_BYTES_PER_LINE-x;r++)
        ColorBuff[START_OF_ASCII_CHAR+x+r]=SpaceStyle;

    LineBuff[END_OF_ASCII_CHAR]=0;

    /* Handle selection */
    for(s=0;s<2;s++)
    {
        if(Line>=SelWindowStart[s] && Line<=SelWindowEnd[s])
        {
            if(SelectionInAscII)
            {
                Offset=START_OF_ASCII_CHAR;
                HighLightStart=SelWindowStartOffset[s];
                HighLightEnd=SelWindowEndOffset[s]+1;
                HighLightMaxSize=HEX_BYTES_PER_LINE;
                HighLightLineSize=SelWindowEndOffset[s]-SelWindowStartOffset[s]+1;
            }
            else
            {
                /* times 3 because we use 3 chars per hex value, and -1 because
                   we don't highlight the last char */
                Offset=0;
                HighLightStart=SelWindowStartOffset[s]*3;
                HighLightEnd=(SelWindowEndOffset[s]+1)*3-1;
                HighLightMaxSize=HEX_BYTES_PER_LINE*3-1;
                HighLightLineSize=(SelWindowEndOffset[s]-
                        SelWindowStartOffset[s]+1)*3-1;
            }

            if(Line==SelWindowStart[s] && Line==SelWindowEnd[s])
            {
                for(r=0;r<HighLightLineSize;r++)
                {
                    ColorBuff[Offset+HighLightStart+r].Attribs=
                            TXT_ATTRIB_REVERSE;
                }
            }
            else if(Line==SelWindowStart[s])
            {
                for(r=HighLightStart;r<HighLightMaxSize;r++)
                    ColorBuff[Offset+r].Attribs=TXT_ATTRIB_REVERSE;
            }
            else if(Line==SelWindowEnd[s])
            {
                for(r=0;r<HighLightEnd;r++)
                    ColorBuff[Offset+r].Attribs=TXT_ATTRIB_REVERSE;
            }
            else
            {
                for(r=0;r<HighLightMaxSize;r++)
                    ColorBuff[Offset+r].Attribs=TXT_ATTRIB_REVERSE;
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

    /* Check if we are clicking a "dead" space */
    if(x<0 || (x>=END_OF_HEX_PX && x<START_OF_ASCII_PX) || x>=END_OF_ASCII_PX)
        return false;
    if(y<0)
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
