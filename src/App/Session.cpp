/*******************************************************************************
 * FILENAME: Session.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the session storage structure in it.
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
#include "App/Connections.h"
#include "App/MainApp.h"
#include "App/Portable.h"
#include "App/Session.h"
#include "App/Util/StorageHelpers.h"
#include "ThirdParty/TinyCFG/TinyCFG.h"
#include "OS/Directorys.h"
#include <string>
#include <string.h>
#include <time.h>

using namespace std;

/*** DEFINES                  ***/
#define SESSION_FILE        "Session.dat"

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
class SessionOpenConnections_TinyCFG : public TinyCFGBaseData
{
    public:
        t_SessionOpenConnectionList *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};

/*** FUNCTION PROTOTYPES      ***/
static void Session_RegisterAllMembers(struct Session &session,
        class TinyCFG &cfg);
static void Session_DefaultSession(struct Session &session);
static bool RegisterCRCType(class TinyCFG &cfg,const char *XmlName,
        e_CRCType &Data);

/*** VARIABLE DEFINITIONS     ***/
struct Session g_Session;
bool g_SessionChanged;  // Has the session changed since the last time we saved?
time_t m_SessionLastSaveCheck;
bool m_SessionChanged;

/*******************************************************************************
 * NAME:
 *    InitSessionSystem
 *
 * SYNOPSIS:
 *    void InitSessionSystem(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This funciton init's the session system.
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
void InitSessionSystem(void)
{
    m_SessionLastSaveCheck=time(NULL);
    Session_DefaultSession(g_Session);
    m_SessionChanged=false;
}

/*******************************************************************************
 * NAME:
 *    LoadSession
 *
 * SYNOPSIS:
 *    bool LoadSession(const char *Filename=NULL);
 *
 * PARAMETERS:
 *    Filename [I] -- The file name to load the session from.  If this NULL
 *                    then the default name / path will be used.
 *
 * FUNCTION:
 *    This function loads the settings from a file into the global settings
 *    'g_Session'.  It defaults them first so if there is an error or missing
 *    settings everything will be defaulted.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    SaveSession()
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool LoadSession(const char *Filename)
{
    class TinyCFG cfg("Session");
    const char *UseFilename;
    string AppData;

    try
    {
        UseFilename=Filename;
        if(Filename==NULL)
        {
            if(GetAppDataPath(AppData)==false)
                return false;
            AppData+=SESSION_FILE;
            UseFilename=AppData.c_str();
        }

        Session_RegisterAllMembers(g_Session,cfg);

        Session_DefaultSession(g_Session);
        cfg.LoadCFGFile(UseFilename);
        m_SessionChanged=false;
    }
    catch(...)
    {
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    SaveSession
 *
 * SYNOPSIS:
 *    bool SaveSession(const char *Filename=NULL);
 *
 * PARAMETERS:
 *    Filename [I] -- The file name to save the session under.  If this NULL
 *                    then the default name / path will be used.
 *
 * FUNCTION:
 *    This function saves the global settings to a file.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    LoadSession()
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool SaveSession(const char *Filename)
{
    class TinyCFG cfg("Session");
    const char *UseFilename;
    string AppData;

    try
    {
        UseFilename=Filename;
        if(Filename==NULL)
        {
            if(GetAppDataPath(AppData)==false)
                return false;
            /* See if this path exists */
            if(!PathExists(AppData.c_str()))
            {
                /* Try making it */
                if(!MakePathDir(AppData.c_str()))
                    return false;
            }

            AppData+=SESSION_FILE;
            UseFilename=AppData.c_str();
        }

        Session_RegisterAllMembers(g_Session,cfg);

        ScanOpenConnections2Session();

/* DEBUG PAUL: We need to ask each plugin to store it's session data here */

        cfg.SaveCFGFile(UseFilename);
    }
    catch(...)
    {
        return false;
    }

    m_SessionChanged=false;

    return true;
}

/*******************************************************************************
 * NAME:
 *    AutoSaveSessionTick
 *
 * SYNOPSIS:
 *    void AutoSaveSessionTick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called regularly to see if the session file needs to
 *    be saved.
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
void AutoSaveSessionTick(void)
{
    /* Only check every 1 minutes */
    if(difftime(time(NULL),m_SessionLastSaveCheck)>60)
    {
        m_SessionLastSaveCheck=time(NULL);
        SaveSessionIfNeeded();
    }
}

/*******************************************************************************
 * NAME:
 *    SaveSessionIfNeeded
 *
 * SYNOPSIS:
 *    void SaveSessionIfNeeded(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function can be called at any point to save out the session file
 *    again.  It will only save out the session file if there was a change.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void SaveSessionIfNeeded(void)
{
    if(m_SessionChanged)
    {
        /* Something changed we need to save it */
        SaveSession();
    }
}

/*******************************************************************************
 * NAME:
 *    NoteSessionChanged
 *
 * SYNOPSIS:
 *    void NoteSessionChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to note that you changed something in the
 *    session data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void NoteSessionChanged(void)
{
    /* If we are shutting down then we ignore session changes (because we have
       already saved the session, and we don't want to keep anything that
       might change the session) */
    if(g_AppShuttingDown)
        return;

    m_SessionChanged=true;
}

/*******************************************************************************
 * NAME:
 *    ScanOpenConnections2Session
 *
 * SYNOPSIS:
 *    void ScanOpenConnections2Session(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function scans all open connections takes a copy of needed info
 *    into the session system.
 *
 *    It also removes closed connections from the session.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void ScanOpenConnections2Session(void)
{
    struct SessionOpenConnection NewOpenConInfo;
    t_ConnectionList ConList;
    i_ConnectionList CurrentCon;
    class Connection *Con;

    g_Session.OpenConnections.clear();

    /* If we aren't restoring connections on startup then we just store blank
       data in session */
    if(!g_Settings.ReopenOnConnectionsOnStartup)
        return;

    try
    {
        Con_GetListOfConnections(ConList);

        for(CurrentCon=ConList.begin();CurrentCon!=ConList.end();CurrentCon++)
        {
            Con=*CurrentCon;

            Con->GetDisplayName(NewOpenConInfo.Name);
            NewOpenConInfo.WasOpen=Con->GetConnectedStatus();

            if(!Con->GetConnectionOptions(NewOpenConInfo.Options))
                throw(0);

            if(!Con->GetURI(NewOpenConInfo.URI))
                throw(0);

            NewOpenConInfo.UseCustomSettings=Con->UsingCustomSettings;
            NewOpenConInfo.CustomSettings=Con->CustomSettings;

            /* Add this connection to the list of open connections */
            g_Session.OpenConnections.push_back(NewOpenConInfo);
        }
    }
    catch(...)
    {
    }
}

/////////////////////
bool SessionOpenConnections_TinyCFG::LoadElement(class TinyCFG *CFG)
{
    struct SessionOpenConnection NewData;
    class TinyCFG SubCFG("Connection");

    Ptr->clear();

    SubCFG.Register("Name",NewData.Name);
    SubCFG.Register("URI",NewData.URI);
    RegisterKVList_TinyCFG(SubCFG,"Option",NewData.Options);
    SubCFG.Register("UseCustomSettings",NewData.UseCustomSettings);
    SubCFG.StartBlock("CustomSettings");
    NewData.CustomSettings.RegisterAllMembers(SubCFG);
    SubCFG.EndBlock();
    SubCFG.Register("WasOpen",NewData.WasOpen);

    NewData.Name="";
    NewData.URI="";
    NewData.UseCustomSettings=false;
    NewData.CustomSettings.DefaultSettings();
    NewData.WasOpen=false;

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
    {
        Ptr->push_back(NewData);
    }

    return true;
}

bool SessionOpenConnections_TinyCFG::SaveElement(class TinyCFG *CFG)
{
    i_SessionOpenConnectionList i;
    string MenuName;
    string Name;
    string URI;
    bool WasOpen;
    t_KVList Options;
    class TinyCFG SubCFG("Connection");
    class ConSettings CustomSettings;
    bool UseCustomSettings;

    SubCFG.Register("Name",Name);
    SubCFG.Register("URI",URI);
    RegisterKVList_TinyCFG(SubCFG,"Option",Options);
    SubCFG.Register("UseCustomSettings",UseCustomSettings);
    SubCFG.StartBlock("CustomSettings");
    CustomSettings.RegisterAllMembers(SubCFG);
    SubCFG.EndBlock();
    SubCFG.Register("WasOpen",WasOpen);

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        Name=i->Name;
        URI=i->URI;
        Options=i->Options;
        UseCustomSettings=i->UseCustomSettings;
        CustomSettings=i->CustomSettings;
        WasOpen=i->WasOpen;

        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

bool RegisterSessionOpenConnectionsList_TinyCFG(class TinyCFG &cfg,
        const char *XmlName,t_SessionOpenConnectionList &Data)
{
    class SessionOpenConnections_TinyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new SessionOpenConnections_TinyCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
/////////////////////

/* DEBUG PAUL: Removed this because it should be done on a connection by connection basis instead 1 global */
/////////////////////
//class LeftPanelTabCFG : public TinyCFGBaseData
//{
//   public:
//      e_LeftPanelTabType *Ptr;
//      bool LoadData(string &LoadedString);
//      bool SaveData(string &StoreString);
//};
//
//bool LeftPanelTabCFG::LoadData(string &LoadedString)
//{
//    *Ptr=e_LeftPanelTab_Display;
//
//    if(strcmp(LoadedString.c_str(),"Display")==0)
//        *Ptr=e_LeftPanelTab_Display;
//    if(strcmp(LoadedString.c_str(),"ConOptions")==0)
//        *Ptr=e_LeftPanelTab_ConOptions;
//    if(strcmp(LoadedString.c_str(),"Capture")==0)
//        *Ptr=e_LeftPanelTab_Capture;
//    if(strcmp(LoadedString.c_str(),"Upload")==0)
//        *Ptr=e_LeftPanelTab_Upload;
//    if(strcmp(LoadedString.c_str(),"Download")==0)
//        *Ptr=e_LeftPanelTab_Download;
//    if(strcmp(LoadedString.c_str(),"Bridge")==0)
//        *Ptr=e_LeftPanelTab_Bridge;
//
//    return true;
//}
//
//bool LeftPanelTabCFG::SaveData(string &StoreString)
//{
//    switch(*Ptr)
//    {
//        default:
//        case e_LeftPanelTab_Display:
//            StoreString="Display";
//        break;
//        case e_LeftPanelTab_ConOptions:
//            StoreString="ConOptions";
//        break;
//        case e_LeftPanelTab_Capture:
//            StoreString="Capture";
//        break;
//        case e_LeftPanelTab_Upload:
//            StoreString="Upload";
//        break;
//        case e_LeftPanelTab_Download:
//            StoreString="Download";
//        break;
//        case e_LeftPanelTab_Bridge:
//            StoreString="Bridge";
//        break;
//    }
//    if(*Ptr>=e_LeftPanelTabMAX)
//        StoreString="Display";
//    return true;
//}
//
//static bool RegisterLeftPanelTab(class TinyCFG &cfg,const char *XmlName,
//        e_LeftPanelTabType &Data)
//{
//    class LeftPanelTabCFG *NewDataClass;
//
//    /* Make a new class to handle this new piece of data */
//    try
//    {
//        NewDataClass=new LeftPanelTabCFG;
//    }
//    catch(std::bad_alloc const &)
//    {
//        return false;
//    }
//
//    /* Setup the data */
//    NewDataClass->Ptr=&Data;
//    NewDataClass->XmlName=XmlName;
//
//    return cfg.RegisterGeneric(NewDataClass);
//}
//////////////////////
//class RightPanelTabCFG : public TinyCFGBaseData
//{
//   public:
//      e_RightPanelTabType *Ptr;
//      bool LoadData(string &LoadedString);
//      bool SaveData(string &StoreString);
//};
//
//bool RightPanelTabCFG::LoadData(string &LoadedString)
//{
//    *Ptr=e_RightPanelTab_StopWatch;
//
//    if(strcmp(LoadedString.c_str(),"StopWatch")==0)
//        *Ptr=e_RightPanelTab_StopWatch;
//
//    return true;
//}
//
//bool RightPanelTabCFG::SaveData(string &StoreString)
//{
//    switch(*Ptr)
//    {
//        case e_RightPanelTab_StopWatch:
//            StoreString="StopWatch";
//        break;
//        case e_RightPanelTabMAX:
//        break;
//    }
//    if(*Ptr>=e_RightPanelTabMAX)
//        StoreString="StopWatch";
//    return true;
//}
//
//static bool RegisterRightPanelTab(class TinyCFG &cfg,const char *XmlName,
//        e_RightPanelTabType &Data)
//{
//    class RightPanelTabCFG *NewDataClass;
//
//    /* Make a new class to handle this new piece of data */
//    try
//    {
//        NewDataClass=new RightPanelTabCFG;
//    }
//    catch(std::bad_alloc const &)
//    {
//        return false;
//    }
//
//    /* Setup the data */
//    NewDataClass->Ptr=&Data;
//    NewDataClass->XmlName=XmlName;
//
//    return cfg.RegisterGeneric(NewDataClass);
//}
//////////////////////
//class BottomPanelTabCFG : public TinyCFGBaseData
//{
//   public:
//      e_BottomPanelTabType *Ptr;
//      bool LoadData(string &LoadedString);
//      bool SaveData(string &StoreString);
//};
//
//bool BottomPanelTabCFG::LoadData(string &LoadedString)
//{
//    *Ptr=e_BottomPanelTab_IncomingHex;
//
//    if(strcmp(LoadedString.c_str(),"Hex")==0)
//        *Ptr=e_BottomPanelTab_IncomingHex;
//    if(strcmp(LoadedString.c_str(),"Injection")==0)
//        *Ptr=e_BottomPanelTab_Injection;
//    if(strcmp(LoadedString.c_str(),"Buffers")==0)
//        *Ptr=e_BottomPanelTab_Buffers;
//
//    return true;
//}
//
//bool BottomPanelTabCFG::SaveData(string &StoreString)
//{
//    switch(*Ptr)
//    {
//        case e_BottomPanelTab_IncomingHex:
//            StoreString="Hex";
//        break;
//        case e_BottomPanelTab_Injection:
//            StoreString="Injection";
//        break;
//        case e_BottomPanelTab_Buffers:
//            StoreString="Buffers";
//        break;
//        case e_BottomPanelTabMAX:
//        break;
//    }
//    if(*Ptr>=e_BottomPanelTabMAX)
//        StoreString="Hex";
//    return true;
//}
//
//static bool RegisterBottomPanelTab(class TinyCFG &cfg,const char *XmlName,
//        e_BottomPanelTabType &Data)
//{
//    class BottomPanelTabCFG *NewDataClass;
//
//    /* Make a new class to handle this new piece of data */
//    try
//    {
//        NewDataClass=new BottomPanelTabCFG;
//    }
//    catch(std::bad_alloc const &)
//    {
//        return false;
//    }
//
//    /* Setup the data */
//    NewDataClass->Ptr=&Data;
//    NewDataClass->XmlName=XmlName;
//
//    return cfg.RegisterGeneric(NewDataClass);
//}
//////////////////////

static void Session_RegisterAllMembers(struct Session &session,
        class TinyCFG &cfg)
{
    cfg.StartBlock("MainWindow");
    cfg.Register("AppMaximized",session.AppMaximized);
    cfg.Register("WindowPosX",session.WindowPosX);
    cfg.Register("WindowPosY",session.WindowPosY);
    cfg.Register("WindowWidth",session.WindowWidth);
    cfg.Register("WindowHeight",session.WindowHeight);
    cfg.Register("SendBufferPath",session.SendBufferPath);

    cfg.StartBlock("Panels");
    cfg.Register("LeftPanelSize",session.LeftPanelSize);
    cfg.Register("RightPanelSize",session.RightPanelSize);
    cfg.Register("BottomPanelSize",session.BottomPanelSize);
    cfg.Register("LeftPanelOpen",session.LeftPanelOpen);
    cfg.Register("RightPanelOpen",session.RightPanelOpen);
    cfg.Register("BottomPanelOpen",session.BottomPanelOpen);
    cfg.EndBlock();

    cfg.EndBlock();

    cfg.StartBlock("Connections");
    cfg.Register("LastConnectionOpened",session.LastConnectionOpened);
    RegisterConnectionOptions_TinyCFG(cfg,"Options",session.ConnectionsOptions);

    RegisterSessionOpenConnectionsList_TinyCFG(cfg,"OpenConnections",session.OpenConnections);

    cfg.EndBlock();

    cfg.StartBlock("Transfers");
    RegisterConUploadOptions_TinyCFG(cfg,"UploadOptions",session.LastUsedUploadOptions);
    RegisterConDownloadOptions_TinyCFG(cfg,"DownloadOptions",session.LastUsedDownloadOptions);
    cfg.EndBlock();

    cfg.StartBlock("Buffers");
    RegisterCRCType(cfg,"LastSelectedCRCType",session.LastSelectedCRCType);
    cfg.EndBlock();

    cfg.StartBlock("CalcCRC");
    RegisterCRCType(cfg,"LastSelectedCalcCRCType",session.LastSelectedCalcCRCType);
    cfg.EndBlock();
}

static void Session_DefaultSession(struct Session &session)
{
    /* Panels */
    session.LeftPanelSize=80;
    session.RightPanelSize=80;
    session.BottomPanelSize=180;
    session.LeftPanelOpen=false;
    session.RightPanelOpen=false;
    session.BottomPanelOpen=false;

    /* Other */
    session.AppMaximized=false;
    session.WindowPosX=-1;
    session.WindowPosY=-1;
    session.WindowWidth=640;
    session.WindowHeight=480;

    session.LastConnectionOpened="";
    session.ConnectionsOptions.clear();
    session.LastUsedUploadOptions.clear();
    session.LastUsedDownloadOptions.clear();
    session.OpenConnections.clear();

    /* Edit Buffer */
    session.LastSelectedCRCType=e_CRC_CRC32;

    /* Calc Buffer */
    session.LastSelectedCalcCRCType=e_CRC_CRC32;
}

///////////////////
class CRCTypeCFG : public TinyCFGBaseData
{
   public:
      e_CRCType *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};
bool CRCTypeCFG::LoadData(string &LoadedString)
{
    *Ptr=e_CRC_CRC32;  /* Defaults to CRC32 */

    if(strcmp(LoadedString.c_str(),"CRC8")==0)
        *Ptr=e_CRC_CRC8;
    if(strcmp(LoadedString.c_str(),"CRC8_SAE_J1850")==0)
        *Ptr=e_CRC_CRC8_SAE_J1850;
    if(strcmp(LoadedString.c_str(),"CRC8_SAE_J1850_ZERO")==0)
        *Ptr=e_CRC_CRC8_SAE_J1850_ZERO;
    if(strcmp(LoadedString.c_str(),"CRC8_8H2F")==0)
        *Ptr=e_CRC_CRC8_8H2F;
    if(strcmp(LoadedString.c_str(),"CRC8_CDMA2000")==0)
        *Ptr=e_CRC_CRC8_CDMA2000;
    if(strcmp(LoadedString.c_str(),"CRC8_DARC")==0)
        *Ptr=e_CRC_CRC8_DARC;
    if(strcmp(LoadedString.c_str(),"CRC8_DVB_S2")==0)
        *Ptr=e_CRC_CRC8_DVB_S2;
    if(strcmp(LoadedString.c_str(),"CRC8_EBU")==0)
        *Ptr=e_CRC_CRC8_EBU;
    if(strcmp(LoadedString.c_str(),"CRC8_ICODE")==0)
        *Ptr=e_CRC_CRC8_ICODE;
    if(strcmp(LoadedString.c_str(),"CRC8_ITU")==0)
        *Ptr=e_CRC_CRC8_ITU;
    if(strcmp(LoadedString.c_str(),"CRC8_MAXIM")==0)
        *Ptr=e_CRC_CRC8_MAXIM;
    if(strcmp(LoadedString.c_str(),"CRC8_ROHC")==0)
        *Ptr=e_CRC_CRC8_ROHC;
    if(strcmp(LoadedString.c_str(),"CRC8_WCDMA")==0)
        *Ptr=e_CRC_CRC8_WCDMA;
    if(strcmp(LoadedString.c_str(),"CRC8_BLUETOOTH")==0)
        *Ptr=e_CRC_CRC8_BLUETOOTH;
    if(strcmp(LoadedString.c_str(),"CRC16_CCIT_ZERO")==0)
        *Ptr=e_CRC_CRC16_CCIT_ZERO;
    if(strcmp(LoadedString.c_str(),"CRC16_ARC")==0)
        *Ptr=e_CRC_CRC16_ARC;
    if(strcmp(LoadedString.c_str(),"CRC16_AUG_CCITT")==0)
        *Ptr=e_CRC_CRC16_AUG_CCITT;
    if(strcmp(LoadedString.c_str(),"CRC16_BUYPASS")==0)
        *Ptr=e_CRC_CRC16_BUYPASS;
    if(strcmp(LoadedString.c_str(),"CRC16_CCITT_FALSE")==0)
        *Ptr=e_CRC_CRC16_CCITT_FALSE;
    if(strcmp(LoadedString.c_str(),"CRC16_CDMA2000")==0)
        *Ptr=e_CRC_CRC16_CDMA2000;
    if(strcmp(LoadedString.c_str(),"CRC16_DDS_110")==0)
        *Ptr=e_CRC_CRC16_DDS_110;
    if(strcmp(LoadedString.c_str(),"CRC16_DECT_R")==0)
        *Ptr=e_CRC_CRC16_DECT_R;
    if(strcmp(LoadedString.c_str(),"CRC16_DECT_X")==0)
        *Ptr=e_CRC_CRC16_DECT_X;
    if(strcmp(LoadedString.c_str(),"CRC16_DNP")==0)
        *Ptr=e_CRC_CRC16_DNP;
    if(strcmp(LoadedString.c_str(),"CRC16_EN_13757")==0)
        *Ptr=e_CRC_CRC16_EN_13757;
    if(strcmp(LoadedString.c_str(),"CRC16_GENIBUS")==0)
        *Ptr=e_CRC_CRC16_GENIBUS;
    if(strcmp(LoadedString.c_str(),"CRC16_MAXIM")==0)
        *Ptr=e_CRC_CRC16_MAXIM;
    if(strcmp(LoadedString.c_str(),"CRC16_MCRF4XX")==0)
        *Ptr=e_CRC_CRC16_MCRF4XX;
    if(strcmp(LoadedString.c_str(),"CRC16_RIELLO")==0)
        *Ptr=e_CRC_CRC16_RIELLO;
    if(strcmp(LoadedString.c_str(),"CRC16_T10_DIF")==0)
        *Ptr=e_CRC_CRC16_T10_DIF;
    if(strcmp(LoadedString.c_str(),"CRC16_TELEDISK")==0)
        *Ptr=e_CRC_CRC16_TELEDISK;
    if(strcmp(LoadedString.c_str(),"CRC16_TMS37157")==0)
        *Ptr=e_CRC_CRC16_TMS37157;
    if(strcmp(LoadedString.c_str(),"CRC16_USB")==0)
        *Ptr=e_CRC_CRC16_USB;
    if(strcmp(LoadedString.c_str(),"CRC16_A")==0)
        *Ptr=e_CRC_CRC16_A;
    if(strcmp(LoadedString.c_str(),"CRC16_KERMIT")==0)
        *Ptr=e_CRC_CRC16_KERMIT;
    if(strcmp(LoadedString.c_str(),"CRC16_MODBUS")==0)
        *Ptr=e_CRC_CRC16_MODBUS;
    if(strcmp(LoadedString.c_str(),"CRC16_X_25")==0)
        *Ptr=e_CRC_CRC16_X_25;
    if(strcmp(LoadedString.c_str(),"CRC16_XMODEM")==0)
        *Ptr=e_CRC_CRC16_XMODEM;
    if(strcmp(LoadedString.c_str(),"CRC32")==0)
        *Ptr=e_CRC_CRC32;
    if(strcmp(LoadedString.c_str(),"CRC32_BZIP2")==0)
        *Ptr=e_CRC_CRC32_BZIP2;
    if(strcmp(LoadedString.c_str(),"CRC32_C")==0)
        *Ptr=e_CRC_CRC32_C;
    if(strcmp(LoadedString.c_str(),"CRC32_D")==0)
        *Ptr=e_CRC_CRC32_D;
    if(strcmp(LoadedString.c_str(),"CRC32_MPEG2")==0)
        *Ptr=e_CRC_CRC32_MPEG2;
    if(strcmp(LoadedString.c_str(),"CRC32_POSIX")==0)
        *Ptr=e_CRC_CRC32_POSIX;
    if(strcmp(LoadedString.c_str(),"CRC32_Q")==0)
        *Ptr=e_CRC_CRC32_Q;
    if(strcmp(LoadedString.c_str(),"CRC32_JAMCRC")==0)
        *Ptr=e_CRC_CRC32_JAMCRC;
    if(strcmp(LoadedString.c_str(),"CRC32_XFER")==0)
        *Ptr=e_CRC_CRC32_XFER;
    if(strcmp(LoadedString.c_str(),"CRC64_ECMA_182")==0)
        *Ptr=e_CRC_CRC64_ECMA_182;
    if(strcmp(LoadedString.c_str(),"CRC64_GO_ISO")==0)
        *Ptr=e_CRC_CRC64_GO_ISO;
    if(strcmp(LoadedString.c_str(),"CRC64_WE")==0)
        *Ptr=e_CRC_CRC64_WE;
    if(strcmp(LoadedString.c_str(),"CRC64_XZ")==0)
        *Ptr=e_CRC_CRC64_XZ;

    return true;
}

bool CRCTypeCFG::SaveData(string &StoreString)
{
    switch(*Ptr)
    {
        default:
        case e_CRCMAX:
            StoreString="CRC32";
        break;
        case e_CRC_CRC8:
            StoreString="CRC8";
        break;
        case e_CRC_CRC8_SAE_J1850:
            StoreString="CRC8_SAE_J1850";
        break;
        case e_CRC_CRC8_SAE_J1850_ZERO:
            StoreString="CRC8_SAE_J1850_ZERO";
        break;
        case e_CRC_CRC8_8H2F:
            StoreString="CRC8_8H2F";
        break;
        case e_CRC_CRC8_CDMA2000:
            StoreString="CRC8_CDMA2000";
        break;
        case e_CRC_CRC8_DARC:
            StoreString="CRC8_DARC";
        break;
        case e_CRC_CRC8_DVB_S2:
            StoreString="CRC8_DVB_S2";
        break;
        case e_CRC_CRC8_EBU:
            StoreString="CRC8_EBU";
        break;
        case e_CRC_CRC8_ICODE:
            StoreString="CRC8_ICODE";
        break;
        case e_CRC_CRC8_ITU:
            StoreString="CRC8_ITU";
        break;
        case e_CRC_CRC8_MAXIM:
            StoreString="CRC8_MAXIM";
        break;
        case e_CRC_CRC8_ROHC:
            StoreString="CRC8_ROHC";
        break;
        case e_CRC_CRC8_WCDMA:
            StoreString="CRC8_WCDMA";
        break;
        case e_CRC_CRC8_BLUETOOTH:
            StoreString="CRC8_BLUETOOTH";
        break;
        case e_CRC_CRC16_CCIT_ZERO:
            StoreString="CRC16_CCIT_ZERO";
        break;
        case e_CRC_CRC16_ARC:
            StoreString="CRC16_ARC";
        break;
        case e_CRC_CRC16_AUG_CCITT:
            StoreString="CRC16_AUG_CCITT";
        break;
        case e_CRC_CRC16_BUYPASS:
            StoreString="CRC16_BUYPASS";
        break;
        case e_CRC_CRC16_CCITT_FALSE:
            StoreString="CRC16_CCITT_FALSE";
        break;
        case e_CRC_CRC16_CDMA2000:
            StoreString="CRC16_CDMA2000";
        break;
        case e_CRC_CRC16_DDS_110:
            StoreString="CRC16_DDS_110";
        break;
        case e_CRC_CRC16_DECT_R:
            StoreString="CRC16_DECT_R";
        break;
        case e_CRC_CRC16_DECT_X:
            StoreString="CRC16_DECT_X";
        break;
        case e_CRC_CRC16_DNP:
            StoreString="CRC16_DNP";
        break;
        case e_CRC_CRC16_EN_13757:
            StoreString="CRC16_EN_13757";
        break;
        case e_CRC_CRC16_GENIBUS:
            StoreString="CRC16_GENIBUS";
        break;
        case e_CRC_CRC16_MAXIM:
            StoreString="CRC16_MAXIM";
        break;
        case e_CRC_CRC16_MCRF4XX:
            StoreString="CRC16_MCRF4XX";
        break;
        case e_CRC_CRC16_RIELLO:
            StoreString="CRC16_RIELLO";
        break;
        case e_CRC_CRC16_T10_DIF:
            StoreString="CRC16_T10_DIF";
        break;
        case e_CRC_CRC16_TELEDISK:
            StoreString="CRC16_TELEDISK";
        break;
        case e_CRC_CRC16_TMS37157:
            StoreString="CRC16_TMS37157";
        break;
        case e_CRC_CRC16_USB:
            StoreString="CRC16_USB";
        break;
        case e_CRC_CRC16_A:
            StoreString="CRC16_A";
        break;
        case e_CRC_CRC16_KERMIT:
            StoreString="CRC16_KERMIT";
        break;
        case e_CRC_CRC16_MODBUS:
            StoreString="CRC16_MODBUS";
        break;
        case e_CRC_CRC16_X_25:
            StoreString="CRC16_X_25";
        break;
        case e_CRC_CRC16_XMODEM:
            StoreString="CRC16_XMODEM";
        break;
        case e_CRC_CRC32:
            StoreString="CRC32";
        break;
        case e_CRC_CRC32_BZIP2:
            StoreString="CRC32_BZIP2";
        break;
        case e_CRC_CRC32_C:
            StoreString="CRC32_C";
        break;
        case e_CRC_CRC32_D:
            StoreString="CRC32_D";
        break;
        case e_CRC_CRC32_MPEG2:
            StoreString="CRC32_MPEG2";
        break;
        case e_CRC_CRC32_POSIX:
            StoreString="CRC32_POSIX";
        break;
        case e_CRC_CRC32_Q:
            StoreString="CRC32_Q";
        break;
        case e_CRC_CRC32_JAMCRC:
            StoreString="CRC32_JAMCRC";
        break;
        case e_CRC_CRC32_XFER:
            StoreString="CRC32_XFER";
        break;
        case e_CRC_CRC64_ECMA_182:
            StoreString="CRC64_ECMA_182";
        break;
        case e_CRC_CRC64_GO_ISO:
            StoreString="CRC64_GO_ISO";
        break;
        case e_CRC_CRC64_WE:
            StoreString="CRC64_WE";
        break;
        case e_CRC_CRC64_XZ:
            StoreString="CRC64_XZ";
        break;
    }
    return true;
}

static bool RegisterCRCType(class TinyCFG &cfg,const char *XmlName,
      e_CRCType &Data)
{
    class CRCTypeCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new CRCTypeCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
