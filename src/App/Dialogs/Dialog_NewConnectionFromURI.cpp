/*******************************************************************************
 * FILENAME: Dialog_NewConnectionFromURI.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the new connection from URI dialog in it.
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
 *    Paul Hutchinson (10 Feb 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Dialog_NewConnectionFromURI.h"
#include "App/Connections.h"
#include "App/IOSystem.h"
#include "UI/UIAsk.h"
#include "UI/UINewConnectionFromURI.h"
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void FillInDriverList(void);
static void DNCFU_UpdateInfoPanel(void);
static void DNCFU_RethinkGUI(void);
static bool DNCFU_OpenConnection(class TheMainWindow *MW);
static void DNCFU_SetURIAndUpdate(std::string &NewURI);
static void DNCFU_PullApartHelpStr(const char *Help);

/*** VARIABLE DEFINITIONS     ***/
struct DriverInfoList *m_RNCFUD_DriverList;

/*******************************************************************************
 * NAME:
 *    RunNewConnectionFromURIDialog
 *
 * SYNOPSIS:
 *    bool RunNewConnectionFromURIDialog(class TheMainWindow *MW,
 *              std::string *URI);
 *
 * PARAMETERS:
 *    MW [I] -- The main window to take info about (the main window that
 *              was active when the command was run)
 *    URI [I/O] -- The URI to fill the URI input with.  This is also where the
 *                 contents of teh URI input is stored when the user clicks
 *                 OK.  This can be NULL to ignore it.
 *
 * FUNCTION:
 *    This function shows the open connection from URI dialog.
 *
 * RETURNS:
 *    true -- User pressed OK
 *    false -- User pressed cancel or there was an error
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (10 Feb 2025)
 *       Created
 ******************************************************************************/
bool RunNewConnectionFromURIDialog(class TheMainWindow *MW,std::string *URI)
{
    bool AllocatedUI;
    bool RetValue;
    bool done;

    AllocatedUI=false;
    m_RNCFUD_DriverList=NULL;
    RetValue=false;
    try
    {
        if(!UIAlloc_NewConnectionFromURI())
            throw("Failed to open window");

        FillInDriverList();

        if(URI!=NULL)
            DNCFU_SetURIAndUpdate(*URI);

        DNCFU_RethinkGUI();

        done=false;
        while(!done)
        {
            if(UIShow_NewConnectionFromURI())
            {
                /* User clicked ok */
                if(DNCFU_OpenConnection(MW))
                {
                    done=true;
                    RetValue=true;
                }
                else
                {
                    UIAsk("Error opening new connection","Invalid URI",
                            e_AskBox_Error,e_AskBttns_Ok);
                }
            }
            else
            {
                done=true;
            }
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

    if(m_RNCFUD_DriverList!=NULL)
        IOS_FreeDriverInfoList(m_RNCFUD_DriverList);
    m_RNCFUD_DriverList=NULL;

    if(AllocatedUI)
        UIFree_NewConnectionFromURI();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    DNCFU_Event
 *
 * SYNOPSIS:
 *    bool DNCFU_Event(const struct DNCFUEvent *Event);
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
bool DNCFU_Event(const struct DNCFUEvent *Event)
{
    bool AcceptEvent;

    AcceptEvent=true;
    switch(Event->EventType)
    {
        case e_DNCFUEvent_DriverChanged:
            DNCFU_UpdateInfoPanel();
        break;
        case e_DNCFUEvent_URIChanged:
            DNCFU_RethinkGUI();
        break;
        case e_DNCFUEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}

/*******************************************************************************
 * NAME:
 *    FillInDriverList
 *
 * SYNOPSIS:
 *    static void FillInDriverList(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears and fills in the driver list view.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void FillInDriverList(void)
{
    struct DriverInfoList *Entry;
    t_UIListViewCtrl *DriversCtrl;

    if(m_RNCFUD_DriverList!=NULL)
        IOS_FreeDriverInfoList(m_RNCFUD_DriverList);
    m_RNCFUD_DriverList=IOS_GetListOfDrivers();

    DriversCtrl=UINC_GetListViewInputHandle(e_UINCFU_ListView_Driver);
    UIClearListView(DriversCtrl);
    for(Entry=m_RNCFUD_DriverList;Entry!=NULL;Entry=Entry->Next)
        UIAddItem2ListView(DriversCtrl,Entry->Name,(uintptr_t)Entry);

    /* Select the first entry */
    UISetListViewSelectedEntry(DriversCtrl,(uintptr_t)m_RNCFUD_DriverList);
    DNCFU_UpdateInfoPanel();
}

/*******************************************************************************
 * NAME:
 *    DNCFU_UpdateInfoPanel
 *
 * SYNOPSIS:
 *    static void DNCFU_UpdateInfoPanel(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function reads the selected driver and updates the info panel.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DNCFU_UpdateInfoPanel(void)
{
    t_UIListViewCtrl *DriversCtrl;
    struct DriverInfoList *SelectedEntry;

    try
    {
        DriversCtrl=UINC_GetListViewInputHandle(e_UINCFU_ListView_Driver);
        SelectedEntry=(struct DriverInfoList *)UIGetListViewSelectedEntry(DriversCtrl);

        UINC_PrivURIHelp_ClearCtrlText();
        if(SelectedEntry->DriverURIHelpString=="")
        {
            UINC_PrivURIHelp_AppendText("Not available",
                    e_PrivURIHelp_Style_TextLine);
        }
        else
        {
            DNCFU_PullApartHelpStr(SelectedEntry->DriverURIHelpString.c_str());
        }
    }
    catch(...)
    {
        UINC_PrivURIHelp_ClearCtrlText();
        UINC_PrivURIHelp_AppendText("Error",e_PrivURIHelp_Style_TextLine);
    }
}

static void DNCFU_PullApartHelpStr(const char *Help)
{
    const char *Pos;
    const char *Start;
    string Tag;
    string Value;
    bool FoundURI;
    bool FoundAnArg;
    bool FoundExample;

    try
    {
        UINC_PrivURIHelp_ClearCtrlText();

        FoundURI=false;
        FoundAnArg=false;
        FoundExample=false;

        /* We need to pull the help string apart */
        Pos=Help;
        while(*Pos!=0)
        {
            if(*Pos=='<')
            {
                /* A tag of some kind, find the end */
                Start=Pos+1;
                while(*Pos!='>' && *Pos!=0)
                    Pos++;
                if(*Pos==0)
                    throw(0);
                Tag.assign(Start,Pos-Start);

                /* Find the end of the tag */
                Pos++;
                Start=Pos;
                while(*Pos!=0)
                {
                    if(*Pos=='<' && *(Pos+1)=='/')
                    {
                        /* See if it's the closing tag */
                        if(strncmp(Pos+2,Tag.c_str(),Tag.length())==0)
                        {
                            /* Found it */
                            Value.assign(Start,Pos-Start);
                            /* Move to the end of the tag */
                            Pos+=2+Tag.length();  // "</"+tag
                            break;
                        }
                    }
                    Pos++;
                }
                if(*Pos==0)
                    throw(0);

                /* See that kind of tag we got */
                if(Tag=="URI")
                {
                    /* It's the URI line */
                    if(FoundURI)
                    {
                        /* We already found the URI */
                        throw(0);
                    }
                    UINC_PrivURIHelp_AppendText("FORMAT",
                            e_PrivURIHelp_Style_Heading);
                    UINC_PrivURIHelp_AppendText(Value.c_str(),
                            e_PrivURIHelp_Style_Def);
                    FoundURI=true;
                }
                else if(Tag=="ARG")
                {
                    if(!FoundAnArg)
                    {
                        UINC_PrivURIHelp_AppendText("WHERE",
                                e_PrivURIHelp_Style_Heading);
                    }
                    UINC_PrivURIHelp_AppendText(Value.c_str(),
                            e_PrivURIHelp_Style_Def);
                    FoundAnArg=true;
                }
                else if(Tag=="Example")
                {
                    if(!FoundExample)
                    {
                        UINC_PrivURIHelp_AppendText("EXAMPLE",
                                e_PrivURIHelp_Style_Heading);
                    }
                    UINC_PrivURIHelp_AppendText(Value.c_str(),
                            e_PrivURIHelp_Style_Def);
                    FoundExample=true;
                }
            }
            Pos++;
        }
        if(!FoundURI)
        {
            /* We didn't find the URI */
            throw(0);
        }
//    "<URI>RTT://[SerialNumber][:USBAddress]/[TargetDeviceType]</URI>"
//    "<ARG>SerialNumber -- The serial number of the J-Link debug probe<ARG>"
//    "<ARG>USBAddress -- If using an older-J-Link you need to provide the USBaddress as the Serial Number will always be 123456<ARG>"
//    "<ARG>TargetDeviceType -- The type of target that will be connected to the JLink<ARG>"
//    "<Example>RTT://158007529/CS32F103C8</Example>"
    }
    catch(...)
    {
        UINC_PrivURIHelp_ClearCtrlText();
        UINC_PrivURIHelp_AppendText("Error",e_PrivURIHelp_Style_TextLine);
    }
}

/*******************************************************************************
 * NAME:
 *    DNCFU_RethinkGUI
 *
 * SYNOPSIS:
 *    static void DNCFU_RethinkGUI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the GUI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DNCFU_RethinkGUI(void)
{
    t_UITextInputCtrl *URICtrl;
    string URI;
    t_UIButtonCtrl *OkBttn;

    OkBttn=UINC_GetButtonInputHandle(e_UINCFU_ButtonInput_Ok);
    URICtrl=UINC_GetTxtInputHandle(e_UINCFU_TxtInput_URI);

    UIGetTextCtrlText(URICtrl,URI);
    if(URI=="")
        UIEnableButton(OkBttn,false);
    else
        UIEnableButton(OkBttn,true);
}

/*******************************************************************************
 * NAME:
 *    DNCFU_OpenConnection
 *
 * SYNOPSIS:
 *    static bool DNCFU_OpenConnection(class TheMainWindow *MW);
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
static bool DNCFU_OpenConnection(class TheMainWindow *MW)
{
    t_UITextInputCtrl *URICtrl;
    string URI;
    t_KVList Options;
    class Connection *NewConnection;

    NewConnection=NULL;
    try
    {
        URICtrl=UINC_GetTxtInputHandle(e_UINCFU_TxtInput_URI);

        UIGetTextCtrlText(URICtrl,URI);

        if(!IOS_UpdateOptionsFromURI(URI.c_str(),Options))
            return false;

        NewConnection=MW->AllocNewTab(NULL,NULL,URI.c_str(),&Options,false);
        if(NewConnection==NULL)
            throw(nullptr);    // We have already prompted
    }
    catch(const char *Error)
    {
        if(NewConnection!=NULL)
            Con_FreeConnection(NewConnection);
        throw;
    }
    catch(...)
    {
        throw;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DNCFU_SetURIAndUpdate
 *
 * SYNOPSIS:
 *    static void DNCFU_SetURIAndUpdate(std::string &NewURI);
 *
 * PARAMETERS:
 *    NewURI [I] -- The new URI to set the dialog to
 *
 * FUNCTION:
 *    This function sets the URI and looks up the driver for that device.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DNCFU_SetURIAndUpdate(std::string &NewURI)
{
    struct DriverInfoList *Entry;
    t_UITextInputCtrl *URICtrl;
    t_UIListViewCtrl *DriversCtrl;
    string UniqueID;

    DriversCtrl=UINC_GetListViewInputHandle(e_UINCFU_ListView_Driver);
    URICtrl=UINC_GetTxtInputHandle(e_UINCFU_TxtInput_URI);

    UISetTextCtrlText(URICtrl,NewURI.c_str());

    /* Find the driver and select it */
    for(Entry=m_RNCFUD_DriverList;Entry!=NULL;Entry=Entry->Next)
    {
        if(IOS_DoesURIMatchBaseURI(NewURI.c_str(),Entry->BaseURI.c_str()))
        {
            UISetListViewSelectedEntry(DriversCtrl,(uintptr_t)Entry);
            break;
        }
    }
}
