/*******************************************************************************
 * FILENAME: MW_OutGoingHexDisplay.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI for the outgoing hex display panel in it.
 *
 * COPYRIGHT:
 *    Copyright 24 Nov 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (24 Nov 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_HexDisplayCopyAs.h"
#include "App/Dialogs/Dialog_CRCFinder.h"
#include "App/Dialogs/Dialog_SendBufferSelect.h"
#include "App/MWPanels/MW_OutGoingHexDisplay.h"
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

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay_HexDisplayBufferEvent
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
static bool MWOutGoingHexDisplay_HexDisplayBufferEvent(const struct HDEvent *Event)
{
    class MWOutGoingHexDisplay *MWHD=(class MWOutGoingHexDisplay *)Event->ID;

    return MWHD->HexDisplayBufferEvent(Event);
}

MWOutGoingHexDisplay::MWOutGoingHexDisplay()
{
    UIWin=NULL;
    MW=NULL;

    OutGoingHistoryHexDisplay=new HexDisplayBuffer();

    PanelActive=false;
}

MWOutGoingHexDisplay::~MWOutGoingHexDisplay()
{
    delete OutGoingHistoryHexDisplay;
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::Setup
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::Setup(class TheMainWindow *Parent,
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
void MWOutGoingHexDisplay::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    MW=Parent;
    UIWin=Win;

    if(!OutGoingHistoryHexDisplay->Init(
            UIMW_GetOutGoingHexDisplayContainerFrameCtrlHandle(UIWin),
            MWOutGoingHexDisplay_HexDisplayBufferEvent,(uintptr_t)this))
    {
        UIAsk("Error","Failed to connect outgoing hex display to UI",e_AskBox_Error,
                e_AskBttns_Ok);
    }

    OutGoingHistoryHexDisplay->SetLossOfFocusBehavior(false);
    OutGoingHistoryHexDisplay->SetFont(g_Settings.HexDisplaysFontName.c_str(),
            g_Settings.HexDisplaysFontSize,g_Settings.HexDisplaysFontBold,
            g_Settings.HexDisplaysFontItalic);
    OutGoingHistoryHexDisplay->SetColors(g_Settings.HexDisplaysFGColor,
            g_Settings.HexDisplaysBGColor,g_Settings.HexDisplaysSelBGColor);
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::ActivateCtrls(bool Active);
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
void MWOutGoingHexDisplay::ActivateCtrls(bool Active)
{
    PanelActive=Active;

    if(UIWin==NULL)
        return;

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::ConnectionAbout2Changed(void);
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
void MWOutGoingHexDisplay::ConnectionAbout2Changed(void)
{
    if(MW->ActiveCon==NULL)
        return;
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::ConnectionChanged(void);
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
void MWOutGoingHexDisplay::ConnectionChanged(void)
{
    t_UICheckboxCtrl *EnabledCheckbox;
    const uint8_t *Buffer;
    const uint8_t *InsertPos;
    bool BufferIsCircular;
    int BufferSize;

    if(MW->ActiveCon==NULL)
    {
        /* We are removing the active connection.  Clear the hex display */
        OutGoingHistoryHexDisplay->ClearSelection();
        OutGoingHistoryHexDisplay->SetBuffer((uint8_t *)NULL,0);
    }
    else
    {
        EnabledCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_OutGoingHexDisplay_Paused);

        UICheckCheckbox(EnabledCheckbox,MW->ActiveCon->GetOutGoingHexDisplayPaused());

        /* Connect this hex display buffer to the data buffers */
        MW->ActiveCon->OutGoingHexDisplayGetBufferInfo(&Buffer,&InsertPos,
                &BufferIsCircular,&BufferSize);
        OutGoingHistoryHexDisplay->SetBuffer(Buffer,BufferSize);
        OutGoingHistoryHexDisplay->SetDisplayParms(InsertPos,BufferIsCircular);

        /* DEBUG PAUL: We need to set the last top byte for this hex display */
    }

    OutGoingHistoryHexDisplay->RebuildDisplay();

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::NewConnectionAllocated
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::NewConnectionAllocated(class Connection *NewCon);
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
void MWOutGoingHexDisplay::NewConnectionAllocated(class Connection *NewCon)
{
    /* Update the GUI */
    ConnectionChanged();
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::InformOfBufferChange
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::InformOfBufferChange(class Connection *EffectedCon,
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
void MWOutGoingHexDisplay::InformOfBufferChange(class Connection *EffectedCon,
        const struct ConMWHexDisplayData *UpdateInfo)
{
    /* Update the GUI */
    ConnectionChanged();
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::RethinkUI
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::RethinkUI(void);
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
void MWOutGoingHexDisplay::RethinkUI(void)
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

    ClearBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_OutGoingHexDisplay_Clear);
    CopyBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_OutGoingHexDisplay_Copy);
    CopyAsBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_OutGoingHexDisplay_CopyAs);
    PausedCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_OutGoingHexDisplay_Paused);

    ContextMenu_Copy=OutGoingHistoryHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Copy);
    ContextMenu_Paste=OutGoingHistoryHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Paste);
    ContextMenu_FindCRCAlgorithm=OutGoingHistoryHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_FindCRCAlgorithm);
    ContextMenu_CopyToSendBuffer=OutGoingHistoryHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_CopyToSendBuffer);

    ControlsEnabled=PanelActive;
    PauseCheckEnabled=PanelActive;
    ClipboardBttnEnabled=PanelActive;

    if(!g_Settings.OutGoingHexDisplayEnabled)
        PanelActive=false;

    if(PanelActive)
    {
        if(MW->ActiveCon==NULL)
            return;

        if(OutGoingHistoryHexDisplay->
                GetSizeOfSelection(e_HDBCFormat_RAW)==0)
        {
            ClipboardBttnEnabled=false;
        }
    }

    OutGoingHistoryHexDisplay->Enable(ControlsEnabled);

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
 *    MWOutGoingHexDisplay::TogglePause
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::TogglePause(void);
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
void MWOutGoingHexDisplay::TogglePause(void)
{
    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->SetOutGoingHexDisplayPaused(!MW->ActiveCon->
            GetOutGoingHexDisplayPaused());
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::Clear
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::Clear(void);
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
void MWOutGoingHexDisplay::Clear(void)
{
    const uint8_t *Buffer;
    const uint8_t *InsertPos;
    bool BufferIsCircular;
    int BufferSize;

    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->OutGoingHexDisplayClear();

    MW->ActiveCon->OutGoingHexDisplayGetBufferInfo(&Buffer,&InsertPos,
            &BufferIsCircular,&BufferSize);

    OutGoingHistoryHexDisplay->ClearSelection();
    OutGoingHistoryHexDisplay->SetDisplayParms(InsertPos,BufferIsCircular);
    OutGoingHistoryHexDisplay->RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::InformOfUpdate
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::InformOfUpdate(class Connection *EffectedCon,
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
void MWOutGoingHexDisplay::InformOfUpdate(class Connection *EffectedCon,
        const struct ConMWHexDisplayData *UpdateInfo)
{
    bool WasAtBottom;

    /* Don't update the screen if this connection is not active */
    if(EffectedCon!=MW->ActiveCon)
        return;

    WasAtBottom=OutGoingHistoryHexDisplay->IsYScrollBarAtBottom();

    OutGoingHistoryHexDisplay->SetDisplayParms(UpdateInfo->InsertPos,
            UpdateInfo->BufferIsCircular);

    /* Only redraw if the current tab is hex display */
    if(EffectedCon->GetBottomPanelInfo()!=e_BottomPanelTab_OutGoingHex)
        return;

    if(WasAtBottom)
        OutGoingHistoryHexDisplay->ScrollToBottom();
    OutGoingHistoryHexDisplay->RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::InformOfPanelTabChange
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::InformOfPanelTabChange(e_BottomPanelTabType PanelTab);
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
void MWOutGoingHexDisplay::InformOfPanelTabChange(e_BottomPanelTabType PanelTab)
{
    if(MW->ActiveCon==NULL)
        return;

    switch(PanelTab)
    {
        case e_BottomPanelTab_IncomingHex:
        break;
        case e_BottomPanelTab_OutGoingHex:
            /* We just switch back to the hex display, redraw */
            OutGoingHistoryHexDisplay->RebuildDisplay();
        break;
//        case e_BottomPanelTab_Injection:
//        break;
        case e_BottomPanelTab_Buffers:
        break;
        case e_BottomPanelTabMAX:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::Copy2Clip
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::Copy2Clip(void);
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
void MWOutGoingHexDisplay::Copy2Clip(void)
{
    OutGoingHistoryHexDisplay->SendSelection2Clipboard(e_Clipboard_Selection,
            e_HDBCFormat_Default);
    OutGoingHistoryHexDisplay->SendSelection2Clipboard(e_Clipboard_Clipboard,
            e_HDBCFormat_Default);
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::CopyAs
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::CopyAs(void);
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
void MWOutGoingHexDisplay::CopyAs(void)
{
    e_HDBCFormatType SelectedFormat;

    SelectedFormat=e_HDBCFormat_HexDump;
    RunHexDisplayCopyAsDialog(SelectedFormat);

    OutGoingHistoryHexDisplay->SendSelection2Clipboard(e_Clipboard_Selection,
            SelectedFormat);
    OutGoingHistoryHexDisplay->SendSelection2Clipboard(e_Clipboard_Clipboard,
            SelectedFormat);
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::HexDisplayBufferEvent
 *
 * SYNOPSIS:
 *    bool MWOutGoingHexDisplay::HexDisplayBufferEvent(const struct HDEvent *Event);
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
bool MWOutGoingHexDisplay::HexDisplayBufferEvent(const struct HDEvent *Event)
{
    switch(Event->EventType)
    {
        case e_HDEvent_SelectionEvent:
            RethinkUI();
        return true;
        case e_HDEvent_MouseMove:
            OutGoingHistoryHexDisplay->
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
 *    MWOutGoingHexDisplay::OpenFindCRCAlgDialog
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::OpenFindCRCAlgDialog(void);
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
void MWOutGoingHexDisplay::OpenFindCRCAlgDialog(void)
{
    int Bytes;
    uint8_t *HexBuff;

    Bytes=OutGoingHistoryHexDisplay->GetSizeOfSelection(e_HDBCFormat_RAW);
    if(Bytes==0)
        return;

    HexBuff=(uint8_t *)malloc(Bytes);
    if(HexBuff==NULL)
    {
        UIAsk("Error","Out of memory",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    OutGoingHistoryHexDisplay->CopySelection2Buffer(HexBuff,e_HDBCFormat_RAW);

    RunCRCFinderDialog(HexBuff,Bytes);

    free(HexBuff);
}

/*******************************************************************************
 * NAME:
 *    MWOutGoingHexDisplay::OpenSendBufferDialog
 *
 * SYNOPSIS:
 *    void MWOutGoingHexDisplay::OpenSendBufferDialog(void);
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
void MWOutGoingHexDisplay::OpenSendBufferDialog(void)
{
    int Bytes;
    uint8_t *HexBuff;

    Bytes=OutGoingHistoryHexDisplay->GetSizeOfSelection(e_HDBCFormat_RAW);
    if(Bytes==0)
        return;

    HexBuff=(uint8_t *)malloc(Bytes);
    if(HexBuff==NULL)
    {
        UIAsk("Error","Out of memory",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    OutGoingHistoryHexDisplay->CopySelection2Buffer(HexBuff,e_HDBCFormat_RAW);

    RunSendBufferSelectDialog(MW,e_SBSD_Copy2Buffer,HexBuff,Bytes);

    free(HexBuff);
}
