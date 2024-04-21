/*******************************************************************************
 * FILENAME: Dialog_NewConnection.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file runs the new connection dialog
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Session.h"
#include "App/MainWindow.h"
#include "App/Settings.h"
#include "App/Connections.h"
#include "App/IOSystem.h"
#include "App/MaxSizes.h"
#include "UI/UIAsk.h"
#include "UI/UINewConnection.h"
#include <string>
#include <map>
#include <string.h>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct DNCConOptions
{
    char Name[MAX_CONNECTION_NAME_LEN+1];
    t_KVList Options;
};

typedef std::map<std::string,struct DNCConOptions> t_DNCConOptions;
typedef t_DNCConOptions::iterator i_DNCConOptions;

/*** FUNCTION PROTOTYPES      ***/
static bool DNC_GetListOfConnections(void);
static void FillInConnectionPullDown(void);
static void DNC_StoreOptionsHistory(void);
static void DNC_UpdateSessionConnectionInfo(t_UIComboBoxCtrl *ConComboBox);
static void DNC_OpenConnection(class TheMainWindow *MW);
static void DNC_OptionsChanged(void *UserData);

/*** VARIABLE DEFINITIONS     ***/
static struct ConnectionInfoList *m_DNC_Connections;
static t_ConnectionOptionsDataType *m_DNC_CurrentOptions;
static struct ConnectionInfoList *m_DNC_LastOptions;
static bool m_IgnoreConnectionListChange;
static t_DNCConOptions m_DNC_ConOptions;
static bool m_NameChanged;

/*******************************************************************************
 * NAME:
 *    RunNewConnectionDialog
 *
 * SYNOPSIS:
 *    void RunNewConnectionDialog(class TheMainWindow *MW);
 *
 * PARAMETERS:
 *    MW [I] -- The main window to take info about (the main window that
 *              was active when the command was run)
 *
 * FUNCTION:
 *    This function shows the new connection dialog and fill in all the
 *    defaults.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
void RunNewConnectionDialog(class TheMainWindow *MW)
{
    bool AllocatedUI;

    AllocatedUI=false;
    m_DNC_Connections=NULL;
    m_IgnoreConnectionListChange=false;
    m_DNC_CurrentOptions=NULL;
    m_DNC_LastOptions=NULL;
    m_NameChanged=false;
    try
    {
        m_DNC_ConOptions.clear();

        if(!UIAlloc_NewConnection())
            throw("Failed to open window");

        FillInConnectionPullDown();

        if(UIShow_NewConnection())
        {
            /* User clicked ok */
            DNC_OpenConnection(MW);
        }
    }
    catch(const char *Error)
    {
        if(Error!=nullptr)
        {
            UIAsk("Error opening new connection",Error,e_AskBox_Error,
                    e_AskBttns_Ok);
        }
    }
    catch(...)
    {
        UIAsk("Error opening new connection","Failed to open new connection",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    if(m_DNC_CurrentOptions!=NULL)
        IOS_FreeConnectionOptions(m_DNC_CurrentOptions);
    if(m_DNC_Connections!=NULL)
        IOS_FreeListOfAvailableConnections(m_DNC_Connections);
    if(AllocatedUI)
        UIFree_NewConnection();
}

///*******************************************************************************
// * NAME:
// *    InformOf_NC_ConnectionListChange
// *
// * SYNOPSIS:
// *    void InformOf_NC_ConnectionListChange(uintptr_t ID);
// *
// * PARAMETERS:
// *    ID [I] -- The ID of the connection what was selected.  This can be
// *              a pointer.
// *
// * FUNCTION:
// *    This function is called when the user changes the connection list
// *    selection from the UI.
// *
// *    It clears the old options (if needed) and adds new ones.
// *
// * RETURNS:
// *    NONE
// *
// * SEE ALSO:
// *    
// *******************************************************************************
// * REVISION HISTORY:
// *    Paul Hutchinson (27 Sep 2018)
// *       Created
// ******************************************************************************/
//void InformOf_NC_ConnectionListChange(uintptr_t ID)

/*******************************************************************************
 * NAME:
 *    NC_ChangeConnectionListSelectedEntry
 *
 * SYNOPSIS:
 *    void NC_ChangeConnectionListSelectedEntry(struct ConnectionInfoList *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The new selected connection.
 *
 * FUNCTION:
 *    This function changes the currently selected connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void NC_ChangeConnectionListSelectedEntry(struct ConnectionInfoList *Con)
{
    string DetectedID;
    i_DNCConOptions DNCOptions;    // The options for this new connection
    t_UITextInputCtrl *NameInput;

    if(m_IgnoreConnectionListChange)
        return;

    /* Find this connection's options */
    DetectedID=Con->UniqueID;

    DNCOptions=m_DNC_ConOptions.find(DetectedID);
    if(DNCOptions==m_DNC_ConOptions.end())
    {
        /* Hu?  We didn't find the options? Should never happen */
        return;
    }

    /* Save the connections options to our temp list of options (so if the
       user reselects this connection we can restore with what they had
       selected) */
    DNC_StoreOptionsHistory();

    if(m_DNC_CurrentOptions!=NULL)
    {
        /* Free the old options */
        IOS_FreeConnectionOptions(m_DNC_CurrentOptions);
    }

    m_DNC_CurrentOptions=IOS_AllocConnectionOptions(Con,
            UINC_GetOptionsFrameContainer(),DNCOptions->second.Options,
            DNC_OptionsChanged,NULL);
    m_DNC_LastOptions=Con;

    NameInput=UINC_GetTxtInputHandle(e_UINC_TxtInput_Name);
    UISetTextCtrlText(NameInput,DNCOptions->second.Name);
    m_NameChanged=false;
}

/*******************************************************************************
 * NAME:
 *    DNC_GetListOfConnections
 *
 * SYNOPSIS:
 *    static bool DNC_GetListOfConnections(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the list of available connections.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool DNC_GetListOfConnections(void)
{
    struct ConnectionInfoList *Entry;
    string DetectedID;
    i_ConnectionsOptions FoundConOptions;    // The options for this new connection
    i_DNCConOptions DNCOptions;
    struct DNCConOptions NewConOptions;
    bool Found;

    if(m_DNC_Connections!=NULL)
    {
        if(m_DNC_CurrentOptions!=NULL)
            IOS_FreeConnectionOptions(m_DNC_CurrentOptions);

        /* Free the old list */
        IOS_FreeListOfAvailableConnections(m_DNC_Connections);
        m_DNC_CurrentOptions=NULL;
        m_DNC_LastOptions=NULL;
    }

    m_DNC_Connections=IOS_GetListOfAvailableConnections();
    if(m_DNC_Connections==NULL)
        return false;

    /* Add new options for any new connections */
    for(Entry=m_DNC_Connections;Entry!=NULL;Entry=Entry->Next)
    {
        /* See if we can find this entry */
        DetectedID=Entry->UniqueID;

        DNCOptions=m_DNC_ConOptions.find(DetectedID);
        if(DNCOptions==m_DNC_ConOptions.end())
        {
            /* We didn't find this connection, add the options for it */

            /* First find the options for this connection in the settings or
               session */
            Found=false;

            /* Try settings */
            if(g_Settings.UseConnectionDefaults)
            {
                FoundConOptions=g_Settings.DefaultConnectionsOptions.
                        find(DetectedID);

                /* Didn't find it in the settings, search the session */
                if(FoundConOptions!=g_Settings.DefaultConnectionsOptions.end())
                    Found=true;
            }

            if(!Found)
            {
                /* Try the session */
                FoundConOptions=g_Session.ConnectionsOptions.find(DetectedID);
                if(FoundConOptions!=g_Session.ConnectionsOptions.end())
                    Found=true;
            }

            strncpy(NewConOptions.Name,Entry->Title.c_str(),
                    MAX_CONNECTION_NAME_LEN);
            NewConOptions.Name[MAX_CONNECTION_NAME_LEN]=0;

            if(Found)
            {
                /* Ok, we found it, make a copy */
                NewConOptions.Options=FoundConOptions->second.Options;
//                NewConOptions.Name[0]=0;        // Restore from session????????????????????????????
                m_DNC_ConOptions.insert(make_pair(DetectedID,NewConOptions));
            }
            else
            {
                /* Wasn't found, use a blank one */
                NewConOptions.Options.clear();
                m_DNC_ConOptions.insert(make_pair(DetectedID,NewConOptions));
            }
        }
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    FillInConnectionPullDown
 *
 * SYNOPSIS:
 *    static void FillInConnectionPullDown(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function fills in the pull down list of connections.  It tries
 *    to keep the same selected entry if it can.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void FillInConnectionPullDown(void)
{
    t_UIComboBoxCtrl *ConPullDown;
    uintptr_t SelectedID;
    struct ConnectionInfoList *Entry;
    struct ConnectionInfoList *NewSelection;
    string OldSelectedLabel;

    m_IgnoreConnectionListChange=true;

    ConPullDown=UINC_GetComboBoxHandle(e_UINC_ComboBox_Connection);
    SelectedID=UIGetComboBoxSelectedEntry(ConPullDown);

    /* Find this entry in the list of connections */
    for(Entry=m_DNC_Connections;Entry!=NULL;Entry=Entry->Next)
        if((uintptr_t)Entry==SelectedID)
            break;

    OldSelectedLabel="";
    if(Entry!=NULL)
    {
        /* Note the label so we can find this entry again */
        OldSelectedLabel=Entry->Name;
    }

    DNC_GetListOfConnections();

    UIClearComboBox(ConPullDown);

    NewSelection=NULL;
    for(Entry=m_DNC_Connections;Entry!=NULL;Entry=Entry->Next)
    {
        UIAddItem2ComboBox(ConPullDown,Entry->Name,(uintptr_t)Entry);
        if(Entry->InUse)
        {
            UIStyleComboBoxItem(ConPullDown,(uintptr_t)Entry,
                    UISTYLE_STRIKETHROUGH);
        }
        if(Entry->Name==OldSelectedLabel)
            NewSelection=Entry;
    }

    if(SelectedID==0)
    {
        /* Ok, nothing was selected before (blank input), we need to
           reselect the last selected connection */
        NewSelection=IOS_FindConnectionFromDetectedID(m_DNC_Connections,
                g_Session.LastConnectionOpened.c_str());
    }

    if(NewSelection!=NULL)
        UISetComboBoxSelectedEntry(ConPullDown,(uintptr_t)NewSelection);

    m_IgnoreConnectionListChange=false;

    if(m_DNC_Connections!=NULL)
    {
        if(NewSelection==NULL)
            NC_ChangeConnectionListSelectedEntry(m_DNC_Connections);
        else
            NC_ChangeConnectionListSelectedEntry(NewSelection);
    }

//    if(NewSelection==NULL)
//        InformOf_NC_ConnectionListChange((uintptr_t)m_DNC_Connections);
//    else
//        InformOf_NC_ConnectionListChange((uintptr_t)NewSelection);
}

/*******************************************************************************
 * NAME:
 *    DNC_StoreOptionsHistory
 *
 * SYNOPSIS:
 *    static void DNC_StoreOptionsHistory(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function stores the current options in the local history for
 *    the currently selected connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DNC_StoreOptionsHistory(void)
{
    string DetectedID;
    i_DNCConOptions DNCOptions;    // The options for this new connection
    t_UITextInputCtrl *NameInput;

    if(m_DNC_LastOptions!=NULL && m_DNC_CurrentOptions!=NULL)
    {
        /* Find this connections options */
        DetectedID=m_DNC_LastOptions->UniqueID;
        DNCOptions=m_DNC_ConOptions.find(DetectedID);
        if(DNCOptions!=m_DNC_ConOptions.end())
        {
            /* Store the current options */
            IOS_StoreConnectionOptions(m_DNC_CurrentOptions,
                    DNCOptions->second.Options);

            /* Store the new connection dialog settings */
            NameInput=UINC_GetTxtInputHandle(e_UINC_TxtInput_Name);
            UIGetTextCtrlText(NameInput,DNCOptions->second.Name,
                    MAX_CONNECTION_NAME_LEN);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DNC_UpdateSessionConnectionInfo
 *
 * SYNOPSIS:
 *    static void DNC_UpdateSessionConnectionInfo(t_UIComboBoxCtrl *ConComboBox);
 *
 * PARAMETERS:
 *    ConComboBox [I] -- The combo box with the current selection in it.
 *
 * FUNCTION:
 *    This function takes the data from the current connection options and
 *    stores it in the session data.
 *
 *    It also removes old entries from the session data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DNC_UpdateSessionConnectionInfo(t_UIComboBoxCtrl *ConComboBox)
{
    struct ConnectionInfoList *Selected;
    string DetectedID;
    i_ConnectionsOptions CurrentConOptions;
    time_t KillTime;    // We kill any records older than this value
    struct ConOptions NewConOptions;
    struct ConnectionInfoList *Con;
    i_ConnectionsOptions DlgOptions;    // The options for this new connection
    i_DNCConOptions DNCOptions;    // The options for this new connection

    try
    {
        KillTime=time(NULL)-60*60*24*365; // Anything older than 1 year gets deleted

        Selected=(struct ConnectionInfoList *)
                UIGetComboBoxSelectedEntry(ConComboBox);
        if(m_DNC_Connections!=NULL && Selected!=NULL)
        {
            /* Update the last used connection */
            g_Session.LastConnectionOpened=Selected->UniqueID;

            /* Now update the session options */
            for(Con=m_DNC_Connections;Con!=NULL;Con=Con->Next)
            {
                DetectedID=Con->UniqueID;

                /* First update the age on all the connections that are
                   currently connected */
                for(CurrentConOptions=g_Session.ConnectionsOptions.begin();
                        CurrentConOptions!=g_Session.ConnectionsOptions.end();
                        CurrentConOptions++)
                {
                    if(DetectedID==CurrentConOptions->first)
                    {
                        /* Found this one */
                        CurrentConOptions->second.Age=time(NULL);
                        break;
                    }
                }
            }

            /* Next remove any connections that are too old */
            for(CurrentConOptions=g_Session.ConnectionsOptions.begin();
                    CurrentConOptions!=g_Session.ConnectionsOptions.end();
                    CurrentConOptions++)
            {
                if(CurrentConOptions->second.Age<KillTime)
                {
                    g_Session.ConnectionsOptions.erase(CurrentConOptions);
                    /* Start again */
                    CurrentConOptions=g_Session.ConnectionsOptions.begin();
                }
            }

            /* Update / add the new connections options */
            CurrentConOptions=g_Session.ConnectionsOptions.find(
                    g_Session.LastConnectionOpened);
            if(CurrentConOptions==g_Session.ConnectionsOptions.end())
            {
                /* Not found, add it */
                g_Session.ConnectionsOptions.insert(make_pair(g_Session.
                        LastConnectionOpened,NewConOptions));
                CurrentConOptions=g_Session.ConnectionsOptions.
                        find(g_Session.LastConnectionOpened);
                if(CurrentConOptions==g_Session.ConnectionsOptions.end())
                {
                    /* Hu? */
                    throw("Failed to find connection info");
                }
            }

            /* Update it */
            CurrentConOptions->second.Age=time(NULL);
            DNCOptions=m_DNC_ConOptions.find(g_Session.LastConnectionOpened);
            if(DNCOptions!=m_DNC_ConOptions.end())
            {
                CurrentConOptions->second.Options=DNCOptions->second.Options;
            }

            NoteSessionChanged();
        }
    }
    catch(const char *Msg)
    {
        UIAsk("Error updating session data",Msg,e_AskBox_Warning,
                e_AskBttns_Ok);
    }
    catch(...)
    {
        UIAsk("Error updating session data","Failed to update session data",
                e_AskBox_Warning,e_AskBttns_Ok);
    }
}

/*******************************************************************************
 * NAME:
 *    DNC_OpenConnection
 *
 * SYNOPSIS:
 *    static void DNC_OpenConnection(class TheMainWindow *MW);
 *
 * PARAMETERS:
 *    MW [I] -- The main window to take info about (the main window that
 *              was active when the command was run)
 *
 * FUNCTION:
 *    This function opens a new connection for the system.  It allocates every
 *    thing needed, and updates the session data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DNC_OpenConnection(class TheMainWindow *MW)
{
    t_UIComboBoxCtrl *ConPullDown;
    class Connection *NewConnection;
    string DetectedID;
    string URI;
    i_DNCConOptions DNCOptions;    // The options for this new connection
    struct ConnectionInfoList *Selected;
    t_KVList *SelectedConOptions;
    t_KVList EmptyOptions;
    string ConnectionName;

    NewConnection=nullptr;
    try
    {
        ConPullDown=UINC_GetComboBoxHandle(e_UINC_ComboBox_Connection);

        /* Store anything the user may have changed */
        DNC_StoreOptionsHistory();
        DNC_UpdateSessionConnectionInfo(ConPullDown);

        Selected=(struct ConnectionInfoList *)
                UIGetComboBoxSelectedEntry(ConPullDown);
        if(Selected==nullptr)
            throw("No selected connection to open");

        /* Find this connections options */
        DetectedID=Selected->UniqueID;

        ConnectionName="New Connection"; // If everything fails then use this name (should never be used)
        DNCOptions=m_DNC_ConOptions.find(DetectedID);
        if(DNCOptions!=m_DNC_ConOptions.end())
        {
            SelectedConOptions=&DNCOptions->second.Options;
            ConnectionName=DNCOptions->second.Name;
        }
        else
        {
            SelectedConOptions=&EmptyOptions;
            ConnectionName=Selected->Title;
        }

        if(!IOS_MakeURIFromDetectedCon(Selected,*SelectedConOptions,URI))
            throw("Failed to make a URI for this connection");

        NewConnection=MW->AllocNewTab(ConnectionName.c_str(),NULL,URI.c_str());
        if(NewConnection==nullptr)
            throw(nullptr);    // We have already prompted

        if(DNCOptions!=m_DNC_ConOptions.end())
        {
            if(!NewConnection->SetConnectionOptions(DNCOptions->
                    second.Options))
            {
                throw("Failed to set connection options");
            }
            NewConnection->SetDisplayName(DNCOptions->second.Name);
        }

//        NewConnection->TextCanvasResize(MW->GetTextCanvasWidth(),
//                MW->GetTextCanvasHeight());
//
//        /* Make this new connection the active one */
//        MW->SetActiveTab(NewConnection);
    }
    catch(const char *Error)
    {
        if(NewConnection!=nullptr)
            Con_FreeConnection(NewConnection);
        throw;
    }
    catch(...)
    {
        throw;
    }
}

/*******************************************************************************
 * NAME:
 *    DNC_Event
 *
 * SYNOPSIS:
 *    bool DNC_Event(const struct DNCEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event we should process.
 *
 * FUNCTION:
 *    This function is called from the UI to tell us there was a UI event.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DNC_Event(const struct DNCEvent *Event)
{
    bool AcceptEvent;
    struct ConnectionInfoList *Con;

    AcceptEvent=true;
    switch(Event->EventType)
    {
        case e_DNCEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_UINC_Button_Rescan:
                    DNC_StoreOptionsHistory();
                    IOS_ScanForConnections();
                    FillInConnectionPullDown();
                break;
                case e_UINC_ButtonMAX:
                default:
                break;
            }
        break;
        case e_DNCEvent_ConnectionListChange:
            Con=(struct ConnectionInfoList *)Event->ID;
            NC_ChangeConnectionListSelectedEntry(Con);
        break;
        case e_DNCEvent_NameChange:
            m_NameChanged=true;
        break;
        case e_DNCEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}

/*******************************************************************************
 * NAME:
 *    DNC_OptionsChanged
 *
 * SYNOPSIS:
 *    static void DNC_OptionsChanged(void *UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- Unused.
 *
 * FUNCTION:
 *    This function is called when the UI to the options changes.  We use
 *    it to refresh the display name.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DNC_OptionsChanged(void *UserData)
{
/* DEBUG PAUL: We want to refresh only the ShortName into the Name of the
   connection here */
    t_UIComboBoxCtrl *ConPullDown;
    t_UITextInputCtrl *NameInput;
    string DetectedID;
    i_DNCConOptions DNCOptions;    // The options for this new connection
    struct ConnectionInfoList *Selected;
    struct ConnectionInfoList CInfo;

    /* If the user changed the name don't over rule them */
    if(m_NameChanged)
        return;

    ConPullDown=UINC_GetComboBoxHandle(e_UINC_ComboBox_Connection);
    NameInput=UINC_GetTxtInputHandle(e_UINC_TxtInput_Name);

    /* Store anything the user may have changed */
    DNC_StoreOptionsHistory();

    Selected=(struct ConnectionInfoList *)
            UIGetComboBoxSelectedEntry(ConPullDown);
    if(Selected==NULL)
        return;

    /* Find this connections options */
    DetectedID=Selected->UniqueID;

    DNCOptions=m_DNC_ConOptions.find(DetectedID);
    if(DNCOptions!=m_DNC_ConOptions.end())
    {
        if(!IOS_GetConnectionInfo(Selected->UniqueID.c_str(),
                DNCOptions->second.Options,&CInfo))
        {
            return;
        }

        UISetTextCtrlText(NameInput,CInfo.Title.c_str());
    }
}

