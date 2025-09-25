/*******************************************************************************
 * FILENAME: MW_HexDisplay.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI for the hex display panel in it.
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
 *    Paul Hutchinson (16 May 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_HexDisplayCopyAs.h"
#include "App/Dialogs/Dialog_CRCFinder.h"
#include "App/Dialogs/Dialog_SendBufferSelect.h"
#include "App/MWPanels/MW_HexDisplay.h"
#include "App/MWPanels/MWPanels.h"
#include "App/MainWindow.h"
#include "App/Connections.h"
#include "App/Settings.h"
#include "App/Session.h"
#include "UI/UIAsk.h"
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <string>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool MWHexDisplay_HexDisplayBufferEvent(const struct HDEvent *Event);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    MWHexDisplay_HexDisplayBufferEvent
 *
 * SYNOPSIS:
 *    static bool MWHexDisplay_HexDisplayBufferEvent(const struct HDEvent *
 *              Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the hex display buffer.
 *
 * FUNCTION:
 *    This function redirects the event to the MWHexDisplay class.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool MWHexDisplay_HexDisplayBufferEvent(const struct HDEvent *Event)
{
    class MWHexDisplay *MWHD=(class MWHexDisplay *)Event->ID;

    return MWHD->HexDisplayBufferEvent(Event);
}

MWHexDisplay::MWHexDisplay()
{
    UIWin=NULL;
    MW=NULL;

    IncomingHistoryHexDisplay=new HexDisplayBuffer();

    PanelActive=false;
}

MWHexDisplay::~MWHexDisplay()
{
    delete IncomingHistoryHexDisplay;
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::Setup
 *
 * SYNOPSIS:
 *    void MWHexDisplay::Setup(class TheMainWindow *Parent,
 *              t_UIMainWindow *Win);
 *
 * PARAMETERS:
 *    Parent [I] -- The main window that this function lives in
 *    Win [I] -- The UI window handle.
 *
 * FUNCTION:
 *    This function setups things needed by this class.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    MW=Parent;
    UIWin=Win;

    if(!IncomingHistoryHexDisplay->Init(
            UIMW_GetHexDisplayContainerFrameCtrlHandle(UIWin),
            MWHexDisplay_HexDisplayBufferEvent,(uintptr_t)this))
    {
        UIAsk("Error","Failed to connect hex display to UI",e_AskBox_Error,
                e_AskBttns_Ok);
    }

    IncomingHistoryHexDisplay->SetLossOfFocusBehavior(false);
    IncomingHistoryHexDisplay->SetFont(g_Settings.HexDisplaysFontName.c_str(),
            g_Settings.HexDisplaysFontSize,g_Settings.HexDisplaysFontBold,
            g_Settings.HexDisplaysFontItalic);
    IncomingHistoryHexDisplay->SetColors(g_Settings.HexDisplaysFGColor,
            g_Settings.HexDisplaysBGColor,g_Settings.HexDisplaysSelBGColor);
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWHexDisplay::ActivateCtrls(bool Active);
 *
 * PARAMETERS:
 *    Active [I] -- Should we activate the controls (true), or deactivate them
 *                  (false).
 *
 * FUNCTION:
 *    This function activates/deactivates the controls for this panel.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::ActivateCtrls(bool Active)
{
    PanelActive=Active;

    if(UIWin==NULL)
        return;

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWHexDisplay::ConnectionAbout2Changed(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called just before the active connection is able to
 *    change to a different connection.
 *
 *    It stores the current UI in to the connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::ConnectionAbout2Changed(void)
{
    if(MW->ActiveCon==NULL)
        return;
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWHexDisplay::ConnectionChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the UI after the active connection has changed
 *    to a new connection (MW->ActiveCon).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::ConnectionChanged(void)
{
    t_UICheckboxCtrl *EnabledCheckbox;
    const uint8_t *Buffer;
    const uint8_t *InsertPos;
    bool BufferIsCircular;
    int BufferSize;

    if(MW->ActiveCon==NULL)
    {
        /* We are removing the active connection.  Clear the hex display */
        IncomingHistoryHexDisplay->ClearSelection();
        IncomingHistoryHexDisplay->SetBuffer((uint8_t *)NULL,0);
    }
    else
    {
        EnabledCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_HexDisplay_Paused);

        UICheckCheckbox(EnabledCheckbox,MW->ActiveCon->GetHexDisplayPaused());

        /* Connect this hex display buffer to the data buffers */
        MW->ActiveCon->HexDisplayGetBufferInfo(&Buffer,&InsertPos,&BufferIsCircular,
                &BufferSize);
        IncomingHistoryHexDisplay->SetBuffer(Buffer,BufferSize);
        IncomingHistoryHexDisplay->SetDisplayParms(InsertPos,BufferIsCircular);

        /* DEBUG PAUL: We need to set the last top byte for this hex display */
    }

    IncomingHistoryHexDisplay->RebuildDisplay();

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::NewConnectionAllocated
 *
 * SYNOPSIS:
 *    void MWHexDisplay::NewConnectionAllocated(class Connection *NewCon);
 *
 * PARAMETERS:
 *    NewCon [I] -- The new connection that was allocated
 *
 * FUNCTION:
 *    This function is called to tell the download panel that a new connection
 *    has been allocated.
 *
 *    It takes the session info and places it in the connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::NewConnectionAllocated(class Connection *NewCon)
{
    /* Update the GUI */
    ConnectionChanged();
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::InformOfBufferChange
 *
 * SYNOPSIS:
 *    void MWHexDisplay::InformOfBufferChange(class Connection *EffectedCon,
 *          const struct ConMWHexDisplayData *UpdateInfo);
 *
 * PARAMETERS:
 *    EffectedCon [I] -- The connection that has had an update
 *    UpdateInfo [I] -- Info about this update
 *
 * FUNCTION:
 *    This tells the hex display that the buffer it is reading has had a change.
 *    The display should take a new copy of the pointer, size and such.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::InformOfBufferChange(class Connection *EffectedCon,
        const struct ConMWHexDisplayData *UpdateInfo)
{
    /* Update the GUI */
    ConnectionChanged();
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::RethinkUI
 *
 * SYNOPSIS:
 *    void MWHexDisplay::RethinkUI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the UI.  It enables / disables things as needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::RethinkUI(void)
{
    t_UIButtonCtrl *ClearBttn;
    t_UIButtonCtrl *CopyBttn;
    t_UIButtonCtrl *CopyAsBttn;
    t_UICheckboxCtrl *PausedCheckbox;
    bool ControlsEnabled;
    bool PauseCheckEnabled;
    bool ClipboardBttnEnabled;
    t_UIContextMenuCtrl *ContextMenu_Copy;
    t_UIContextMenuCtrl *ContextMenu_Paste;
    t_UIContextMenuCtrl *ContextMenu_FindCRCAlgorithm;
    t_UIContextMenuCtrl *ContextMenu_CopyToSendBuffer;

    ClearBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_HexDisplay_Clear);
    CopyBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_HexDisplay_Copy);
    CopyAsBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_HexDisplay_CopyAs);
    PausedCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_HexDisplay_Paused);

    ContextMenu_Copy=IncomingHistoryHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Copy);
    ContextMenu_Paste=IncomingHistoryHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Paste);
    ContextMenu_FindCRCAlgorithm=IncomingHistoryHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_FindCRCAlgorithm);
    ContextMenu_CopyToSendBuffer=IncomingHistoryHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_CopyToSendBuffer);

    ControlsEnabled=PanelActive;
    PauseCheckEnabled=PanelActive;
    ClipboardBttnEnabled=PanelActive;

    if(!g_Settings.HexDisplayEnabled)
        PanelActive=false;

    if(PanelActive)
    {
        if(MW->ActiveCon==NULL)
            return;

        if(IncomingHistoryHexDisplay->
                GetSizeOfSelection(e_HDBCFormat_RAW)==0)
        {
            ClipboardBttnEnabled=false;
        }
    }

    IncomingHistoryHexDisplay->Enable(ControlsEnabled);

    UIEnableButton(ClearBttn,ControlsEnabled);
    UIEnableButton(CopyBttn,ClipboardBttnEnabled);
    UIEnableButton(CopyAsBttn,ClipboardBttnEnabled);
    UIEnableCheckbox(PausedCheckbox,PauseCheckEnabled);

    UIEnableContextMenu(ContextMenu_Copy,ClipboardBttnEnabled);
    UIEnableContextMenu(ContextMenu_FindCRCAlgorithm,ClipboardBttnEnabled);
    UIEnableContextMenu(ContextMenu_CopyToSendBuffer,ClipboardBttnEnabled);

    UISetContextMenuVisible(ContextMenu_Paste,false);
    UISetContextMenuVisible(ContextMenu_FindCRCAlgorithm,true);
    UISetContextMenuVisible(ContextMenu_CopyToSendBuffer,true);
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::TogglePause
 *
 * SYNOPSIS:
 *    void MWHexDisplay::TogglePause(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the hex display paused state.  It takes the current
 *    value and toggles it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::TogglePause(void)
{
    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->SetHexDisplayPaused(!MW->ActiveCon->GetHexDisplayPaused());
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::Clear
 *
 * SYNOPSIS:
 *    void MWHexDisplay::Clear(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the hex display display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::Clear(void)
{
    const uint8_t *Buffer;
    const uint8_t *InsertPos;
    bool BufferIsCircular;
    int BufferSize;

    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->HexDisplayClear();

    MW->ActiveCon->HexDisplayGetBufferInfo(&Buffer,&InsertPos,&BufferIsCircular,
            &BufferSize);

    IncomingHistoryHexDisplay->ClearSelection();
    IncomingHistoryHexDisplay->SetDisplayParms(InsertPos,BufferIsCircular);
    IncomingHistoryHexDisplay->RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::InformOfUpdate
 *
 * SYNOPSIS:
 *    void MWHexDisplay::InformOfUpdate(class Connection *EffectedCon,
 *          const struct ConMWHexDisplayData *UpdateInfo);
 *
 * PARAMETERS:
 *    EffectedCon [I] -- The connection that has had an update
 *    UpdateInfo [I] -- Info about this update
 *
 * FUNCTION:
 *    This function is called when there have been changes to the hex buffer.
 *    It redraws the cavnas.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::InformOfUpdate(class Connection *EffectedCon,
        const struct ConMWHexDisplayData *UpdateInfo)
{
    bool WasAtBottom;

    /* Don't update the screen if this connection is not active */
    if(EffectedCon!=MW->ActiveCon)
        return;

    WasAtBottom=IncomingHistoryHexDisplay->IsYScrollBarAtBottom();

    IncomingHistoryHexDisplay->SetDisplayParms(UpdateInfo->InsertPos,
            UpdateInfo->BufferIsCircular);

    /* Only redraw if the current tab is hex display */
    if(EffectedCon->GetBottomPanelInfo()!=e_BottomPanelTab_Hex)
        return;

    if(WasAtBottom)
        IncomingHistoryHexDisplay->ScrollToBottom();
    IncomingHistoryHexDisplay->RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::InformOfPanelTabChange
 *
 * SYNOPSIS:
 *    void MWHexDisplay::InformOfPanelTabChange(e_BottomPanelTabType PanelTab);
 *
 * PARAMETERS:
 *    PanelTab [I] -- What tab was selected in the bottom panel.
 *
 * FUNCTION:
 *    This function is called to tell us that the active tab in the bottom
 *    panel has changed.
 *
 *    We will rebuild the hex display if we are changing into the hex display
 *    tab.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::InformOfPanelTabChange(e_BottomPanelTabType PanelTab)
{
    if(MW->ActiveCon==NULL)
        return;

    if(PanelTab==e_BottomPanelTab_Hex)
    {
        /* We just switch back to the hex display, redraw */
        IncomingHistoryHexDisplay->RebuildDisplay();
    }
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::Copy2Clip
 *
 * SYNOPSIS:
 *    void MWHexDisplay::Copy2Clip(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function copies the selection to the clipboard.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::Copy2Clip(void)
{
    IncomingHistoryHexDisplay->SendSelection2Clipboard(e_Clipboard_Selection,
            e_HDBCFormat_Default);
    IncomingHistoryHexDisplay->SendSelection2Clipboard(e_Clipboard_Clipboard,
            e_HDBCFormat_Default);
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::CopyAs
 *
 * SYNOPSIS:
 *    void MWHexDisplay::CopyAs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user for how they want to copy the selection
 *    to the clip board.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::CopyAs(void)
{
    e_HDBCFormatType SelectedFormat;

    SelectedFormat=e_HDBCFormat_HexDump;
    RunHexDisplayCopyAsDialog(SelectedFormat);

    IncomingHistoryHexDisplay->SendSelection2Clipboard(e_Clipboard_Selection,
            SelectedFormat);
    IncomingHistoryHexDisplay->SendSelection2Clipboard(e_Clipboard_Clipboard,
            SelectedFormat);
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::HexDisplayBufferEvent
 *
 * SYNOPSIS:
 *    bool MWHexDisplay::HexDisplayBufferEvent(const struct HDEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the hex display buffer.
 *
 * FUNCTION:
 *    This function handles events from the hex display buffer.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool MWHexDisplay::HexDisplayBufferEvent(const struct HDEvent *Event)
{
    switch(Event->EventType)
    {
        case e_HDEvent_SelectionEvent:
            RethinkUI();
        return true;
        case e_HDEvent_MouseMove:
            IncomingHistoryHexDisplay->
                    SendSelection2Clipboard(e_Clipboard_Selection,
                    e_HDBCFormat_Default);
        return true;
        case e_HDEvent_ContextMenu:
            switch(Event->Info.Context.Menu)
            {
                case e_UICTW_ContextMenu_Copy:
                    Copy2Clip();
                break;
                case e_UICTW_ContextMenu_ClearScreen:
                    Clear();
                break;
                case e_UICTW_ContextMenu_FindCRCAlgorithm:
                    OpenFindCRCAlgDialog();
                break;
                case e_UICTW_ContextMenu_CopyToSendBuffer:
                    OpenSendBufferDialog();
                break;
                case e_UICTW_ContextMenu_Edit:
                case e_UICTW_ContextMenu_EndianSwap:
                case e_UICTW_ContextMenu_Paste:
                case e_UICTW_ContextMenu_ZoomIn:
                case e_UICTW_ContextMenu_ZoomOut:
                case e_UICTW_ContextMenuMAX:
                default:
                break;
            }
        break;
        case e_HDEvent_CursorMove:
        case e_HDEvent_BufferResize:
        case e_HDEvent_BufferChange:
        case e_HDEventMAX:
        default:
        break;
    }
    return false;
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::OpenFindCRCAlgDialog
 *
 * SYNOPSIS:
 *    void MWHexDisplay::OpenFindCRCAlgDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the user select they want to find a CRC
 *    alg.  It opens the find CRC dialog with the current select in it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::OpenFindCRCAlgDialog(void)
{
    int Bytes;
    uint8_t *HexBuff;

    Bytes=IncomingHistoryHexDisplay->GetSizeOfSelection(e_HDBCFormat_RAW);
    if(Bytes==0)
        return;

    HexBuff=(uint8_t *)malloc(Bytes);
    if(HexBuff==NULL)
    {
        UIAsk("Error","Out of memory",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    IncomingHistoryHexDisplay->CopySelection2Buffer(HexBuff,e_HDBCFormat_RAW);

    RunCRCFinderDialog(HexBuff,Bytes);

    free(HexBuff);
}

/*******************************************************************************
 * NAME:
 *    MWHexDisplay::OpenSendBufferDialog
 *
 * SYNOPSIS:
 *    void MWHexDisplay::OpenSendBufferDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function opens the select send buffer dialog with the select in it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWHexDisplay::OpenSendBufferDialog(void)
{
    int Bytes;
    uint8_t *HexBuff;

    Bytes=IncomingHistoryHexDisplay->GetSizeOfSelection(e_HDBCFormat_RAW);
    if(Bytes==0)
        return;

    HexBuff=(uint8_t *)malloc(Bytes);
    if(HexBuff==NULL)
    {
        UIAsk("Error","Out of memory",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    IncomingHistoryHexDisplay->CopySelection2Buffer(HexBuff,e_HDBCFormat_RAW);

    RunSendBufferSelectDialog(MW,e_SBSD_Copy2Buffer,HexBuff,Bytes);

    free(HexBuff);
}
