/*******************************************************************************
 * FILENAME: MW_Download.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI for the download panel in it.
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
 *    Paul Hutchinson (04 May 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/MWPanels/MW_Download.h"
#include "App/MWPanels/MWPanels.h"
#include "App/MainWindow.h"
#include "App/Connections.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/Settings.h"
#include "App/Session.h"
#include "OS/Directorys.h"
#include "UI/UIAsk.h"
#include "UI/UIFileReq.h"
#include "UI/UISystem.h"
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <string>
#include <algorithm>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool SortFTPsList(const struct FTPS_ProtocolInfo &A,const struct FTPS_ProtocolInfo &B);

/*** VARIABLE DEFINITIONS     ***/

MWDownload::MWDownload()
{
    UIWin=NULL;
    MW=NULL;

    OptionWidgets=NULL;

    PanelActive=false;
}

MWDownload::~MWDownload()
{
    if(OptionWidgets!=NULL)
    {
        FTPS_FreeProtocolOptions(OptionWidgets);
        OptionWidgets=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    SortFTPsList
 *
 * SYNOPSIS:
 *    static bool SortFTPsList(const struct FTPS_ProtocolInfo &A,const struct FTPS_ProtocolInfo &B);
 *
 * PARAMETERS:
 *    A [I] -- The first thing to cmp
 *    B [I] -- The second thing to cmp
 *
 * FUNCTION:
 *    This is a helper function for the sort() of protocols.
 *
 * RETURNS:
 *    true -- first argument is less than the second
 *    false -- first argument is greater or equ to the second
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool SortFTPsList(const struct FTPS_ProtocolInfo &A,
        const struct FTPS_ProtocolInfo &B)
{
    return strcmp(A.DisplayName,B.DisplayName)<0;
}

/*******************************************************************************
 * NAME:
 *    MWDownload::Setup
 *
 * SYNOPSIS:
 *    void MWDownload::Setup(class TheMainWindow *Parent,
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
void MWDownload::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    t_UILabelCtrl *FilenameLabel;
    t_UILabelCtrl *BytesRxLabel;

    MW=Parent;
    UIWin=Win;

    FilenameLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Download_Filename);
    BytesRxLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Download_BytesRx);

    RescanAvailableProtocols();

    UISetLabelText(BytesRxLabel,"");
    UISetLabelText(FilenameLabel,"");
}

/*******************************************************************************
 * NAME:
 *    MWDownload::RescanAvailableProtocols()
 *
 * SYNOPSIS:
 *    void MWDownload::RescanAvailableProtocols(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the current protocols and reloads the list from
 *    the FTPS and rebuilds the list.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWDownload::RescanAvailableProtocols(void)
{
    t_UIComboBoxCtrl *ProtocolCB;
    string SelProtoIDStr;
    int SelectedIndex;
    char buff[100];
    unsigned int Index;
    e_UIMenuCtrl *NewMenuItem;

    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Download_Protocol);

    /* Find out what protocol is currently selected */
    SelProtoIDStr="";
    SelectedIndex=UIGetComboBoxSelectedIndex(ProtocolCB);
    if(SelectedIndex>=0 && SelectedIndex<(int)FTPsAvail.size())
        SelProtoIDStr=FTPsAvail[SelectedIndex].IDStr;

    /* Fill in the available download protocols */
    FTPS_GetListOfFTProtocols(e_FileTransferProtocolMode_Download,FTPsAvail);

    /* Sort them */
    sort(FTPsAvail.begin(),FTPsAvail.end(),SortFTPsList);

    /* Build the combox and the menu */
    UIMW_AddFTPDownloadClearAllMenus(UIWin);
    MenuItems.clear();
    UIClearComboBox(ProtocolCB);
    SelectedIndex=-1;
    for(Index=0;Index<FTPsAvail.size();Index++)
    {
        if(SelProtoIDStr==FTPsAvail[Index].IDStr)
            SelectedIndex=Index;

        UIAddItem2ComboBox(ProtocolCB,FTPsAvail[Index].DisplayName,Index);

        strncpy(buff,FTPsAvail[Index].DisplayName,sizeof(buff)-4);
        buff[sizeof(buff)-4]=0;
        strcat(buff,"...");

        NewMenuItem=UIMW_AddFTPDownloadMenuItem(UIWin,buff,Index);
        MenuItems.push_back(NewMenuItem);
    }

    if(SelectedIndex>=0)
        UISetComboBoxSelectedEntry(ProtocolCB,SelectedIndex);
}

/*******************************************************************************
 * NAME:
 *    MWDownload::FreePluginResourcesIfNeeded
 *
 * SYNOPSIS:
 *    void MWDownload::FreePluginResourcesIfNeeded(const char *PluginIDStr);
 *
 * PARAMETERS:
 *    PluginIDStr [I] -- The plugin ID string for the plugin to free the
 *                       resources of
 *
 * FUNCTION:
 *    This function is called to tell us to release any resources used by
 *    a plugin.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWDownload::FreePluginResourcesIfNeeded(const char *PluginIDStr)
{
    t_UIComboBoxCtrl *ProtocolCB;
    string FileName;
    unsigned int ProtocolIndex;
    t_KVList *Options;

    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Download_Protocol);
    ProtocolIndex=UIGetComboBoxSelectedEntry(ProtocolCB);

    /* Check if this is the active protocol */
    if(ProtocolIndex>=FTPsAvail.size())
        return;

    /* See if this is the selected protocol */
    if(strcmp(FTPsAvail[ProtocolIndex].IDStr,PluginIDStr)!=0)
        return;

    /* We are using this plugin free the resources */

    /* Save any changes */
    if(MW->ActiveCon!=NULL && OptionWidgets!=NULL)
    {
        MW->ActiveCon->SetDownloadProtocol(FTPsAvail[ProtocolIndex].IDStr);
        Options=MW->ActiveCon->GetDownloadOptionsPtr();
        FTPS_StoreOptions(OptionWidgets,*Options);
    }

    if(OptionWidgets!=NULL)
        FTPS_FreeProtocolOptions(OptionWidgets);
    OptionWidgets=NULL;
}

/*******************************************************************************
 * NAME:
 *    MWDownload::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWDownload::ActivateCtrls(bool Active);
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
void MWDownload::ActivateCtrls(bool Active)
{
    PanelActive=Active;

    if(UIWin==NULL)
        return;

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWDownload::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWDownload::ConnectionAbout2Changed(void);
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
void MWDownload::ConnectionAbout2Changed(void)
{
    t_UIComboBoxCtrl *ProtocolCB;
    string FileName;
    unsigned int ProtocolIndex;
    t_KVList *Options;

    if(MW->ActiveCon==NULL)
        return;

    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Download_Protocol);

    /* Protocol */
    ProtocolIndex=UIGetComboBoxSelectedEntry(ProtocolCB);
    if(ProtocolIndex<FTPsAvail.size())
        MW->ActiveCon->SetDownloadProtocol(FTPsAvail[ProtocolIndex].IDStr);

    /* Store the options */
    if(OptionWidgets!=NULL)
    {
        Options=MW->ActiveCon->GetDownloadOptionsPtr();
        FTPS_StoreOptions(OptionWidgets,*Options);
        FTPS_FreeProtocolOptions(OptionWidgets);
        OptionWidgets=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    MWDownload::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWDownload::ConnectionChanged(void);
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
void MWDownload::ConnectionChanged(void)
{
    const struct DownloadStats *DLStat;
    t_UIComboBoxCtrl *ProtocolCB;
    t_UILayoutContainerCtrl *OptionsFrame;
    string FileName;
    string SelectedProtocolID;
    unsigned int SelProto;
    t_KVList *Options;
    t_UILabelCtrl *FilenameLabel;
    t_UILabelCtrl *BytesRxLabel;
    char buff[100];

    if(MW->ActiveCon==NULL)
        return;

    /* Update the UI */
    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Download_Protocol);
    FilenameLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Download_Filename);
    BytesRxLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Download_BytesRx);
    OptionsFrame=UIMW_GetDownloadOptionsFrameContainer(UIWin);
    DLStat=MW->ActiveCon->DownloadGetStats();

    sprintf(buff,"%" PRId64,DLStat->BytesRx);
    UISetLabelText(BytesRxLabel,buff);
    UISetLabelText(FilenameLabel,Basename(MW->ActiveCon->
            GetDownloadFileName()));

    MW->ActiveCon->GetDownloadProtocol(SelectedProtocolID);
    for(SelProto=0;SelProto<FTPsAvail.size();SelProto++)
    {
        if(FTPsAvail[SelProto].IDStr==SelectedProtocolID)
        {
            UISetComboBoxSelectedEntry(ProtocolCB,SelProto);
            break;
        }
    }
    /* If we didn't find this downoad protocol just select the first thing. */
    if(SelProto==FTPsAvail.size())
    {
        SelProto=0;
        UISetComboBoxSelectedEntry(ProtocolCB,SelProto);
    }

    /* Add the options */
    Options=MW->ActiveCon->GetDownloadOptionsPtr();
    if(OptionWidgets!=NULL)
    {
        FTPS_FreeProtocolOptions(OptionWidgets);
        OptionWidgets=NULL;
    }
    OptionWidgets=FTPS_AllocProtocolOptions(FTPsAvail[SelProto].IDStr,
            OptionsFrame,*Options);

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWDownload::NewConnectionAllocated
 *
 * SYNOPSIS:
 *    void MWDownload::NewConnectionAllocated(class Connection *NewCon);
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
void MWDownload::NewConnectionAllocated(class Connection *NewCon)
{
    i_ConDownloadOptions SessOpt;
    string UniqueID;
    t_UIComboBoxCtrl *ProtocolCB;

    /* Setup this UI to the last stored */
    NewCon->GetConnectionUniqueID(UniqueID);
    SessOpt=g_Session.LastUsedDownloadOptions.find(UniqueID);
    if(SessOpt!=g_Session.LastUsedDownloadOptions.end())
    {
        NewCon->SetDownloadProtocol(SessOpt->second.Protocol.c_str());
//        NewCon->SetDownloadFilename(SessOpt->second.Filename.c_str());
        *NewCon->GetDownloadOptionsPtr()=SessOpt->second.Options;

        /* Update the age so we know we have used it */
        SessOpt->second.Age=time(NULL);
        NoteSessionChanged();
    }

    /* Make sure the first entry is selected */
    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Download_Protocol);
    UISetComboBoxSelectedEntry(ProtocolCB,0);
    DownloadProtocolChange(0);

    /* Update the GUI */
    ConnectionChanged();
}

/*******************************************************************************
 * NAME:
 *    MWDownload::RethinkUI
 *
 * SYNOPSIS:
 *    void MWDownload::RethinkUI(void);
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
void MWDownload::RethinkUI(void)
{
    t_UIComboBoxCtrl *ProtocolCB;
    t_UIButtonCtrl *StartBttn;
    t_UIButtonCtrl *AbortBttn;
    t_UIProgressBarCtrl *ProgressBar;
    t_UILayoutContainerCtrl *OptionsFrame;
    i_FTPDownloadMenuList MenuItem;
    const struct UploadStats *UStat;
    const struct DownloadStats *DLStat;
    bool StartEnabled;
    bool OptionsEnabled;
    bool ProtocolEnabled;
    bool ProgressEnabled;
    bool AbortEnabled;

    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Download_Protocol);
    StartBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Download_Start);
    AbortBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Download_Abort);
    ProgressBar=UIMW_GetProgressBarHandle(UIWin,e_UIMWProgressBar_Download);
    OptionsFrame=UIMW_GetDownloadOptionsFrameContainer(UIWin);

    StartEnabled=PanelActive;
    AbortEnabled=PanelActive;
    OptionsEnabled=PanelActive;
    ProtocolEnabled=PanelActive;
    ProgressEnabled=PanelActive;

    if(PanelActive)
    {
        if(MW->ActiveCon==NULL)
            return;

        UStat=MW->ActiveCon->UploadGetStats();
        DLStat=MW->ActiveCon->DownloadGetStats();

        if(UStat->InProgress)
        {
            StartEnabled=false;
            OptionsEnabled=false;
            ProtocolEnabled=false;
            ProgressEnabled=false;
            AbortEnabled=false;
            ProgressEnabled=false;
        }
        else
        {
            if(DLStat->InProgress)
            {
                StartEnabled=false;
                OptionsEnabled=false;
                ProtocolEnabled=false;

                /* Only enable if we know the file size */
                if(DLStat->TotalFileSize>0)
                    ProgressEnabled=true;
            }
            else
            {
                AbortEnabled=false;
                ProgressEnabled=false;
            }
        }
    }

    UIEnableButton(StartBttn,StartEnabled);
    UIEnableButton(AbortBttn,AbortEnabled);

    UIEnableComboBox(ProtocolCB,ProtocolEnabled);

    UIEnableProgressBar(ProgressBar,ProgressEnabled);
    UIProgressBarVisible(ProgressBar,!ProgressEnabled);

    for(MenuItem=MenuItems.begin();MenuItem!=MenuItems.end();MenuItem++)
        UIEnableMenu(*MenuItem,ProtocolEnabled);

    UIEnableContainerCtrl(OptionsFrame,OptionsEnabled);
}

/*******************************************************************************
 * NAME:
 *    MWDownload::Start
 *
 * SYNOPSIS:
 *    void MWDownload::Start(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function starts an download transfer using the currently selected
 *    protocol.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    MWDownload::Abort()
 ******************************************************************************/
void MWDownload::Start(void)
{
    t_UIProgressBarCtrl *ProgressBar;
//    const struct DownloadStats *DLStat;
    t_KVList *Options;
    i_ConDownloadOptions SessOpt;
    time_t KillTime;    // We kill any records older than this value
    string UniqueID;
    struct ConDownloadOptions NewDownloadOptions;

    if(UIWin==NULL || MW->ActiveCon==NULL)
        return;

    try
    {
        Options=NULL;
        /* Store the options */
        if(OptionWidgets!=NULL)
        {
            Options=MW->ActiveCon->GetDownloadOptionsPtr();
            FTPS_StoreOptions(OptionWidgets,*Options);
        }

        /* Update options to the session */
        MW->ActiveCon->GetConnectionUniqueID(UniqueID);

        /* See if we have download from this connection before */
        SessOpt=g_Session.LastUsedDownloadOptions.find(UniqueID);
        if(SessOpt==g_Session.LastUsedDownloadOptions.end())
        {
            /* Ok, we don't have any history for this connection add an entry */
            g_Session.LastUsedDownloadOptions.insert(
                    make_pair(UniqueID,NewDownloadOptions));
            SessOpt=g_Session.LastUsedDownloadOptions.find(UniqueID);
            if(SessOpt==g_Session.LastUsedDownloadOptions.end())
            {
                /* Hu? */
                throw("Internal error.  Could not find last used download "
                        "options we just added.");
            }
        }

        SessOpt->second.Age=time(NULL);
        MW->ActiveCon->GetDownloadProtocol(SessOpt->second.Protocol);
//        MW->ActiveCon->GetDownloadFilename(SessOpt->second.Filename);
        if(Options!=NULL)
            SessOpt->second.Options=*Options;

        /* Next remove any that are too old */
        KillTime=time(NULL)-60*60*24*365; // Anything older than 1 year gets deleted
        for(SessOpt=g_Session.LastUsedDownloadOptions.begin();
                SessOpt!=g_Session.LastUsedDownloadOptions.end();
                SessOpt++)
        {
            if(SessOpt->second.Age<KillTime)
            {
                g_Session.LastUsedDownloadOptions.erase(SessOpt);
                /* Start again */
                SessOpt=g_Session.LastUsedDownloadOptions.begin();
            }
        }
        NoteSessionChanged();

        ProgressBar=UIMW_GetProgressBarHandle(UIWin,e_UIMWProgressBar_Download);

        switch(MW->ActiveCon->StartDownload())
        {
            case e_FileTransErr_Success:
//                DLStat=MW->ActiveCon->DownloadGetStats();

                /* Update the progress bar.  We also need to update it's
                   current progress after we change the total size */
                UISetProgressBarSteps(ProgressBar,1);
                DownloadStatChanged();

                RethinkUI();
            break;
            case e_FileTransErr_ProtocolInitFail:
                UIAsk("File download error",
                        "There was an error setting up the protocol plugin to "
                        "do the download.",e_AskBox_Error,e_AskBttns_Ok);
            break;
            case e_FileTransErrMAX:
            case e_FileTransErr_FileError:
            default:
            break;
        }
    }
    catch(const char *Msg)
    {
        UIAsk("File download error",Msg,e_AskBox_Error,e_AskBttns_Ok);
    }
    catch(...)
    {
        UIAsk("File download error","Internal error tring start download",
                e_AskBox_Error,e_AskBttns_Ok);
    }
}

/*******************************************************************************
 * NAME:
 *    MWDownload::Abort
 *
 * SYNOPSIS:
 *    void MWDownload::Abort(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when you want to abort a transfer that is in
 *    progress
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    MWDownload::Start()
 ******************************************************************************/
void MWDownload::Abort(void)
{
    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->AbortDownload();
    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWDownload::DownloadProtocolChange
 *
 * SYNOPSIS:
 *    void MWDownload::DownloadProtocolChange(int Index);
 *
 * PARAMETERS:
 *    Index [I] -- The new protocol to select
 *
 * FUNCTION:
 *    This function changes the selected protocol.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWDownload::DownloadProtocolChange(int Index)
{
    t_UILayoutContainerCtrl *OptionsFrame;
    t_KVList *Options;
    string OldIDStr;

    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->GetDownloadProtocol(OldIDStr);
    MW->ActiveCon->SetDownloadProtocol(FTPsAvail[Index].IDStr);

    /* Handle changing the options */
    Options=MW->ActiveCon->GetDownloadOptionsPtr();

    OptionsFrame=UIMW_GetDownloadOptionsFrameContainer(UIWin);
    if(OptionWidgets!=NULL)
    {
        FTPS_FreeProtocolOptions(OptionWidgets);
        OptionWidgets=NULL;
    }
    if(FTPsAvail[Index].IDStr!=OldIDStr)
        Options->clear();
    OptionWidgets=FTPS_AllocProtocolOptions(FTPsAvail[Index].IDStr,
            OptionsFrame,*Options);
}

/*******************************************************************************
 * NAME:
 *    MWDownload::DownloadStatChanged
 *
 * SYNOPSIS:
 *    void MWDownload::DownloadStatChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to tell the download panel that it need to
 *    refresh the download progress stats.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWDownload::DownloadStatChanged(void)
{
    t_UIProgressBarCtrl *ProgressBar;
    const struct DownloadStats *DLStat;
    t_UILabelCtrl *FilenameLabel;
    t_UILabelCtrl *BytesRxLabel;
    char buff[100];

    if(UIWin==NULL || MW->ActiveCon==NULL)
        return;

    ProgressBar=UIMW_GetProgressBarHandle(UIWin,e_UIMWProgressBar_Download);
    FilenameLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Download_Filename);
    BytesRxLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Download_BytesRx);

    DLStat=MW->ActiveCon->DownloadGetStats();

    if(DLStat->TotalFileSize==0)
    {
        /* Go to 100% */
        UISetProgressBarSteps(ProgressBar,1);
        UISetProgressBarLevel(ProgressBar,1);
    }
    else
    {
        UISetProgressBarSteps(ProgressBar,DLStat->TotalFileSize);
        UISetProgressBarLevel(ProgressBar,DLStat->BytesRx);
    }
    sprintf(buff,"%" PRId64,DLStat->BytesRx);
    UISetLabelText(BytesRxLabel,buff);

    UISetLabelText(FilenameLabel,Basename(MW->ActiveCon->
            GetDownloadFileName()));
}

/*******************************************************************************
 * NAME:
 *    MWDownload::InformOfDownloadAborted
 *
 * SYNOPSIS:
 *    void MWDownload::InformOfDownloadAborted(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function should be called when the download driver aborts an
 *    download.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWDownload::InformOfDownloadAborted(void)
{
//    UIAsk("File download aborted","The download has been aborted",
//            e_AskBox_Warning,e_AskBttns_Ok);

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWDownload::InformOfDownloadDone
 *
 * SYNOPSIS:
 *    void MWDownload::InformOfDownloadDone(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function should be called when the download driver finishs a
 *    download.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWDownload::InformOfDownloadDone(void)
{
    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWDownload::DownloadMenuTriggered
 *
 * SYNOPSIS:
 *    void MWDownload::DownloadMenuTriggered(uint64_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The ID of the menu that was triggered (this was added with
 *              UIMW_AddFTPDownloadMenuItem()
 *
 * FUNCTION:
 *    This function is called when the user selects an item in the download
 *    menu.  It starts an download using that protocol.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWDownload::DownloadMenuTriggered(uint64_t ID)
{
    t_UIComboBoxCtrl *ProtocolCB;

    /* Fill in the available download protocols */
    FTPS_GetListOfFTProtocols(e_FileTransferProtocolMode_Download,FTPsAvail);

    if(ID>=FTPsAvail.size())
    {
        UIAsk("File download error",
                "Unknown file transfer protocol.",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Download_Protocol);

    /* See if we already have this protocol selected or not */
    if(ID!=UIGetComboBoxSelectedEntry(ProtocolCB))
    {
        /* Change to this protocol */
        UISetComboBoxSelectedEntry(ProtocolCB,ID);
        DownloadProtocolChange(ID);
    }

    /* Show the download panel */
    MW->ShowPanel(e_MWPanels_Downloadload);

    Start();
}

/*******************************************************************************
 * NAME:
 *    MWDownload::UpdateGUI
 *
 * SYNOPSIS:
 *    void MWDownload::UpdateGUI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function tells the panel to rethink it's UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWDownload::UpdateGUI(void)
{
    RethinkUI();
}
