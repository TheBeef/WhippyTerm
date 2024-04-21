/*******************************************************************************
 * FILENAME: MW_SendBuffers.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI for the send buffers panel in it.
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
 *    Paul Hutchinson (17 Jul 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
//#include "App/Dialogs/Dialog_HexDisplayCopyAs.h"
#include "App/MWPanels/MW_SendBuffers.h"
#include "App/MWPanels/MWPanels.h"
#include "App/Dialogs/Dialog_EditSendBuffer.h"
#include "App/MainWindow.h"
#include "App/Connections.h"
#include "App/Settings.h"
#include "App/Session.h"
#include "App/SendBuffer.h"
#include "UI/UIAsk.h"
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <string>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool MWSendBuffers_SendBuffersBufferEvent(const struct HDEvent *Event);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    MWSendBuffers_SendBuffersBufferEvent
 *
 * SYNOPSIS:
 *    static bool MWSendBuffers_SendBuffersBufferEvent(const struct HDEvent *
 *              Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the hex display buffer.
 *
 * FUNCTION:
 *    This function redirects the event to the MWSendBuffers class.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool MWSendBuffers_SendBuffersBufferEvent(const struct HDEvent *Event)
{
    class MWSendBuffers *MWHD=(class MWSendBuffers *)Event->ID;

    return MWHD->SendBuffersBufferEvent(Event);
}

MWSendBuffers::MWSendBuffers()
{
    UIWin=NULL;
    MW=NULL;

    Buffer2SendHexDisplay=new HexDisplayBuffer();

    PanelActive=false;
}

MWSendBuffers::~MWSendBuffers()
{
    delete Buffer2SendHexDisplay;
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::Setup
 *
 * SYNOPSIS:
 *    void MWSendBuffers::Setup(class TheMainWindow *Parent,
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
void MWSendBuffers::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    t_UIListViewCtrl *BufferList;
    int b;
    char *Name;

    MW=Parent;
    UIWin=Win;

    if(!Buffer2SendHexDisplay->Init(
            UIMW_GetSendBuffersContainerFrameCtrlHandle(UIWin),
            MWSendBuffers_SendBuffersBufferEvent,(uintptr_t)this))
    {
        UIAsk("Error","Failed to connect send buffers hex display to UI",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    Buffer2SendHexDisplay->SetDisplayParms((const uint8_t *)NULL,false);

    Buffer2SendHexDisplay->SetLossOfFocusBehavior(false);
    Buffer2SendHexDisplay->SetFont(g_Settings.HexDisplaysFontName.c_str(),
            g_Settings.HexDisplaysFontSize,g_Settings.HexDisplaysFontBold,
            g_Settings.HexDisplaysFontItalic);
    Buffer2SendHexDisplay->SetColors(g_Settings.HexDisplaysFGColor,
            g_Settings.HexDisplaysBGColor,g_Settings.HexDisplaysSelBGColor);

    BufferList=UIMW_GetListViewHandle(UIWin,e_UIMWListView_Buffers_List);

    for(b=0;b<MAX_SEND_BUFFERS;b++)
    {
        Name=g_SendBuffers.GetBufferName(b);
        if(Name!=NULL)
            UIAddItem2ListView(BufferList,Name,b);
        else
            UIAddItem2ListView(BufferList,"ERROR",b);
    }
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWSendBuffers::ActivateCtrls(bool Active);
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
void MWSendBuffers::ActivateCtrls(bool Active)
{
    PanelActive=Active;

    if(UIWin==NULL)
        return;

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWSendBuffers::ConnectionAbout2Changed(void);
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
void MWSendBuffers::ConnectionAbout2Changed(void)
{
    if(MW->ActiveCon==NULL)
        return;
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWSendBuffers::ConnectionChanged(void);
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
void MWSendBuffers::ConnectionChanged(void)
{
//    t_UICheckboxCtrl *EnabledCheckbox;
//    const uint8_t *Buffer;
//    const uint8_t *InsertPos;
//    bool BufferIsCircular;
//    int BufferSize;

    if(MW->ActiveCon==NULL)
        return;

//    EnabledCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_HexDisplay_Paused);
//
//    UICheckCheckbox(EnabledCheckbox,MW->ActiveCon->GetHexDisplayPaused());
//
//    /* Connect this hex display buffer to the data buffers */
//    MW->ActiveCon->HexDisplayGetBufferInfo(&Buffer,&InsertPos,&BufferIsCircular,
//            &BufferSize);
//    Buffer2SendHexDisplay->SetBuffer(Buffer,BufferSize);
//    Buffer2SendHexDisplay->SetDisplayParms(InsertPos,BufferIsCircular);
//
//    /* DEBUG PAUL: We need to set the last top byte for this hex display */
//
//    Buffer2SendHexDisplay->RebuildDisplay();

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::NewConnectionAllocated
 *
 * SYNOPSIS:
 *    void MWSendBuffers::NewConnectionAllocated(class Connection *NewCon);
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
void MWSendBuffers::NewConnectionAllocated(class Connection *NewCon)
{
    /* Update the GUI */
    ConnectionChanged();
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::RethinkUI
 *
 * SYNOPSIS:
 *    void MWSendBuffers::RethinkUI(void);
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
void MWSendBuffers::RethinkUI(void)
{
    t_UIButtonCtrl *EditBttn;
    t_UIButtonCtrl *SendBttn;
    bool EditEnabled;
    bool SendEnabled;
    t_UIListViewCtrl *BufferList;
    bool BufferSeleced;
    bool HexDisplayEnabled;

    EditBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_SendBuffers_Edit);
    SendBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_SendBuffers_Send);
    BufferList=UIMW_GetListViewHandle(UIWin,e_UIMWListView_Buffers_List);

    BufferSeleced=UIListViewHasSelectedEntry(BufferList);

    SendEnabled=PanelActive;
    EditEnabled=true;
    HexDisplayEnabled=true;

    if(PanelActive)
    {
        if(MW->ActiveCon==NULL)
            return;
    }

    if(!BufferSeleced)
    {
        EditEnabled=false;
        HexDisplayEnabled=false;
    }

    Buffer2SendHexDisplay->Enable(HexDisplayEnabled);

    UIEnableButton(EditBttn,EditEnabled);
    UIEnableButton(SendBttn,SendEnabled);
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::Copy2Clip
 *
 * SYNOPSIS:
 *    void MWSendBuffers::Copy2Clip(void);
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
void MWSendBuffers::Copy2Clip(void)
{
    Buffer2SendHexDisplay->SendSelection2Clipboard(e_Clipboard_Selection,
            e_HDBCFormat_Default);
    Buffer2SendHexDisplay->SendSelection2Clipboard(e_Clipboard_Clipboard,
            e_HDBCFormat_Default);
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::SendBuffersBufferEvent
 *
 * SYNOPSIS:
 *    bool MWSendBuffers::SendBuffersBufferEvent(const struct HDEvent *Event);
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
bool MWSendBuffers::SendBuffersBufferEvent(const struct HDEvent *Event)
{
    switch(Event->EventType)
    {
        case e_HDEvent_SelectionEvent:
            RethinkUI();
        return true;
        case e_HDEvent_MouseMove:
            Buffer2SendHexDisplay->
                    SendSelection2Clipboard(e_Clipboard_Selection,
                    e_HDBCFormat_Default);
        return true;
        case e_HDEvent_CursorMove:
        case e_HDEvent_BufferResize:
        case e_HDEventMAX:
        default:
        break;
    }
    return false;
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffer::SelectedBufferChanged
 *
 * SYNOPSIS:
 *    void MWSendBuffer::SelectedBufferChanged(int BufferIndex);
 *
 * PARAMETERS:
 *    BufferIndex [I] -- The new buffer to select
 *
 * FUNCTION:
 *    This function is called when the user changes what buffer is selected.
 *    It does not change the selected buffer in the list view.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::SelectedBufferChanged(int BufferIndex)
{
    RethinkUI();
    RethinkBuffer();
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::RethinkBuffer
 *
 * SYNOPSIS:
 *    void MWSendBuffers::RethinkBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the buffers.  It will set the hex display to the
 *    currently selected buffer, rethink the send button.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::RethinkBuffer(void)
{
    t_UIListViewCtrl *BufferList;
    t_UIButtonCtrl *SendBttn;
    int SelectedBuffer;
    const uint8_t *BuffMem;
    uint32_t BuffSize;
    e_CmdType cmd;
    const char *KeyStr;
    char buff[100];

    BufferList=UIMW_GetListViewHandle(UIWin,e_UIMWListView_Buffers_List);
    SendBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_SendBuffers_Send);

    SelectedBuffer=UIGetListViewSelectedEntry(BufferList);

    if(!g_SendBuffers.GetBufferInfo(SelectedBuffer,&BuffMem,&BuffSize))
        Buffer2SendHexDisplay->SetBuffer((uint8_t *)NULL,0);
    else
        Buffer2SendHexDisplay->SetBuffer(BuffMem,BuffSize);

    Buffer2SendHexDisplay->RebuildDisplay();

    cmd=ConvertBuffer2Cmd(SelectedBuffer);

    /* Find the key mapping for this command */
    if(!KeySeqMapped(&g_Settings.KeyMapping[cmd]))
    {
        strcpy(buff,"Send");
    }
    else
    {
        KeyStr=ConvertKeySeq2String(&g_Settings.KeyMapping[cmd]);
        sprintf(buff,"Send (%s)",KeyStr);
    }
    UISetButtonLabel(SendBttn,buff);
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::SendBttn2Cmd
 *
 * SYNOPSIS:
 *    void MWSendBuffers::SendBttn2Cmd(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function converts the send button into the correct command to send
 *    the correct buffer.
 *
 *    In then calls the ExeCmd() function in the main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::SendBttn2Cmd(void)
{
    t_UIListViewCtrl *BufferList;
    int SelectedBuffer;
    e_CmdType cmd;

    BufferList=UIMW_GetListViewHandle(UIWin,e_UIMWListView_Buffers_List);

    if(!UIListViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetListViewSelectedEntry(BufferList);

    cmd=ConvertBuffer2Cmd(SelectedBuffer);
    if(cmd!=e_CmdMAX)
        MW->ExeCmd(cmd);
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::ConvertBuffer2Cmd
 *
 * SYNOPSIS:
 *    e_CmdType MWSendBuffers::ConvertBuffer2Cmd(int Buffer);
 *
 * PARAMETERS:
 *    Buffer [I] -- The buffer number to convert
 *
 * FUNCTION:
 *    This function takes a buffer number and converts it to the command needed
 *    to send this buffer.
 *
 * RETURNS:
 *    The command to send this buffer or e_CmdMAX if there isn't one.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_CmdType MWSendBuffers::ConvertBuffer2Cmd(int Buffer)
{
    switch(Buffer)
    {
        case 0:
            return e_Cmd_SendBuffer_Send1;
        break;
        case 1:
            return e_Cmd_SendBuffer_Send2;
        break;
        case 2:
            return e_Cmd_SendBuffer_Send3;
        break;
        case 3:
            return e_Cmd_SendBuffer_Send4;
        break;
        case 4:
            return e_Cmd_SendBuffer_Send5;
        break;
        case 5:
            return e_Cmd_SendBuffer_Send6;
        break;
        case 6:
            return e_Cmd_SendBuffer_Send7;
        break;
        case 7:
            return e_Cmd_SendBuffer_Send8;
        break;
        case 8:
            return e_Cmd_SendBuffer_Send9;
        break;
        case 9:
            return e_Cmd_SendBuffer_Send10;
        break;
        case 10:
            return e_Cmd_SendBuffer_Send11;
        break;
        case 11:
            return e_Cmd_SendBuffer_Send12;
        break;
        default:
        break;
    }
    return e_CmdMAX;
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::SendBuffer
 *
 * SYNOPSIS:
 *    void MWSendBuffers::SendBuffer(int Buffer);
 *
 * PARAMETERS:
 *    Buffer [I] -- What buffer to send
 *
 * FUNCTION:
 *    This function sends a buffer out the current connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::SendBuffer(int Buffer)
{
    const uint8_t *BuffMem;
    uint32_t BuffSize;

    if(MW->ActiveCon==NULL)
        return;

    if(!g_SendBuffers.GetBufferInfo(Buffer,&BuffMem,&BuffSize))
    {
        UIAsk("Error","Failed to send the buffer.  Buffer not valid",
                e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    if(MW->ActiveCon->WriteData(BuffMem,BuffSize,e_ConWriteSource_Buffers)!=
            e_ConWrite_Success)
    {
        UIAsk("Error","Failed to send the buffer.  Write error",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    /* Force the send as if we are doing a block send device we want to send
       everything we just queued */
    MW->ActiveCon->TransmitQueuedData();
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::EditCurrentBuffer
 *
 * SYNOPSIS:
 *    void MWSendBuffers::EditCurrentBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function opens the edit send buffer dialog for editing of the
 *    currently selected buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::EditCurrentBuffer(void)
{
    t_UIListViewCtrl *BufferList;
    int SelectedBuffer;
    char *Name;

    BufferList=UIMW_GetListViewHandle(UIWin,e_UIMWListView_Buffers_List);

    if(!UIListViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetListViewSelectedEntry(BufferList);

    if(RunEditSendBufferDialog(SelectedBuffer))
    {
        /* Update the UI with any changes */
        Name=g_SendBuffers.GetBufferName(SelectedBuffer);
        if(Name!=NULL)
            UIReplaceItemInListView(BufferList,SelectedBuffer,Name);

        RethinkBuffer();
    }
}
