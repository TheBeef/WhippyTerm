/*******************************************************************************
 * FILENAME: Frame_MainTextAreaAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    How this thing is put together:
 *
 *    Connection->DisplayText->Frame_MainTextArea->Frame_MainTextColumn->
 *    Widget_TextCanvas
 *
 *    Connection->DisplayText: This is where the connection connects to the
 *          display, it picks a display type (text/binary/etc)
 *
 *    DisplayText->Frame_MainTextArea: This allocates the full frame
 *          including the send block controls at the bottom.  This also includes
 *          the side scroll bar and has access functions to work with the
 *          full frame and all it's sub components.
 *
 *    Frame_MainTextArea->Frame_MainTextColumn: This is another frame with the
 *          text area, the header and bottom scroll bar.
 *
 *    Frame_MainTextColumn->Widget_TextCanvas: This is the connection to the
 *          actual text drawing area (text canvas).
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
#include "Frame_MainTextColumn.h"
#include "Widget_TextCanvas.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool UITC_EventHandler(const struct WTCEvent *Event);
static bool UITC_HeaderEventHandler(const struct WHQEvent *Event);
static bool UITC_MainTextAreaColumnEventHandler(const struct FMTCEvent *Event);

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

        NewTextArea->ui->PrimaryTextArea->ui->TextDisplayBox->
                SetEventHandler(UITC_EventHandler,(uintptr_t)NewTextArea);
        NewTextArea->ui->PrimaryTextArea->ui->HeaderLabel->
                SetEventHandler(UITC_HeaderEventHandler,(uintptr_t)NewTextArea);
        NewTextArea->ui->PrimaryTextArea->
                SetEventHandler(UITC_MainTextAreaColumnEventHandler,
                (uintptr_t)NewTextArea);

        NewTextArea->ui->PrimaryTextArea->SetParentSplitter(NewTextArea->ui->
                TextArea_splitter);

        /* By default we do not show the headers */
        NewTextArea->ui->PrimaryTextArea->HeaderVisible(false);

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
 *    UITC_GetTextDisplayPrimaryColumn
 *
 * SYNOPSIS:
 *    t_UITextDisplayColumn *UITC_GetTextDisplayPrimaryColumn(t_UITextDisplayCtrl *ctrl);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control to work on
 *
 * FUNCTION:
 *    This function gets a handle to the primary column.
 *
 * RETURNS:
 *    A handle to the primary column.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UITextDisplayColumn *UITC_GetTextDisplayPrimaryColumn(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return (t_UITextDisplayColumn *)TextDisplay->ui->PrimaryTextArea;
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
        case e_UITD_ContextMenu_CalcCRC:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionCalculate_CRC;
        case e_UITD_ContextMenu_FindCRCAlgorithm:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionFind_CRC_Algorithm;
        case e_UITD_ContextMenu_SendToSendBuffer:
            return (t_UIContextMenuCtrl *)TextDisplay->ui->actionCopy_To_Send_Buffer;
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
        case e_UITD_ContextMenu_ColumnHeader_Hide:
            /* We need more info to get this handle, use need to include
               t_UITextDisplayColumn. */
            return NULL;
        break;

        case e_UITD_ContextMenuMAX:
        default:
            break;
    }
    return NULL;
}

t_UIContextMenuCtrl *UITC_GetContextMenuHandle(t_UITextDisplayCtrl *ctrl,
        e_UITD_ContextMenuType UIObj,t_UITextDisplayColumn *Handle)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    switch(UIObj)
    {
        case e_UITD_ContextMenu_ColumnHeader_Hide:
            return (t_UIContextMenuCtrl *)Column->ui->Menu_HeaderLabel_x1_Hide;
        break;

        case e_UITD_ContextMenu_SendBuffers:
        case e_UITD_ContextMenu_CalcCRC:
        case e_UITD_ContextMenu_FindCRCAlgorithm:
        case e_UITD_ContextMenu_SendToSendBuffer:
        case e_UITD_ContextMenu_Copy:
        case e_UITD_ContextMenu_Paste:
        case e_UITD_ContextMenu_ClearScreen:
        case e_UITD_ContextMenu_ZoomIn:
        case e_UITD_ContextMenu_ZoomOut:
        case e_UITD_ContextMenu_Edit:
        case e_UITD_ContextMenu_EndianSwap:
        case e_UITD_ContextMenu_Bold:
        case e_UITD_ContextMenu_Italics:
        case e_UITD_ContextMenu_Underline:
        case e_UITD_ContextMenu_StrikeThrough:
        case e_UITD_ContextMenu_StyleBGColor_Black:
        case e_UITD_ContextMenu_StyleBGColor_Blue:
        case e_UITD_ContextMenu_StyleBGColor_Green:
        case e_UITD_ContextMenu_StyleBGColor_Cyan:
        case e_UITD_ContextMenu_StyleBGColor_Red:
        case e_UITD_ContextMenu_StyleBGColor_Magenta:
        case e_UITD_ContextMenu_StyleBGColor_Brown:
        case e_UITD_ContextMenu_StyleBGColor_White:
        case e_UITD_ContextMenu_StyleBGColor_Gray:
        case e_UITD_ContextMenu_StyleBGColor_LightBlue:
        case e_UITD_ContextMenu_StyleBGColor_LightGreen:
        case e_UITD_ContextMenu_StyleBGColor_LightCyan:
        case e_UITD_ContextMenu_StyleBGColor_LightRed:
        case e_UITD_ContextMenu_StyleBGColor_LightMagenta:
        case e_UITD_ContextMenu_StyleBGColor_Yellow:
        case e_UITD_ContextMenu_StyleBGColor_BrightWhite:
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
    Frame_MainTextColumn *MainTextColumn;
    struct TextDisplayEvent NewEvent;

    if(ThisFrame->EventHandler==nullptr)
        return true;

    /* If you change the Frame_MainTextColumn.ui then you need to fix the
       parents */
    MainTextColumn=static_cast<Frame_MainTextColumn *>(Event->Source->parent()->parent());

    /* Translate the event */
    NewEvent.ID=ThisFrame->ID;
    NewEvent.Ctrl=(t_UITextDisplayCtrl *)ThisFrame;
    NewEvent.Column=(t_UITextDisplayColumn *)MainTextColumn;

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
        case e_WTCEvent_MouseDoubleClick:
            NewEvent.EventType=e_TextDisplayEvent_MouseDoubleClick;
            NewEvent.Info.Mouse.x=Event->Info->Mouse.x;
            NewEvent.Info.Mouse.y=Event->Info->Mouse.y;
        break;
        case e_WTCEvent_MouseTripleClick:
            NewEvent.EventType=e_TextDisplayEvent_MouseTripleClick;
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
 *    UITC_HeaderEventHandler
 *
 * SYNOPSIS:
 *    static bool UITC_HeaderEventHandler(const struct WHQEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The Widget Text Cavnas event
 *
 * FUNCTION:
 *    This function is called from the widget header QLabel when there is an
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
static bool UITC_HeaderEventHandler(const struct WHQEvent *Event)
{
    Frame_MainTextArea *ThisFrame=(Frame_MainTextArea *)Event->UserData;
    Frame_MainTextColumn *MainTextColumn;
    struct TextDisplayEvent NewEvent;

    if(ThisFrame->EventHandler==nullptr)
        return true;

    /* If you change the Frame_MainTextColumn.ui then you need to fix the
       parents */
    MainTextColumn=static_cast<Frame_MainTextColumn *>(Event->Source->parent()->parent());

    /* Translate the event */
    NewEvent.ID=ThisFrame->ID;
    NewEvent.Ctrl=(t_UITextDisplayCtrl *)ThisFrame;
    NewEvent.Column=(t_UITextDisplayColumn *)MainTextColumn;

    switch(Event->EventType)
    {
        case e_WHQEvent_HeaderMoved:
            NewEvent.EventType=e_TextDisplayEvent_HeadersRearranged;
            NewEvent.Column=NULL;   // global
        break;
        case e_WHQEventMAX:
        default:
            return true;
    }
    return ThisFrame->EventHandler(&NewEvent);
}

/*******************************************************************************
 * NAME:
 *    UITC_MainTextAreaColumnEventHandler
 *
 * SYNOPSIS:
 *    static bool UITC_MainTextAreaColumnEventHandler(const struct FMTCEvent *Event)
 *
 * PARAMETERS:
 *    Event [I] -- The frame main text column event
 *
 * FUNCTION:
 *    This function is called from the Frame_MainTextColumn when there is an
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
static bool UITC_MainTextAreaColumnEventHandler(const struct FMTCEvent *Event)
{
    Frame_MainTextArea *ThisFrame=(Frame_MainTextArea *)Event->UserData;
    Frame_MainTextColumn *MainTextColumn;
    struct TextDisplayEvent NewEvent;

    if(ThisFrame->EventHandler==nullptr)
        return true;

    MainTextColumn=static_cast<Frame_MainTextColumn *>(Event->Source);

    /* Translate the event */
    NewEvent.ID=ThisFrame->ID;
    NewEvent.Ctrl=(t_UITextDisplayCtrl *)ThisFrame;
    NewEvent.Column=(t_UITextDisplayColumn *)MainTextColumn;

    switch(Event->EventType)
    {
        case e_FMTCEvent_ScrollX:
            NewEvent.EventType=e_TextDisplayEvent_DisplayFrameScrollX;
            NewEvent.Info.Scroll.Amount=Event->Info->Scroll.Amount;
        break;
        case e_FMTCEvent_ContextMenu:
            NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
            NewEvent.Info.Context.Menu=Event->Info->Context.Menu;
        break;
        case e_FMTCEventMAX:
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
 *    void UITC_SetFont(t_UITextDisplayColumn *Handle,const char *FontName,
 *          int Size,bool Bold,bool Italic);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetFont(t_UITextDisplayColumn *Handle,const char *FontName,int Size,
        bool Bold,bool Italic)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetFont(FontName,Size,Bold,Italic);
}

/*******************************************************************************
 * NAME:
 *    UITC_ClearAllLines
 *
 * SYNOPSIS:
 *    void UITC_ClearAllLines(t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_ClearAllLines(t_UITextDisplayColumn *Handle)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->ClearAllLines();
}

/*******************************************************************************
 * NAME:
 *    UITC_Begin
 *
 * SYNOPSIS:
 *    void UITC_Begin(t_UITextDisplayColumn *Handle,int Line);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_Begin(t_UITextDisplayColumn *Handle,int Line)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->WorkingLine=Line;
}

/*******************************************************************************
 * NAME:
 *    UITC_End
 *
 * SYNOPSIS:
 *    void UITC_End(t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_End(t_UITextDisplayColumn *Handle)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->RedrawLine(Column->WorkingLine);
}

/*******************************************************************************
 * NAME:
 *    UITC_ClearLine
 *
 * SYNOPSIS:
 *    void UITC_ClearLine(t_UITextDisplayColumn *Handle,uint32_t BGColor);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_ClearLine(t_UITextDisplayColumn *Handle,uint32_t BGColor)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->ClearLine(Column->WorkingLine,BGColor);
}

/*******************************************************************************
 * NAME:
 *    UITC_AddFragment
 *
 * SYNOPSIS:
 *    void UITC_AddFragment(t_UITextDisplayColumn *Handle,
 *          const struct TextCanvasFrag *Frag);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_AddFragment(t_UITextDisplayColumn *Handle,const struct TextCanvasFrag *Frag)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->AppendTextFrag(Column->WorkingLine,Frag);
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

    TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->CursorColor=Color;
    TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->RedrawScreen();
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

    TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->
            SetCursorBlinking(Blinking);
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

    TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->
            ChangeCursorStyle(Style);
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

    TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->SetCursorPos(x,y);
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
            TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->
                    setFocus(Qt::OtherFocusReason);
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
 *    int UITC_GetFragWidth(t_UITextDisplayColumn *Handle,
 *              const struct TextCanvasFrag *Frag);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
int UITC_GetFragWidth(t_UITextDisplayColumn *Handle,const struct TextCanvasFrag *Frag)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    return Column->ui->TextDisplayBox->GetTextWidth(Frag);
}

/*******************************************************************************
 * NAME:
 *    UITC_GetWidgetWidth
 *
 * SYNOPSIS:
 *    int UITC_GetWidgetWidth(t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
 *
 * FUNCTION:
 *    This function gets the size of the text canvas in pixels.
 *
 * RETURNS:
 *    The width of the text cavnas widget
 *
 * LIMITATIONS:
 *    This maybe 0 if the widget hasn't been drawen on the screen
 *
 * SEE ALSO:
 *    UITC_GetWidgetHeight(), UITC_GetColumnWidth()
 ******************************************************************************/
int UITC_GetWidgetWidth(t_UITextDisplayColumn *Handle)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    return Column->ui->TextDisplayBox->GetWidgetWidth();
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

    return TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->
            GetWidgetHeight();
}

/*******************************************************************************
 * NAME:
 *    UITC_GetCharPxWidth
 *
 * SYNOPSIS:
 *    int UITC_GetCharPxWidth(t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
int UITC_GetCharPxWidth(t_UITextDisplayColumn *Handle)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    return Column->ui->TextDisplayBox->GetCharPxWidth();
}

/*******************************************************************************
 * NAME:
 *    UITC_GetCharPxHeight
 *
 * SYNOPSIS:
 *    int UITC_GetCharPxHeight(t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
int UITC_GetCharPxHeight(t_UITextDisplayColumn *Handle)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    return Column->ui->TextDisplayBox->GetCharPxHeight();
}

/*******************************************************************************
 * NAME:
 *    UITC_GetHorzSlider
 *
 * SYNOPSIS:
 *    t_UIScrollBarCtrl *UITC_GetHorzSlider(t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
t_UIScrollBarCtrl *UITC_GetHorzSlider(t_UITextDisplayColumn *Handle)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    return (t_UIScrollBarCtrl *)Column->ui->TexthorizontalScrollBar;
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
 *    void UITC_SetXOffset(t_UITextDisplayColumn *Handle,int XOffsetPx);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetXOffset(t_UITextDisplayColumn *Handle,int XOffsetPx)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetXOffsetPx(XOffsetPx);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetMaxLines
 *
 * SYNOPSIS:
 *    void UITC_SetMaxLines(t_UITextDisplayColumn *Handle,int MaxLines,
 *          uint32_t BGColor);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetMaxLines(t_UITextDisplayColumn *Handle,int MaxLines,uint32_t BGColor)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetMaxLines(MaxLines,BGColor);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetClippingWindow
 *
 * SYNOPSIS:
 *    void UITC_SetClippingWindow(t_UITextDisplayColumn *Handle,int LeftEdge,
 *          int TopEdge,int Width,int Height);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetClippingWindow(t_UITextDisplayColumn *Handle,int LeftEdge,int TopEdge,
        int Width,int Height)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetDisplaySize(LeftEdge,TopEdge,Width,Height);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetTextAreaBackgroundColor
 *
 * SYNOPSIS:
 *    void UITC_SetTextAreaBackgroundColor(t_UITextDisplayColumn *Handle,
 *              uint32_t BgColor);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetTextAreaBackgroundColor(t_UITextDisplayColumn *Handle,
        uint32_t BgColor)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetTextAreaBackgroundColor(BgColor);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetTextDefaultColor
 *
 * SYNOPSIS:
 *    void UITC_SetTextDefaultColor(t_UITextDisplayColumn *Handle,
 *              uint32_t FgColor);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetTextDefaultColor(t_UITextDisplayColumn *Handle,uint32_t FgColor)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetTextDefaultColor(FgColor);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetBorderBackgroundColor
 *
 * SYNOPSIS:
 *    void UITC_SetBorderBackgroundColor(t_UITextDisplayColumn *Handle,
 *              uint32_t BgColor,bool Fill);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetBorderBackgroundColor(t_UITextDisplayColumn *Handle,
        uint32_t BgColor,bool Fill)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetDisplayBackgroundColor(BgColor,Fill);
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

    TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->
            SetOverrideMsg(Msg,OnOff);
}

/*******************************************************************************
 * NAME:
 *    UITC_RedrawScreen
 *
 * SYNOPSIS:
 *    void UITC_RedrawScreen(t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_RedrawScreen(t_UITextDisplayColumn *Handle)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->RedrawScreen();
}

/*******************************************************************************
 * NAME:
 *    UITC_SetDrawMask
 *
 * SYNOPSIS:
 *    void UITC_SetDrawMask(t_UITextDisplayColumn *Handle,uint16_t Mask);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetDrawMask(t_UITextDisplayColumn *Handle,uint16_t Mask)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetDrawMask(Mask);
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

    TextDisplay->ui->PrimaryTextArea->ui->TextDisplayBox->ShowBell();
}

/*******************************************************************************
 * NAME:
 *    UITC_SetMouseCursor
 *
 * SYNOPSIS:
 *    void UITC_SetMouseCursor(t_UITextDisplayColumn *Handle,
 *              e_UIMouse_CursorType Cursor);
 *
 * PARAMETERS:
 *    Handle [I] -- What column to work on
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
void UITC_SetMouseCursor(t_UITextDisplayColumn *Handle,
        e_UIMouse_CursorType Cursor)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->TextDisplayBox->SetMouseCursor(Cursor);
}

/*******************************************************************************
 * NAME:
 *    UITC_GetSendHexDisplayContainerFrameCtrlHandle
 *
 * SYNOPSIS:
 *    t_UIFrameContainerCtrl *UITC_GetSendHexDisplayContainerFrameCtrlHandle(t_UITextDisplayCtrl *ctrl);
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
t_UIFrameContainerCtrl *UITC_GetSendHexDisplayContainerFrameCtrlHandle(t_UITextDisplayCtrl *ctrl)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;

    return (t_UIFrameContainerCtrl *)TextDisplay->ui->BlockSendHex_frame;
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

/*******************************************************************************
 * NAME:
 *    UITC_AddColumn
 *
 * SYNOPSIS:
 *    t_UITextDisplayColumn *UITC_AddColumn(t_UITextDisplayCtrl *ctrl,
 *              const char *Name,bool Add2Right);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control to work on
 *    Name [I] -- The name for the header of this column.
 *    Add2Right [I] -- If this is true then it will be added to the far right,
 *                     if false then it will be added to the far left.
 *
 * FUNCTION:
 *    This function adds a new column to a text display control.  It will have
 *    a header turned on by default.
 *
 * RETURNS:
 *    A pointer to the new column or NULL if there was an error.
 *
 * SEE ALSO:
 *    UITC_DeleteColumn(), UITC_GetColumnIndex(), UITC_SetColumnIndex(),
 *    UITC_GetAllColumnsWidths(), UITC_SetAllColumnsWidths(),
 *    UITC_SetColumnWidth(), UITC_SetColumnHeaderLabel(),
 *    UITC_SetColumnHeaderVisible()
 ******************************************************************************/
t_UITextDisplayColumn *UITC_AddColumn(t_UITextDisplayCtrl *ctrl,
        const char *Name,bool Add2Right)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;
    Frame_MainTextColumn *NewColumn;
    int NewIndex;

    NewColumn=NULL;
    try
    {
        if(Add2Right)
            NewIndex=-1;
        else
            NewIndex=0;

        NewColumn=new class Frame_MainTextColumn(TextDisplay->ui->TextArea_splitter);
        NewColumn->HeaderVisible(true);
        NewColumn->SetParentSplitter(TextDisplay->ui->TextArea_splitter);
        NewColumn->SetHeaderLabel(Name);
        NewColumn->ui->TextDisplayBox->SetEventHandler(UITC_EventHandler,
                (uintptr_t)TextDisplay);
        NewColumn->ui->HeaderLabel->SetEventHandler(UITC_HeaderEventHandler,
                (uintptr_t)TextDisplay);
        NewColumn->SetEventHandler(UITC_MainTextAreaColumnEventHandler,
                (uintptr_t)TextDisplay);
        NewColumn->ui->TextDisplayBox->SetCursorBlinking(false);
        NewColumn->ui->TextDisplayBox->ChangeCursorStyle(e_TextCursorStyle_Hidden);
        TextDisplay->ui->TextArea_splitter->insertWidget(NewIndex,NewColumn);
    }
    catch(...)
    {
        if(NewColumn!=NULL)
            delete NewColumn;
        NewColumn=NULL;
    }

    return (t_UITextDisplayColumn *)NewColumn;
}

/*******************************************************************************
 * NAME:
 *    UITC_DeleteColumn
 *
 * SYNOPSIS:
 *    void UITC_DeleteColumn(t_UITextDisplayCtrl *ctrl,
 *              t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control the has this column in it
 *    Handle [I] -- The handle to the column to delete
 *
 * FUNCTION:
 *    This function deletes a column from a text display control.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    You can not delete the primary column
 *
 * SEE ALSO:
 *    UITC_AddColumn()
 ******************************************************************************/
void UITC_DeleteColumn(t_UITextDisplayCtrl *ctrl,t_UITextDisplayColumn *Handle)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    if(Column==TextDisplay->ui->PrimaryTextArea)
        return;

    Column->ui->TextDisplayBox->SetEventHandler(NULL,0);
    delete Column;
}

/*******************************************************************************
 * NAME:
 *    UITC_GetColumnIndex
 *
 * SYNOPSIS:
 *    int UITC_GetColumnIndex(t_UITextDisplayCtrl *ctrl,
 *              t_UITextDisplayColumn *Handle);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control the has this column in it
 *    Handle [I] -- The handle to the column to work on
 *
 * FUNCTION:
 *    This function gets the index of this column.  The index is the array
 *    position of the column.  For example if you have 3 column in this
 *    order A,B,C.  then A will have index 0, B=1, C=2.  If you reorder the
 *    columns to C,B,A then the indexes change to C=0,B=1,A=2.
 *
 * RETURNS:
 *    The index of this column.
 *
 * NOTES:
 *    When a new column is added then the indexs of all the column after
 *    the new column will change.
 *
 * SEE ALSO:
 *    UITC_AddColumn(), UITC_SetColumnIndex()
 ******************************************************************************/
int UITC_GetColumnIndex(t_UITextDisplayCtrl *ctrl,t_UITextDisplayColumn *Handle)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    return TextDisplay->ui->TextArea_splitter->indexOf(Column);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetColumnIndex
 *
 * SYNOPSIS:
 *    void UITC_SetColumnIndex(t_UITextDisplayCtrl *ctrl,
 *              t_UITextDisplayColumn *Handle,int NewIndex);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control the has this column in it
 *    Handle [I] -- The handle to the column to work on
 *    NewIndex [I] -- The new index for this column.
 *
 * FUNCTION:
 *    This function sets the index of this column.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UITC_AddColumn(), UITC_GetColumnIndex()
 ******************************************************************************/
void UITC_SetColumnIndex(t_UITextDisplayCtrl *ctrl,
        t_UITextDisplayColumn *Handle,int NewIndex)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    TextDisplay->ui->TextArea_splitter->insertWidget(NewIndex,Column);
}

/*******************************************************************************
 * NAME:
 *    UITC_GetAllColumnsWidths
 *
 * SYNOPSIS:
 *    int UITC_GetAllColumnsWidths(t_UITextDisplayCtrl *ctrl,
 *              unsigned int *RetArray,unsigned int MaxRetArrayLen);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control the has this column in it
 *    RetArray [O] -- An array to fill in with the sizes.
 *    MaxRetArrayLen [I] -- The number of entries that will fit into 'RetArray'
 *
 * FUNCTION:
 *    This function gets all the widths of the columns in a text display
 *    control.
 *
 * RETURNS:
 *    The number of columns or -1 if 'RetArray' wasn't big enough to fit the
 *    columns.
 *
 * SEE ALSO:
 *    UITC_AddColumn(), UITC_SetAllColumnsWidths()
 ******************************************************************************/
int UITC_GetAllColumnsWidths(t_UITextDisplayCtrl *ctrl,
        unsigned int *RetArray,unsigned int MaxRetArrayLen)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;
    QList<int> Sizes;
    int r;

    Sizes=TextDisplay->ui->TextArea_splitter->sizes();
    if(Sizes.length()>MaxRetArrayLen)
    {
        /* Reject */
        return -1;
    }

    for(r=0;r<Sizes.length();r++)
        RetArray[r]=Sizes[r];

    return Sizes.length();
}

/*******************************************************************************
 * NAME:
 *    UITC_SetAllColumnsWidths
 *
 * SYNOPSIS:
 *    void UITC_SetAllColumnsWidths(t_UITextDisplayCtrl *ctrl,
 *              unsigned int *SizeArray,unsigned int SizeArrayLen);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control the has this column in it
 *    SizeArray [I] -- An array with the new sizes in it.
 *    SizeArrayLen [I] -- The number of entries in 'SizeArray'.  This must
 *                        have the same number of columns connected to 'ctrl'
 *                        or less.
 *
 * FUNCTION:
 *    This function sets the widths of all the columns on a 'ctrl'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UITC_AddColumn(), UITC_GetAllColumnsWidths()
 ******************************************************************************/
void UITC_SetAllColumnsWidths(t_UITextDisplayCtrl *ctrl,unsigned int *SizeArray,
        unsigned int SizeArrayLen)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;
    QList<int> Sizes;
    unsigned int r;

    Sizes=TextDisplay->ui->TextArea_splitter->sizes();
    if(Sizes.length()<SizeArrayLen)
    {
        /* Reject */
        return;
    }

    for(r=0;r<SizeArrayLen;r++)
        Sizes[r]=SizeArray[r];
    TextDisplay->ui->TextArea_splitter->setSizes(Sizes);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetColumnWidth
 *
 * SYNOPSIS:
 *    void UITC_SetColumnWidth(t_UITextDisplayCtrl *ctrl,
 *              t_UITextDisplayColumn *Handle,unsigned int NewWidth);
 *
 * PARAMETERS:
 *    ctrl [I] -- What control the has this column in it
 *    Handle [I] -- The handle to the column to work on
 *    NewWidth [I] -- The new width of the column.
 *
 * FUNCTION:
 *    This function sets the width of a column.  The other columns will have
 *    their widths changed as well.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UITC_AddColumn()
 ******************************************************************************/
void UITC_SetColumnWidth(t_UITextDisplayCtrl *ctrl,
        t_UITextDisplayColumn *Handle,unsigned int NewWidth)
{
    Frame_MainTextArea *TextDisplay=(Frame_MainTextArea *)ctrl;
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;
    QList<int> Sizes;
    int Index;
    int Delta;
    int Needed;
    int NewSize;
    int r;

    Index=TextDisplay->ui->TextArea_splitter->indexOf(Column);
    if(Index<0)
        return;

    Sizes=TextDisplay->ui->TextArea_splitter->sizes();

    /* See if we are shrinking the current column or expanding it */
    if(Sizes[Index]>(int)NewWidth)
    {
        /* We are shrinking it, so we just add the extra to the next one */
        Delta=Sizes[Index]-NewWidth;
        Sizes[Index]=NewWidth;
        if(Index+1<Sizes.length())
            Sizes[Index+1]+=Delta;
    }
    else
    {
        /* We are growing it, so we need to take from the next column */
        Needed=NewWidth-Sizes[Index];
        Sizes[Index]=NewWidth;
        for(r=Index+1;r<Sizes.length() && Needed>0;r++)
        {
            NewSize=Sizes[r]-Needed;

            /* Don't let it get smaller than 50 (why 50? just a number that
               didn't seem to small) */
            if(NewSize<50)
                NewSize=50;

            Needed-=Sizes[r]-NewSize;

            Sizes[r]=NewSize;
        }
        /* If Needed is still >0 then we didn't have enough space, in this
           case we are just going to hope for the best */
    }

    TextDisplay->ui->TextArea_splitter->setSizes(Sizes);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetColumnHeaderVisible
 *
 * SYNOPSIS:
 *    void UITC_SetColumnHeaderVisible(t_UITextDisplayColumn *Handle,
 *              bool Visible);
 *
 * PARAMETERS:
 *    Handle [I] -- The handle to the column to work on
 *    Visible [I] -- true = show header, false = hide header
 *
 * FUNCTION:
 *    This function changes if the header if visible or not.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UITC_AddColumn()
 ******************************************************************************/
void UITC_SetColumnHeaderVisible(t_UITextDisplayColumn *Handle,bool Visible)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->HeaderVisible(Visible);
}

/*******************************************************************************
 * NAME:
 *    UITC_SetColumnHeaderLabel
 *
 * SYNOPSIS:
 *    void UITC_SetColumnHeaderLabel(t_UITextDisplayColumn *Handle,
 *              const char *Text);
 *
 * PARAMETERS:
 *    Handle [I] -- The handle to the column to work on
 *    Text [I] -- The new text for the label.
 *
 * FUNCTION:
 *    This function changes a columns header label.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UITC_AddColumn()
 ******************************************************************************/
void UITC_SetColumnHeaderLabel(t_UITextDisplayColumn *Handle,const char *Text)
{
    Frame_MainTextColumn *Column=(Frame_MainTextColumn *)Handle;

    Column->ui->HeaderLabel->setText(Text);
}
