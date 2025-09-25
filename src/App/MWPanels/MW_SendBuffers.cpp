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
#include "UI/UITextInputBox.h"
#include "UI/UIFileReq.h"
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <string>

using namespace std;

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
    t_UIContextMenuCtrl *ContextMenu_Paste;
    t_UIContextMenuCtrl *ContextMenu_ClearScreen;
    t_UIContextMenuCtrl *ContextMenu_Edit;

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

    ContextMenu_Paste=Buffer2SendHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Paste);
    ContextMenu_ClearScreen=Buffer2SendHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_ClearScreen);
    ContextMenu_Edit=Buffer2SendHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Edit);

    UISetContextMenuVisible(ContextMenu_Paste,false);
    UISetContextMenuVisible(ContextMenu_ClearScreen,false);
    UISetContextMenuVisible(ContextMenu_Edit,true);

    RethinkBufferList();
    RethinkBuffer();
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

    RethinkBufferList();
    RethinkBuffer();
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

    RethinkBufferList();
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
    t_UIContextMenuCtrl *SendContextMenu;
    bool EditEnabled;
    bool SendEnabled;
    t_UIColumnView *BufferList;
    bool BufferSeleced;
    bool HexDisplayEnabled;
    t_UIContextMenuCtrl *ContextMenu_Copy;
    bool CopyEnabled;

    EditBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_SendBuffers_Edit);
    SendBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_SendBuffers_Send);
    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);
    SendContextMenu=UIMW_GetContextMenuHandle(UIWin,e_UIMW_ContextMenu_SendBuffers_Send);
    ContextMenu_Copy=Buffer2SendHexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Copy);

    BufferSeleced=UIColumnViewHasSelectedEntry(BufferList);

    SendEnabled=PanelActive;
    EditEnabled=true;
    HexDisplayEnabled=true;

    if(PanelActive)
    {
        if(MW->ActiveCon==NULL)
            return;
        if(!MW->ActiveCon->GetConnectedStatus())
            SendEnabled=false;
    }

    if(!BufferSeleced)
    {
        EditEnabled=false;
        HexDisplayEnabled=false;
    }

    CopyEnabled=false;
    if(Buffer2SendHexDisplay->GetSizeOfSelection(e_HDBCFormat_RAW)!=0)
        CopyEnabled=true;

    Buffer2SendHexDisplay->Enable(HexDisplayEnabled);

    UIEnableButton(EditBttn,EditEnabled);
    UIEnableButton(SendBttn,SendEnabled);
    UIEnableContextMenu(SendContextMenu,SendEnabled);

    UIEnableContextMenu(ContextMenu_Copy,CopyEnabled);
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
        case e_HDEvent_ContextMenu:
            switch(Event->Info.Context.Menu)
            {
                case e_UICTW_ContextMenu_Edit:
                    MW->ExeCmd(e_Cmd_SendBuffer_Edit);
                break;
                case e_UICTW_ContextMenu_Copy:
                    Copy2Clip();
                break;
                case e_UICTW_ContextMenu_FindCRCAlgorithm:
                case e_UICTW_ContextMenu_CopyToSendBuffer:
                case e_UICTW_ContextMenu_EndianSwap:
                case e_UICTW_ContextMenu_ClearScreen:
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
    t_UIColumnView *BufferList;
    t_UIButtonCtrl *SendBttn;
    int SelectedBuffer;
    const uint8_t *BuffMem;
    uint32_t BuffSize;
    e_CmdType cmd;
    const char *KeyStr;
    char buff[100];

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);
    SendBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_SendBuffers_Send);

    if(!UIColumnViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);

    if(!g_SendBuffers.GetBufferInfo(SelectedBuffer,&BuffMem,&BuffSize))
        Buffer2SendHexDisplay->SetBuffer((uint8_t *)NULL,0);
    else
        Buffer2SendHexDisplay->SetBuffer(BuffMem,BuffSize);

    Buffer2SendHexDisplay->RebuildDisplay();

    if(SelectedBuffer>=0 && SelectedBuffer<MAX_QUICK_SEND_BUFFERS)
        cmd=(e_CmdType)((int)e_Cmd_SendBuffer_Send1+SelectedBuffer);
    else
        cmd=e_CmdMAX;

    /* Find the key mapping for this command */
    if(cmd==e_CmdMAX || !KeySeqMapped(&g_Settings.KeyMapping[cmd]))
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
    t_UIColumnView *BufferList;
    int SelectedBuffer;
    e_CmdType cmd;

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);

    if(!UIColumnViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);

    if(SelectedBuffer<0 || SelectedBuffer>=MAX_QUICK_SEND_BUFFERS)
    {
        /* Just send it directly */
        cmd=e_Cmd_SendBuffer_SendSelectedBuffer;
    }
    else
    {
        cmd=(e_CmdType)((int)e_Cmd_SendBuffer_Send1+SelectedBuffer);
    }
    MW->ExeCmd(cmd);
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
    if(MW->ActiveCon==NULL)
        return;

    if(!g_SendBuffers.Send(MW->ActiveCon,Buffer))
    {
        UIAsk("Error","Failed to send the buffer.",e_AskBox_Error,
                e_AskBttns_Ok);
    }
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
    t_UIColumnView *BufferList;
    int SelectedBuffer;
    char *Name;

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);

    if(!UIColumnViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);

    if(RunEditSendBufferDialog(SelectedBuffer,NULL,0))
    {
        /* Update the UI with any changes */
        Name=g_SendBuffers.GetBufferName(SelectedBuffer);
        if(Name!=NULL)
            UIColumnViewSetColumnText(BufferList,0,SelectedBuffer,Name);

        RethinkBuffer();
    }
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::RenameCurrentBuffer
 *
 * SYNOPSIS:
 *    void MWSendBuffers::RenameCurrentBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function renames the current send buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::RenameCurrentBuffer(void)
{
    t_UIColumnView *BufferList;
    int SelectedBuffer;
    char *Name;
    string NewName;
    char buff[100];

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);

    if(!UIColumnViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);

    Name=g_SendBuffers.GetBufferName(SelectedBuffer);
    if(Name!=NULL)
    {
        NewName=Name;
        if(UITextInputBox("Rename Send Buffer",
                "Change the name of this send buffer",NewName))
        {
            if(NewName=="")
            {
                sprintf(buff,"Buffer %d",SelectedBuffer+1);
                NewName=buff;
            }

            g_SendBuffers.SetBufferName(SelectedBuffer,NewName.c_str());
            g_SendBuffers.SaveBuffers();

            UIColumnViewSetColumnText(BufferList,0,SelectedBuffer,
                    NewName.c_str());
            RethinkBuffer();
        }
    }
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::ClearCurrentBuffer
 *
 * SYNOPSIS:
 *    void MWSendBuffers::ClearCurrentBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clear (sets to blank) the current send buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::ClearCurrentBuffer(void)
{
    t_UIColumnView *BufferList;
    int SelectedBuffer;
    char buff[100];

    if(UIAsk("Are you sure?","Are you sure you want to erase this buffer?",
            e_AskBox_Question,e_AskBttns_YesNo)!=e_AskRet_Yes)
    {
        return;
    }

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);

    if(!UIColumnViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);

    sprintf(buff,"Buffer %d",SelectedBuffer+1);

    g_SendBuffers.SetBufferName(SelectedBuffer,buff);
    g_SendBuffers.SetBuffer(SelectedBuffer,NULL,0);
    g_SendBuffers.SaveBuffers();

    UIColumnViewSetColumnText(BufferList,0,SelectedBuffer,buff);
    RethinkBuffer();
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::LoadOverCurrentBuffer
 *
 * SYNOPSIS:
 *    void MWSendBuffers::LoadOverCurrentBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function loads a new buffer over the current buffer
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::LoadOverCurrentBuffer(void)
{
    string File;
    string LoadFilename;
    char *Name;
    uint8_t *EditBuffer;
    uint32_t BufferSize;
    t_UIColumnView *BufferList;
    int SelectedBuffer;

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);

    if(!UIColumnViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);

    if(!UI_LoadFileReq("Load buffer",g_Session.SendBufferPath,File,
            "Buffers (.buf)|*.buf\nAll Files|*",0))
    {
        return;
    }

    LoadFilename=UI_ConcatFile2Path(g_Session.SendBufferPath,File);

    if(SendBuffer::LoadBufferFromFile(LoadFilename.c_str(),&Name,
            &EditBuffer,&BufferSize))
    {
        g_SendBuffers.SetBufferName(SelectedBuffer,Name);
        g_SendBuffers.SetBuffer(SelectedBuffer,EditBuffer,BufferSize);
        g_SendBuffers.SaveBuffers();

        UIColumnViewSetColumnText(BufferList,0,SelectedBuffer,Name);
        RethinkBuffer();

        free(Name);
        free(EditBuffer);
    }
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::SaveCurrentBuffer
 *
 * SYNOPSIS:
 *    void MWSendBuffers::SaveCurrentBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function saves the current buffer to disk.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::SaveCurrentBuffer(void)
{
    string File;
    string SaveFilename;
    char *Name;
    const uint8_t *EditBuffer;
    uint32_t BufferSize;
    t_UIColumnView *BufferList;
    int SelectedBuffer;

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);

    if(!UIColumnViewHasSelectedEntry(BufferList))
        return;

    SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);

    Name=g_SendBuffers.GetBufferName(SelectedBuffer);
    if(Name==NULL)
        return;

    if(!g_SendBuffers.GetBufferInfo(SelectedBuffer,&EditBuffer,&BufferSize))
        return;

    if(!UI_SaveFileReq("Save buffer",g_Session.SendBufferPath,File,
            "Buffers|*.buf\nAll Files|*",0))
    {
        return;
    }

    SaveFilename=UI_ConcatFile2Path(g_Session.SendBufferPath,File);

    SendBuffer::SaveBuffer2File(SaveFilename.c_str(),Name,EditBuffer,
            BufferSize);
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::NewSendBufferSetLoaded
 *
 * SYNOPSIS:
 *    void MWSendBuffers::NewSendBufferSetLoaded(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function will refresh the display after a new command set is loaded.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::NewSendBufferSetLoaded(void)
{
    RethinkBufferList();
    RethinkBuffer();
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::RethinkBufferList
 *
 * SYNOPSIS:
 *    void MWSendBuffers::RethinkBufferList(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rebuilds the list of buffers and the keys they map to.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::RethinkBufferList(void)
{
    int b;
    char *Name;
    t_UIColumnView *BufferList;
    int SelectedBuffer;
    char KeyNameBuff[100];
    e_CmdType cmd;
    const char *KeyStr;

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);

    SelectedBuffer=0;
    if(UIColumnViewHasSelectedEntry(BufferList))
        SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);

    UIColumnViewClear(BufferList);
    for(b=0;b<MAX_SEND_BUFFERS;b++)
    {
        UIColumnViewAddRow(BufferList);
        Name=g_SendBuffers.GetBufferName(b);
        if(Name!=NULL)
            UIColumnViewSetColumnText(BufferList,0,b,Name);
        else
            UIColumnViewSetColumnText(BufferList,0,b,"ERROR");

        /* Find the key that this buffer is connected to */
        if(MW->ActiveCon==NULL)
        {
            KeyNameBuff[0]=0;
        }
        else
        {
            if(b<MAX_QUICK_SEND_BUFFERS)
            {
                cmd=(e_CmdType)((int)e_Cmd_SendBuffer_Send1+b);

                /* Find the key mapping for this command */
                if(cmd==e_CmdMAX || !KeySeqMapped(&g_Settings.KeyMapping[cmd]))
                {
                    KeyNameBuff[0]=0;
                }
                else
                {
                    KeyStr=ConvertKeySeq2String(&g_Settings.KeyMapping[cmd]);
                    sprintf(KeyNameBuff,"%s",KeyStr);
                }
            }
            else
            {
                if(MW->ActiveCon->IsConnectionBinary())
                    sprintf(KeyNameBuff,"%c",b-MAX_QUICK_SEND_BUFFERS+'A');
                else
                    KeyNameBuff[0]=0;
            }
        }
        UIColumnViewSetColumnText(BufferList,1,b,KeyNameBuff);
    }

    UIColumnViewSelectRow(BufferList,SelectedBuffer);
}

/*******************************************************************************
 * NAME:
 *    MWSendBuffers::SendCurrentBuffer
 *
 * SYNOPSIS:
 *    void MWSendBuffers::SendCurrentBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sends whatever buffer is currently selected.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWSendBuffers::SendCurrentBuffer(void)
{
    t_UIColumnView *BufferList;
    int SelectedBuffer;

    BufferList=UIMW_GetColumnViewHandle(UIWin,e_UIMWColumnView_Buffers_List);

    if(UIColumnViewHasSelectedEntry(BufferList))
    {
        SelectedBuffer=UIGetColumnViewSelectedEntry(BufferList);
        SendBuffer(SelectedBuffer);
    }
}
