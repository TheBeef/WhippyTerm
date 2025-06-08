/*******************************************************************************
 * FILENAME: Frame_CustomTextWidgetAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (05 May 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UICustomTextWidget.h"
#include "Frame_CustomTextWidget.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool UICTW_EventHandler(const struct WTCEvent *Event);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UICTW_AllocCustomTextWidget
 *
 * SYNOPSIS:
 *    t_UICustomTextWidgetCtrl *UICTW_AllocCustomTextWidget(void *ParentWidget,
 *          bool (*EventHandler)(const struct UICTWEvent *Event),
 *          uintptr_t ID);
 *
 * PARAMETERS:
 *    ParentWidget [I] -- The control this display frame lives in
 *    EventHandler [I] -- The event handle for this text area.
 *    ID [I] -- An identifier for this frame
 *
 * FUNCTION:
 *    This function allocates a new copy of the main text area (the contents
 *    of a tab including all the other controls).
 *
 * RETURNS:
 *    A handle to the text area frame.
 *
 * SEE ALSO:
 *    UICTW_FreeMainTextAreaFrame()
 ******************************************************************************/
t_UICustomTextWidgetCtrl *UICTW_AllocCustomTextWidget(void *ParentWidget,
        bool (*EventHandler)(const struct UICTWEvent *Event),uintptr_t ID)
{
    QWidget *QParent=(QWidget *)ParentWidget;
    Frame_CustomTextWidget *NewTextArea;

    NewTextArea=NULL;
    try
    {
        NewTextArea=new Frame_CustomTextWidget(QParent);
        NewTextArea->EventHandler=EventHandler;
        NewTextArea->ID=ID;

        NewTextArea->ui->TextDisplay->SetEventHandler(UICTW_EventHandler,
                (uintptr_t)NewTextArea);

        NewTextArea->Layout=new QHBoxLayout(QParent);
        NewTextArea->Layout->setSpacing(0);
        NewTextArea->Layout->setContentsMargins(0, 0, 0, 0);
        NewTextArea->Layout->addWidget(NewTextArea);

        NewTextArea->show();
    }
    catch(...)
    {
        if(NewTextArea!=NULL)
            UICTW_FreeCustomTextWidget((t_UICustomTextWidgetCtrl *)NewTextArea);
        NewTextArea=NULL;
    }

    return (t_UICustomTextWidgetCtrl *)NewTextArea;
}

/*******************************************************************************
 * NAME:
 *    UICTW_FreeCustomTextWidget
 *
 * SYNOPSIS:
 *    void UICTW_FreeCustomTextWidget(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The display frame to free
 *
 * FUNCTION:
 *    This function frees a display frame that was allocated with
 *    UICTW_AllocMainTextAreaFrame().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UICTW_AllocMainTextAreaFrame()
 ******************************************************************************/
void UICTW_FreeCustomTextWidget(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

//    TextDisplay->ShutDown();

    if(TextDisplay->Layout!=NULL)
        delete TextDisplay->Layout;

    delete TextDisplay;
}

/*******************************************************************************
 * NAME:
 *    UICTW_Reparent
 *
 * SYNOPSIS:
 *    void UICTW_Reparent(t_UICustomTextWidgetCtrl *ctrl,void *NewParentWidget);
 *
 * PARAMETERS:
 *    ctrl [I] -- The display frame to work on
 *    NewParentWidget [I] -- The new control that this display frame lives in
 *
 * FUNCTION:
 *    This function changes what UI control that this display frame is in.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_Reparent(t_UICustomTextWidgetCtrl *ctrl,void *NewParentWidget)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;
    QWidget *QParent=(QWidget *)NewParentWidget;

    TextDisplay->setParent(QParent);

    delete TextDisplay->Layout;

    TextDisplay->Layout=new QHBoxLayout(QParent);
    TextDisplay->Layout->setSpacing(0);
    TextDisplay->Layout->setContentsMargins(0, 0, 0, 0);
    TextDisplay->Layout->addWidget(TextDisplay);
}

t_UIContextMenuCtrl *UICTW_GetContextMenuHandle(t_UICustomTextWidgetCtrl *ctrl,e_UICTW_ContextMenuType UIObj)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    switch(UIObj)
    {
        case e_UICTW_ContextMenu_Copy:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionCopy;
        case e_UICTW_ContextMenu_Paste:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionPaste;
        case e_UICTW_ContextMenu_ClearScreen:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionClear_Screen;
        case e_UICTW_ContextMenu_ZoomIn:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionZoom_In;
        case e_UICTW_ContextMenu_ZoomOut:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionZoom_Out;
        case e_UICTW_ContextMenu_Edit:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionEdit;
        case e_UICTW_ContextMenu_EndianSwap:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionEndian_Swap;
        case e_UICTW_ContextMenu_FindCRCAlgorithm:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionFind_CRC_Algorithm;
        case e_UICTW_ContextMenuMAX:
        default:
            break;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICTW_EventHandler
 *
 * SYNOPSIS:
 *    static bool UICTW_EventHandler(const struct WTCEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The Widget Text Cavnas event
 *
 * FUNCTION:
 *    This function is called from the widget text canvas when there is an
 *    event.
 *
 *    This function will convert the event and call our event handler callback.
 *
 * RETURNS:
 *    true -- Let the event continue
 *    false -- Cancel the event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool UICTW_EventHandler(const struct WTCEvent *Event)
{
    Frame_CustomTextWidget *ThisFrame=(Frame_CustomTextWidget *)Event->UserData;
    struct UICTWEvent NewEvent;

    if(ThisFrame->EventHandler==nullptr)
        return true;

    /* Translate the event */
    NewEvent.ID=ThisFrame->ID;
    NewEvent.Ctrl=(t_UICustomTextWidgetCtrl *)ThisFrame;
    switch(Event->EventType)
    {
        case e_WTCEvent_MouseDown:
            NewEvent.EventType=e_UICTWEvent_MouseDown;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_MouseUp:
            NewEvent.EventType=e_UICTWEvent_MouseUp;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_MouseRightDown:
            ThisFrame->ContextMenu->exec(QCursor::pos());
            return false;
        break;
        case e_WTCEvent_MouseRightUp:
            return false;
        break;
        case e_WTCEvent_MouseMiddleDown:
            NewEvent.EventType=e_UICTWEvent_MouseMiddleDown;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_MouseMiddleUp:
            NewEvent.EventType=e_UICTWEvent_MouseMiddleUp;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_MouseWheel:
            NewEvent.EventType=e_UICTWEvent_MouseWheel;
            NewEvent.Info.MouseWheel.Steps=Event->Info->MouseWheel.Steps;
            NewEvent.Info.MouseWheel.Mods=Event->Info->MouseWheel.Mods;
        break;
        case e_WTCEvent_MouseMove:
            NewEvent.EventType=e_UICTWEvent_MouseMove;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_Resize:
            NewEvent.EventType=e_UICTWEvent_Resize;
            NewEvent.Info.NewSize.Width=Event->Info->NewSize.Width;
            NewEvent.Info.NewSize.Height=Event->Info->NewSize.Height;
        break;
        case e_WTCEvent_LostFocus:
            NewEvent.EventType=e_UICTWEvent_LostFocus;
        break;
        case e_WTCEvent_GotFocus:
            NewEvent.EventType=e_UICTWEvent_GotFocus;
        break;
        case e_WTCEvent_KeyEvent:
            NewEvent.EventType=e_UICTWEvent_KeyEvent;
            NewEvent.Info.Key.Mods=Event->Info->Key.Mods;
            NewEvent.Info.Key.Key=Event->Info->Key.Key;
            NewEvent.Info.Key.TextPtr=Event->Info->Key.TextPtr;
            NewEvent.Info.Key.TextLen=Event->Info->Key.TextLen;
        break;
        case e_WTCEventMAX:
        default:
            return true;
    }
    return ThisFrame->EventHandler(&NewEvent);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetFont
 *
 * SYNOPSIS:
 *    void UICTW_SetFont(t_UICustomTextWidgetCtrl *ctrl,const char *FontName,int Size,
 *              bool Bold,bool Italic);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control to work on
 *    FontName [I] -- The font to use
 *    Size [I] -- The size of the font (height)
 *    Bold [I] -- Use the bold version
 *    Italic [I] -- Use the italic version
 *
 * FUNCTION:
 *    This function sets what font is used for this text display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetFont(t_UICustomTextWidgetCtrl *ctrl,const char *FontName,int Size,
        bool Bold,bool Italic)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetFont(FontName,Size,Bold,Italic);
}

/*******************************************************************************
 * NAME:
 *    UICTW_ClearAllLines
 *
 * SYNOPSIS:
 *    void UICTW_ClearAllLines(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function clears all the lines from the control.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_ClearAllLines(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->ClearAllLines();
}

/*******************************************************************************
 * NAME:
 *    UICTW_Begin
 *
 * SYNOPSIS:
 *    void UICTW_Begin(t_UICustomTextWidgetCtrl *ctrl,int Line);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Line [I] -- What screen line to work on
 *
 * FUNCTION:
 *    This function starts the control to work on a line.  This is done so
 *    we can only draw the line when we are finished.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UICTW_End(), UICTW_ClearLine(), UICTW_AddFragment()
 ******************************************************************************/
void UICTW_Begin(t_UICustomTextWidgetCtrl *ctrl,int Line)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->WorkingLine=Line;
    /* DEBUG PAUL: Setup any pointers to the line we are going to work on here */
}

/*******************************************************************************
 * NAME:
 *    UICTW_End
 *
 * SYNOPSIS:
 *    void UICTW_End(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function ends the changes we are making to a line on the screen.
 *    This will redraw this line.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UICTW_Begin()
 ******************************************************************************/
void UICTW_End(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->RedrawLine(TextDisplay->WorkingLine);
}

/*******************************************************************************
 * NAME:
 *    UICTW_ClearLine
 *
 * SYNOPSIS:
 *    void UICTW_ClearLine(t_UICustomTextWidgetCtrl *ctrl,uint32_t BGColor);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    BGColor [I] -- The background color for this line.
 *
 * FUNCTION:
 *    This function clears a line.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UICTW_Begin()
 ******************************************************************************/
void UICTW_ClearLine(t_UICustomTextWidgetCtrl *ctrl,uint32_t BGColor)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->ClearLine(TextDisplay->WorkingLine,BGColor);
}

/*******************************************************************************
 * NAME:
 *    UICTW_AddFragment
 *
 * SYNOPSIS:
 *    void UICTW_AddFragment(t_UICustomTextWidgetCtrl *ctrl,
 *          const struct TextCanvasFrag *Frag);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Frag [I] -- The fragment to add to the screen
 *
 * FUNCTION:
 *    This function adds a fragment to the screen (copies the fragment and
 *    setups up the copy for rendering).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UICTW_Begin()
 ******************************************************************************/
void UICTW_AddFragment(t_UICustomTextWidgetCtrl *ctrl,const struct TextCanvasFrag *Frag)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->AppendTextFrag(TextDisplay->WorkingLine,Frag);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetCursorColor
 *
 * SYNOPSIS:
 *    void UICTW_SetCursorColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t Color);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Color [I] -- The new color for the cursor
 *
 * FUNCTION:
 *    This function changes the color of the cursor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetCursorColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t Color)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->CursorColor=Color;
    TextDisplay->ui->TextDisplay->RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetCursorBlinking
 *
 * SYNOPSIS:
 *    void UICTW_SetCursorBlinking(t_UICustomTextWidgetCtrl *ctrl,bool Blinking);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Blinking [I] -- Is the cursor currently blinking (true) or not (false)
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
void UICTW_SetCursorBlinking(t_UICustomTextWidgetCtrl *ctrl,bool Blinking)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetCursorBlinking(Blinking);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetCursorStyle
 *
 * SYNOPSIS:
 *    void UICTW_SetCursorStyle(t_UICustomTextWidgetCtrl *ctrl,
 *          e_TextCursorStyleType Style);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Style [I] -- What style to draw the cursor in
 *
 * FUNCTION:
 *    This function changes what style the cursor is drawen as
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetCursorStyle(t_UICustomTextWidgetCtrl *ctrl,e_TextCursorStyleType Style)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->ChangeCursorStyle(Style);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetCursorPos
 *
 * SYNOPSIS:
 *    void UICTW_SetCursorPos(t_UICustomTextWidgetCtrl *ctrl,unsigned int x,
 *          unsigned int y);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    x [I] -- The new X pos of the cursor in this display
 *    y [I] -- The new Y pos of the cursor in this display
 *
 * FUNCTION:
 *    This function sets the cursor position in the display.  It is relative 
 *    to top,left corner of the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetCursorPos(t_UICustomTextWidgetCtrl *ctrl,unsigned int x,unsigned int y)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetCursorPos(x,y);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetFocus
 *
 * SYNOPSIS:
 *    void UICTW_SetFocus(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function gives this text display control focus.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetFocus(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->setFocus(Qt::OtherFocusReason);
}

/*******************************************************************************
 * NAME:
 *    UICTW_GetFragWidth
 *
 * SYNOPSIS:
 *    int UICTW_GetFragWidth(t_UICustomTextWidgetCtrl *ctrl,
 *              const struct TextCanvasFrag *Frag);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Frag [I] -- The frag to get the render width of
 *
 * FUNCTION:
 *    This function gets the width of a fragment.
 *
 * RETURNS:
 *    The number of px this frag will take on screen.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int UICTW_GetFragWidth(t_UICustomTextWidgetCtrl *ctrl,const struct TextCanvasFrag *Frag)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    return TextDisplay->ui->TextDisplay->GetTextWidth(Frag);
}

/*******************************************************************************
 * NAME:
 *    UICTW_GetWidgetWidth
 *
 * SYNOPSIS:
 *    int UICTW_GetWidgetWidth(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function gets the size of the widget in pixels.
 *
 * RETURNS:
 *    The width of the widget
 *
 * LIMITATIONS:
 *    This maybe 0 if the widget hasn't been drawen on the screen
 *
 * SEE ALSO:
 *    UICTW_GetWidgetHeight()
 ******************************************************************************/
int UICTW_GetWidgetWidth(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    return TextDisplay->ui->TextDisplay->GetWidgetWidth();
}

/*******************************************************************************
 * NAME:
 *    UICTW_GetWidgetHeight
 *
 * SYNOPSIS:
 *    int UICTW_GetWidgetHeight(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function gets the size of the widget in pixels.
 *
 * RETURNS:
 *    The height of the widget
 *
 * LIMITATIONS:
 *    This maybe 0 if the widget hasn't been drawen on the screen
 *
 * SEE ALSO:
 *    UICTW_GetWidgetWidth()
 ******************************************************************************/
int UICTW_GetWidgetHeight(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    return TextDisplay->ui->TextDisplay->GetWidgetHeight();
}

/*******************************************************************************
 * NAME:
 *    UICTW_GetCharPxWidth
 *
 * SYNOPSIS:
 *    int UICTW_GetCharPxWidth(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function gets the average width of a char.
 *
 * RETURNS:
 *    The width of a char in pixels.
 *
 * SEE ALSO:
 *    UICTW_GetCharPxHeight()
 ******************************************************************************/
int UICTW_GetCharPxWidth(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    return TextDisplay->ui->TextDisplay->GetCharPxWidth();
}

/*******************************************************************************
 * NAME:
 *    UICTW_GetCharPxHeight
 *
 * SYNOPSIS:
 *    int UICTW_GetCharPxHeight(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function gets the average height of a char.
 *
 * RETURNS:
 *    The height of a char in pixels.
 *
 * SEE ALSO:
 *    UICTW_GetCharPxWidth()
 ******************************************************************************/
int UICTW_GetCharPxHeight(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    return TextDisplay->ui->TextDisplay->GetCharPxHeight();
}

/*******************************************************************************
 * NAME:
 *    UICTW_GetHorzSlider
 *
 * SYNOPSIS:
 *    t_UIScrollBarCtrl *UICTW_GetHorzSlider(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function gets the sub-widget for the horz slider
 *
 * RETURNS:
 *    A pointer to the horz slider.
 *
 * SEE ALSO:
 *    UICTW_GetVertSlider()
 ******************************************************************************/
t_UIScrollBarCtrl *UICTW_GetHorzSlider(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    return (t_UIScrollBarCtrl *)TextDisplay->ui->TexthorizontalScrollBar;

}

/*******************************************************************************
 * NAME:
 *    UICTW_GetVertSlider
 *
 * SYNOPSIS:
 *    t_UIScrollBarCtrl *UICTW_GetVertSlider(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function gets the sub-widget for the vert slider
 *
 * RETURNS:
 *    A pointer to the vert slider.
 *
 * SEE ALSO:
 *    UICTW_GetHorzSlider()
 ******************************************************************************/
t_UIScrollBarCtrl *UICTW_GetVertSlider(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    return (t_UIScrollBarCtrl *)TextDisplay->ui->TextverticalScrollBar;
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetXOffset
 *
 * SYNOPSIS:
 *    void UICTW_SetXOffset(t_UICustomTextWidgetCtrl *ctrl,int XOffsetPx);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    XOffsetPx [I] -- The number of pixels to skip off the left side of
 *                     text.
 *
 * FUNCTION:
 *    This function sets the X offset we are appling to the text when we draw
 *    it.  That is to say the number of pixels to skip before drawing the
 *    text lines.
 *
 *    The effect of this is that we offset the text by -XOffsetPx to the
 *    left of the draw area (we clip the text)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetXOffset(t_UICustomTextWidgetCtrl *ctrl,int XOffsetPx)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetXOffsetPx(XOffsetPx);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetMaxLines
 *
 * SYNOPSIS:
 *    void UICTW_SetMaxLines(t_UICustomTextWidgetCtrl *ctrl,int MaxLines,
 *          uint32_t BGColor);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    MaxLiners [I] -- The max number of lines to draw.
 *    BGColor [I] -- The background color for these lines.
 *
 * FUNCTION:
 *    This function sets the max number of lines that we will drawn on repaint.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetMaxLines(t_UICustomTextWidgetCtrl *ctrl,int MaxLines,uint32_t BGColor)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetMaxLines(MaxLines,BGColor);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetClippingWindow
 *
 * SYNOPSIS:
 *    void UICTW_SetClippingWindow(t_UICustomTextWidgetCtrl *ctrl,int LeftEdge,
 *          int TopEdge,int Width,int Height);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    LeftEdge [I] -- The left edge of the window box
 *    TopEdge [I] -- The top edge of the window box
 *    Width [I] -- The width of the window box
 *    Height [I] -- The height of the window box
 *
 * FUNCTION:
 *    This function sets the window box that the text will be drawn inside.
 *    The text will be offset from the edge of the widget and clipping
 *    will be set to only draw inside the width and height.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetClippingWindow(t_UICustomTextWidgetCtrl *ctrl,int LeftEdge,int TopEdge,
        int Width,int Height)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetDisplaySize(LeftEdge,TopEdge,Width,Height);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetTextAreaBackgroundColor
 *
 * SYNOPSIS:
 *    void UICTW_SetTextAreaBackgroundColor(t_UICustomTextWidgetCtrl *ctrl,
 *              uint32_t BgColor);
 *
 * PARAMETERS:
 *    BgColor [I] -- The background fill color
 *
 * FUNCTION:
 *    This function sets the color that any part of the text area that doesn't
 *    have any lines in it are fill with.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetTextAreaBackgroundColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t BgColor)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetTextAreaBackgroundColor(BgColor);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetTextDefaultColor
 *
 * SYNOPSIS:
 *    void UICTW_SetTextDefaultColor(t_UICustomTextWidgetCtrl *ctrl,
 *              uint32_t FgColor);
 *
 * PARAMETERS:
 *    BgColor [I] -- The background fill color
 *
 * FUNCTION:
 *    This function sets the default color that will be used for the text.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetTextDefaultColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t FgColor)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetTextDefaultColor(FgColor);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetBorderBackgroundColor
 *
 * SYNOPSIS:
 *    void UICTW_SetBorderBackgroundColor(t_UICustomTextWidgetCtrl *ctrl,
 *              uint32_t BgColor,bool Fill);
 *
 * PARAMETERS:
 *    BgColor [I] -- The background fill color
 *    Fill [I] -- True = fill the border area, false = just ignore the border
 *                       screen area.
 *
 * FUNCTION:
 *    This function sets the fill color around the text area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetBorderBackgroundColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t BgColor,
        bool Fill)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetDisplayBackgroundColor(BgColor,Fill);
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetOverrideMsg
 *
 * SYNOPSIS:
 *    void UICTW_SetOverrideMsg(t_UICustomTextWidgetCtrl *ctrl,const char *Msg,
 *              bool OnOff);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Msg [I] -- The message to display.
 *    OnOff [I] -- True = Show the override box, false = Hide the orverride box
 *
 * FUNCTION:
 *    This function shows the override box and sets it's text.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetOverrideMsg(t_UICustomTextWidgetCtrl *ctrl,const char *Msg,bool OnOff)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetOverrideMsg(Msg,OnOff);
}

/*******************************************************************************
 * NAME:
 *    UICTW_RedrawScreen
 *
 * SYNOPSIS:
 *    void UICTW_RedrawScreen(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function redraws the control.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_RedrawScreen(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetDrawMask
 *
 * SYNOPSIS:
 *    void UICTW_SetDrawMask(t_UICustomTextWidgetCtrl *ctrl,uint16_t Mask);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Mask [I] -- What mask to apply to drawing attributes.  If the bit is
 *                set then this is drawen.
 *
 * FUNCTION:
 *    This function sets the mask of what attributes are drawen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetDrawMask(t_UICustomTextWidgetCtrl *ctrl,uint16_t Mask)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetDrawMask(Mask);
}

/*******************************************************************************
 * NAME:
 *    UICTW_ShowBellIcon
 *
 * SYNOPSIS:
 *    void UICTW_ShowBellIcon(t_UICustomTextWidgetCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *
 * FUNCTION:
 *    This function tells the text area to show the bell icon for a while then
 *    clear it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_ShowBellIcon(t_UICustomTextWidgetCtrl *ctrl)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->ShowBell();
}

/*******************************************************************************
 * NAME:
 *    UICTW_SetMouseCursor
 *
 * SYNOPSIS:
 *    void UICTW_SetMouseCursor(t_UICustomTextWidgetCtrl *ctrl,
 *              e_UIMouse_CursorType Cursor);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Cursor [I] -- What mouse pointer to use
 *
 * FUNCTION:
 *    This function changes the mouse pointer when on this widget.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UICTW_SetMouseCursor(t_UICustomTextWidgetCtrl *ctrl,e_UIMouse_CursorType Cursor)
{
    Frame_CustomTextWidget *TextDisplay=(Frame_CustomTextWidget *)ctrl;

    TextDisplay->ui->TextDisplay->SetMouseCursor(Cursor);
}
