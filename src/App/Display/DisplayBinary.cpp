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
#include <string.h>

/*** DEFINES                  ***/
#define HEX_BYTES_PER_LINE              16
//#define HEX_MIN_LINES                   24
#define HEX_MIN_LINES                   3

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
bool DisplayBin_EventHandlerCB(const struct TextDisplayEvent *Event);

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

DisplayBinary::DisplayBinary()
{
    TextDisplayCtrl=NULL;
    HexBuffer=NULL;
    HexBufferSize=0;
    TopLine=NULL;
    ProLine=NULL;
    ConLine=NULL;
    EndOfHexBuffer=NULL;
    ScreenWidthPx=0;
    ScreenHeightPx=0;
    CharWidthPx=1;
    CharHeightPx=1;
}

bool DisplayBinary::Init(void *ParentWidget,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData)
{
    TextDisplayCtrl=NULL;
    try
    {
        if(!InitBase(EventCallback,UserData))
            throw(0);

        /* Allocate the text canvas */
        TextDisplayCtrl=UITC_AllocTextDisplay(ParentWidget,
                DisplayBinary_EventHandlerCB,(uintptr_t)this);
        if(TextDisplayCtrl==nullptr)
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
        ProLine=HexBuffer;
        InsertPoint=0;
        ConLine=HexBuffer;
        TopLine=ConLine;

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

        SetupCanvas();

        InitCalled=true;
    }
    catch(...)
    {
        if(HexBuffer!=NULL)
            free(HexBuffer);
        if(TextDisplayCtrl!=NULL)
        {
            UITC_FreeTextDisplay(TextDisplayCtrl);
            TextDisplayCtrl=NULL;
        }
        return false;
    }

    return true;
}

DisplayBinary::~DisplayBinary()
{
    InitCalled=false;

    if(HexBuffer!=NULL)
        free(HexBuffer);

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
    ProLine[InsertPoint++]=*Chr;
    RedrawCurrentLine();

    if(InsertPoint>=HEX_BYTES_PER_LINE)
    {
        InsertPoint=0;

        /* Shift the buffer */
        ProLine+=HEX_BYTES_PER_LINE;
        if(ProLine>=EndOfHexBuffer)
            ProLine=HexBuffer;

        if(ProLine==ConLine)
        {
            ConLine+=HEX_BYTES_PER_LINE;
            if(ConLine>=EndOfHexBuffer)
                ConLine=HexBuffer;
        }

        if(TopLine==ProLine)
        {
            /* Ok, we ran out of data, move topline too */
            TopLine=ProLine+HEX_BYTES_PER_LINE;
            if(TopLine>=EndOfHexBuffer)
                TopLine=HexBuffer;
            RedrawScreen();
        }
        RethinkScrollBars();
    }

    MakeCurrentLineVisble();
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

    if(!InitCalled)
        return false;

    switch(Event->EventType)
    {
        case e_TextDisplayEvent_DisplayFrameScrollX:
        break;
        case e_TextDisplayEvent_DisplayFrameScrollY:
            TopLine=ConLine+Event->Info.Scroll.Amount*HEX_BYTES_PER_LINE;
            if(TopLine>EndOfHexBuffer)
                TopLine=HexBuffer+(TopLine-EndOfHexBuffer);
/* DEBUG PAUL: handle wrap of buffer */
            RedrawScreen();
        break;
        case e_TextDisplayEvent_MouseDown:
        break;
        case e_TextDisplayEvent_MouseUp:
        break;
        case e_TextDisplayEvent_MouseRightDown:
        break;
        case e_TextDisplayEvent_MouseRightUp:
            RedrawScreen();
        break;
        case e_TextDisplayEvent_MouseMiddleDown:
        break;
        case e_TextDisplayEvent_MouseMiddleUp:
        break;
        case e_TextDisplayEvent_MouseWheel:
        break;
        case e_TextDisplayEvent_MouseMove:
        break;
        case e_TextDisplayEvent_Resize:
            ScreenResize();
            RethinkWindowSize();
            RethinkScrollBars();
            RedrawScreen();
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
    if(TextDisplayCtrl==nullptr)
        return;

    ScreenWidthPx=UITC_GetWidgetWidth(TextDisplayCtrl);
    ScreenHeightPx=UITC_GetWidgetHeight(TextDisplayCtrl);
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::RethinkScrollBars
 *
 * SYNOPSIS:
 *    void DisplayBinary::RethinkScrollBars(void);
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
void DisplayBinary::RethinkScrollBars(void)
{
    int TotalLines;
    int Bytes;
    int TopLineY;
    t_UIScrollBarCtrl *HorzScroll;
    t_UIScrollBarCtrl *VertScroll;

    if(TextDisplayCtrl==nullptr)
        return;

    if(ConLine>ProLine)
        Bytes=(EndOfHexBuffer-ConLine)+(ProLine-HexBuffer);
    else
        Bytes=ProLine-ConLine;
    TotalLines=Bytes/HEX_BYTES_PER_LINE;

    if(TopLine>ProLine)
        Bytes=(EndOfHexBuffer-TopLine)+(ProLine-HexBuffer);
    else
        Bytes=ProLine-TopLine;
    TopLineY=Bytes/HEX_BYTES_PER_LINE;

    /* Vert */
    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);

    UISetScrollBarPageSizeAndMax(VertScroll,ScreenHeightPx/CharHeightPx,
            TotalLines);
    UISetScrollBarPos(VertScroll,TopLineY);

//    int MaxLength;
//    int ScreenWidth;
//    int ScreenWidthPx;
//
//    if(TextDisplayCtrl==nullptr)
//        return;
//
//    /* Hozr */
//    HorzScroll=UITC_GetHorzSlider(TextDisplayCtrl);
//    ScreenWidthPx=ScreenWidthChars*CharWidthPx;
//
//    MaxLength=LongestLinePx;
//    if(MaxLength<ScreenWidthChars*CharWidthPx)
//        MaxLength=ScreenWidthChars*CharWidthPx;
//
//    ScreenWidth=TextAreaWidthPx;
//    if(Settings->TermSizeFixedWidth && TextAreaWidthPx>ScreenWidthPx)
//        ScreenWidth=ScreenWidthPx;
//
//    UISetScrollBarPageSizeAndMax(HorzScroll,ScreenWidth,MaxLength);
//
//    WindowXOffsetPx=UIGetScrollBarPos(HorzScroll);
//
//    UISetScrollBarStepSize(HorzScroll,CharWidthPx);
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
    /* Set the clipping and offset from the edge of the widget */
    if(TextDisplayCtrl!=nullptr)
    {
        UITC_SetClippingWindow(TextDisplayCtrl,0,0,ScreenWidthPx,
                ScreenHeightPx);
    }
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

    CharWidthPx=UITC_GetCharPxWidth(TextDisplayCtrl);
    CharHeightPx=UITC_GetCharPxHeight(TextDisplayCtrl);
    if(CharWidthPx<1)
        CharWidthPx=1;
    if(CharHeightPx<1)
        CharHeightPx=1;

    ScreenResize();
    RethinkWindowSize();
    RethinkScrollBars();
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
    int DisplayLines;
    const uint8_t *StartOfLine;
    int y;
    int Bytes2Draw;
    int Bytes;

    DisplayLines=ScreenHeightPx/CharHeightPx;

    if(TopLine>ProLine)
        Bytes=(EndOfHexBuffer-TopLine)+(ProLine-HexBuffer);
    else
        Bytes=ProLine-TopLine;
    y=Bytes/HEX_BYTES_PER_LINE;

    /* Don't do anything if the insert line is not on the screen */
    if(y>DisplayLines)
        return;

    DrawLine(ProLine,y,InsertPoint);
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
    int DisplayLines;
    const uint8_t *StartOfLine;
    int y;
    int Bytes2Draw;

    DisplayLines=ScreenHeightPx/CharHeightPx;
    StartOfLine=TopLine;
    for(y=0;y<DisplayLines;y++)
    {
        if(StartOfLine>=EndOfHexBuffer)
            StartOfLine=HexBuffer;
        Bytes2Draw=HEX_BYTES_PER_LINE;
        if(StartOfLine==ProLine)
            Bytes2Draw=InsertPoint;

        DrawLine(StartOfLine,y,Bytes2Draw);
        if(Bytes2Draw!=HEX_BYTES_PER_LINE)
            break;

        StartOfLine+=HEX_BYTES_PER_LINE;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::DrawLine
 *
 * SYNOPSIS:
 *    void DisplayBinary::DrawLine(const uint8_t *Line,int ScreenLine,int Bytes);
 *
 * PARAMETERS:
 *    Line [I] -- The start of the line to draw
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
void DisplayBinary::DrawLine(const uint8_t *Line,int ScreenLine,int Bytes)
{
    struct TextCanvasFrag DisplayFrag;
    char LineBuff[MAX_BINARY_HEX_BYTES_PER_LINE*3+3+MAX_BINARY_HEX_BYTES_PER_LINE+1];
    uint8_t c;
    int x;

struct CharStyling AltStyle;
AltStyle.FGColor=Settings->DefaultColors[e_DefaultColors_FG];
AltStyle.BGColor=0x0000FF;
AltStyle.Attribs=0;
AltStyle.ULineColor=CurrentStyle.FGColor;


    for(x=0;x<Bytes;x++)
    {
        c=Line[x];
        sprintf(&LineBuff[x*3],"%02X ",c);
        if(c<32 || c>127)
            c='.';
        LineBuff[HEX_BYTES_PER_LINE*3+3+x]=c;
    }
    memset(&LineBuff[x*3],' ',(HEX_BYTES_PER_LINE*3+3)-(x*3));
    memset(&LineBuff[HEX_BYTES_PER_LINE*3+3+x],' ',HEX_BYTES_PER_LINE-x);
    LineBuff[HEX_BYTES_PER_LINE*3+HEX_BYTES_PER_LINE+3]=0;

    DisplayFrag.FragType=e_TextCanvasFrag_String;
    DisplayFrag.Text=LineBuff;
    if(Line==HexBuffer)
        DisplayFrag.Styling=AltStyle;
    else
        DisplayFrag.Styling=CurrentStyle;
    DisplayFrag.Value=0;
    DisplayFrag.Data=0;

    UITC_Begin(TextDisplayCtrl,ScreenLine);
    UITC_ClearLine(TextDisplayCtrl,Settings->DefaultColors[e_DefaultColors_BG]);
    UITC_AddFragment(TextDisplayCtrl,&DisplayFrag);
    UITC_End(TextDisplayCtrl);
}

void DisplayBinary::MakeCurrentLineVisble(void)
{
}

