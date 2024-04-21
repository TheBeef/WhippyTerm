/*******************************************************************************
 * FILENAME: HexDisplayBuffers.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the hex display / edit widget in it.  It connects to
 *    a container frame handle.
 *
 *    This is like the display buffer but is for hex display / edit.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (15 May 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Display/HexDisplayBuffers.h"
#include "App/Settings.h"
#include <string.h>
#include <stdlib.h>

//#include "UI/UIDebug.h"

/*** DEFINES                  ***/
#define BYTESPERLINE                    16  // Number bytes per line to display
#define HEX_DIG_SIZE                    3
#define SPACE_BETWEEN_HEX_AND_ASCII     3
#define MAX_DISPLAY_COLUMNS             (BYTESPERLINE*HEX_DIG_SIZE+SPACE_BETWEEN_HEX_AND_ASCII+BYTESPERLINE)+1     // The max number of columns we can draw per line
#define ASCIILEFTEDGE                   (BYTESPERLINE*HEX_DIG_SIZE+SPACE_BETWEEN_HEX_AND_ASCII)

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool HexDisplayBuffer_EventHandler(const struct TextDisplayEvent *Event);

/*** VARIABLE DEFINITIONS     ***/
HexDisplayBuffer::HexDisplayBuffer()
{
    TextDisplayCtrl=nullptr;

    WeAllocBuffer=false;

    FirstResize=true;
    ResizeNeeded=true;
    HasFocus=false;
    HideSelectionOnLossOfFocus=true;
    LeftMouseDown=false;

    Buffer=NULL;
    StartOfData=NULL;
    InsertPos=NULL;
    BufferIsCircular=false;
    BufferSize=0;
    BufferAllocatedSize=0;

    FGColor=0xFFFFFF;
    BGColor=0x000000;
    SelBGColor=0x0000FF;
    FontName="";
    FontSize=11;
    FontBold=false;
    FontItalic=false;

    DisplayEnabled=true;
    LastTotalLines=0;
    LastView_CharsY=0;
    TopLine=0;

    View_WidthPx=0;
    View_HeightPx=0;

    View_CharsX=0;
    View_CharsY=0;
    BufferBytes2Draw=0;

    WindowXOffsetPx=0;

    Selection_OrgAnchor=0;
    Selection_Anchor=0;
    Cursor_Pos=0;
    Last_Cursor_Pos=0;
    InAscIIArea=false;
    SelectionMouseDown=false;
    SelectionValid=false;
    EditMode=e_HDB_EditState_FirstNib;

    InEditMode=false;
    InInsertMode=true;

    DoingDotInputChar=false;
    DoingCycleInputChar=false;
    NibCycleValue=0;
}

HexDisplayBuffer::~HexDisplayBuffer()
{
    if(WeAllocBuffer)
        free(Buffer);

    if(TextDisplayCtrl!=nullptr)
        UITC_FreeTextDisplay(TextDisplayCtrl);
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::Init
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::Init(void *ParentWidget);
 *
 * PARAMETERS:
 *    ParentWidget [I] -- The frame this hex display will live in
 *
 * FUNCTION:
 *    This function init's the hex display and connects it to the display
 *    frame it will live inside of.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::Init(void *ParentWidget,
        bool (*EventHandler)(const struct HDEvent *Event),uintptr_t ID)
{
    try
    {
        HDID=ID;
        HDEventHandler=EventHandler;

        TextDisplayCtrl=UITC_AllocTextDisplay(ParentWidget,
                HexDisplayBuffer_EventHandler,(uintptr_t)this);
        if(TextDisplayCtrl==nullptr)
            throw(0);

        SetupCanvas();

        ClearSelection();
    }
    catch(...)
    {
        /* Failed to malloc memory.  Bad because we just got started */
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer_EventHandler
 *
 * SYNOPSIS:
 *    static bool HexDisplayBuffer_EventHandler(
 *              const struct TextDisplayEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event for the canvas.  The ID is used to get the
 *    hex display buffer class that this event goes with.
 *
 * FUNCTION:
 *    This function takes the ID from the event and uses it as the
 *    'HexDisplayBuffer' to pass this event off to.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    HexDisplayBuffer::EventHandler()
 ******************************************************************************/
static bool HexDisplayBuffer_EventHandler(const struct TextDisplayEvent *Event)
{
    class HexDisplayBuffer *HexBuffer;

    HexBuffer=(class HexDisplayBuffer *)Event->ID;

    if(HexBuffer==NULL)
        return false;

    return HexBuffer->EventHandler(Event);
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::EventHandler
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::EventHandler(const struct TextDisplayEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The canvas event to act on
 *
 * FUNCTION:
 *    This function is the event handler for the text canvas that is connected
 *    to this hex display buffer.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::EventHandler(const struct TextDisplayEvent *Event)
{
    bool AcceptEvent;
    struct HDEvent HDEvent;

    AcceptEvent=true;
    switch(Event->EventType)
    {
        case e_TextDisplayEvent_DisplayFrameScrollX:
            WindowXOffsetPx=Event->Info.Scroll.Amount;

            if(TextDisplayCtrl!=nullptr)
            {
                UITC_SetXOffset(TextDisplayCtrl,WindowXOffsetPx);
                RebuildDisplay();
            }
        break;
        case e_TextDisplayEvent_DisplayFrameScrollY:
            TopLine=Event->Info.Scroll.Amount;
            RebuildDisplay();
        break;
        case e_TextDisplayEvent_MouseDown:
            HandleSelectionMouseUpDown(true,Event->Info.Mouse.x,
                    Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_MouseUp:
            HandleSelectionMouseUpDown(false,Event->Info.Mouse.x,
                    Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_MouseMiddleDown:
        break;
        case e_TextDisplayEvent_MouseMiddleUp:
        break;
        case e_TextDisplayEvent_MouseWheel:
            MouseWheelMoved(Event->Info.MouseWheel.Steps,
                    Event->Info.MouseWheel.Mods);
        break;
        case e_TextDisplayEvent_MouseMove:
            HandleSelectionMouseUpDownMove(Event->Info.Mouse.x,
                    Event->Info.Mouse.y);

            if(HDEventHandler!=NULL)
            {
                HDEvent.EventType=e_HDEvent_MouseMove;
                HDEvent.ID=HDID;

                HDEventHandler(&HDEvent);
            }
        break;
        case e_TextDisplayEvent_Resize:
            SetCanvasSize(Event->Info.NewSize.Width,Event->Info.NewSize.
                    Height);
        break;
        case e_TextDisplayEvent_KeyEvent:
            AcceptEvent=KeyPress(Event->Info.Key.Mods,Event->Info.Key.Key,
                    Event->Info.Key.TextPtr,Event->Info.Key.TextLen);
        break;
        case e_TextDisplayEvent_LostFocus:
            HasFocus=false;
            RebuildDisplay();
            UITC_SetCursorStyle(TextDisplayCtrl,e_TextCursorStyle_Hidden);
        break;
        case e_TextDisplayEvent_GotFocus:
            HasFocus=true;
            RebuildDisplay();
            RethinkCursorLook();
        break;

        case e_TextDisplayEvent_MouseRightDown:
        case e_TextDisplayEvent_MouseRightUp:
        case e_TextDisplayEvent_SendBttn:
        case e_TextDisplayEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetBuffer
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::SetBuffer(const uint8_t *Data,int Size);
 *    bool HexDisplayBuffer::SetBuffer(uint8_t *Data,int Size);
 *
 * PARAMETERS:
 *    Data [I] -- The pointer to the buffer to display.  Only the pointer
 *                is copied so this buffer must be static.
 *    Size [I] -- The number of bytes to in the buffer
 *
 * FUNCTION:
 *    This function sets the buffer to display the byte from.  If we are in
 *    edit mode then a copy of the 'Data' will be made.
 *
 * RETURNS:
 *    true -- We are now working with this buffer.
 *    false -- There was an error.  Most likely out of memory.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::SetBuffer(uint8_t *Data,int Size)
{
    if(InEditMode)
    {
        if(WeAllocBuffer)
            free(Buffer);
        Buffer=(uint8_t *)malloc(Size);
        if(Buffer==NULL)
            return false;
        BufferAllocatedSize=Size;

        memcpy(Buffer,Data,Size);

        WeAllocBuffer=true;
    }
    else
    {
        Buffer=Data;
    }

    SetNewBufferSize(Size);
    InsertPos=Buffer+Size;
    StartOfData=Buffer;
    BufferIsCircular=false;

    ClearSelection();

    return false;
}
bool HexDisplayBuffer::SetBuffer(const uint8_t *Data,int Size)
{
    return SetBuffer((uint8_t *)Data,Size);
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetBufferSize
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SetBufferSize(int Size);
 *
 * PARAMETERS:
 *    Size [I] -- The size in bytes of the buffer.
 *
 * FUNCTION:
 *    This function will change the size of the buffer.  It can expand the
 *    size or shrink it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::SetBufferSize(int Size)
{
    int SavedBufferSize;

    SavedBufferSize=BufferSize;

    if(!GrowBufferAsNeeded(Size))
        return;
    if(Size>SavedBufferSize)
    {
        /* Ok, fill with 0 */
        memset(&Buffer[SavedBufferSize],0x00,Size-SavedBufferSize);
    }

    SetNewBufferSize(Size);
    StartOfData=Buffer;
    InsertPos=Buffer+BufferSize;
    Cursor_Pos=0;
    ClearSelection();
    MakeOffsetVisable(Cursor_Pos,InAscIIArea,false);
    RebuildDisplay();
    RethinkCursorPos();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetCanvasSize
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SetCanvasSize(int Width,int Height);
 *
 * PARAMETERS:
 *    Width [I] -- The new width (in pixels) of the display.
 *    Height [I] -- The new height (in pixels) of the display.
 *
 * FUNCTION:
 *    This function is called when the text canvas is resized.
 *
 *    It recal's the number of lines visble and the width.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::SetCanvasSize(int Width,int Height)
{
    View_WidthPx=Width;
    View_HeightPx=Height;

    if(TextDisplayCtrl==nullptr)
        return;

    CharWidthPx=UITC_GetCharPxWidth(TextDisplayCtrl);
    CharHeightPx=UITC_GetCharPxHeight(TextDisplayCtrl);

    if(CharWidthPx==0 || CharHeightPx==0)
    {
        /* We have a Hu? */
        View_CharsX=0;
        View_CharsY=0;
        return;
    }

    View_CharsX=View_WidthPx/CharWidthPx;
    View_CharsY=View_HeightPx/CharHeightPx;

    UITC_SetClippingWindow(TextDisplayCtrl,0,0,View_WidthPx,View_HeightPx);
    RebuildDisplay();
    RethinkYScrollBar();
    RethinkXScrollBar();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::RebuildDisplay
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::RebuildDisplay(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rebuilds the screen lines based on the top byte and how
 *    many lines are available to be displayed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::RebuildDisplay(void)
{
    int l;
    char Line[MAX_DISPLAY_COLUMNS];
    struct CharStyling LineStyling[MAX_DISPLAY_COLUMNS];
    uint8_t *CurPos;
    uint8_t *EndOfBuffPos;
    unsigned char c;
    int r;
    int g;
    int b;
    bool FirstUseOfStyle;
    struct TextCanvasFrag DisplayFrag;
    struct TextCanvasFrag StyledFrag;
    int SelectionStart;
    int SelectionEnd;
    uint32_t CursorPosColor;
    int BytesDrawen;
    int StartOfLine;
    bool UseStyle;
    unsigned int i;
    unsigned int e;

    if(TextDisplayCtrl==NULL)
        return;

    UITC_ClearAllLines(TextDisplayCtrl);

    if(BufferBytes2Draw==0 && Buffer==NULL && !DisplayEnabled)
    {
        UITC_RedrawScreen(TextDisplayCtrl);
        return;
    }

    /* Make the cursor char brighter */
    CursorPosColor=FGColor;
    r=(CursorPosColor>>16)&0xFF;
    g=(CursorPosColor>>8)&0xFF;
    b=(CursorPosColor>>0)&0xFF;
    r=r+128;
    if(r>255)
        r=255;
    g=g+128;
    if(g>255)
        g=255;
    b=b+128;
    if(b>255)
        b=255;
    CursorPosColor=r<<16|g<<8|b;

    /* Build the display lines */
    CurPos=StartOfData;
    EndOfBuffPos=Buffer+BufferSize;

    /* Skip until we get to the top line */
    CurPos+=TopLine*BYTESPERLINE;
    if(CurPos>EndOfBuffPos)
    {
        /* Wrapped */
        CurPos-=BufferSize;
    }

    BytesDrawen=TopLine*BYTESPERLINE;

    if(Cursor_Pos<Selection_Anchor)
    {
        SelectionStart=Cursor_Pos;
        SelectionEnd=Selection_Anchor;
    }
    else
    {
        SelectionStart=Selection_Anchor;
        SelectionEnd=Cursor_Pos;
    }

    DisplayFrag.FragType=e_TextCanvasFrag_String;
    DisplayFrag.Text=Line;
    DisplayFrag.Styling.FGColor=FGColor;
    DisplayFrag.Styling.BGColor=BGColor;
    DisplayFrag.Styling.ULineColor=FGColor;
    DisplayFrag.Styling.Attribs=0;
    DisplayFrag.Value=0;
    DisplayFrag.Data=0;

    StyledFrag=DisplayFrag;

    memset(Line,' ',sizeof(Line));
    Line[sizeof(Line)-1]=0;
    for(i=0;i<sizeof(LineStyling)/sizeof(struct CharStyling);i++)
    {
        LineStyling[i].FGColor=FGColor;
        LineStyling[i].BGColor=BGColor;
        LineStyling[i].ULineColor=FGColor;
        LineStyling[i].Attribs=0;
    }

    for(l=0;l<View_CharsY && BytesDrawen<BufferBytes2Draw;l++)
    {
        UITC_Begin(TextDisplayCtrl,l);
        UITC_ClearLine(TextDisplayCtrl,BGColor);

        StartOfLine=BytesDrawen;
        FirstUseOfStyle=true;
        for(i=0;i<BYTESPERLINE && BytesDrawen<BufferBytes2Draw;i++)
        {
            c=*CurPos;
            sprintf(&Line[i*HEX_DIG_SIZE],"%02X ",c);
            if(c<32 || c>126)
                c='.';
            Line[ASCIILEFTEDGE+i]=c;

            /* Move to the next char */
            CurPos++;
            if(CurPos>=EndOfBuffPos)
                CurPos=Buffer;

            BytesDrawen++;
        }

        /* Finish off any half lines */
        if(i!=BYTESPERLINE)
        {
            memset(&Line[i*HEX_DIG_SIZE],' ',(BYTESPERLINE*HEX_DIG_SIZE-i*HEX_DIG_SIZE+1));
            memset(&Line[ASCIILEFTEDGE+i],' ',BYTESPERLINE-i);
        }
        else
        {
            /* Fix the \0 that the sprintf() added to the last hex value */
            Line[BYTESPERLINE*HEX_DIG_SIZE]=' ';
        }

        if(InEditMode)
        {
            /* Is the cursor on this line? */
            if(Cursor_Pos>=StartOfLine && Cursor_Pos<StartOfLine+BYTESPERLINE)
            {
                i=(Cursor_Pos-StartOfLine)*HEX_DIG_SIZE;
                switch(EditMode)
                {
                    case e_HDB_EditStateMAX:
                    case e_HDB_EditState_FirstNib:
                        if(DoingCycleInputChar)
                            sprintf(&Line[i],"%01X ",EditByteValue>>4);
                    break;
                    case e_HDB_EditState_SecondNib:
                        if(DoingCycleInputChar)
                            sprintf(&Line[i],"%02X",EditByteValue);
                        else
                            sprintf(&Line[i],"%01X ",EditByteValue>>4);
                    break;
                    default:
                    break;
                }
                Line[i+2]=' ';  // Restore the space (that was trashed by a \0)
            }
        }

        /* Add styling to this line */
        UseStyle=false;

        if(SelectionValid)
        {
            if(SelectionStart>=StartOfLine &&
                    SelectionStart<StartOfLine+BYTESPERLINE)
            {
                /* Start of selection on this line */
                if(FirstUseOfStyle)
                {
                    RebuildDisplay_ClearStyleHelper(LineStyling);
                    FirstUseOfStyle=false;
                }
                e=BYTESPERLINE-1;
                if(SelectionEnd<StartOfLine+BYTESPERLINE)
                    e=SelectionEnd-StartOfLine;
                for(i=SelectionStart-StartOfLine;i<=e;i++)
                {
                    for(r=0;r<HEX_DIG_SIZE;r++)
                        LineStyling[i*HEX_DIG_SIZE+r].BGColor=SelBGColor;
                    LineStyling[ASCIILEFTEDGE+i].BGColor=SelBGColor;
                }
                /* Don't style the padding between hex and AscII */
                LineStyling[i*HEX_DIG_SIZE-1].BGColor=BGColor;
                UseStyle=true;
            }
            else if(SelectionEnd>=StartOfLine &&
                    SelectionEnd<StartOfLine+BYTESPERLINE)
            {
                if(FirstUseOfStyle)
                {
                    RebuildDisplay_ClearStyleHelper(LineStyling);
                    FirstUseOfStyle=false;
                }
                e=SelectionEnd-StartOfLine;
                for(i=0;i<=e;i++)
                {
                    for(r=0;r<HEX_DIG_SIZE;r++)
                        LineStyling[i*HEX_DIG_SIZE+r].BGColor=SelBGColor;
                    LineStyling[ASCIILEFTEDGE+i].BGColor=SelBGColor;
                }
                /* Don't style the padding between hex and AscII */
                LineStyling[i*HEX_DIG_SIZE-1].BGColor=BGColor;
                UseStyle=true;
            }
            else if(StartOfLine>=SelectionStart && StartOfLine<=SelectionEnd)
            {
                if(FirstUseOfStyle)
                {
                    RebuildDisplay_ClearStyleHelper(LineStyling);
                    FirstUseOfStyle=false;
                }

                for(i=0;i<BYTESPERLINE*HEX_DIG_SIZE-1;i++)
                    LineStyling[i].BGColor=SelBGColor;

                for(i=0;i<BYTESPERLINE;i++)
                    LineStyling[ASCIILEFTEDGE+i].BGColor=SelBGColor;

                UseStyle=true;
            }
        }

        if(InEditMode)
        {
            /* Is the cursor on this line? */
            if(Cursor_Pos>=StartOfLine && Cursor_Pos<StartOfLine+BYTESPERLINE)
            {
                if(FirstUseOfStyle)
                {
                    RebuildDisplay_ClearStyleHelper(LineStyling);
                    FirstUseOfStyle=false;
                }

                i=Cursor_Pos-StartOfLine;
                for(r=0;r<2;r++)
                {
                    LineStyling[i*HEX_DIG_SIZE+r].FGColor=CursorPosColor;
                    LineStyling[i*HEX_DIG_SIZE+r].ULineColor=CursorPosColor;
                    LineStyling[i*HEX_DIG_SIZE+r].Attribs|=TXT_ATTRIB_UNDERLINE;
                }

                LineStyling[ASCIILEFTEDGE+i].FGColor=CursorPosColor;
                LineStyling[ASCIILEFTEDGE+i].ULineColor=CursorPosColor;
                LineStyling[ASCIILEFTEDGE+i].Attribs|=TXT_ATTRIB_UNDERLINE;

                UseStyle=true;
            }
        }

        if(UseStyle)
        {
            /* Ok, we are using some style, break the string up into
               fragments */
            StyledFrag.Styling=LineStyling[0];
            StyledFrag.Text=Line;
            for(i=1;i<ASCIILEFTEDGE+BYTESPERLINE;i++)
            {
                if(!CmpCharStyle(&StyledFrag.Styling,&LineStyling[i]))
                {
                    /* They are not the same add this frag */
                    c=Line[i];  // The the char at the end of the string
                    Line[i]=0;
                    UITC_AddFragment(TextDisplayCtrl,&StyledFrag);
                    Line[i]=c;  // Restore the saved char
                    StyledFrag.Text=&Line[i];
                    StyledFrag.Styling=LineStyling[i];
                }
            }

            /* Add the last frag */
            UITC_AddFragment(TextDisplayCtrl,&StyledFrag);
        }
        else
        {
            UITC_AddFragment(TextDisplayCtrl,&DisplayFrag);
        }

        UITC_End(TextDisplayCtrl);
    }

    /* We have an edge case where if we are 1 pass the end of the data and
       we have an nothing but full lines then we wouldn't draw the last line
       with a blank and the underline, so we handle that here */
    if(InEditMode && BufferBytes2Draw%BYTESPERLINE==0)
    {
        /* Is the cursor past the end of the data */
        if(Cursor_Pos>=BufferBytes2Draw)
        {
            UITC_Begin(TextDisplayCtrl,l);
            UITC_ClearLine(TextDisplayCtrl,BGColor);

            /* Output a caned __                _ */
            memset(Line,' ',sizeof(Line));
            Line[sizeof(Line)-1]=0;
            StyledFrag.Styling=DisplayFrag.Styling;
            StyledFrag.Text=Line;

            /* The hex __ */
            Line[2]=0;
            StyledFrag.Styling.Attribs=TXT_ATTRIB_UNDERLINE;
            UITC_AddFragment(TextDisplayCtrl,&StyledFrag);

            /* The blank between hex and AscII */
            Line[2]=' ';
            Line[BYTESPERLINE*HEX_DIG_SIZE+SPACE_BETWEEN_HEX_AND_ASCII-2]=0;
            StyledFrag.Styling.Attribs=0;
            UITC_AddFragment(TextDisplayCtrl,&StyledFrag);

            /* The AscII _ */
            Line[1]=0;
            StyledFrag.Styling.Attribs=TXT_ATTRIB_UNDERLINE;
            UITC_AddFragment(TextDisplayCtrl,&StyledFrag);

            UITC_End(TextDisplayCtrl);
        }
    }

    UITC_RedrawScreen(TextDisplayCtrl);
}

void HexDisplayBuffer::RebuildDisplay_ClearStyleHelper(struct CharStyling *style)
{
    int b;

    /* Clear the styling for this line */
    for(b=0;b<BYTESPERLINE*HEX_DIG_SIZE;b++)
    {
        style[b].FGColor=FGColor;
        style[b].BGColor=BGColor;
        style[b].ULineColor=FGColor;
        style[b].Attribs=0;
    }
    for(b=0;b<BYTESPERLINE;b++)
    {
        style[b+ASCIILEFTEDGE].FGColor=FGColor;
        style[b+ASCIILEFTEDGE].BGColor=BGColor;
        style[b+ASCIILEFTEDGE].ULineColor=FGColor;
        style[b+ASCIILEFTEDGE].Attribs=0;
    }
}

void HexDisplayBuffer::ApplySettingsChange(void)
{
}

void HexDisplayBuffer::SetupCanvas(void)
{
    UITC_SetFont(TextDisplayCtrl,FontName.c_str(),FontSize,FontBold,FontItalic);

    UITC_SetCursorColor(TextDisplayCtrl,0xFFFFFF);

    UITC_SetTextAreaBackgroundColor(TextDisplayCtrl,BGColor);

    UITC_ShowSendPanel(TextDisplayCtrl,false);

    RethinkCursorLook();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::RethinkCursorLook
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::RethinkCursorLook(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function changes what the cursor looks like depending on the current
 *    mode.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::RethinkCursorLook(void)
{
    e_TextCursorStyleType CursorStyle;

    CursorStyle=e_TextCursorStyle_Hidden;

    if(InEditMode)
    {
        if(DoingDotInputChar)
            CursorStyle=e_TextCursorStyle_Box;
        else if(InInsertMode)
            CursorStyle=e_TextCursorStyle_Line;
        else
            CursorStyle=e_TextCursorStyle_Block;
    }

    UITC_SetCursorStyle(TextDisplayCtrl,CursorStyle);
}

void HexDisplayBuffer::SetFont(const char *NewFontName,int NewSize,bool NewBold,
        bool NewItalic)
{
    FontName=NewFontName;
    FontSize=NewSize;
    FontBold=NewBold;
    FontItalic=NewItalic;

    if(TextDisplayCtrl==NULL)
        return;

    UITC_SetFont(TextDisplayCtrl,FontName.c_str(),FontSize,FontBold,FontItalic);

    RethinkYScrollBar();
    RethinkXScrollBar();
}

void HexDisplayBuffer::SetColors(uint32_t NewFGColor,uint32_t NewBGColor,
        uint32_t NewSelBGColor)
{
    FGColor=NewFGColor;
    BGColor=NewBGColor;
    SelBGColor=NewSelBGColor;

    SetupCanvas();
    RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetDisplayParms
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SetDisplayParms(const uint8_t *NewInsertPos,
 *          bool NewBufferIsCircular);
 *    void HexDisplayBuffer::SetDisplayParms(uint8_t *NewInsertPos,
 *          bool NewBufferIsCircular);
 *
 * PARAMETERS:
 *    NewInsertPos [I] -- A pointer into the buffer that has the last byte of
 *                        data to display in it.  This is used to only display
 *                        some of the data in the buffer.  If you pass NULL then
 *                        the whole buffer is used.
 *    NewBufferIsCircular [I] -- If this buffer is circular (data wraps at the
 *                               bottom to the top) then set this is to 'true'.
 *                               If it's just a normal buffer set this to
 *                               'false'.
 *
 * FUNCTION:
 *    This function sets how we will extract data from the memory buffer.
 *    The system supports circular and linear.
 *
 *    If the buffer is editable then this does nothing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::SetDisplayParms(uint8_t *NewInsertPos,
        bool NewBufferIsCircular)
{
    int OffsetFromStart;
    int OffsetFromStartRounded;
    uint8_t *OldStartOfData;
    int Delta;
    bool SendEvent;

    if(InEditMode)
        return;

    if(NewInsertPos==NULL)
        InsertPos=Buffer+BufferSize;
    else
        InsertPos=NewInsertPos;
    BufferIsCircular=NewBufferIsCircular;

    /* Always move start of data to be on the start of a line */
    if(BufferIsCircular)
    {
        OffsetFromStart=NewInsertPos-Buffer;

        /* Round to the start of the next line */
        OffsetFromStartRounded=((OffsetFromStart+BYTESPERLINE-1)/BYTESPERLINE)*
                BYTESPERLINE;

        BufferBytes2Draw=BufferSize-(OffsetFromStartRounded-OffsetFromStart);

        if(OffsetFromStartRounded>=BufferSize)
            OffsetFromStartRounded=0;
    }
    else
    {
        BufferBytes2Draw=NewInsertPos-Buffer;
        OffsetFromStartRounded=0;
    }

    OldStartOfData=StartOfData;
    StartOfData=Buffer+OffsetFromStartRounded;

    if(OldStartOfData!=NULL && OldStartOfData!=StartOfData)
    {
        /* Ok, the start of data moved, we need to move the selection by the
           amount of the change */
        if(Selection_Anchor!=Cursor_Pos)
        {
            if(OldStartOfData>StartOfData)
            {
                /* Ok, we wrapped back to the start */
                Delta=StartOfData-(OldStartOfData-BufferSize);
            }
            else
            {
                Delta=StartOfData-OldStartOfData;
            }

            Selection_OrgAnchor-=Delta;
            Selection_Anchor-=Delta;
            Cursor_Pos-=Delta;

            SendEvent=false;

            if(Selection_Anchor<0)
            {
                Selection_Anchor=0;
                SendEvent=true;
            }
            if(Cursor_Pos<0)
            {
                Cursor_Pos=0;
                SendEvent=true;
            }
            if(Selection_OrgAnchor<0)
            {
                Selection_OrgAnchor=0;
                SendEvent=true;
            }

            if(SendEvent)
                SendSelectionEvent();
        }
    }

    RethinkYScrollBar();
}
void HexDisplayBuffer::SetDisplayParms(const uint8_t *NewInsertPos,
        bool NewBufferIsCircular)
{
    SetDisplayParms((uint8_t *)NewInsertPos,NewBufferIsCircular);
}

void HexDisplayBuffer::Enable(bool Enable)
{
    DisplayEnabled=Enable;
    RebuildDisplay();
}

void HexDisplayBuffer::RethinkYScrollBar(void)
{
    t_UIScrollBarCtrl *ScrollY;
    int TotalLines;

    if(TextDisplayCtrl==NULL)
        return;

    /* We do a messed up cal'c here so we end up being off by 1 when we have
       full lines (if we have 15 bytes then we want to say there are 1 line,
       if we have 16 bytes then we want to say there are 2 lines, and if
       we have 17 bytes then we want to say there are 2 lines) */
    TotalLines=(BufferBytes2Draw+BYTESPERLINE)/BYTESPERLINE;

    if(TotalLines!=LastTotalLines || View_CharsY!=LastView_CharsY)
    {
        LastTotalLines=TotalLines;
        LastView_CharsY=View_CharsY;

        ScrollY=UITC_GetVertSlider(TextDisplayCtrl);

        if(View_CharsY==0)
            UISetScrollBarPageSizeAndMax(ScrollY,0,0);
        else
            UISetScrollBarPageSizeAndMax(ScrollY,View_CharsY,TotalLines);
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::RethinkXScrollBar
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::RethinkXScrollBar(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the X scroll bar size and page size.  It takes
 *    the size of the font, window size, and other things into account.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    RethinkYScrollBar()
 ******************************************************************************/
void HexDisplayBuffer::RethinkXScrollBar(void)
{
    t_UIScrollBarCtrl *ScrollX;
    int TotalWidth;

    TotalWidth=BYTESPERLINE*HEX_DIG_SIZE;  // BYTESPERLINE's bytes * 2 for the hex and 1 for the space
    TotalWidth+=SPACE_BETWEEN_HEX_AND_ASCII;    // spaces between hex and ASCII preview
    TotalWidth+=BYTESPERLINE;   // Add the BYTESPERLINE's for the ASCII preview
    TotalWidth*=CharWidthPx;

    ScrollX=UITC_GetHorzSlider(TextDisplayCtrl);
    if(View_CharsX==0)
        UISetScrollBarPageSizeAndMax(ScrollX,0,0);
    else
        UISetScrollBarPageSizeAndMax(ScrollX,View_WidthPx,TotalWidth);
    UISetScrollBarStepSize(ScrollX,CharWidthPx);
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::KeyPress
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::KeyPress(uint8_t Mods,e_UIKeys Key,
 *              const uint8_t *TextPtr,unsigned int TextLen);
 *
 * PARAMETERS:
 *    Mods [I] -- What keys where held when this key event happended
 *    Key [I] -- What "special" key was pressed (things like arrow keys)
 *    TextPtr [I] -- A string with the key in it (UTF8).  If the key can be
 *                   converted to a char this will have it.
 *    TextLen [I] -- The number of bytes in 'TextPtr'
 *
 * FUNCTION:
 *    This function is called when there is a text canvas key press (a key
 *    press that is to be sent).
 *
 * RETURNS:
 *    true -- The key press was consumed
 *    false -- The key press was ignored.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::KeyPress(uint8_t Mods,e_UIKeys Key,
        const uint8_t *TextPtr,unsigned int TextLen)
{
    bool RetValue;
    t_UIScrollBarCtrl *ScrollY;
    t_UIScrollBarCtrl *ScrollX;
    int LastAnchor;
    int LastCursor;
    unsigned int r;

    ScrollY=UITC_GetVertSlider(TextDisplayCtrl);
    ScrollX=UITC_GetHorzSlider(TextDisplayCtrl);

    LastAnchor=Selection_Anchor;
    LastCursor=Cursor_Pos;

    RetValue=false;

    if(Mods&KEYMOD_SHIFT)
    {
        RetValue=true;
        switch(Key)
        {
            case e_UIKeys_Shift:
                if(!SelectionValid)
                {
                    /* No selection, make sure anchor is on the cursor */
                    Selection_Anchor=Cursor_Pos;
                }
            break;
            case e_UIKeys_Home:
                AbortEdit();
                Cursor_Pos=Cursor_Pos/BYTESPERLINE*BYTESPERLINE;
            break;
            case e_UIKeys_End:
                AbortEdit();
                Cursor_Pos=Cursor_Pos/BYTESPERLINE*BYTESPERLINE+BYTESPERLINE-1;
            break;
            case e_UIKeys_Up:
                AbortEdit();
                Cursor_Pos-=BYTESPERLINE;
            break;
            case e_UIKeys_Down:
                AbortEdit();
                Cursor_Pos+=BYTESPERLINE;
            break;
            case e_UIKeys_Left:
                AbortEdit();
                Cursor_Pos--;
            break;
            case e_UIKeys_Right:
                AbortEdit();
                Cursor_Pos++;
            break;
            case e_UIKeys_PageUp:
                AbortEdit();
                Cursor_Pos-=View_CharsY*BYTESPERLINE;
            break;
            case e_UIKeys_PageDown:
                AbortEdit();
                Cursor_Pos+=View_CharsY*BYTESPERLINE;
            break;

            case e_UIKeys_Escape:
            case e_UIKeys_Tab:
            case e_UIKeys_Backtab:
            case e_UIKeys_Backspace:
            case e_UIKeys_Return:
            case e_UIKeys_Enter:
            case e_UIKeys_Insert:
            case e_UIKeys_Delete:
            case e_UIKeys_Pause:
            case e_UIKeys_Print:
            case e_UIKeys_SysReq:
            case e_UIKeys_Clear:
            case e_UIKeys_Control:
            case e_UIKeys_Meta:
            case e_UIKeys_Alt:
            case e_UIKeys_AltGr:
            case e_UIKeys_CapsLock:
            case e_UIKeys_NumLock:
            case e_UIKeys_ScrollLock:
            case e_UIKeys_F1:
            case e_UIKeys_F2:
            case e_UIKeys_F3:
            case e_UIKeys_F4:
            case e_UIKeys_F5:
            case e_UIKeys_F6:
            case e_UIKeys_F7:
            case e_UIKeys_F8:
            case e_UIKeys_F9:
            case e_UIKeys_F10:
            case e_UIKeys_F11:
            case e_UIKeys_F12:
            case e_UIKeys_F13:
            case e_UIKeys_F14:
            case e_UIKeys_F15:
            case e_UIKeys_F16:
            case e_UIKeys_F17:
            case e_UIKeys_F18:
            case e_UIKeys_F19:
            case e_UIKeys_F20:
            case e_UIKeys_F21:
            case e_UIKeys_F22:
            case e_UIKeys_F23:
            case e_UIKeys_F24:
            case e_UIKeys_F25:
            case e_UIKeys_F26:
            case e_UIKeys_F27:
            case e_UIKeys_F28:
            case e_UIKeys_F29:
            case e_UIKeys_F30:
            case e_UIKeys_F31:
            case e_UIKeys_F32:
            case e_UIKeys_F33:
            case e_UIKeys_F34:
            case e_UIKeys_F35:
            case e_UIKeys_Super_L:
            case e_UIKeys_Super_R:
            case e_UIKeys_Menu:
            case e_UIKeys_Hyper_L:
            case e_UIKeys_Hyper_R:
            case e_UIKeys_Help:
            case e_UIKeys_Direction_L:
            case e_UIKeys_Direction_R:
            case e_UIKeys_Back:
            case e_UIKeys_Forward:
            case e_UIKeys_Stop:
            case e_UIKeys_Refresh:
            case e_UIKeys_VolumeDown:
            case e_UIKeys_VolumeMute:
            case e_UIKeys_VolumeUp:
            case e_UIKeys_BassBoost:
            case e_UIKeys_BassUp:
            case e_UIKeys_BassDown:
            case e_UIKeys_TrebleUp:
            case e_UIKeys_TrebleDown:
            case e_UIKeys_MediaPlay:
            case e_UIKeys_MediaStop:
            case e_UIKeys_MediaPrevious:
            case e_UIKeys_MediaNext:
            case e_UIKeys_MediaRecord:
            case e_UIKeys_MediaPause:
            case e_UIKeys_MediaTogglePlayPause:
            case e_UIKeys_HomePage:
            case e_UIKeys_Favorites:
            case e_UIKeys_Search:
            case e_UIKeys_Standby:
            case e_UIKeys_OpenUrl:
            case e_UIKeys_LaunchMail:
            case e_UIKeys_LaunchMedia:
            case e_UIKeys_Launch0:
            case e_UIKeys_Launch1:
            case e_UIKeys_Launch2:
            case e_UIKeys_Launch3:
            case e_UIKeys_Launch4:
            case e_UIKeys_Launch5:
            case e_UIKeys_Launch6:
            case e_UIKeys_Launch7:
            case e_UIKeys_Launch8:
            case e_UIKeys_Launch9:
            case e_UIKeys_LaunchA:
            case e_UIKeys_LaunchB:
            case e_UIKeys_LaunchC:
            case e_UIKeys_LaunchD:
            case e_UIKeys_LaunchE:
            case e_UIKeys_LaunchF:
            case e_UIKeys_LaunchG:
            case e_UIKeys_LaunchH:
            case e_UIKeys_MonBrightnessUp:
            case e_UIKeys_MonBrightnessDown:
            case e_UIKeys_KeyboardLightOnOff:
            case e_UIKeys_KeyboardBrightnessUp:
            case e_UIKeys_KeyboardBrightnessDown:
            case e_UIKeys_PowerOff:
            case e_UIKeys_WakeUp:
            case e_UIKeys_Eject:
            case e_UIKeys_ScreenSaver:
            case e_UIKeys_WWW:
            case e_UIKeys_Memo:
            case e_UIKeys_LightBulb:
            case e_UIKeys_Shop:
            case e_UIKeys_History:
            case e_UIKeys_AddFavorite:
            case e_UIKeys_HotLinks:
            case e_UIKeys_BrightnessAdjust:
            case e_UIKeys_Finance:
            case e_UIKeys_Community:
            case e_UIKeys_AudioRewind:
            case e_UIKeys_BackForward:
            case e_UIKeys_ApplicationLeft:
            case e_UIKeys_ApplicationRight:
            case e_UIKeys_Book:
            case e_UIKeys_CD:
            case e_UIKeys_Calculator:
            case e_UIKeys_ToDoList:
            case e_UIKeys_ClearGrab:
            case e_UIKeys_Close:
            case e_UIKeys_Copy:
            case e_UIKeys_Cut:
            case e_UIKeys_Display:
            case e_UIKeys_DOS:
            case e_UIKeys_Documents:
            case e_UIKeys_Excel:
            case e_UIKeys_Explorer:
            case e_UIKeys_Game:
            case e_UIKeys_Go:
            case e_UIKeys_iTouch:
            case e_UIKeys_LogOff:
            case e_UIKeys_Market:
            case e_UIKeys_Meeting:
            case e_UIKeys_MenuKB:
            case e_UIKeys_MenuPB:
            case e_UIKeys_MySites:
            case e_UIKeys_News:
            case e_UIKeys_OfficeHome:
            case e_UIKeys_Option:
            case e_UIKeys_Paste:
            case e_UIKeys_Phone:
            case e_UIKeys_Calendar:
            case e_UIKeys_Reply:
            case e_UIKeys_Reload:
            case e_UIKeys_RotateWindows:
            case e_UIKeys_RotationPB:
            case e_UIKeys_RotationKB:
            case e_UIKeys_Save:
            case e_UIKeys_Send:
            case e_UIKeys_Spell:
            case e_UIKeys_SplitScreen:
            case e_UIKeys_Support:
            case e_UIKeys_TaskPane:
            case e_UIKeys_Terminal:
            case e_UIKeys_Tools:
            case e_UIKeys_Travel:
            case e_UIKeys_Video:
            case e_UIKeys_Word:
            case e_UIKeys_Xfer:
            case e_UIKeys_ZoomIn:
            case e_UIKeys_ZoomOut:
            case e_UIKeys_Away:
            case e_UIKeys_Messenger:
            case e_UIKeys_WebCam:
            case e_UIKeys_MailForward:
            case e_UIKeys_Pictures:
            case e_UIKeys_Music:
            case e_UIKeys_Battery:
            case e_UIKeys_Bluetooth:
            case e_UIKeys_WLAN:
            case e_UIKeys_UWB:
            case e_UIKeys_AudioForward:
            case e_UIKeys_AudioRepeat:
            case e_UIKeys_AudioRandomPlay:
            case e_UIKeys_Subtitle:
            case e_UIKeys_AudioCycleTrack:
            case e_UIKeys_Time:
            case e_UIKeys_Hibernate:
            case e_UIKeys_View:
            case e_UIKeys_TopMenu:
            case e_UIKeys_PowerDown:
            case e_UIKeys_Suspend:
            case e_UIKeys_ContrastAdjust:
            case e_UIKeys_MediaLast:
            case e_UIKeys_unknown:
            case e_UIKeys_Call:
            case e_UIKeys_Camera:
            case e_UIKeys_CameraFocus:
            case e_UIKeys_Context1:
            case e_UIKeys_Context2:
            case e_UIKeys_Context3:
            case e_UIKeys_Context4:
            case e_UIKeys_Flip:
            case e_UIKeys_Hangup:
            case e_UIKeys_No:
            case e_UIKeys_Select:
            case e_UIKeys_Yes:
            case e_UIKeys_ToggleCallHangup:
            case e_UIKeys_VoiceDial:
            case e_UIKeys_LastNumberRedial:
            case e_UIKeys_Execute:
            case e_UIKeys_Printer:
            case e_UIKeys_Play:
            case e_UIKeys_Sleep:
            case e_UIKeys_Zoom:
            case e_UIKeys_Cancel:
            case e_UIKeysMAX:
            default:
                RetValue=false;
            break;
        }
        if(RetValue)
        {
            ClipCursorPos(true);

            if(Selection_Anchor!=Cursor_Pos)
                SelectionValid=true;

            if(LastAnchor!=Selection_Anchor || LastCursor!=Cursor_Pos)
            {
                /* We changed the selection make sure it's on screen */
                if(Cursor_Pos<=Selection_Anchor)
                {
                    /* End is not 1 past start but actually the start */
                    MakeOffsetVisable(Cursor_Pos,InAscIIArea,false);
                }
                else
                {
                    /* End is 1 past the end of the selection */
                    MakeOffsetVisable(Cursor_Pos-1,InAscIIArea,false);
                }
                RethinkCursorPos();
                SendSelectionEvent();
            }
            RebuildDisplay();
        }
    }
    if(RetValue)
        return RetValue;

    if(InEditMode)
    {
        RetValue=true;
        switch(Key)
        {
            case e_UIKeys_Delete:
                if(SelectionValid)
                {
                    /* Erase the selection */

                    const uint8_t *SelStart;
                    const uint8_t *SelEnd;
                    int Bytes2Del;

                    GetSelectionBounds(&SelStart,&SelEnd);

                    Bytes2Del=SelEnd-SelStart;

                    memmove((void *)SelStart,SelEnd,&Buffer[BufferSize]-SelEnd);

                    SetNewBufferSize(BufferSize-Bytes2Del);
                    Cursor_Pos=SelStart-Buffer;
                }
                else
                {
                    if(EditMode==e_HDB_EditState_FirstNib &&
                            Cursor_Pos<BufferSize)
                    {
                        AbortEdit();
                        AbortDotInput();

                        /* Delete the current byte and shorten the buffer */
                        memmove(&Buffer[Cursor_Pos],&Buffer[Cursor_Pos+1],
                                BufferSize-Cursor_Pos);
                        SetNewBufferSize(BufferSize-1);
                        LastCursor=-1;  // Force a selection / cursor rethink
                    }
                    else
                    {
                        AbortDotInput();
                        AbortEdit();
                        RebuildDisplay();
                        RethinkCursorPos();
                    }
                }
            break;
            case e_UIKeys_Insert:
                AbortDotInput();
                InInsertMode=!InInsertMode;
                RethinkCursorLook();
            break;
            case e_UIKeys_Backspace:
                if(EditMode==e_HDB_EditState_FirstNib && Cursor_Pos>0)
                {
                    AbortEdit();
                    AbortDotInput();

                    /* Delete the prev byte and shorten the buffer */
                    memmove(&Buffer[Cursor_Pos-1],&Buffer[Cursor_Pos],
                            BufferSize-Cursor_Pos);
                    SetNewBufferSize(BufferSize-1);
                    Cursor_Pos--;
                }
                else
                {
                    AbortDotInput();
                    AbortEdit();
                    RebuildDisplay();
                    RethinkCursorPos();
                }
            break;
            case e_UIKeys_Escape:
                ClearSelection();

                AbortDotInput();
                AbortEdit();
                RebuildDisplay();
                RethinkCursorPos();
            break;
            case e_UIKeys_Up:
                AbortDotInput();
                AbortEdit();
                if(Cursor_Pos-BYTESPERLINE>=0)
                    Cursor_Pos-=BYTESPERLINE;
            break;
            case e_UIKeys_Down:
                AbortDotInput();
                AbortEdit();
                Cursor_Pos+=BYTESPERLINE;
            break;
            case e_UIKeys_Left:
                AbortDotInput();
                AbortEdit();
                Cursor_Pos--;
            break;
            case e_UIKeys_Right:
                AbortDotInput();
                AbortEdit();
                Cursor_Pos++;
            break;
            case e_UIKeys_Return:
            case e_UIKeys_Enter:
                AbortDotInput();
                AcceptCycleInput();
            break;

            case e_UIKeys_Home:
                AbortDotInput();
                AbortEdit();
                Cursor_Pos=Cursor_Pos/BYTESPERLINE*BYTESPERLINE;
            break;
            case e_UIKeys_End:
                AbortDotInput();
                AbortEdit();
                Cursor_Pos=Cursor_Pos/BYTESPERLINE*BYTESPERLINE+BYTESPERLINE-1;
            break;
            case e_UIKeys_PageUp:
                AbortDotInput();
                AbortEdit();
                Cursor_Pos-=View_CharsY*BYTESPERLINE;
            break;
            case e_UIKeys_PageDown:
                AbortDotInput();
                AbortEdit();
                Cursor_Pos+=View_CharsY*BYTESPERLINE;
            break;

            case e_UIKeys_Tab:
            case e_UIKeys_Backtab:
            case e_UIKeys_Pause:
            case e_UIKeys_Print:
            case e_UIKeys_SysReq:
            case e_UIKeys_Clear:
            case e_UIKeys_Shift:
            case e_UIKeys_Control:
            case e_UIKeys_Meta:
            case e_UIKeys_Alt:
            case e_UIKeys_AltGr:
            case e_UIKeys_CapsLock:
            case e_UIKeys_NumLock:
            case e_UIKeys_ScrollLock:
            case e_UIKeys_F1:
            case e_UIKeys_F2:
            case e_UIKeys_F3:
            case e_UIKeys_F4:
            case e_UIKeys_F5:
            case e_UIKeys_F6:
            case e_UIKeys_F7:
            case e_UIKeys_F8:
            case e_UIKeys_F9:
            case e_UIKeys_F10:
            case e_UIKeys_F11:
            case e_UIKeys_F12:
            case e_UIKeys_F13:
            case e_UIKeys_F14:
            case e_UIKeys_F15:
            case e_UIKeys_F16:
            case e_UIKeys_F17:
            case e_UIKeys_F18:
            case e_UIKeys_F19:
            case e_UIKeys_F20:
            case e_UIKeys_F21:
            case e_UIKeys_F22:
            case e_UIKeys_F23:
            case e_UIKeys_F24:
            case e_UIKeys_F25:
            case e_UIKeys_F26:
            case e_UIKeys_F27:
            case e_UIKeys_F28:
            case e_UIKeys_F29:
            case e_UIKeys_F30:
            case e_UIKeys_F31:
            case e_UIKeys_F32:
            case e_UIKeys_F33:
            case e_UIKeys_F34:
            case e_UIKeys_F35:
            case e_UIKeys_Super_L:
            case e_UIKeys_Super_R:
            case e_UIKeys_Menu:
            case e_UIKeys_Hyper_L:
            case e_UIKeys_Hyper_R:
            case e_UIKeys_Help:
            case e_UIKeys_Direction_L:
            case e_UIKeys_Direction_R:
            case e_UIKeys_Back:
            case e_UIKeys_Forward:
            case e_UIKeys_Stop:
            case e_UIKeys_Refresh:
            case e_UIKeys_VolumeDown:
            case e_UIKeys_VolumeMute:
            case e_UIKeys_VolumeUp:
            case e_UIKeys_BassBoost:
            case e_UIKeys_BassUp:
            case e_UIKeys_BassDown:
            case e_UIKeys_TrebleUp:
            case e_UIKeys_TrebleDown:
            case e_UIKeys_MediaPlay:
            case e_UIKeys_MediaStop:
            case e_UIKeys_MediaPrevious:
            case e_UIKeys_MediaNext:
            case e_UIKeys_MediaRecord:
            case e_UIKeys_MediaPause:
            case e_UIKeys_MediaTogglePlayPause:
            case e_UIKeys_HomePage:
            case e_UIKeys_Favorites:
            case e_UIKeys_Search:
            case e_UIKeys_Standby:
            case e_UIKeys_OpenUrl:
            case e_UIKeys_LaunchMail:
            case e_UIKeys_LaunchMedia:
            case e_UIKeys_Launch0:
            case e_UIKeys_Launch1:
            case e_UIKeys_Launch2:
            case e_UIKeys_Launch3:
            case e_UIKeys_Launch4:
            case e_UIKeys_Launch5:
            case e_UIKeys_Launch6:
            case e_UIKeys_Launch7:
            case e_UIKeys_Launch8:
            case e_UIKeys_Launch9:
            case e_UIKeys_LaunchA:
            case e_UIKeys_LaunchB:
            case e_UIKeys_LaunchC:
            case e_UIKeys_LaunchD:
            case e_UIKeys_LaunchE:
            case e_UIKeys_LaunchF:
            case e_UIKeys_LaunchG:
            case e_UIKeys_LaunchH:
            case e_UIKeys_MonBrightnessUp:
            case e_UIKeys_MonBrightnessDown:
            case e_UIKeys_KeyboardLightOnOff:
            case e_UIKeys_KeyboardBrightnessUp:
            case e_UIKeys_KeyboardBrightnessDown:
            case e_UIKeys_PowerOff:
            case e_UIKeys_WakeUp:
            case e_UIKeys_Eject:
            case e_UIKeys_ScreenSaver:
            case e_UIKeys_WWW:
            case e_UIKeys_Memo:
            case e_UIKeys_LightBulb:
            case e_UIKeys_Shop:
            case e_UIKeys_History:
            case e_UIKeys_AddFavorite:
            case e_UIKeys_HotLinks:
            case e_UIKeys_BrightnessAdjust:
            case e_UIKeys_Finance:
            case e_UIKeys_Community:
            case e_UIKeys_AudioRewind:
            case e_UIKeys_BackForward:
            case e_UIKeys_ApplicationLeft:
            case e_UIKeys_ApplicationRight:
            case e_UIKeys_Book:
            case e_UIKeys_CD:
            case e_UIKeys_Calculator:
            case e_UIKeys_ToDoList:
            case e_UIKeys_ClearGrab:
            case e_UIKeys_Close:
            case e_UIKeys_Copy:
            case e_UIKeys_Cut:
            case e_UIKeys_Display:
            case e_UIKeys_DOS:
            case e_UIKeys_Documents:
            case e_UIKeys_Excel:
            case e_UIKeys_Explorer:
            case e_UIKeys_Game:
            case e_UIKeys_Go:
            case e_UIKeys_iTouch:
            case e_UIKeys_LogOff:
            case e_UIKeys_Market:
            case e_UIKeys_Meeting:
            case e_UIKeys_MenuKB:
            case e_UIKeys_MenuPB:
            case e_UIKeys_MySites:
            case e_UIKeys_News:
            case e_UIKeys_OfficeHome:
            case e_UIKeys_Option:
            case e_UIKeys_Paste:
            case e_UIKeys_Phone:
            case e_UIKeys_Calendar:
            case e_UIKeys_Reply:
            case e_UIKeys_Reload:
            case e_UIKeys_RotateWindows:
            case e_UIKeys_RotationPB:
            case e_UIKeys_RotationKB:
            case e_UIKeys_Save:
            case e_UIKeys_Send:
            case e_UIKeys_Spell:
            case e_UIKeys_SplitScreen:
            case e_UIKeys_Support:
            case e_UIKeys_TaskPane:
            case e_UIKeys_Terminal:
            case e_UIKeys_Tools:
            case e_UIKeys_Travel:
            case e_UIKeys_Video:
            case e_UIKeys_Word:
            case e_UIKeys_Xfer:
            case e_UIKeys_ZoomIn:
            case e_UIKeys_ZoomOut:
            case e_UIKeys_Away:
            case e_UIKeys_Messenger:
            case e_UIKeys_WebCam:
            case e_UIKeys_MailForward:
            case e_UIKeys_Pictures:
            case e_UIKeys_Music:
            case e_UIKeys_Battery:
            case e_UIKeys_Bluetooth:
            case e_UIKeys_WLAN:
            case e_UIKeys_UWB:
            case e_UIKeys_AudioForward:
            case e_UIKeys_AudioRepeat:
            case e_UIKeys_AudioRandomPlay:
            case e_UIKeys_Subtitle:
            case e_UIKeys_AudioCycleTrack:
            case e_UIKeys_Time:
            case e_UIKeys_Hibernate:
            case e_UIKeys_View:
            case e_UIKeys_TopMenu:
            case e_UIKeys_PowerDown:
            case e_UIKeys_Suspend:
            case e_UIKeys_ContrastAdjust:
            case e_UIKeys_MediaLast:
            case e_UIKeys_unknown:
            case e_UIKeys_Call:
            case e_UIKeys_Camera:
            case e_UIKeys_CameraFocus:
            case e_UIKeys_Context1:
            case e_UIKeys_Context2:
            case e_UIKeys_Context3:
            case e_UIKeys_Context4:
            case e_UIKeys_Flip:
            case e_UIKeys_Hangup:
            case e_UIKeys_No:
            case e_UIKeys_Select:
            case e_UIKeys_Yes:
            case e_UIKeys_ToggleCallHangup:
            case e_UIKeys_VoiceDial:
            case e_UIKeys_LastNumberRedial:
            case e_UIKeys_Execute:
            case e_UIKeys_Printer:
            case e_UIKeys_Play:
            case e_UIKeys_Sleep:
            case e_UIKeys_Zoom:
            case e_UIKeys_Cancel:
            case e_UIKeysMAX:
            default:
                RetValue=false;
                if(InEditMode)
                {
                    if(TextLen==1)
                    {
                        RetValue=true;
                        if(TextPtr[0]>='0' && TextPtr[0]<='9' && !InAscIIArea)
                        {
                            EditHandleNibbleInput(TextPtr[0]-'0');
                        }
                        else if(TextPtr[0]>='a' && TextPtr[0]<='f' &&
                                !InAscIIArea)
                        {
                            AbortDotInput();
                            EditHandleNibbleInput(TextPtr[0]-'a'+10);
                        }
                        else if(TextPtr[0]>='A' && TextPtr[0]<='F' &&
                                !InAscIIArea)
                        {
                            AbortDotInput();
                            EditHandleNibbleInput(TextPtr[0]-'A'+10);
                        }
                        else if(TextPtr[0]=='.' && !InAscIIArea)
                        {
                            DoingDotInputChar=!DoingDotInputChar;
                            RethinkCursorLook();
                        }
                        else if(TextPtr[0]=='+' && !InAscIIArea)
                        {
                            AbortDotInput();
                            EditHandleNibbleCycle(1);
                        }
                        else if(TextPtr[0]=='-' && !InAscIIArea)
                        {
                            AbortDotInput();
                            EditHandleNibbleCycle(-1);
                        }
                        else if(InAscIIArea)
                        {
                            /* Add this char (all bytes of it) */
                            for(r=0;r<TextLen;r++)
                            {
                                if(!ReadyForAddingChar())
                                    return true;
                                StartOfData[Cursor_Pos]=TextPtr[r];
                                Cursor_Pos++;
                                ClipCursorPos(false);
                            }
                        }
                        else
                        {
                            AbortDotInput();
                            RetValue=false;
                        }
                    }
                }
            break;
        }
        if(RetValue)
        {
            ClipCursorPos(false);
            if(LastAnchor!=Selection_Anchor || LastCursor!=Cursor_Pos)
            {
                MakeOffsetVisable(Cursor_Pos,InAscIIArea,false);
                ClearSelection();
                RebuildDisplay();
                RethinkCursorPos();
            }
        }
    }
    else
    {
        RetValue=false;
        switch(Key)
        {
            case e_UIKeys_Escape:
                ClearSelection();
                RebuildDisplay();
                RethinkCursorPos();
            break;
            case e_UIKeys_Home:
                UISetScrollBarPos(ScrollY,0);
                RetValue=true;
            break;
            case e_UIKeys_End:
                UISetScrollBarPos(ScrollY,LastTotalLines-View_CharsY);
                RetValue=true;
            break;
            case e_UIKeys_Up:
                UISetScrollBarPos(ScrollY,UIGetScrollBarPos(ScrollY)-1);
                RetValue=true;
            break;
            case e_UIKeys_Down:
                UISetScrollBarPos(ScrollY,UIGetScrollBarPos(ScrollY)+1);
                RetValue=true;
            break;
            case e_UIKeys_Left:
                UISetScrollBarPos(ScrollX,UIGetScrollBarPos(ScrollX)-1);
                RetValue=true;
            break;
            case e_UIKeys_Right:
                UISetScrollBarPos(ScrollX,UIGetScrollBarPos(ScrollX)+1);
                RetValue=true;
            break;
            case e_UIKeys_PageUp:
                UISetScrollBarPos(ScrollY,UIGetScrollBarPos(ScrollY)-View_CharsY);
                RetValue=true;
            break;
            case e_UIKeys_PageDown:
                UISetScrollBarPos(ScrollY,UIGetScrollBarPos(ScrollY)+View_CharsY);
                RetValue=true;
            break;

            case e_UIKeys_Tab:
            case e_UIKeys_Backtab:
            case e_UIKeys_Backspace:
            case e_UIKeys_Return:
            case e_UIKeys_Enter:
            case e_UIKeys_Insert:
            case e_UIKeys_Delete:
            case e_UIKeys_Pause:
            case e_UIKeys_Print:
            case e_UIKeys_SysReq:
            case e_UIKeys_Clear:
            case e_UIKeys_Shift:
            case e_UIKeys_Control:
            case e_UIKeys_Meta:
            case e_UIKeys_Alt:
            case e_UIKeys_AltGr:
            case e_UIKeys_CapsLock:
            case e_UIKeys_NumLock:
            case e_UIKeys_ScrollLock:
            case e_UIKeys_F1:
            case e_UIKeys_F2:
            case e_UIKeys_F3:
            case e_UIKeys_F4:
            case e_UIKeys_F5:
            case e_UIKeys_F6:
            case e_UIKeys_F7:
            case e_UIKeys_F8:
            case e_UIKeys_F9:
            case e_UIKeys_F10:
            case e_UIKeys_F11:
            case e_UIKeys_F12:
            case e_UIKeys_F13:
            case e_UIKeys_F14:
            case e_UIKeys_F15:
            case e_UIKeys_F16:
            case e_UIKeys_F17:
            case e_UIKeys_F18:
            case e_UIKeys_F19:
            case e_UIKeys_F20:
            case e_UIKeys_F21:
            case e_UIKeys_F22:
            case e_UIKeys_F23:
            case e_UIKeys_F24:
            case e_UIKeys_F25:
            case e_UIKeys_F26:
            case e_UIKeys_F27:
            case e_UIKeys_F28:
            case e_UIKeys_F29:
            case e_UIKeys_F30:
            case e_UIKeys_F31:
            case e_UIKeys_F32:
            case e_UIKeys_F33:
            case e_UIKeys_F34:
            case e_UIKeys_F35:
            case e_UIKeys_Super_L:
            case e_UIKeys_Super_R:
            case e_UIKeys_Menu:
            case e_UIKeys_Hyper_L:
            case e_UIKeys_Hyper_R:
            case e_UIKeys_Help:
            case e_UIKeys_Direction_L:
            case e_UIKeys_Direction_R:
            case e_UIKeys_Back:
            case e_UIKeys_Forward:
            case e_UIKeys_Stop:
            case e_UIKeys_Refresh:
            case e_UIKeys_VolumeDown:
            case e_UIKeys_VolumeMute:
            case e_UIKeys_VolumeUp:
            case e_UIKeys_BassBoost:
            case e_UIKeys_BassUp:
            case e_UIKeys_BassDown:
            case e_UIKeys_TrebleUp:
            case e_UIKeys_TrebleDown:
            case e_UIKeys_MediaPlay:
            case e_UIKeys_MediaStop:
            case e_UIKeys_MediaPrevious:
            case e_UIKeys_MediaNext:
            case e_UIKeys_MediaRecord:
            case e_UIKeys_MediaPause:
            case e_UIKeys_MediaTogglePlayPause:
            case e_UIKeys_HomePage:
            case e_UIKeys_Favorites:
            case e_UIKeys_Search:
            case e_UIKeys_Standby:
            case e_UIKeys_OpenUrl:
            case e_UIKeys_LaunchMail:
            case e_UIKeys_LaunchMedia:
            case e_UIKeys_Launch0:
            case e_UIKeys_Launch1:
            case e_UIKeys_Launch2:
            case e_UIKeys_Launch3:
            case e_UIKeys_Launch4:
            case e_UIKeys_Launch5:
            case e_UIKeys_Launch6:
            case e_UIKeys_Launch7:
            case e_UIKeys_Launch8:
            case e_UIKeys_Launch9:
            case e_UIKeys_LaunchA:
            case e_UIKeys_LaunchB:
            case e_UIKeys_LaunchC:
            case e_UIKeys_LaunchD:
            case e_UIKeys_LaunchE:
            case e_UIKeys_LaunchF:
            case e_UIKeys_LaunchG:
            case e_UIKeys_LaunchH:
            case e_UIKeys_MonBrightnessUp:
            case e_UIKeys_MonBrightnessDown:
            case e_UIKeys_KeyboardLightOnOff:
            case e_UIKeys_KeyboardBrightnessUp:
            case e_UIKeys_KeyboardBrightnessDown:
            case e_UIKeys_PowerOff:
            case e_UIKeys_WakeUp:
            case e_UIKeys_Eject:
            case e_UIKeys_ScreenSaver:
            case e_UIKeys_WWW:
            case e_UIKeys_Memo:
            case e_UIKeys_LightBulb:
            case e_UIKeys_Shop:
            case e_UIKeys_History:
            case e_UIKeys_AddFavorite:
            case e_UIKeys_HotLinks:
            case e_UIKeys_BrightnessAdjust:
            case e_UIKeys_Finance:
            case e_UIKeys_Community:
            case e_UIKeys_AudioRewind:
            case e_UIKeys_BackForward:
            case e_UIKeys_ApplicationLeft:
            case e_UIKeys_ApplicationRight:
            case e_UIKeys_Book:
            case e_UIKeys_CD:
            case e_UIKeys_Calculator:
            case e_UIKeys_ToDoList:
            case e_UIKeys_ClearGrab:
            case e_UIKeys_Close:
            case e_UIKeys_Copy:
            case e_UIKeys_Cut:
            case e_UIKeys_Display:
            case e_UIKeys_DOS:
            case e_UIKeys_Documents:
            case e_UIKeys_Excel:
            case e_UIKeys_Explorer:
            case e_UIKeys_Game:
            case e_UIKeys_Go:
            case e_UIKeys_iTouch:
            case e_UIKeys_LogOff:
            case e_UIKeys_Market:
            case e_UIKeys_Meeting:
            case e_UIKeys_MenuKB:
            case e_UIKeys_MenuPB:
            case e_UIKeys_MySites:
            case e_UIKeys_News:
            case e_UIKeys_OfficeHome:
            case e_UIKeys_Option:
            case e_UIKeys_Paste:
            case e_UIKeys_Phone:
            case e_UIKeys_Calendar:
            case e_UIKeys_Reply:
            case e_UIKeys_Reload:
            case e_UIKeys_RotateWindows:
            case e_UIKeys_RotationPB:
            case e_UIKeys_RotationKB:
            case e_UIKeys_Save:
            case e_UIKeys_Send:
            case e_UIKeys_Spell:
            case e_UIKeys_SplitScreen:
            case e_UIKeys_Support:
            case e_UIKeys_TaskPane:
            case e_UIKeys_Terminal:
            case e_UIKeys_Tools:
            case e_UIKeys_Travel:
            case e_UIKeys_Video:
            case e_UIKeys_Word:
            case e_UIKeys_Xfer:
            case e_UIKeys_ZoomIn:
            case e_UIKeys_ZoomOut:
            case e_UIKeys_Away:
            case e_UIKeys_Messenger:
            case e_UIKeys_WebCam:
            case e_UIKeys_MailForward:
            case e_UIKeys_Pictures:
            case e_UIKeys_Music:
            case e_UIKeys_Battery:
            case e_UIKeys_Bluetooth:
            case e_UIKeys_WLAN:
            case e_UIKeys_UWB:
            case e_UIKeys_AudioForward:
            case e_UIKeys_AudioRepeat:
            case e_UIKeys_AudioRandomPlay:
            case e_UIKeys_Subtitle:
            case e_UIKeys_AudioCycleTrack:
            case e_UIKeys_Time:
            case e_UIKeys_Hibernate:
            case e_UIKeys_View:
            case e_UIKeys_TopMenu:
            case e_UIKeys_PowerDown:
            case e_UIKeys_Suspend:
            case e_UIKeys_ContrastAdjust:
            case e_UIKeys_MediaLast:
            case e_UIKeys_unknown:
            case e_UIKeys_Call:
            case e_UIKeys_Camera:
            case e_UIKeys_CameraFocus:
            case e_UIKeys_Context1:
            case e_UIKeys_Context2:
            case e_UIKeys_Context3:
            case e_UIKeys_Context4:
            case e_UIKeys_Flip:
            case e_UIKeys_Hangup:
            case e_UIKeys_No:
            case e_UIKeys_Select:
            case e_UIKeys_Yes:
            case e_UIKeys_ToggleCallHangup:
            case e_UIKeys_VoiceDial:
            case e_UIKeys_LastNumberRedial:
            case e_UIKeys_Execute:
            case e_UIKeys_Printer:
            case e_UIKeys_Play:
            case e_UIKeys_Sleep:
            case e_UIKeys_Zoom:
            case e_UIKeys_Cancel:
            case e_UIKeysMAX:
            default:
            break;
        }
    }

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::EditHandleNibbleInput
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::EditHandleNibbleInput(int Nib);
 *
 * PARAMETERS:
 *    Nib [I] -- The number to set the current nibble to.  0-15
 *
 * FUNCTION:
 *    This function handles the input of a nibble.  It will move the cursor
 *    to the next pos, switch nibbles, and set the current byte as needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::EditHandleNibbleInput(int Nib)
{
    if(DoingDotInputChar)
    {
        /* Ok we convert this so 0-5 (or 1-6) maps to A-F */
        DoingDotInputChar=false;
        RethinkCursorLook();

        if(!g_Settings.DotInputStartsAt0)
            if(Nib>0)
                Nib--;

        if(Nib>5)
        {
            RebuildDisplay();
            return;
        }
        Nib+=0x0A;
    }
    if(DoingCycleInputChar)
    {
        if(EditMode==e_HDB_EditState_FirstNib)
        {
            /* We where doing a cycle input, so we jump to SecondNib and insert
               as normal */
            EditMode=e_HDB_EditState_SecondNib;
        }
        else
        {
            /* Ok, in this case we ignore the key press and do an accept
               instead */
            AcceptCycleInput();
            return;
        }
    }
    switch(EditMode)
    {
        case e_HDB_EditState_FirstNib:
            if(!ReadyForAddingChar())
                return;

            EditMode=e_HDB_EditState_SecondNib;
            EditByteValue=Nib<<4;
        break;
        case e_HDB_EditState_SecondNib:
            EditMode=e_HDB_EditState_FirstNib;
            EditByteValue&=0xF0;    // Clear the bottom nib
            EditByteValue|=Nib;
            StartOfData[Cursor_Pos]=EditByteValue;
            Cursor_Pos++;
            ClipCursorPos(false);
        break;
        case e_HDB_EditStateMAX:
        default:
            EditMode=e_HDB_EditState_FirstNib;
        break;
    }

    if(DoingCycleInputChar)
    {
        /* Cancel the DoingCycleInputChar mode */
        DoingCycleInputChar=false;
    }

    RethinkCursorPos();
    RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::EditHandleNibbleCycle
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::EditHandleNibbleCycle(int Dir);
 *
 * PARAMETERS:
 *    Dir [I] -- A +1 or -1 to add or sub 1 from the current nibble hex value
 *
 * FUNCTION:
 *    This function handles the cycling of hex digits when doing a cycle input.
 *    This is called when the '+' or '-' key is pressed to cycle though [A-F]
 *    of the current nibble.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::EditHandleNibbleCycle(int Dir)
{
    if(Dir<0 && NibCycleValue==0)
        NibCycleValue=5;
    else if(Dir>0 && NibCycleValue==5)
        NibCycleValue=0;
    else
        NibCycleValue+=Dir;

    if(!DoingCycleInputChar)
    {
        if(Dir>0)
            NibCycleValue=0;    // Start at A
        else
            NibCycleValue=5;    // Start at F

        /* We need to add a char to edit */
        if(!ReadyForAddingChar())
            return;
    }
    if(EditMode==e_HDB_EditState_FirstNib)
    {
        EditByteValue=(NibCycleValue+0x0A)<<4;
    }
    else
    {
        EditByteValue&=0xF0;    // Clear lower nib
        EditByteValue|=(NibCycleValue+0x0A);
    }
    DoingCycleInputChar=true;
    RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::ReadyForAddingChar
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::ReadyForAddingChar(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function readys the current insert pos for editing of a char.
 *    If the mode is INSERT and we are on the first nibble then a char is
 *    added, otherwize nothing happends.
 *
 *    This also handles the cycle input.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- We had problems resizing the buffer.  Abort
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::ReadyForAddingChar(void)
{
    int BytesAfterCursor;

    /* Check if we have anything to do */
    if(DoingCycleInputChar || EditMode!=e_HDB_EditState_FirstNib)
        return true;

    if(InInsertMode)
    {
        /* Ok, we need to insert a blank byte */
        BytesAfterCursor=BufferSize-Cursor_Pos;

        if(GrowBufferAsNeeded(BufferSize+1))
        {
            memmove(&Buffer[Cursor_Pos+1],&Buffer[Cursor_Pos],
                    BytesAfterCursor);
            Buffer[Cursor_Pos]=0;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if(Cursor_Pos>=BufferSize)
        {
            if(!GrowBufferAsNeeded(BufferSize+1))
                return false;
        }
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::AcceptCycleInput
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::AcceptCycleInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function accepts whatever has been input with the cycle input.
 *    It will store the current value and move to the next position.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::AcceptCycleInput(void)
{
    /* If we are doing cycle input then we move to the next nibble */
    if(DoingCycleInputChar)
    {
        if(EditMode==e_HDB_EditState_FirstNib)
        {
            EditMode=e_HDB_EditState_SecondNib;
            DoingCycleInputChar=false;  // Must be before RebuildDisplay()
            RethinkCursorPos();
            RebuildDisplay();
        }
        else
        {
            /* We are done entering this byte */
            EditMode=e_HDB_EditState_FirstNib;
            StartOfData[Cursor_Pos]=EditByteValue;
            Cursor_Pos++;
            DoingCycleInputChar=false;
        }
    }
}


/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::AbortEdit
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::AbortEdit(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function aborts the active edit of a byte (if there is one).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::AbortEdit(void)
{
    if((EditMode==e_HDB_EditState_SecondNib || DoingCycleInputChar) &&
            InInsertMode)
    {
        /* Ok, we have already inserted a byte so we need to undo that */
        memmove(&Buffer[Cursor_Pos],&Buffer[Cursor_Pos+1],
                BufferSize-Cursor_Pos-1);
        SetNewBufferSize(BufferSize-1);
    }
    DoingDotInputChar=false;
    DoingCycleInputChar=false;
    EditMode=e_HDB_EditState_FirstNib;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::AbortDotInput
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::AbortDotInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function aborts the dot input mode.  It returns the system to normal
 *    input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::AbortDotInput(void)
{
    if(DoingDotInputChar)
    {
        DoingDotInputChar=false;
        RethinkCursorLook();
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::ClipCursorPos
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::ClipCursorPos(bool SelectionRules);
 *
 * PARAMETERS:
 *    SelectionRules [I] -- Use selection rules (cursor must stay inside
 *                          valid bytes range)
 *
 * FUNCTION:
 *    This function makes sure the 'Cursor_Pos' is in a valid range.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::ClipCursorPos(bool SelectionRules)
{
    if(Cursor_Pos<0)
        Cursor_Pos=0;

    if(InEditMode || !SelectionRules)
    {
        /* Let the user move the cursor 1 past the end of the buffer (for
           inserting new bytes) */
        if(Cursor_Pos>BufferBytes2Draw)
            Cursor_Pos=BufferBytes2Draw;
    }
    else
    {
        if(Cursor_Pos>BufferBytes2Draw-1)
            Cursor_Pos=BufferBytes2Draw-1;
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::MouseWheelMoved
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::MouseWheelMoved(int Steps,uint8_t Mods);
 *
 * PARAMETERS:
 *    Steps [I] -- The number of steps the wheel has been moved.
 *                 Positive values means the wheel was moved away from the user,
 *                 negative values means the wheel was moved toward the user.
 *    Mods [I] -- What keys where held when this event happended
 *
 * FUNCTION:
 *    This function is called when the user moves the mouse wheel over the hex
 *    display buffer.
 *
 *    It moves the vertical scroll bar.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::MouseWheelMoved(int Steps,uint8_t Mods)
{
    int NewScrollY;
    int CurrentScrollY;
    t_UIScrollBarCtrl *ScrollY;

    ScrollY=UITC_GetVertSlider(TextDisplayCtrl);

    CurrentScrollY=UIGetScrollBarPos(ScrollY);

    NewScrollY=CurrentScrollY-Steps;

    if(NewScrollY<0)
        return;

    if(NewScrollY>LastTotalLines-View_CharsY)
        NewScrollY=LastTotalLines-View_CharsY;

    UISetScrollBarPos(ScrollY,NewScrollY);
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::HandleSelectionMouseUpDown
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::HandleSelectionMouseUpDown(bool BttnDown,
 *              int x,int y);
 *
 * PARAMETERS:
 *    BttnDown [I] -- If the left mouse going down (true) or up (false)
 *    x [I] -- The x pos (in pixels) of the mouse
 *    y [I] -- The y pos (in pixels) of the mouse
 *
 * FUNCTION:
 *    This function is called when the user presses the left mouse button.
 *    It handles starting / ending the selection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    HexDisplayBuffer::HandleSelectionMouseUpDownMove()
 ******************************************************************************/
void HexDisplayBuffer::HandleSelectionMouseUpDown(bool BttnDown,int x,int y)
{
    int SelectedByteOffset;
    int InBoundsSelectedByteOffset;
    bool InAscII;
    bool OutOfBounds;

    /* Check if there's anything to do */
    if(BttnDown==SelectionMouseDown)
        return;

    SelectedByteOffset=CalSelectionFromMouse(x,y,InAscII,OutOfBounds);
    InBoundsSelectedByteOffset=SelectedByteOffset;
    if(InBoundsSelectedByteOffset>=BufferBytes2Draw)
        InBoundsSelectedByteOffset=BufferBytes2Draw-1;

    if(OutOfBounds)
    {
        /* Clear the selection */
        ClearSelection();
        SelectionMouseDown=false;
    }
    else
    {
        SelectionMouseDown=BttnDown;

        if(BttnDown)
        {
            /* We are starting the selection */
            Selection_OrgAnchor=InBoundsSelectedByteOffset;
            Selection_Anchor=InBoundsSelectedByteOffset;
            if(Cursor_Pos<InBoundsSelectedByteOffset)
                Cursor_Pos=InBoundsSelectedByteOffset;
            InAscIIArea=InAscII;
            ClearSelection();
        }
        else
        {
            /* We are ending the selection */
            if(InBoundsSelectedByteOffset<Selection_OrgAnchor)
            {
                /* Ok, we are before where we started, set start instead */
                Selection_Anchor=InBoundsSelectedByteOffset;
                Cursor_Pos=Selection_OrgAnchor;
            }
            else
            {
                Selection_Anchor=Selection_OrgAnchor;
                Cursor_Pos=InBoundsSelectedByteOffset;
            }
            if(!SelectionValid)
            {
                /* No selection, cursor is permitted out side of valid data */
                Cursor_Pos=SelectedByteOffset;
            }
        }
    }

    AbortEdit();
    SendSelectionEvent();
    RebuildDisplay();
    RethinkCursorPos();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::HandleSelectionMouseUpDownMove
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::HandleSelectionMouseUpDownMove(int x,int y);
 *
 * PARAMETERS:
 *    x [I] -- The x pos (in pixels) of the mouse
 *    y [I] -- The y pos (in pixels) of the mouse
 *
 * FUNCTION:
 *    This function is called when the mouse moves.  It handles the selection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    HexDisplayBuffer::HandleSelectionMouseUpDown()
 ******************************************************************************/
void HexDisplayBuffer::HandleSelectionMouseUpDownMove(int x,int y)
{
    int SelectedByteOffset;
    bool InAscII;
    bool OutOfBounds;

    if(!SelectionMouseDown)
        return;

    SelectedByteOffset=CalSelectionFromMouse(x,y,InAscII,OutOfBounds);
    if(SelectedByteOffset>=BufferBytes2Draw)
        SelectedByteOffset=BufferBytes2Draw-1;

    if(OutOfBounds)
    {
        HandleSelectionScrollFromMouse(x,y,true);
    }
    else
    {
        if(InAscII!=InAscIIArea)
            return;

        if(SelectedByteOffset<Selection_OrgAnchor)
        {
            /* Ok, we are before where we started, set start instead */
            Selection_Anchor=SelectedByteOffset;
            Cursor_Pos=Selection_OrgAnchor;
        }
        else
        {
            Selection_Anchor=Selection_OrgAnchor;
            Cursor_Pos=SelectedByteOffset;
        }

        /* Turn the selection on if the anchor and cursor ever leave the
           same cell */
        if(Cursor_Pos!=Selection_Anchor)
            SelectionValid=true;
    }

    SendSelectionEvent();
    RebuildDisplay();
    RethinkCursorPos();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::CalSelectionFromMouse
 *
 * SYNOPSIS:
 *    int HexDisplayBuffer::CalSelectionFromMouse(int x,int y,bool &InAscII,
 *              bool &OutOfBounds);
 *
 * PARAMETERS:
 *    x [I] -- The x pos (in pixels) of the mouse
 *    y [I] -- The y pos (in pixels) of the mouse
 *    InAscII [O] -- This is set to true if x,y is in the AscII preview, false
 *                   if it's in the hex display
 *    OutOfBounds [O] -- This is set to true if x,y are out of bounds, and
 *                       false if it's in bounds.
 *
 * FUNCTION:
 *    This function figures out the byte offset into the data based on an x,y
 *    point.
 *
 * RETURNS:
 *    The byte offset into the buffer.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int HexDisplayBuffer::CalSelectionFromMouse(int x,int y,bool &InAscII,
        bool &OutOfBounds)
{
    int charx,chary;
    int RetValue;
    int TotalLines;
    int AdjustedX;

    if(x<0 || y<0)
    {
        OutOfBounds=true;
        return 0;
    }

    AdjustedX=x+WindowXOffsetPx;

    /* Figure out what byte the user clicked on */
    charx=AdjustedX/CharWidthPx;
    chary=y/CharHeightPx;

    /* See if we are off the end */
    if(charx<0 || x>View_WidthPx)
    {
        OutOfBounds=true;
        return 0;
    }

    if(chary<0 || chary>=View_CharsY)
    {
        OutOfBounds=true;
        return 0;
    }

    /* We do a messed up cal'c here so we end up being off by 1 when we have
       full lines (if we have 15 bytes then we want to say there are 1 line,
       if we have 16 bytes then we want to say there are 2 lines, and if
       we have 17 bytes then we want to say there are 2 lines) */
    TotalLines=(BufferBytes2Draw+BYTESPERLINE)/BYTESPERLINE;
    if(chary>=TotalLines)
    {
        OutOfBounds=true;
        return 0;
    }

    /* See if we are in the dead space */
    if(charx>=BYTESPERLINE*HEX_DIG_SIZE && charx<ASCIILEFTEDGE)
    {
        /* We are in the dead space between Hex and AscII */
        OutOfBounds=true;
        return 0;
    }

    OutOfBounds=false;

    /* See if we are in the AscII or not */
    if(charx>BYTESPERLINE*HEX_DIG_SIZE)
        InAscII=true;
    else
        InAscII=false;

    if(InAscII)
    {
        charx-=ASCIILEFTEDGE;
        RetValue=chary*BYTESPERLINE+charx;
    }
    else
    {
        charx/=HEX_DIG_SIZE;   // We use 3 bytes per byte
        RetValue=chary*BYTESPERLINE+charx;
    }

    /* We need to take the top line into account */
    RetValue+=TopLine*BYTESPERLINE;

    if(RetValue>=BufferBytes2Draw)
        RetValue=BufferBytes2Draw;

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::HandleSelectionScrollFromMouse
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::HandleSelectionScrollFromMouse(int x,int y,
 *              bool RedrawScreen);
 *
 * PARAMETERS:
 *    x [I] -- The x pos (in pixels) of the mouse
 *    y [I] -- The y pos (in pixels) of the mouse
 *    RedrawScreen [I] -- If true then we rebuild the screen display, if false
 *                        we update the pointer and scroll bars but do not
 *                        call RebuildDisplay()
 *
 * FUNCTION:
 *    This function handles scrolling the hex display if the user is selecting
 *    text and moves out of the text area.  It will scroll as needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::HandleSelectionScrollFromMouse(int x,int y,
        bool RedrawScreen)
{
    t_UIScrollBarCtrl *ScrollY;
    t_UIScrollBarCtrl *ScrollX;
    bool SetY;
    bool SetX;
    int TotalLines;
    int TotalWidth;

    ScrollY=UITC_GetVertSlider(TextDisplayCtrl);
    ScrollX=UITC_GetHorzSlider(TextDisplayCtrl);

    SetY=false;
    TotalLines=(BufferBytes2Draw+BYTESPERLINE-1)/BYTESPERLINE;
    if(y<0 && TopLine>0)
    {
        TopLine--;
        SetY=true;
    }
    if(y>View_CharsY*CharHeightPx && TopLine<TotalLines-(View_CharsY-1)-1)
    {
        TopLine++;
        SetY=true;
    }

    /* Handle scrolling left<->right */
    SetX=false;
    TotalWidth=BYTESPERLINE*HEX_DIG_SIZE;  // BYTESPERLINE's bytes * 2 for the hex and 1 for the space
    TotalWidth+=SPACE_BETWEEN_HEX_AND_ASCII;    // spaces between hex and ASCII preview
    TotalWidth+=BYTESPERLINE;   // Add the BYTESPERLINE's for the ASCII preview
    TotalWidth*=CharWidthPx;

    if(x<0 && WindowXOffsetPx>0)
    {
        WindowXOffsetPx-=CharWidthPx;
        if(WindowXOffsetPx<0)
            WindowXOffsetPx=0;
        SetX=true;
    }
    if(x>View_WidthPx && WindowXOffsetPx<TotalWidth-View_CharsX)
    {
        WindowXOffsetPx+=CharWidthPx;
        if(WindowXOffsetPx>TotalWidth-View_WidthPx)
            WindowXOffsetPx=TotalWidth-View_WidthPx;
        SetX=true;
    }

    if(SetY)
        UISetScrollBarPos(ScrollY,TopLine);
    if(SetX)
        UISetScrollBarPos(ScrollX,WindowXOffsetPx);

    if(RedrawScreen && (SetX || SetY))
        RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::ClearSelection
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::ClearSelection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the selection in the hex display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::ClearSelection(void)
{
    SelectionValid=false;
    Selection_Anchor=Cursor_Pos;

    SendSelectionEvent();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::MakeOffsetVisable
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::MakeOffsetVisable(int Offset,bool ShowInAscII,
 *              bool RedrawScreen);
 *
 * PARAMETERS:
 *    Offset [I] -- The offset from 0 to make visable.
 *    ShowInAscII [I] -- Show the char in the AscII (true) or in the hex (false)
 *    RedrawScreen [I] -- If true then we rebuild the screen display, if false
 *                        we update the pointer and scroll bars but do not
 *                        call RebuildDisplay()
 *
 * FUNCTION:
 *    This function scroll the hex display to a point so that 'Offset' is on
 *    screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::MakeOffsetVisable(int Offset,bool ShowInAscII,
        bool RedrawScreen)
{
    int OffsetLine;
    int OffsetChar;
    int CharPosPx;
    bool SetY;
    bool SetX;
    t_UIScrollBarCtrl *ScrollY;
    t_UIScrollBarCtrl *ScrollX;

    ScrollY=UITC_GetVertSlider(TextDisplayCtrl);
    ScrollX=UITC_GetHorzSlider(TextDisplayCtrl);

    /* Fix Y */
    OffsetLine=Offset/BYTESPERLINE;
    SetY=false;
    if(OffsetLine<TopLine)
    {
        TopLine=OffsetLine;
        SetY=true;
    }
    if(OffsetLine>TopLine+View_CharsY-1)
    {
        TopLine=OffsetLine-(View_CharsY-1);
        SetY=true;
    }

    /* Fix X */
    OffsetChar=(Offset%BYTESPERLINE);
    SetX=false;
    if(ShowInAscII)
    {
        CharPosPx=(ASCIILEFTEDGE+OffsetChar)*CharWidthPx;

        if(CharPosPx<WindowXOffsetPx)
        {
            WindowXOffsetPx=CharPosPx;
            SetX=true;
        }

        if(CharPosPx>WindowXOffsetPx+View_WidthPx-CharWidthPx)
        {
            WindowXOffsetPx=CharPosPx-View_WidthPx+CharWidthPx;
            SetX=true;
        }
    }
    else
    {
        CharPosPx=(OffsetChar*HEX_DIG_SIZE)*CharWidthPx;

        if(CharPosPx<WindowXOffsetPx)
        {
            WindowXOffsetPx=CharPosPx;
            SetX=true;
        }

        if(CharPosPx>WindowXOffsetPx+View_WidthPx-CharWidthPx*HEX_DIG_SIZE)
        {
            WindowXOffsetPx=CharPosPx-View_WidthPx+CharWidthPx*HEX_DIG_SIZE;
            SetX=true;
        }
    }

    if(SetY)
        UISetScrollBarPos(ScrollY,TopLine);
    if(SetX)
        UISetScrollBarPos(ScrollX,WindowXOffsetPx);

    if(RedrawScreen && (SetX || SetY))
        RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::GetSelectionBounds
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::GetSelectionBounds(const uint8_t **StartPtr,
 *              const uint8_t **EndPtr);
 *
 * PARAMETERS:
 *    StartPtr [O] -- A pointer to fill in with the start of the selection
 *                    (can be NULL).
 *    EndPtr [O] -- A pointer to fill in with the end of the selection
 *                  (can be NULL).
 *
 * FUNCTION:
 *    This function gets the selection position.  This is a pair of pointers
 *    into the hex 'Buffer', because of this you have to handle
 *    wrapping ('Buffer' maybe circular).
 *
 * RETURNS:
 *    true -- There was a selection and the values have been updated
 *    false -- There is no selection and the values have not been changed.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::GetSelectionBounds(const uint8_t **StartPtr,
        const uint8_t **EndPtr)
{
    int SelectionStart;
    int SelectionEnd;
    const uint8_t *CurPos;
    const uint8_t *EndOfBuffPos;
    const uint8_t *StartOfSel;
    const uint8_t *EndOfSel;

    if(!SelectionValid)
        return false;

    if(Cursor_Pos<Selection_Anchor)
    {
        SelectionStart=Cursor_Pos;
        SelectionEnd=Selection_Anchor;
    }
    else
    {
        SelectionStart=Selection_Anchor;
        SelectionEnd=Cursor_Pos+1;
    }

    /* Keep the selection inside of the valid bytes */
    if(SelectionEnd>BufferBytes2Draw)
        SelectionEnd=BufferBytes2Draw;

    CurPos=StartOfData;
    EndOfBuffPos=Buffer+BufferSize;

    StartOfSel=CurPos+SelectionStart;
    if(StartOfSel>EndOfBuffPos)
    {
        /* Wrapped */
        StartOfSel-=BufferSize;
    }

    EndOfSel=CurPos+SelectionEnd;
    if(EndOfSel>EndOfBuffPos)
    {
        /* Wrapped */
        EndOfSel-=BufferSize;
    }

    if(StartPtr!=NULL)
        *StartPtr=StartOfSel;
    if(EndPtr!=NULL)
        *EndPtr=EndOfSel;

    return true;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetSelectionBounds
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SetSelectionBounds(const uint8_t *StartPtr,
 *              const uint8_t *EndPtr);
 *
 * PARAMETERS:
 *    StartPtr [I] -- A pointer to fill in with the start of the selection.
 *    EndPtr [I] -- A pointer to fill in with the end of the selection.
 *
 * FUNCTION:
 *    This function sets the selection position.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    SetBuffer()
 ******************************************************************************/
void HexDisplayBuffer::SetSelectionBounds(const uint8_t *StartPtr,
        const uint8_t *EndPtr)
{
    /* Check the pointers */
    if(StartPtr<Buffer || StartPtr>=Buffer+BufferSize)
        return;

    if(EndPtr<Buffer || EndPtr>=Buffer+BufferSize)
        return;

    ClearSelection();

    if(StartPtr<EndPtr)
    {
        Selection_Anchor=StartPtr-Buffer;
        Cursor_Pos=EndPtr-Buffer;
    }
    else
    {
        Cursor_Pos=StartPtr-Buffer;
        Selection_Anchor=EndPtr-Buffer;
    }

    SelectionValid=true;

    MakeOffsetVisable(Cursor_Pos,InAscIIArea,false);
    RebuildDisplay();
    RethinkCursorPos();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::GetBufferInfo
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::GetBufferInfo(const uint8_t **StartOfBuffer,
 *              int *Size);
 *
 * PARAMETERS:
 *    StartOfBuffer [O] -- The start of the buffer memory.  This is NOT
 *                         the insert pos but the start of buffer memory.
 *                         Pass NULL to ignore.
 *    Size [O] -- The number of bytes in the buffer.  Pass NULL to ignore.
 *
 * FUNCTION:
 *    This function returns info about the buffers in use.
 *
 * RETURNS:
 *    true -- We got the info
 *    false -- There was an error
 *
 * LIMITATIONS:
 *    This is only valid if in edit mode.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::GetBufferInfo(const uint8_t **StartOfBuffer,int *Size)
{
    if(!InEditMode)
        return false;

    if(StartOfBuffer!=NULL)
        *StartOfBuffer=Buffer;
    if(Size!=NULL)
        *Size=BufferSize;

    return true;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::GetCursorPos
 *
 * SYNOPSIS:
 *    int HexDisplayBuffer::GetCursorPos(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function return the current cursor pos.
 *
 * RETURNS:
 *    The cursor pos.  This is the number of bytes from the start of the
 *    buffer.
 *
 * NOTES:
 *    Although the cursor is valid when no in edit mode it is hidden and
 *    doesn't really have any meaning.
 *
 * SEE ALSO:
 *    SetCursorPos(), GetAnchorPos()
 ******************************************************************************/
int HexDisplayBuffer::GetCursorPos(void)
{
    int RetValue;

    RetValue=Cursor_Pos;

    /* Keep the cursor inside of the valid bytes */
    if(RetValue>BufferBytes2Draw)
        RetValue=BufferBytes2Draw;

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetCursorPos
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SetCursorPos(int NewPos);
 *
 * PARAMETERS:
 *    NewPos [I] -- The new pos for the cursor.
 *
 * FUNCTION:
 *    This function sets the pos into the buffer of the cursor.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    Although the cursor is valid when no in edit mode it is hidden and
 *    doesn't really have any meaning.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::SetCursorPos(int NewPos)
{
    ClearSelection();

    Cursor_Pos=NewPos;
    ClipCursorPos(false);
    RethinkCursorPos();
    MakeOffsetVisable(Cursor_Pos,InAscIIArea,false);
    RethinkCursorPos();
    RebuildDisplay();

    SendSelectionEvent();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::GetAnchorPos
 *
 * SYNOPSIS:
 *    int HexDisplayBuffer::GetAnchorPos(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function return the current selection anchor pos.  A selection goes
 *    from the cursor to the anchor (or from the anchor to the cursor if
 *    it is inverted).
 *
 *    So if your buffer is:
 *          ABCDEFGHIJ
 *
 *    and the "DEF" is selected the cursor and anchor will be offsets
 *      cursor=3, anchor=5
 *          or
 *      cursor=5, anchor=3 (if it's inverted)
 *
 * RETURNS:
 *    The anchor pos.  This is the number of bytes from the start of the
 *    buffer.
 *
 * NOTES:
 *    Anchor can be less than cursor depending on how the selection was made.
 *
 * SEE ALSO:
 *    GetCursorPos()
 ******************************************************************************/
int HexDisplayBuffer::GetAnchorPos(void)
{
    int RetValue;

    RetValue=Selection_Anchor;

    /* Keep the anchor inside of the valid bytes */
    if(RetValue>BufferBytes2Draw)
        RetValue=BufferBytes2Draw;

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::GetSizeOfSelection
 *
 * SYNOPSIS:
 *    int HexDisplayBuffer::GetSizeOfSelection(e_HDBCFormatType ClipFormat);
 *
 * PARAMETERS:
 *    ClipFormat [I] -- How do you want the selection formated.  See
 *                      CopySelection2Buffer() for a list of valid
 *                      options.
 *
 * FUNCTION:
 *    This function gets the number of bytes in the current selection.
 *
 * RETURNS:
 *    The number of bytes in the current selection.
 *
 * SEE ALSO:
 *    HexDisplayBuffer::CopySelection2Buffer(),
 *    HexDisplayBuffer::GetSelectionBounds()
 ******************************************************************************/
int HexDisplayBuffer::GetSizeOfSelection(e_HDBCFormatType ClipFormat)
{
    int SelectionStart;
    int SelectionEnd;
    int SelectionBytes;
    e_HDBCFormatType UseClipFormat;
    int RetValue;
    int ByteNeeded;

    UseClipFormat=ClipFormat;

    if(!SelectionValid)
        return 0;

    if(Cursor_Pos<Selection_Anchor)
    {
        SelectionStart=Cursor_Pos;
        SelectionEnd=Selection_Anchor;
    }
    else
    {
        SelectionStart=Selection_Anchor;
        SelectionEnd=Cursor_Pos;
    }

    SelectionBytes=SelectionEnd-SelectionStart+1;   // +1 because 0 still has 1 byte in it
    if(UseClipFormat==e_HDBCFormat_Default)
    {
        if(InAscIIArea)
            UseClipFormat=e_HDBCFormat_AscII;
        else
            UseClipFormat=e_HDBCFormat_Hex;
    }
    switch(UseClipFormat)
    {
        case e_HDBCFormat_HexDump:
            /* Make sure we have a full last line */
            ByteNeeded=(SelectionBytes+BYTESPERLINE-1)/BYTESPERLINE*BYTESPERLINE;
            RetValue=ByteNeeded*HEX_DIG_SIZE;  // Every byte will be converted to 3 digits
            RetValue+=ByteNeeded;   // + the AscII preview
            RetValue+=(ByteNeeded/BYTESPERLINE)*(SPACE_BETWEEN_HEX_AND_ASCII+1); // + 4 bytes per line (3 between the hex and preview, 1 for newline)
            RetValue+=1;    // + a \0 at the end
        break;
        case e_HDBCFormat_Hex:
            RetValue=SelectionBytes*HEX_DIG_SIZE+1;  // 3 bytes per byte + \0
        break;
        case e_HDBCFormat_AscII:
            RetValue=SelectionBytes+1;
        break;
        case e_HDBCFormat_RAW:
            RetValue=SelectionBytes;
        break;
        case e_HDBCFormat_Default:
        case e_HDBCFormatMAX:
        default:
            RetValue=0;
        break;
    }
    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::CopySelection2Buffer
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::CopySelection2Buffer(uint8_t *OutBuff,
 *          e_HDBCFormatType ClipFormat);
 *
 * PARAMETERS:
 *    OutBuff [O] -- This buffer is filled in with the data from the selection.
 *    ClipFormat [I] -- What format do you want the selection formated as.
 *                      Valid options:
 *                          e_HDBCFormat_Default -- Format based on how
 *                              the user has selected data.
 *                          e_HDBCFormat_HexDump -- Output has a hex dump.
 *                              Hex values on the left and AscII preview
 *                              on the right.
 *                          e_HDBCFormat_Hex -- Output hex values only.
 *                          e_HDBCFormat_AscII -- Output AscII preview with
 *                              invalid chars replaced with '.'
 *                          e_HDBCFormat_RAW -- The raw bytes without 
 *                              any convertion.
 *
 * FUNCTION:
 *    This function copies the selected data from 'Buffer' to 'OutBuff'
 *
 * RETURNS:
 *    NONE
 *
 * LIMITATIONS:
 *    This function does not check that 'OutBuff' is big enough before doing
 *    the copy.  You need to make sure using
 *    HexDisplayBuffer::GetSizeOfSelection()
 *
 * SEE ALSO:
 *    HexDisplayBuffer::GetSizeOfSelection(),
 *    HexDisplayBuffer::GetSelectionBounds()
 ******************************************************************************/
void HexDisplayBuffer::CopySelection2Buffer(uint8_t *OutBuff,
        e_HDBCFormatType ClipFormat)
{
    uint8_t *BufferEnd;
    const uint8_t *SelStart;
    const uint8_t *SelEnd;
    int CopyBytes;
    int Bytes;
    unsigned char c;
    e_HDBCFormatType UseClipFormat;
    const uint8_t *Source;
    uint8_t *Dest;
    int ByteCount;

    UseClipFormat=ClipFormat;

    Bytes=GetSizeOfSelection(ClipFormat);
    if(Bytes==0)
        return;

    if(!GetSelectionBounds(&SelStart,&SelEnd))
        return;

    BufferEnd=Buffer+BufferSize;

    Source=SelStart;
    Dest=OutBuff;

    if(UseClipFormat==e_HDBCFormat_Default)
    {
        if(InAscIIArea)
            UseClipFormat=e_HDBCFormat_AscII;
        else
            UseClipFormat=e_HDBCFormat_Hex;
    }
    switch(UseClipFormat)
    {
        case e_HDBCFormat_HexDump:
            ByteCount=0;
            while(Source!=SelEnd)
            {
                if(Source>=BufferEnd)
                    Source=Buffer;

                c=*Source;
                sprintf((char *)&Dest[ByteCount*HEX_DIG_SIZE],"%02X ",c);

                if(c<32 || c>127)
                    c='.';
                Dest[ASCIILEFTEDGE+ByteCount]=c;

                ByteCount++;
                if(ByteCount==BYTESPERLINE)
                {
                    /* End of the line */
                    Dest[ASCIILEFTEDGE+BYTESPERLINE]='\n';

                    /* 3 space between the hex and Asc II */
                    memset(&Dest[BYTESPERLINE*HEX_DIG_SIZE],' ',
                            SPACE_BETWEEN_HEX_AND_ASCII);
                    Dest+=ASCIILEFTEDGE+BYTESPERLINE+1;  // 1 newline char
                    *Dest=0;    // Make it a string
                    ByteCount=0;
                }

                Source++;
            }

            if(ByteCount>0)
            {
                /* We have to finish this line */

                /* Padd with spaces */
                for(;ByteCount<BYTESPERLINE;ByteCount++)
                {
                    /* Space for AscII */
                    Dest[ASCIILEFTEDGE+ByteCount]=' ';

                    /* 3 spaces for hex */
                    Dest[ByteCount*HEX_DIG_SIZE]=' ';
                    Dest[ByteCount*HEX_DIG_SIZE+1]=' ';
                    Dest[ByteCount*HEX_DIG_SIZE+2]=' ';
                }

                /* 3 spaces for between hex and AscII */
                memset(&Dest[BYTESPERLINE*HEX_DIG_SIZE],' ',SPACE_BETWEEN_HEX_AND_ASCII);

                Dest[ASCIILEFTEDGE+BYTESPERLINE]='\n';
                Dest[ASCIILEFTEDGE+BYTESPERLINE+1]=0; // String
            }
        break;
        case e_HDBCFormat_Hex:
            while(Source!=SelEnd)
            {
                if(Source>=BufferEnd)
                    Source=Buffer;

                sprintf((char *)Dest,"%02X ",*Source);
                Dest+=HEX_DIG_SIZE;

                Source++;
            }
            *(Dest-1)=0;    // Last space becomes a String
        break;
        case e_HDBCFormat_AscII:
            while(Source!=SelEnd)
            {
                if(Source>=BufferEnd)
                    Source=Buffer;

                c=*Source;
                if(c<32 || c>127)
                    c='.';
                *Dest++=c;

                Source++;
            }
            *Dest=0;    // String
        break;
        case e_HDBCFormat_RAW:
            if(SelStart+Bytes>BufferEnd)
            {
                /* We need to split it over to copies */
                CopyBytes=BufferEnd-SelStart;
                memcpy(OutBuff,SelStart,CopyBytes);
                memcpy(&OutBuff[CopyBytes],Buffer,Bytes-CopyBytes);
            }
            else
            {
                /* Things will fit copy the whole thing */
                memcpy(OutBuff,SelStart,Bytes);
            }
        break;
        case e_HDBCFormat_Default:
        case e_HDBCFormatMAX:
        default:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SendSelection2Clipboard
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SendSelection2Clipboard(e_ClipboardType Clip,
 *              e_HDBCFormatType Format);
 *
 * PARAMETERS:
 *    Clip [I] -- What clipboard to send the text to
 *    Format [I] -- What format to apply to the copy.  See
 *                  CopySelection2Buffer()
 *
 * FUNCTION:
 *    This is a helper function that gets the selection from the hex display
 *    buffer and sends it to the clipboard.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::SendSelection2Clipboard(e_ClipboardType Clip,
        e_HDBCFormatType Format)
{
    int Bytes;
    uint8_t *Dest;

    Bytes=GetSizeOfSelection(Format);
    if(Bytes>0)
    {
        Dest=(uint8_t *)malloc(Bytes+1);
        if(Dest!=NULL)
        {
            CopySelection2Buffer(Dest,Format);
            Dest[Bytes]=0;
            UI_SetClipboardTextCStr((const char *)Dest,Clip);
            free(Dest);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SendSelectionEvent
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SendSelectionEvent(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Sends a selection event.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::SendSelectionEvent(void)
{
    struct HDEvent Event;

    if(HDEventHandler!=NULL)
    {
        Event.EventType=e_HDEvent_SelectionEvent;
        Event.ID=HDID;
        if(!SelectionValid)
        {
            Event.Info.Select.SelectionValid=false;
        }
        else
        {
            Event.Info.Select.SelectionValid=true;
            GetSelectionBounds(&Event.Info.Select.StartOfSel,
                    &Event.Info.Select.EndOfSel);
        }

        HDEventHandler(&Event);
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetLossOfFocusBehavior
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SetLossOfFocusBehavior(bool HideSelection);
 *
 * PARAMETERS:
 *    HideSelection [I] -- Do we hide the selection if we lose focus?
 *
 * FUNCTION:
 *    This function sets how we behave when we lose focus.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::SetLossOfFocusBehavior(bool HideSelection)
{
    HideSelectionOnLossOfFocus=HideSelection;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetEditMode
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::SetEditMode(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function turns on edit mode of the hex display.
 *
 *    The data buffer (set with SetBuffer()) will be copied to a new
 *    allocation.  You will need to get the new buffers memory with
 *    GetBuffer().
 *
 *    The old buffer will NOT be freed.
 *
 *    If switch to edit mode you must call GetBuffer() to get the buffer as
 *    this class WILL NOT free the buffer.
 *
 * RETURNS:
 *    true -- We have switched to edit mode.
 *    false -- There was a problem switching to edit mode (out of memory,
 *             invalid buffer setup?)
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::SetEditMode(void)
{
    uint8_t *NewBuffer;

    if(BufferIsCircular)
        return false;

    InEditMode=true;

    if(!WeAllocBuffer)
    {
        /* We need to copy the buffer */
        if(BufferSize>0)
        {
            NewBuffer=(uint8_t *)malloc(BufferSize);
            if(NewBuffer==NULL)
                return false;
            BufferAllocatedSize=BufferSize;

            memcpy(NewBuffer,Buffer,BufferSize);

            Buffer=NewBuffer;   // Switch to our copy
            StartOfData=NewBuffer;
            InsertPos=NewBuffer+BufferSize;

            WeAllocBuffer=true;
        }
        else
        {
            Buffer=NULL;
            SetNewBufferSize(0);
            BufferAllocatedSize=0;
        }
    }

    RethinkCursorLook();

    return true;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::RethinkCursorPos
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::RethinkCursorPos(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes the 'Cursor_Pos' and moves the text canvas cursor
 *    to the correct pos.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::RethinkCursorPos(void)
{
    t_UIScrollBarCtrl *ScrollY;
    int CursorX;
    int CursorY;
    int TopEdge;
    struct HDEvent HDEvent;

    ScrollY=UITC_GetVertSlider(TextDisplayCtrl);

    TopEdge=UIGetScrollBarPos(ScrollY);

    CursorX=Cursor_Pos%BYTESPERLINE;
    CursorY=Cursor_Pos/BYTESPERLINE;

    if(InAscIIArea)
    {
        CursorX+=ASCIILEFTEDGE;
    }
    else
    {
        CursorX*=HEX_DIG_SIZE;

        switch(EditMode)
        {
            case e_HDB_EditState_FirstNib:
            break;
            case e_HDB_EditState_SecondNib:
                CursorX++;
            break;
            case e_HDB_EditStateMAX:
            default:
                EditMode=e_HDB_EditState_FirstNib;
            break;
        }
    }

    CursorY-=TopEdge;
    if(CursorY<0)
        CursorY=0;

    UITC_SetCursorPos(TextDisplayCtrl,CursorX,CursorY);

    if(Cursor_Pos!=Last_Cursor_Pos)
    {
        /* Cursor moved, send event */
        if(HDEventHandler!=NULL)
        {
            HDEvent.EventType=e_HDEvent_CursorMove;
            HDEvent.ID=HDID;
            HDEvent.Info.Cursor.Pos=Cursor_Pos;

            HDEventHandler(&HDEvent);
        }
        Last_Cursor_Pos=Cursor_Pos;
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::GrowBufferAsNeeded
 *
 * SYNOPSIS:
 *    bool HexDisplayBuffer::GrowBufferAsNeeded(int NewBufferSize);
 *
 * PARAMETERS:
 *    NewBufferSize [I] -- The new buffer size
 *
 * FUNCTION:
 *    This function will change the 'Buffer' size.  It will reallocate the
 *    buffer if needed and copy the old data over.
 *
 *    'BufferSize' and others will to updated if successful.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- Could not resize the buffer.  'Buffer' is still valid.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HexDisplayBuffer::GrowBufferAsNeeded(int NewBufferSize)
{
    uint8_t *NewBuffer;

    if(NewBufferSize>BufferAllocatedSize)
    {
        /* Ok we need to allocate a new buffer */
        NewBuffer=(uint8_t *)realloc(Buffer,NewBufferSize+100); // 100 extra byte for us to grow into
        if(NewBuffer==NULL)
            return false;
        BufferAllocatedSize=NewBufferSize+100;
        Buffer=NewBuffer;
    }
    SetNewBufferSize(NewBufferSize);
    StartOfData=Buffer;
    InsertPos=Buffer+BufferSize;

    return true;
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::SetNewBufferSize
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::SetNewBufferSize(int NewSize);
 *
 * PARAMETERS:
 *    NewSize [I] -- The new size for the buffer.
 *
 * FUNCTION:
 *    This function sets the size of the buffer.  It will send events as needed
 *    and update 'BufferSize'
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::SetNewBufferSize(int NewSize)
{
    struct HDEvent HDEvent;

    BufferSize=NewSize;
    BufferBytes2Draw=NewSize;
    RethinkYScrollBar();

    /* Buffer has been resized, send event */
    if(HDEventHandler!=NULL)
    {
        HDEvent.EventType=e_HDEvent_BufferResize;
        HDEvent.ID=HDID;
        HDEvent.Info.Buffer.Size=BufferSize;

        HDEventHandler(&HDEvent);
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::FillSelectionWithValue
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::FillSelectionWithValue(uint8_t Value);
 *
 * PARAMETERS:
 *    Value [I] -- The value to set the selection to
 *
 * FUNCTION:
 *    This function changes the buffers selected bytes to a value (it replaces
 *    the selection with a value).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::FillSelectionWithValue(uint8_t Value)
{
    const uint8_t *SelStartPtr;
    const uint8_t *SelEndPtr;

    if(!InEditMode || !SelectionValid)
        return;

    if(GetSelectionBounds(&SelStartPtr,&SelEndPtr))
    {
        /* Replace the current selection with the value */
        memset((void *)SelStartPtr,Value,SelEndPtr-SelStartPtr);

        RebuildDisplay();
    }
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::FillWithValue
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::FillWithValue(int InsertOffset,const uint8_t *Data,
 *          int Bytes,bool Replace);
 *
 * PARAMETERS:
 *    InsertOffset [I] -- The offset into the buffer to start the insert
 *    Data [I] -- The data to insert/replace
 *    Bytes [I] -- The number of bytes in 'Data'
 *    Replace [I] -- true = replace the bytes, false = insert new bytes
 *
 * FUNCTION:
 *    This function inserts/replaces a number of bytes in the buffer.  This only
 *    works in edit mode.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::FillWithValue(int InsertOffset,const uint8_t *Data,
        int Bytes,bool Replace)
{
    int BytesAfterInsertPoint;
    int PreBufferSize;

    if(!InEditMode)
        return;

    if(InsertOffset>BufferSize)
        return;

    if(!Replace)
    {
        /* Ok, we need to insert space */
        PreBufferSize=BufferSize;   // Save the size of the buffer before we grow

        if(GrowBufferAsNeeded(BufferSize+Bytes))
        {
            BytesAfterInsertPoint=PreBufferSize-InsertOffset;

            memmove(&Buffer[InsertOffset+Bytes],&Buffer[InsertOffset],
                    BytesAfterInsertPoint);
        }
    }
    else
    {
        if(InsertOffset+Bytes>=BufferSize)
        {
            /* We need to make the buffer bigger */
            if(!GrowBufferAsNeeded(InsertOffset+Bytes))
                return;
        }
    }
    memcpy(&Buffer[InsertOffset],Data,Bytes);

    if(!SelectionValid)
        Cursor_Pos+=Bytes;
    else
        Cursor_Pos+=1;  // 1 past the end of the selection
    ClipCursorPos(false);
    MakeOffsetVisable(Cursor_Pos,InAscIIArea,false);
    ClearSelection();
    RebuildDisplay();
    RethinkCursorPos();
}

/*******************************************************************************
 * NAME:
 *    HexDisplayBuffer::GiveFocus
 *
 * SYNOPSIS:
 *    void HexDisplayBuffer::GiveFocus(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gives the hex display input focus.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HexDisplayBuffer::GiveFocus(void)
{
    UITC_SetFocus(TextDisplayCtrl);
}
