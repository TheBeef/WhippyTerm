/*******************************************************************************
 * FILENAME: MW_Upload.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (17 Mar 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/MWPanels/MW_Upload.h"
#include "App/MWPanels/MWPanels.h"
#include "App/MainWindow.h"
#include "App/Connections.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/Settings.h"
#include "App/Session.h"
#include "UI/UIAsk.h"
#include "UI/UIFileReq.h"
#include "UI/UISystem.h"
#include <inttypes.h>
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

MWUpload::MWUpload()
{
    UIWin=NULL;
    MW=NULL;

    OptionWidgets=NULL;

    PanelActive=false;
}

MWUpload::~MWUpload()
{
    if(OptionWidgets!=NULL)
    {
        FTPS_FreeProtocolOptions(OptionWidgets);
        OptionWidgets=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    MWUpload::Setup
 *
 * SYNOPSIS:
 *    void MWUpload::Setup(class TheMainWindow *Parent,
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
void MWUpload::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    t_UIComboBoxCtrl *ProtocolCB;
    unsigned int Index;
    e_UIMenuCtrl *NewMenuItem;
    t_UILabelCtrl *BytesTxLabel;
    char buff[100];

    MW=Parent;
    UIWin=Win;

    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Upload_Protocol);
    BytesTxLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Upload_BytesTx);

    /* Fill in the available upload protocols */
    FTPS_GetListOfFTProtocols(e_FileTransferProtocolMode_Upload,FTPsAvail);

    for(Index=0;Index<FTPsAvail.size();Index++)
    {
        UIAddItem2ComboBox(ProtocolCB,FTPsAvail[Index].DisplayName,Index);

        strncpy(buff,FTPsAvail[Index].DisplayName,sizeof(buff)-4);
        buff[sizeof(buff)-4]=0;
        strcat(buff,"...");

        NewMenuItem=UIMW_AddFTPUploadMenuItem(UIWin,buff,Index);
        MenuItems.push_back(NewMenuItem);
    }

    UISetLabelText(BytesTxLabel,"");
}

/*******************************************************************************
 * NAME:
 *    MWUpload::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWUpload::ActivateCtrls(bool Active);
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
void MWUpload::ActivateCtrls(bool Active)
{
    PanelActive=Active;

    if(UIWin==NULL)
        return;

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWUpload::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWUpload::ConnectionAbout2Changed(void);
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
void MWUpload::ConnectionAbout2Changed(void)
{
    t_UITextInputCtrl *FilenameInput;
    t_UIComboBoxCtrl *ProtocolCB;
    string FileName;
    unsigned int ProtocolIndex;
    t_KVList *Options;

    if(MW->ActiveCon==NULL)
        return;

    FilenameInput=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Upload_Filename);
    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Upload_Protocol);

    /* File name */
    UIGetTextCtrlText(FilenameInput,FileName);
    MW->ActiveCon->SetUploadFilename(FileName.c_str());

    /* Protocol */
    ProtocolIndex=UIGetComboBoxSelectedEntry(ProtocolCB);
    if(ProtocolIndex<FTPsAvail.size())
        MW->ActiveCon->SetUploadProtocol(FTPsAvail[ProtocolIndex].IDStr);

    /* Store the options */
    if(OptionWidgets!=NULL)
    {
        Options=MW->ActiveCon->GetUploadOptionsPtr();
        FTPS_StoreOptions(OptionWidgets,*Options);
        FTPS_FreeProtocolOptions(OptionWidgets);
        OptionWidgets=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    MWUpload::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWUpload::ConnectionChanged(void);
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
void MWUpload::ConnectionChanged(void)
{
    t_UITextInputCtrl *FilenameInput;
    t_UIComboBoxCtrl *ProtocolCB;
    t_UIContainerCtrl *OptionsFrame;
    t_UILabelCtrl *BytesTxLabel;
    string FileName;
    string SelectedProtocolID;
    unsigned int SelProto;
    t_KVList *Options;
    const struct UploadStats *UStat;
    char buff[100];

    if(MW->ActiveCon==NULL)
        return;

    /* Update the UI */
    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Upload_Protocol);
    FilenameInput=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Upload_Filename);
    OptionsFrame=UIMW_GetUploadOptionsFrameContainer(UIWin);
    BytesTxLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Upload_BytesTx);

    UStat=MW->ActiveCon->UploadGetStats();

    sprintf(buff,"%" PRId64,UStat->BytesSent);
    UISetLabelText(BytesTxLabel,buff);

    MW->ActiveCon->GetUploadFilename(FileName);
    UISetTextCtrlText(FilenameInput,FileName.c_str());

    MW->ActiveCon->GetUploadProtocol(SelectedProtocolID);
    for(SelProto=0;SelProto<FTPsAvail.size();SelProto++)
    {
        if(FTPsAvail[SelProto].IDStr==SelectedProtocolID)
        {
            UISetComboBoxSelectedEntry(ProtocolCB,SelProto);
            break;
        }
    }
    /* If we didn't find this upload protocol just select the first thing. */
    if(SelProto==FTPsAvail.size())
    {
        SelProto=0;
        UISetComboBoxSelectedEntry(ProtocolCB,SelProto);
    }

    /* Add the options */
    Options=MW->ActiveCon->GetUploadOptionsPtr();
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
 *    MWUpload::NewConnectionAllocated
 *
 * SYNOPSIS:
 *    void MWUpload::NewConnectionAllocated(class Connection *NewCon);
 *
 * PARAMETERS:
 *    NewCon [I] -- The new connection that was allocated
 *
 * FUNCTION:
 *    This function is called to tell the upload panel that a new connection
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
void MWUpload::NewConnectionAllocated(class Connection *NewCon)
{
    i_ConUploadOptions SessOpt;
    string UniqueID;

    /* Setup this UI to the last stored */
    NewCon->GetConnectionUniqueID(UniqueID);
    SessOpt=g_Session.LastUsedUploadOptions.find(UniqueID);
    if(SessOpt!=g_Session.LastUsedUploadOptions.end())
    {
        NewCon->SetUploadProtocol(SessOpt->second.Protocol.c_str());
        NewCon->SetUploadFilename(SessOpt->second.Filename.c_str());
        *NewCon->GetUploadOptionsPtr()=SessOpt->second.Options;

        /* Update the age so we know we have used it */
        SessOpt->second.Age=time(NULL);
        NoteSessionChanged();
    }

    /* Update the GUI */
    ConnectionChanged();
}

/*******************************************************************************
 * NAME:
 *    MWUpload::RethinkUI
 *
 * SYNOPSIS:
 *    void MWUpload::RethinkUI(void);
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
void MWUpload::RethinkUI(void)
{
    t_UIComboBoxCtrl *ProtocolCB;
    t_UITextInputCtrl *FilenameInput;
    t_UIButtonCtrl *StartBttn;
    t_UIButtonCtrl *AbortBttn;
    t_UIButtonCtrl *SelectFilenameBttn;
    t_UIProgressBarCtrl *ProgressBar;
    t_UIContainerCtrl *OptionsFrame;
    i_FTPUploadMenuList MenuItem;
    const struct UploadStats *UStat;
    bool StartEnabled;
    bool OptionsEnabled;
    bool FileEnabled;
    bool ProtocolEnabled;
    bool ProgressEnabled;
    bool AbortEnabled;
    std::string Path;

    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Upload_Protocol);
    FilenameInput=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Upload_Filename);
    StartBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Upload_Start);
    AbortBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Upload_Abort);
    SelectFilenameBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Upload_SelectFilename);
    ProgressBar=UIMW_GetProgressBarHandle(UIWin,e_UIMWProgressBar_Upload);
    OptionsFrame=UIMW_GetUploadOptionsFrameContainer(UIWin);

    StartEnabled=PanelActive;
    AbortEnabled=PanelActive;
    OptionsEnabled=PanelActive;
    FileEnabled=PanelActive;
    ProtocolEnabled=PanelActive;
    ProgressEnabled=PanelActive;

    if(PanelActive)
    {
        if(MW->ActiveCon==NULL)
            return;

        MW->ActiveCon->GetUploadFilename(Path);

        if(Path=="")
            StartEnabled=false;

        UStat=MW->ActiveCon->UploadGetStats();

        if(UStat->InProgress)
        {
            StartEnabled=false;
            OptionsEnabled=false;
            FileEnabled=false;
            ProtocolEnabled=false;
            ProgressEnabled=true;
        }
        else
        {
            AbortEnabled=false;
            ProgressEnabled=false;
        }
    }

    UIEnableButton(StartBttn,StartEnabled);
    UIEnableButton(AbortBttn,AbortEnabled);
    UIEnableButton(SelectFilenameBttn,FileEnabled);

    UIEnableComboBox(ProtocolCB,ProtocolEnabled);

    UIEnableProgressBar(ProgressBar,ProgressEnabled);

    for(MenuItem=MenuItems.begin();MenuItem!=MenuItems.end();MenuItem++)
        UIEnableMenu(*MenuItem,ProtocolEnabled);

    UIEnableTextCtrl(FilenameInput,FileEnabled);

    UIEnableContainerCtrl(OptionsFrame,OptionsEnabled);
}

/*******************************************************************************
 * NAME:
 *    MWUpload::Start
 *
 * SYNOPSIS:
 *    void MWUpload::Start(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function starts an upload transfer using the currently selected
 *    protocol.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    MWUpload::Abort()
 ******************************************************************************/
void MWUpload::Start(void)
{
    t_UIProgressBarCtrl *ProgressBar;
    const struct UploadStats *UStat;
    t_KVList *Options;
    i_ConUploadOptions SessOpt;
    time_t KillTime;    // We kill any records older than this value
    string UniqueID;
    struct ConUploadOptions NewUploadOptions;

    if(UIWin==NULL || MW->ActiveCon==NULL)
        return;

    try
    {
        Options=NULL;

        /* Store the options */
        if(OptionWidgets!=NULL)
        {
            Options=MW->ActiveCon->GetUploadOptionsPtr();
            FTPS_StoreOptions(OptionWidgets,*Options);
        }

        /* Update options to the session */
        MW->ActiveCon->GetConnectionUniqueID(UniqueID);

        /* See if we have uploaded to this connection before */
        SessOpt=g_Session.LastUsedUploadOptions.find(UniqueID);
        if(SessOpt==g_Session.LastUsedUploadOptions.end())
        {
            /* Ok, we don't have any history for this connection add an entry */
            g_Session.LastUsedUploadOptions.insert(
                    make_pair(UniqueID,NewUploadOptions));
            SessOpt=g_Session.LastUsedUploadOptions.find(UniqueID);
            if(SessOpt==g_Session.LastUsedUploadOptions.end())
            {
                /* Hu? */
                throw("Internal error.  Could not find last used upload "
                        "option we just added.");
            }
        }

        SessOpt->second.Age=time(NULL);
        MW->ActiveCon->GetUploadProtocol(SessOpt->second.Protocol);
        MW->ActiveCon->GetUploadFilename(SessOpt->second.Filename);
        if(Options!=NULL)
            SessOpt->second.Options=*Options;

        /* Next remove any that are too old */
        KillTime=time(NULL)-60*60*24*365; // Anything older than 1 year gets deleted
        for(SessOpt=g_Session.LastUsedUploadOptions.begin();
                SessOpt!=g_Session.LastUsedUploadOptions.end();
                SessOpt++)
        {
            if(SessOpt->second.Age<KillTime)
            {
                g_Session.LastUsedUploadOptions.erase(SessOpt);
                /* Start again */
                SessOpt=g_Session.LastUsedUploadOptions.begin();
            }
        }
        NoteSessionChanged();

        ProgressBar=UIMW_GetProgressBarHandle(UIWin,e_UIMWProgressBar_Upload);

        switch(MW->ActiveCon->StartUpload())
        {
            case e_FileTransErr_Success:
                UStat=MW->ActiveCon->UploadGetStats();

                /* Update the progress bar.  We also need to update it's
                   current progress after we change the total size */
                UISetProgressBarSteps(ProgressBar,UStat->TotalFileSize);
                UploadStatChanged();

                RethinkUI();
            break;
            case e_FileTransErr_FileError:
                UIAsk("File upload error",
                        "There was an error opening file for upload",
                        e_AskBox_Error,e_AskBttns_Ok);
            break;
            case e_FileTransErr_ProtocolInitFail:
                UIAsk("File upload error",
                        "There was an error setting up the protocol plugin to "
                        "do the upload.",e_AskBox_Error,e_AskBttns_Ok);
            break;
            case e_FileTransErrMAX:
            default:
            break;
        }
    }
    catch(const char *Msg)
    {
        UIAsk("File upload error",Msg,e_AskBox_Error,e_AskBttns_Ok);
    }
    catch(...)
    {
        UIAsk("File upload error","Internal error tring start upload",
                e_AskBox_Error,e_AskBttns_Ok);
    }
}

/*******************************************************************************
 * NAME:
 *    MWUpload::Abort
 *
 * SYNOPSIS:
 *    void MWUpload::Abort(void);
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
 *    MWUpload::Start()
 ******************************************************************************/
void MWUpload::Abort(void)
{
    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->AbortUpload();
    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWUpload::SelectFilename
 *
 * SYNOPSIS:
 *    void MWUpload::SelectFilename(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function selects a filename for the upload.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWUpload::SelectFilename(void)
{
    std::string Path;
    std::string Filename;
    t_UITextInputCtrl *FilenameInput;

    if(!PanelActive)
        return;

    FilenameInput=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Upload_Filename);

    UIGetTextCtrlText(FilenameInput,Path);

    Filename="";
    if(Path=="")
        Filename="Upload.txt";
    if(UI_LoadFileReq("Upload file",Path,Filename,
            "All Files|*\n",0))
    {
        Filename=UI_ConcatFile2Path(Path,Filename);
        UISetTextCtrlText(FilenameInput,Filename.c_str());
        MW->ActiveCon->SetUploadFilename(Filename.c_str());
    }

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWUpload::UploadProtocolChange
 *
 * SYNOPSIS:
 *    void MWUpload::UploadProtocolChange(int Index);
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
void MWUpload::UploadProtocolChange(int Index)
{
    t_UIContainerCtrl *OptionsFrame;
    t_KVList *Options;
    string OldIDStr;

    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->GetUploadProtocol(OldIDStr);
    MW->ActiveCon->SetUploadProtocol(FTPsAvail[Index].IDStr);

    /* Handle changing the options */
    Options=MW->ActiveCon->GetUploadOptionsPtr();

    OptionsFrame=UIMW_GetUploadOptionsFrameContainer(UIWin);
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
 *    MWUpload::NoteFilenameChanged
 *
 * SYNOPSIS:
 *    void MWUpload::NoteFilenameChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the filename input has been changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWUpload::NoteFilenameChanged(void)
{
    t_UITextInputCtrl *FilenameInput;
    string Filename;

    FilenameInput=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Upload_Filename);

    UIGetTextCtrlText(FilenameInput,Filename);
    MW->ActiveCon->SetUploadFilename(Filename.c_str());

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWUpload::UploadStatChanged
 *
 * SYNOPSIS:
 *    void MWUpload::UploadStatChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to tell the upload panel that it need to
 *    refresh the upload progress stats.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWUpload::UploadStatChanged(void)
{
    t_UIProgressBarCtrl *ProgressBar;
    t_UILabelCtrl *BytesTxLabel;
    const struct UploadStats *UStat;
    char buff[100];

    if(UIWin==NULL || MW->ActiveCon==NULL)
        return;

    ProgressBar=UIMW_GetProgressBarHandle(UIWin,e_UIMWProgressBar_Upload);
    BytesTxLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Upload_BytesTx);

    UStat=MW->ActiveCon->UploadGetStats();

    UISetProgressBarLevel(ProgressBar,UStat->BytesSent);

    sprintf(buff,"%" PRId64,UStat->BytesSent);
    UISetLabelText(BytesTxLabel,buff);
}

/*******************************************************************************
 * NAME:
 *    MWUpload::InformOfUploadAborted
 *
 * SYNOPSIS:
 *    void MWUpload::InformOfUploadAborted(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function should be called when the upload driver aborts an upload.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWUpload::InformOfUploadAborted(void)
{
    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWUpload::InformOfUploadDone
 *
 * SYNOPSIS:
 *    void MWUpload::InformOfUploadDone(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function should be called when the upload driver finishs an upload.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWUpload::InformOfUploadDone(void)
{
    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWUpload::UploadMenuTriggered
 *
 * SYNOPSIS:
 *    void MWUpload::UploadMenuTriggered(uint64_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The ID of the menu that was triggered (this was added with
 *              UIMW_AddFTPUploadMenuItem()
 *
 * FUNCTION:
 *    This function is called when the user selects an item in the upload
 *    menu.  It starts an upload using that protocol.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWUpload::UploadMenuTriggered(uint64_t ID)
{
    t_UIComboBoxCtrl *ProtocolCB;

    /* Fill in the available download protocols */
    FTPS_GetListOfFTProtocols(e_FileTransferProtocolMode_Upload,FTPsAvail);

    if(ID>=FTPsAvail.size())
    {
        UIAsk("File upload error",
                "Unknown file transfer protocol.",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    ProtocolCB=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Upload_Protocol);

    /* See if we already have this protocol selected or not */
    if(ID!=UIGetComboBoxSelectedEntry(ProtocolCB))
    {
        /* Change to this protocol */
        UISetComboBoxSelectedEntry(ProtocolCB,ID);
        UploadProtocolChange(ID);
    }

    /* Prompt for the filename (don't trust the input widget) */
    SelectFilename();

    Start();
}

