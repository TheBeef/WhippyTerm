/*******************************************************************************
 * FILENAME: Frame_MainTextAreaAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (07 Aug 2023)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UITextMainArea.h"
#include "Frame_MainTextArea.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool UITC_EventHandler(const struct WTCEvent *Event);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UITC_AllocTextDisplay
 *
 * SYNOPSIS:
 *    t_UITextDisplayCtrl *UITC_AllocTextDisplay(void *ParentWidget,
 *          bool (*EventHandler)(const struct TextDisplayEvent *Event),
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
 *    UITC_FreeMainTextAreaFrame()
 ******************************************************************************/
t_UITextDisplayCtrl *UITC_AllocTextDisplay(void *ParentWidget,
        bool (*EventHandler)(const struct TextDisplayEvent *Event),uintptr_t ID)
{
    QWidget *QParent=(QWidget *)ParentWidget;
    Frame_MainTextArea *NewTextArea;

    NewTextArea=NULL;
    try
    {
        NewTextArea=new Frame_MainTextArea(QParent);
        NewTextArea->EventHandler=EventHandler;
        NewTextArea->ID=ID;

        NewTextArea->ui->TextDisplay->SetEventHandler(UITC_EventHandler,
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
            UITC_FreeTextDisplay((t_UITextDisplayCtrl *)NewTextArea);
        NewTextArea=NULL;
    }

    return (t_UITextDisplayCtrl *)NewTextArea;
}

/*******************************************************************************
 * NAME:
 *    UITC_FreeTextDisplay
 *
 * SYNOPSIS:
 *    void UITC_FreeTextDisplay(t_UITextDisplayCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- The display frame to free
 *
 * FUNCTION:
 *    This function frees a display frame that was allocated with
 *    UITC_AllocMainTextAreaFrame().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UITC_AllocMainTextAreaFrame()
 ******************************************************************************/
void UITC_FreeTextDisplay(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

//    TextDisplay->ShutDown();

    if(TextDisplay->Layout!=NULL)
        delete TextDisplay->Layout;

    delete TextDisplay;
}

/*******************************************************************************
 * NAME:
 *    UITC_Reparent
 *
 * SYNOPSIS:
 *    void UITC_Reparent(t_UITextDisplayCtrl *ctrl,void *NewParentWidget);
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
void UITC_Reparent(t_UITextDisplayCtrl *ctrl,void *NewParentWidget)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;
    QWidget *QParent=(QWidget *)NewParentWidget;

    TextDisplay->setParent(QParent);

    delete TextDisplay->Layout;

    TextDisplay->Layout=new QHBoxLayout(QParent);
    TextDisplay->Layout->setSpacing(0);
    TextDisplay->Layout->setContentsMargins(0, 0, 0, 0);
    TextDisplay->Layout->addWidget(TextDisplay);
}

t_UIContextMenuCtrl *UITC_GetContextMenuHandle(t_UITextDisplayCtrl *ctrl,e_UITD_ContextMenuType UIObj)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    switch(UIObj)
    {
        case e_UITD_ContextMenu_SendBuffers:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionSend_Buffer;
        case e_UITD_ContextMenu_FindCRCAlgorithm:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionFind_CRC_Algorithm;
        case e_UITD_ContextMenu_Copy:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionCopy;
        case e_UITD_ContextMenu_Paste:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionPaste;
        case e_UITD_ContextMenu_ClearScreen:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionClear_Screen;
        case e_UITD_ContextMenu_ZoomIn:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionZoom_In;
        case e_UITD_ContextMenu_ZoomOut:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionZoom_Out;
        case e_UITD_ContextMenu_Edit:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionEdit;
        case e_UITD_ContextMenu_EndianSwap:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionEndian_Swap;
        case e_UITD_ContextMenu_Bold:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBold;
        case e_UITD_ContextMenu_Italics:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleItalics;
        case e_UITD_ContextMenu_Underline:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleUnderline;
        case e_UITD_ContextMenu_StrikeThrough:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleStrike_Through;
        case e_UITD_ContextMenu_StyleBGColor_Black:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Black;
        case e_UITD_ContextMenu_StyleBGColor_Blue:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Blue;
        case e_UITD_ContextMenu_StyleBGColor_Green:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Green;
        case e_UITD_ContextMenu_StyleBGColor_Cyan:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Cyan;
        case e_UITD_ContextMenu_StyleBGColor_Red:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Red;
        case e_UITD_ContextMenu_StyleBGColor_Magenta:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Magenta;
        case e_UITD_ContextMenu_StyleBGColor_Brown:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Brown;
        case e_UITD_ContextMenu_StyleBGColor_White:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_White;
        case e_UITD_ContextMenu_StyleBGColor_Gray:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Gray;
        case e_UITD_ContextMenu_StyleBGColor_LightBlue:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_LightBlue;
        case e_UITD_ContextMenu_StyleBGColor_LightGreen:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_LightGreen;
        case e_UITD_ContextMenu_StyleBGColor_LightCyan:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_LightCyan;
        case e_UITD_ContextMenu_StyleBGColor_LightRed:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_LightRed;
        case e_UITD_ContextMenu_StyleBGColor_LightMagenta:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_LightMagenta;
        case e_UITD_ContextMenu_StyleBGColor_Yellow:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_Yellow;
        case e_UITD_ContextMenu_StyleBGColor_BrightWhite:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionStyleBGColor_BrightWhite;

        case e_UITD_ContextMenuMAX:
        default:
            break;
    }
    return NULL;
}

t_UIContextSubMenuCtrl *UITC_GetContextSubMenuHandle(t_UITextDisplayCtrl *ctrl,e_UITD_ContextSubMenuType UIObj)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    switch(UIObj)
    {
        case e_UITD_ContextSubMenu_BGColor:
            return (t_UIContextSubMenuCtrl *)TextDisplay->ColorBGSubmenu;
        case e_UITD_ContextSubMenuMAX:
        default:
            break;
    }
    return NULL;
}

t_UIButtonCtrl *UITC_GetButtonHandle(t_UITextDisplayCtrl *ctrl,e_UITC_BttnType Bttn)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    switch(Bttn)
    {
        case e_UITC_Bttn_Send:
            return (t_UIButtonCtrl *)TextDisplay->ui->BlockSendSend_pushButton;
        case e_UITC_Bttn_HexEdit:
            return (t_UIButtonCtrl *)TextDisplay->ui->BlockSendHex_Edit_pushButton;
        case e_UITC_Bttn_Clear:
            return (t_UIButtonCtrl *)TextDisplay->ui->BlockSendHex_Clear_pushButton;
        case e_UITC_BttnMAX:
        default:
            break;
    }
    return NULL;
}

t_UITextInputCtrl *UITC_GetTextInputHandle(t_UITextDisplayCtrl *ctrl,e_UITC_TxtType Txt)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    switch(Txt)
    {
        case e_UITC_Txt_Pos:
            return (t_UITextInputCtrl *)TextDisplay->ui->BlockSendHex_Position_lineEdit;
        case e_UITC_TxtMAX:
        default:
            break;
    }
    return NULL;
}

t_UIMuliLineTextInputCtrl *UITC_GetMuliLineTextInputHandle(t_UITextDisplayCtrl *ctrl,e_UITC_MuliTxtType Txt)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    switch(Txt)
    {
        case e_UITC_MuliTxt_TextInput:
            return (t_UIMuliLineTextInputCtrl *)TextDisplay->ui->BlockSend_textEdit;
        case e_UITC_MuliTxtMAX:
        default:
            break;
    }
    return NULL;
}

t_UIRadioBttnCtrl *UITC_GetRadioButton(t_UITextDisplayCtrl *ctrl,e_UITC_RadioButtonType bttn)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    switch(bttn)
    {
        case e_UITC_RadioButton_Text:
            return (t_UIRadioBttnCtrl *)TextDisplay->ui->BlockSend_TextMode_radioButton;
        case e_UITC_RadioButton_Hex:
            return (t_UIRadioBttnCtrl *)TextDisplay->ui->BlockSend_HexMode_radioButton;
        case e_UITC_RadioButtonMAX:
        default:
            break;
    }
    return NULL;
}

t_UIComboBoxCtrl *UITC_GetComboBoxHandle(t_UITextDisplayCtrl *ctrl,e_UITC_ComboxType UIObj)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    switch(UIObj)
    {
        case e_UITC_Combox_LineEnd:
            return (t_UIComboBoxCtrl *)TextDisplay->ui->BlockSend_LineEnd_comboBox;
        case e_UITC_ComboxMAX:
        default:
            break;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UITC_EventHandler
 *
 * SYNOPSIS:
 *    static bool UITC_EventHandler(const struct WTCEvent *Event);
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
static bool UITC_EventHandler(const struct WTCEvent *Event)
{
    Frame_MainTextArea *ThisFrame=(Frame_MainTextArea *)Event->UserData;
    struct TextDisplayEvent NewEvent;

    if(ThisFrame->EventHandler==nullptr)
        return true;

    /* Translate the event */
    NewEvent.ID=ThisFrame->ID;
    NewEvent.Ctrl=(t_UITextDisplayCtrl *)ThisFrame;
    switch(Event->EventType)
    {
        case e_WTCEvent_MouseDown:
            NewEvent.EventType=e_TextDisplayEvent_MouseDown;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_MouseUp:
            NewEvent.EventType=e_TextDisplayEvent_MouseUp;
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
            NewEvent.EventType=e_TextDisplayEvent_MouseMiddleDown;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_MouseMiddleUp:
            NewEvent.EventType=e_TextDisplayEvent_MouseMiddleUp;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_MouseWheel:
            NewEvent.EventType=e_TextDisplayEvent_MouseWheel;
            NewEvent.Info.MouseWheel.Steps=Event->Info->MouseWheel.Steps;
            NewEvent.Info.MouseWheel.Mods=Event->Info->MouseWheel.Mods;
        break;
        case e_WTCEvent_MouseMove:
            NewEvent.EventType=e_TextDisplayEvent_MouseMove;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_Resize:
            NewEvent.EventType=e_TextDisplayEvent_Resize;
            NewEvent.Info.NewSize.Width=Event->Info->NewSize.Width;
            NewEvent.Info.NewSize.Height=Event->Info->NewSize.Height;
        break;
        case e_WTCEvent_LostFocus:
            NewEvent.EventType=e_TextDisplayEvent_LostFocus;
        break;
        case e_WTCEvent_GotFocus:
            NewEvent.EventType=e_TextDisplayEvent_GotFocus;
        break;
        case e_WTCEvent_KeyEvent:
            NewEvent.EventType=e_TextDisplayEvent_KeyEvent;
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
 *    UITC_ShowSendPanel
 *
 * SYNOPSIS:
 *    void UITC_ShowSendPanel(t_UITextDisplayCtrl *ctrl,bool Visible);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control to work on
 *    Visible [I] -- true = show the panel, false = hide the panel.
 *
 * FUNCTION:
 *    This function shows/hides the send panel (the text input with the
 *    send button beside it).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UITC_ShowSendPanel(t_UITextDisplayCtrl *ctrl,bool Visible)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->BlockSend_GroupBox->setVisible(Visible);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetFont
 *
 * SYNOPSIS:
 *    void UITC_SetFont(t_UITextDisplayCtrl *ctrl,const char *FontName,int Size,
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
void UITC_SetFont(t_UITextDisplayCtrl *ctrl,const char *FontName,int Size,
        bool Bold,bool Italic)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetFont(FontName,Size,Bold,Italic);
}

/*******************************************************************************
 * NAME:
 *    UITC_ClearAllLines
 *
 * SYNOPSIS:
 *    void UITC_ClearAllLines(t_UITextDisplayCtrl *ctrl);
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
void UITC_ClearAllLines(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->ClearAllLines();
}

/*******************************************************************************
 * NAME:
 *    UITC_Begin
 *
 * SYNOPSIS:
 *    void UITC_Begin(t_UITextDisplayCtrl *ctrl,int Line);
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
 *    UITC_End(), UITC_ClearLine(), UITC_AddFragment()
 ******************************************************************************/
void UITC_Begin(t_UITextDisplayCtrl *ctrl,int Line)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->WorkingLine=Line;
    /* DEBUG PAUL: Setup any pointers to the line we are going to work on here */
}

/*******************************************************************************
 * NAME:
 *    UITC_End
 *
 * SYNOPSIS:
 *    void UITC_End(t_UITextDisplayCtrl *ctrl);
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
 *    UITC_Begin()
 ******************************************************************************/
void UITC_End(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->RedrawLine(TextDisplay->WorkingLine);
}

/*******************************************************************************
 * NAME:
 *    UITC_ClearLine
 *
 * SYNOPSIS:
 *    void UITC_ClearLine(t_UITextDisplayCtrl *ctrl,uint32_t BGColor);
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
 *    UITC_Begin()
 ******************************************************************************/
void UITC_ClearLine(t_UITextDisplayCtrl *ctrl,uint32_t BGColor)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->ClearLine(TextDisplay->WorkingLine,BGColor);
}

/*******************************************************************************
 * NAME:
 *    UITC_AddFragment
 *
 * SYNOPSIS:
 *    void UITC_AddFragment(t_UITextDisplayCtrl *ctrl,
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
 *    UITC_Begin()
 ******************************************************************************/
void UITC_AddFragment(t_UITextDisplayCtrl *ctrl,const struct TextCanvasFrag *Frag)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->AppendTextFrag(TextDisplay->WorkingLine,Frag);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetCursorColor
 *
 * SYNOPSIS:
 *    void UITC_SetCursorColor(t_UITextDisplayCtrl *ctrl,uint32_t Color);
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
void UITC_SetCursorColor(t_UITextDisplayCtrl *ctrl,uint32_t Color)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->CursorColor=Color;
    TextDisplay->ui->TextDisplay->RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    UITC_SetCursorBlinking
 *
 * SYNOPSIS:
 *    void UITC_SetCursorBlinking(t_UITextDisplayCtrl *ctrl,bool Blinking);
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
void UITC_SetCursorBlinking(t_UITextDisplayCtrl *ctrl,bool Blinking)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetCursorBlinking(Blinking);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetCursorStyle
 *
 * SYNOPSIS:
 *    void UITC_SetCursorStyle(t_UITextDisplayCtrl *ctrl,
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
void UITC_SetCursorStyle(t_UITextDisplayCtrl *ctrl,e_TextCursorStyleType Style)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->ChangeCursorStyle(Style);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetCursorPos
 *
 * SYNOPSIS:
 *    void UITC_SetCursorPos(t_UITextDisplayCtrl *ctrl,unsigned int x,
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
void UITC_SetCursorPos(t_UITextDisplayCtrl *ctrl,unsigned int x,unsigned int y)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetCursorPos(x,y);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetFocus
 *
 * SYNOPSIS:
 *    void UITC_SetFocus(t_UITextDisplayCtrl *ctrl,e_UITCSetFocusType What);
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
void UITC_SetFocus(t_UITextDisplayCtrl *ctrl,e_UITCSetFocusType What)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    switch(What)
    {
        case e_UITCSetFocus_Main:
            TextDisplay->ui->TextDisplay->setFocus(Qt::OtherFocusReason);
        break;
        case e_UITCSetFocus_SendPanel:
            TextDisplay->ui->BlockSend_textEdit->setFocus(Qt::OtherFocusReason);
        break;
        case e_UITCSetFocusMAX:
        default:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    UITC_GetFragWidth
 *
 * SYNOPSIS:
 *    int UITC_GetFragWidth(t_UITextDisplayCtrl *ctrl,
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
int UITC_GetFragWidth(t_UITextDisplayCtrl *ctrl,const struct TextCanvasFrag *Frag)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return TextDisplay->ui->TextDisplay->GetTextWidth(Frag);
}

/*******************************************************************************
 * NAME:
 *    UITC_GetWidgetWidth
 *
 * SYNOPSIS:
 *    int UITC_GetWidgetWidth(t_UITextDisplayCtrl *ctrl);
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
 *    UITC_GetWidgetHeight()
 ******************************************************************************/
int UITC_GetWidgetWidth(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return TextDisplay->ui->TextDisplay->GetWidgetWidth();
}

/*******************************************************************************
 * NAME:
 *    UITC_GetWidgetHeight
 *
 * SYNOPSIS:
 *    int UITC_GetWidgetHeight(t_UITextDisplayCtrl *ctrl);
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
 *    UITC_GetWidgetWidth()
 ******************************************************************************/
int UITC_GetWidgetHeight(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return TextDisplay->ui->TextDisplay->GetWidgetHeight();
}

/*******************************************************************************
 * NAME:
 *    UITC_GetCharPxWidth
 *
 * SYNOPSIS:
 *    int UITC_GetCharPxWidth(t_UITextDisplayCtrl *ctrl);
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
 *    UITC_GetCharPxHeight()
 ******************************************************************************/
int UITC_GetCharPxWidth(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return TextDisplay->ui->TextDisplay->GetCharPxWidth();
}

/*******************************************************************************
 * NAME:
 *    UITC_GetCharPxHeight
 *
 * SYNOPSIS:
 *    int UITC_GetCharPxHeight(t_UITextDisplayCtrl *ctrl);
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
 *    UITC_GetCharPxWidth()
 ******************************************************************************/
int UITC_GetCharPxHeight(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return TextDisplay->ui->TextDisplay->GetCharPxHeight();
}

/*******************************************************************************
 * NAME:
 *    UITC_GetHorzSlider
 *
 * SYNOPSIS:
 *    t_UIScrollBarCtrl *UITC_GetHorzSlider(t_UITextDisplayCtrl *ctrl);
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
 *    UITC_GetVertSlider()
 ******************************************************************************/
t_UIScrollBarCtrl *UITC_GetHorzSlider(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return (t_UIScrollBarCtrl *)TextDisplay->ui->TexthorizontalScrollBar;

}

/*******************************************************************************
 * NAME:
 *    UITC_GetVertSlider
 *
 * SYNOPSIS:
 *    t_UIScrollBarCtrl *UITC_GetVertSlider(t_UITextDisplayCtrl *ctrl);
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
 *    UITC_GetHorzSlider()
 ******************************************************************************/
t_UIScrollBarCtrl *UITC_GetVertSlider(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return (t_UIScrollBarCtrl *)TextDisplay->ui->TextverticalScrollBar;
}

/*******************************************************************************
 * NAME:
 *    UITC_SetXOffset
 *
 * SYNOPSIS:
 *    void UITC_SetXOffset(t_UITextDisplayCtrl *ctrl,int XOffsetPx);
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
void UITC_SetXOffset(t_UITextDisplayCtrl *ctrl,int XOffsetPx)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetXOffsetPx(XOffsetPx);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetMaxLines
 *
 * SYNOPSIS:
 *    void UITC_SetMaxLines(t_UITextDisplayCtrl *ctrl,int MaxLines,
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
void UITC_SetMaxLines(t_UITextDisplayCtrl *ctrl,int MaxLines,uint32_t BGColor)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetMaxLines(MaxLines,BGColor);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetClippingWindow
 *
 * SYNOPSIS:
 *    void UITC_SetClippingWindow(t_UITextDisplayCtrl *ctrl,int LeftEdge,
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
void UITC_SetClippingWindow(t_UITextDisplayCtrl *ctrl,int LeftEdge,int TopEdge,
        int Width,int Height)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetDisplaySize(LeftEdge,TopEdge,Width,Height);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetTextAreaBackgroundColor
 *
 * SYNOPSIS:
 *    void UITC_SetTextAreaBackgroundColor(t_UITextDisplayCtrl *ctrl,
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
void UITC_SetTextAreaBackgroundColor(t_UITextDisplayCtrl *ctrl,uint32_t BgColor)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetTextAreaBackgroundColor(BgColor);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetTextDefaultColor
 *
 * SYNOPSIS:
 *    void UITC_SetTextDefaultColor(t_UITextDisplayCtrl *ctrl,
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
void UITC_SetTextDefaultColor(t_UITextDisplayCtrl *ctrl,uint32_t FgColor)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetTextDefaultColor(FgColor);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetBorderBackgroundColor
 *
 * SYNOPSIS:
 *    void UITC_SetBorderBackgroundColor(t_UITextDisplayCtrl *ctrl,
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
void UITC_SetBorderBackgroundColor(t_UITextDisplayCtrl *ctrl,uint32_t BgColor,
        bool Fill)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetDisplayBackgroundColor(BgColor,Fill);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetOverrideMsg
 *
 * SYNOPSIS:
 *    void UITC_SetOverrideMsg(t_UITextDisplayCtrl *ctrl,const char *Msg,
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
void UITC_SetOverrideMsg(t_UITextDisplayCtrl *ctrl,const char *Msg,bool OnOff)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetOverrideMsg(Msg,OnOff);
}

/*******************************************************************************
 * NAME:
 *    UITC_RedrawScreen
 *
 * SYNOPSIS:
 *    void UITC_RedrawScreen(t_UITextDisplayCtrl *ctrl);
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
void UITC_RedrawScreen(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    UITC_SetDrawMask
 *
 * SYNOPSIS:
 *    void UITC_SetDrawMask(t_UITextDisplayCtrl *ctrl,uint16_t Mask);
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
void UITC_SetDrawMask(t_UITextDisplayCtrl *ctrl,uint16_t Mask)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetDrawMask(Mask);
}

/*******************************************************************************
 * NAME:
 *    UITC_ShowBellIcon
 *
 * SYNOPSIS:
 *    void UITC_ShowBellIcon(t_UITextDisplayCtrl *ctrl);
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
void UITC_ShowBellIcon(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->ShowBell();
}

/*******************************************************************************
 * NAME:
 *    UITC_SetMouseCursor
 *
 * SYNOPSIS:
 *    void UITC_SetMouseCursor(t_UITextDisplayCtrl *ctrl,
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
void UITC_SetMouseCursor(t_UITextDisplayCtrl *ctrl,e_UIMouse_CursorType Cursor)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    TextDisplay->ui->TextDisplay->SetMouseCursor(Cursor);
}

/*******************************************************************************
 * NAME:
 *    UITC_GetSendHexDisplayContainerFrameCtrlHandle
 *
 * SYNOPSIS:
 *    t_UIContainerFrameCtrl *UITC_GetSendHexDisplayContainerFrameCtrlHandle(t_UITextDisplayCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control to work on
 *
 * FUNCTION:
 *    This function gets the container that the send hex input lives.
 *
 * RETURNS:
 *    A pointer to the send hex input container.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerFrameCtrl *UITC_GetSendHexDisplayContainerFrameCtrlHandle(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return (t_UIContainerFrameCtrl *)TextDisplay->ui->BlockSendHex_frame;
}

/*******************************************************************************
 * NAME:
 *    UITC_SendPanelShowHexOrTextInput
 *
 * SYNOPSIS:
 *    void UITC_SendPanelShowHexOrTextInput(t_UITextDisplayCtrl *ctrl,bool ShowText);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control to work on
 *    ShowText [I] -- Show the text input or the hex input
 *
 * FUNCTION:
 *    This function shows the text input or the hex input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UITC_SendPanelShowHexOrTextInput(t_UITextDisplayCtrl *ctrl,bool ShowText)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    if(ShowText)
    {
        TextDisplay->ui->BlockSend_textEdit->setVisible(true);
        TextDisplay->ui->SendPanelHexView_widget->setVisible(false);
    }
    else
    {
        TextDisplay->ui->BlockSend_textEdit->setVisible(false);
        TextDisplay->ui->SendPanelHexView_widget->setVisible(true);
    }
}

