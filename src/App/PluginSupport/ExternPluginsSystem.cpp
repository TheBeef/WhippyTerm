/*******************************************************************************
 * FILENAME: ExternPluginsSystem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the system used to load plugins from disk and register
 *    them.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (01 Mar 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_InstallPlugin.h"
#include "App/PluginSupport/ExternPluginsSystem.h"
#include "App/PluginSupport/SystemSupport.h"
#include "App/IOSystem.h"
#include "OS/Directorys.h"
#include "OS/FilePaths.h"
#include "OS/System.h"
#include "UI/UIFileReq.h"
#include "UI/UIAsk.h"
#include "ThirdParty/TinyCFG/TinyCFG.h"
#include "ThirdParty/SimpleRIFF/RIFF.h"
#include "Version.h"
#include <string.h>
#include <stdio.h>
#include <list>

using namespace std;

/*** DEFINES                  ***/
#define PLUGIN_DIR              "Plugins"
#define PLUGIN_LIST_FILENAME    "Plugins.cfg"

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

typedef list<struct DLLHandle *> t_DLLLoadedListType;
typedef t_DLLLoadedListType::iterator i_DLLLoadedListType;

typedef list<struct ExternPluginInfo> t_ExternPluginInfoType;
typedef t_ExternPluginInfoType::iterator i_ExternPluginInfoType;

typedef unsigned int (*RegisterPluginFnType)(const struct PI_SystemAPI *SysAPI,unsigned int Version);

class ExternPluginInfoList : public TinyCFGBaseData
{
    public:
        list<struct ExternPluginInfo> *Ptr;
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};

/*** FUNCTION PROTOTYPES      ***/
bool LoadPlugin(const char *File,const char *Name);
static bool RegisterExternPluginInfoList(class TinyCFG &cfg,const char *XmlName,
        list<struct ExternPluginInfo> &Data);
static void LoadPluginList(void);
static void SavePluginList(void);
static void ReadStringChunk(class RIFF &PluginFile,std::string &Str,
        uint32_t ChunkLen);
static void ExternPluginInstallDLL(class RIFF &PluginFile,
        const char *DescFilename,uint32_t ChunkLen);
static bool LoadInfoAboutExternPlugin(const char *Filename,
        struct ExternPluginInfo &Info,bool InstallDLL);

/*** VARIABLE DEFINITIONS     ***/
t_DLLLoadedListType m_DLLLoaded;
t_ExternPluginInfoType m_ExternPlugins;

/*******************************************************************************
 * NAME:
 *    RegisterExternPlugins
 *
 * SYNOPSIS:
 *    void RegisterExternPlugins(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function scans and loads all the external plugins.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RegisterExternPlugins(void)
{
    i_ExternPluginInfoType Plugin;
    string PluginDir;
    string PluginFilename;
    const char *LoadFilename;
    char buff[200];

//    TmpSavePluginData();
    LoadPluginList();
    if(m_ExternPlugins.size()==0)
    {
        /* No plugins to load */
        return;
    }

    /* Ok, load each of the plugins that are enabled */
    if(!GetAppDataPath(PluginDir))
    {
        UIAsk("Error","Failed to load plugins.  App data path unknown.",
                e_AskBox_Error);
        return;
    }

    PluginDir+=PLUGIN_DIR;

    if(!PathExists(PluginDir.c_str()))
    {
        snprintf(buff,sizeof(buff),"Failed to load plugins.  %s missing.",
                PluginDir.c_str());
        UIAsk("Error",buff,e_AskBox_Error);
        return;
    }

    for(Plugin=m_ExternPlugins.begin();Plugin!=m_ExternPlugins.end();Plugin++)
    {
        if(!Plugin->Enabled)
            continue;

        PluginFilename=PluginDir;
        PluginFilename+="/";
        PluginFilename+=Plugin->Filename;
        LoadFilename=ConvertPath2Native(PluginFilename.c_str());
        if(LoadFilename==NULL)
        {
            snprintf(buff,sizeof(buff)-1,"Failed to load plugin.  "
                    "\"%s\" failed to filename + path to long.",
                    Plugin->Filename.c_str());
            UIAsk("Error",buff,e_AskBox_Error);
            continue;
        }

        Plugin->DLLFound=LoadPlugin(LoadFilename,Plugin->PluginName.c_str());
    }
}

/*******************************************************************************
 * NAME:
 *    LoadPluginList
 *
 * SYNOPSIS:
 *    static void LoadPluginList(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function loads the list of installed plugins.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    User will be prompted on error
 *
 * SEE ALSO:
 *    SavePluginList()
 ******************************************************************************/
static void LoadPluginList(void)
{
    class TinyCFG cfg("PluginData");
    string Path;
    string PluginListFilename;

    try
    {
        if(GetAppDataPath(Path)==false)
        {
            UIAsk("Error","Failed to load plugins.  App data path unknown.",
                    e_AskBox_Error);
            return;
        }

        /* See if this path exists */
        if(!PathExists(Path.c_str()))
        {
            /* Try making it */
            if(!MakePathDir(Path.c_str()))
            {
                UIAsk("Error","Failed to load plugins.  App data path could "
                        "not be made.",e_AskBox_Error);
                return;
            }
        }

        PluginListFilename=Path;
        PluginListFilename+=PLUGIN_LIST_FILENAME;

        RegisterExternPluginInfoList(cfg,"Plugins",m_ExternPlugins);
        cfg.LoadCFGFile(PluginListFilename.c_str());
    }
    catch(...)
    {
        return;
    }
}

/*******************************************************************************
 * NAME:
 *    SavePluginList
 *
 * SYNOPSIS:
 *    static void SavePluginList(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function saves the 'm_ExternPlugins' list to the disk.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    User will be prompted on error
 *
 * SEE ALSO:
 *    LoadPluginList();
 ******************************************************************************/
static void SavePluginList(void)
{
    class TinyCFG cfg("PluginData");
    string Path;
    string PluginListFilename;

    try
    {
        if(GetAppDataPath(Path)==false)
        {
            UIAsk("Error","Failed to save plugins.  App data path unknown.",
                    e_AskBox_Error);
            return;
        }

        /* See if this path exists */
        if(!PathExists(Path.c_str()))
        {
            /* Try making it */
            if(!MakePathDir(Path.c_str()))
            {
                UIAsk("Error","Failed to save plugins.  App data path could "
                        "not be made.",e_AskBox_Error);
                return;
            }
        }

        PluginListFilename=Path;
        PluginListFilename+=PLUGIN_LIST_FILENAME;

        RegisterExternPluginInfoList(cfg,"Plugins",m_ExternPlugins);
        if(!cfg.SaveCFGFile(PluginListFilename.c_str()))
        {
            UIAsk("Error","There was an error saving the plugin list.",
                    e_AskBox_Error);
            return;
        }
    }
    catch(...)
    {
        return;
    }
}

/*******************************************************************************
 * NAME:
 *    FreeLoadedExternPlugins
 *
 * SYNOPSIS:
 *    void FreeLoadedExternPlugins(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees any external plugins.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void FreeLoadedExternPlugins(void)
{
    i_DLLLoadedListType i;

    for(i=m_DLLLoaded.begin();i!=m_DLLLoaded.end();i++)
        CloseDLL(*i);
    m_DLLLoaded.clear();
}

/*******************************************************************************
 * NAME:
 *    LoadPlugin
 *
 * SYNOPSIS:
 *    bool LoadPlugin(const char *File,const char *Name);
 *
 * PARAMETERS:
 *    File [I] -- The filename (with path) of the plugin to load.
 *    Name [I] -- The name to display on error.
 *
 * FUNCTION:
 *    This function loads and registers a plugin from disk.
 *
 * RETURNS:
 *    true -- DLL was loaded
 *    false -- There was a problem loading the DLL
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool LoadPlugin(const char *File,const char *Name)
{
    struct DLLHandle *Plugin;
    char buff[200];
    const char *Error;
    RegisterPluginFnType RegisterPlugin;
    unsigned int ReqVer;
    uint8_t Maj;
    uint8_t Min;
    uint8_t Patch;
    uint8_t Letter;
    const char *Msg;

    Plugin=LoadDLL(File);
    if(Plugin==NULL)
    {
        Error=LastDLLError();
        if(Error==NULL)
            Error="";
        snprintf(buff,sizeof(buff)-1,"Failed to load plugin.  "
                "\"%s\" failed to load:\n%s",Name,Error);
        UIAsk("Error",buff,e_AskBox_Error);
        return false;
    }

    /* Call the init in the plugin */
    RegisterPlugin=(RegisterPluginFnType)DLL2Function(Plugin,"RegisterPlugin");
    if(RegisterPlugin==NULL)
    {
        /* Not a Whippy Term plugin as the plugin must have a register plugin
           function */
        snprintf(buff,sizeof(buff)-1,"Failed to load plugin.  "
                "\"%s\" is not a WhipyTerm plugin.",Name);
        UIAsk("Error",buff,e_AskBox_Error);
        return false;
    }

    /* Ok, call the init function */
    ReqVer=RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    if(ReqVer!=0)
    {
        /* Tell user what version of WhippyTerm is needed */
        Maj=(ReqVer>>24)&0xFF;
        Min=(ReqVer>>16)&0xFF;
        Patch=(ReqVer>>8)&0xFF;
        Letter=ReqVer&0xFF;
        if(Patch==0 && Letter==0)
            Msg="The plugin %s requires version %d.%d of WhippyTerm";
        else if(Letter==0)
            Msg="The plugin %s requires version %d.%d.%d of WhippyTerm";
        else
            Msg="The plugin %s requires version %d.%d.%d%c of WhippyTerm";
        snprintf(buff,sizeof(buff),Msg,Name,Maj,Min,Patch,Letter+'A'-1);
        UIAsk("Error",buff,e_AskBox_Error);
        return false;
    }

    m_DLLLoaded.push_back(Plugin);

    return true;
}

/*******************************************************************************
 * NAME:
 *    ExternPluginInfoList::LoadElement
 *
 * SYNOPSIS:
 *    bool ExternPluginInfoList::LoadElement(class TinyCFG *CFG);
 *
 * PARAMETERS:
 *    CFG [I] -- The tiny cfg to connect to
 *
 * FUNCTION:
 *    This function loads elements from a tiny cfg for the extern plugin
 *    list.
 *
 * RETURNS:
 *    true -- data loaded
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool ExternPluginInfoList::LoadElement(class TinyCFG *CFG)
{
    struct ExternPluginInfo NewData;
    class TinyCFG SubCFG("Plugin");

    Ptr->clear();

    NewData.Register(&SubCFG);

    /* System info */
    NewData.Filename="";
    NewData.Enabled=false;
    NewData.PluginName="";
    NewData.Description="";
    NewData.Contributors="";
    NewData.Copyright="";
    NewData.ReleaseDate="";
    NewData.PluginClass=e_ExtPluginClass_Unknown;
    NewData.PluginSubClass=0;
    NewData.DLLFound=false;     // Not stored to disk

    SubCFG.ConnectToParentCFGForReading(CFG);

    while(SubCFG.ReadNextCFG())
        Ptr->push_back(NewData);

    return true;
}

/*******************************************************************************
 * NAME:
 *    ExternPluginInfoList::SaveElement
 *
 * SYNOPSIS:
 *    bool ExternPluginInfoList::SaveElement(class TinyCFG *CFG);
 *
 * PARAMETERS:
 *    CFG [I] -- The tiny cfg to connect to
 *
 * FUNCTION:
 *    This function saves data from the extern plugin list and a tiny cfg.
 *
 * RETURNS:
 *    true -- things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool ExternPluginInfoList::SaveElement(class TinyCFG *CFG)
{
    list<struct ExternPluginInfo>::iterator i;
    class TinyCFG SubCFG("Plugin");
    struct ExternPluginInfo NewData;

    NewData.Register(&SubCFG);

    for(i=Ptr->begin();i!=Ptr->end();i++)
    {
        NewData=*i;
        SubCFG.WriteCFGUsingParentCFG(CFG);
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    RegisterExternPluginInfoList
 *
 * SYNOPSIS:
 *    bool RegisterExternPluginInfoList(class TinyCFG &cfg,const char *XmlName,
 *              list<struct ExternPluginInfo> &Data);
 *
 * PARAMETERS:
 *    cfg [I] -- The cfg to connect to
 *    XmlName [I] -- The name to use to stick the data into
 *    Data [I/O] -- The var to load / save data into / out of.
 *
 * FUNCTION:
 *    This is a function that connects a variable to a tiny cfg class for
 *    loading/saving.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RegisterExternPluginInfoList(class TinyCFG &cfg,const char *XmlName,
        list<struct ExternPluginInfo> &Data)
{
    class ExternPluginInfoList *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new ExternPluginInfoList;
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

/*******************************************************************************
 * NAME:
 *    GetExternPluginInfo
 *
 * SYNOPSIS:
 *    bool GetExternPluginInfo(int Index,struct ExternPluginInfo &RetInfo);
 *
 * PARAMETERS:
 *    Index [I] -- The index into the plugin list starting at 0.
 *    RetInfo [O] -- Info about this plugin
 *
 * FUNCTION:
 *    This function gets info about a plugin.
 *
 * RETURNS:
 *    true -- The 'RetInfo' is valid
 *    false -- This index does not exist.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool GetExternPluginInfo(int Index,struct ExternPluginInfo &RetInfo)
{
    i_ExternPluginInfoType Plugin;
    int Count;

    Count=0;
    for(Plugin=m_ExternPlugins.begin();Plugin!=m_ExternPlugins.end() &&
            Count<Index;Plugin++,Count++)
    {
    }
    if(Plugin==m_ExternPlugins.end())
        return false;

    RetInfo=*Plugin;

    return true;
}

/*******************************************************************************
 * NAME:
 *    SetExternPluginEnabled
 *
 * SYNOPSIS:
 *    void SetExternPluginEnabled(int Index,bool Enabled);
 *
 * PARAMETERS:
 *    Enabled [I] -- true = enable the plugin, false = disable the plugin
 *
 * FUNCTION:
 *    This function marks the plugin as enabled or disabled.
 *
 * LIMITATIONS:
 *    You need to restart for the uninstall to take effect.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void SetExternPluginEnabled(int Index,bool Enabled)
{
    i_ExternPluginInfoType Plugin;
    int Count;

    Count=0;
    for(Plugin=m_ExternPlugins.begin();Plugin!=m_ExternPlugins.end() &&
            Count<Index;Plugin++,Count++)
    {
    }
    if(Plugin==m_ExternPlugins.end())
        return;

    Plugin->Enabled=Enabled;

    SavePluginList();
}

/*******************************************************************************
 * NAME:
 *    UninstallExternPlugin
 *
 * SYNOPSIS:
 *    void UninstallExternPlugin(int Index);
 *
 * PARAMETERS:
 *    Index [I] -- The index of the plugin to uninstall.
 *
 * FUNCTION:
 *    This function uninstalls a plugin.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    The indexes to the other plugins are effected by this function.  You
 *    need to rescan after calling this.
 *
 * LIMITATIONS:
 *    You need to restart for the uninstall to take effect.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UninstallExternPlugin(int Index)
{
    i_ExternPluginInfoType Plugin;
    int Count;
    string PluginDir;
    string PluginFilename;
    const char *rmFilename;
    char buff[200];

    Count=0;
    for(Plugin=m_ExternPlugins.begin();Plugin!=m_ExternPlugins.end() &&
            Count<Index;Plugin++,Count++)
    {
    }
    if(Plugin==m_ExternPlugins.end())
        return;

    /* Erase the dll */
    /* Ok, load each of the plugins that are enabled */
    if(!GetAppDataPath(PluginDir))
    {
        UIAsk("Error","Failed to uninstall plugin.  App data path unknown.",
                e_AskBox_Error);
        return;
    }

    PluginDir+=PLUGIN_DIR;

    if(!PathExists(PluginDir.c_str()))
    {
        snprintf(buff,sizeof(buff),"Failed to uninstall plugin.  %s missing.",
                PluginDir.c_str());
        UIAsk("Error",buff,e_AskBox_Error);
        return;
    }

    PluginFilename=PluginDir;
    PluginFilename+="/";
    PluginFilename+=Plugin->Filename;
    rmFilename=ConvertPath2Native(PluginFilename.c_str());
    if(rmFilename==NULL)
    {
        UIAsk("Error","Failed to uninstall plugin.  "
                "Filename + path to long.",e_AskBox_Error);
        return;
    }

    remove(rmFilename);

    m_ExternPlugins.erase(Plugin);

    SavePluginList();
}

/*******************************************************************************
 * NAME:
 *    InstallNewExternPlugin
 *
 * SYNOPSIS:
 *    bool InstallNewExternPlugin(const char *Filename);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename of the .wtp file
 *
 * FUNCTION:
 *    This function installs a WhippyTerm Plugin.  It loads the plugin file
 *    make sure everythings ok, and then installs the plugin.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    GetNewExternPluginInfo()
 ******************************************************************************/
bool InstallNewExternPlugin(const char *Filename)
{
    struct ExternPluginInfo Info;
    string PluginDir;
    string PluginFilename;
    const char *LoadFilename;
    char buff[200];
    char Verbuff[100];
    uint8_t Maj;
    uint8_t Min;
    uint8_t Patch;
    uint8_t Letter;

    if(!LoadInfoAboutExternPlugin(Filename,Info,false))
        return false;

    if(Info.APIVersion>WHIPPYTERM_VERSION)
    {
        Maj=(Info.APIVersion>>24)&0xFF;
        Min=(Info.APIVersion>>16)&0xFF;
        Patch=(Info.APIVersion>>8)&0xFF;
        Letter=(Info.APIVersion>>0)&0xFF;
        if(Patch==0 && Letter==0)
            sprintf(Verbuff,"%d.%d",Maj,Min);
        else if(Letter==0)
            sprintf(Verbuff,"%d.%d.%d",Maj,Min,Patch);
        else
            sprintf(Verbuff,",%d.%d.%d%c",Maj,Min,Patch,Letter+'A'-1);

        snprintf(buff,sizeof(buff),"Failed to install plugin.  This plugin "
                "requires a newer version of WhippyTerm\nRequired version:"
                "%s",Verbuff);
        UIAsk("Error",buff,e_AskBox_Error);
        return false;
    }

    /* Ok do it again, but this time install the files */
    if(!LoadInfoAboutExternPlugin(Filename,Info,true))
        return false;

    Info.Enabled=true;

    /* Register this plugin */
    if(!GetAppDataPath(PluginDir))
    {
        UIAsk("Error","Failed to install plugin.  App data path unknown.",
                e_AskBox_Error);
        return false;
    }

    PluginDir+=PLUGIN_DIR;

    if(!PathExists(PluginDir.c_str()))
    {
        snprintf(buff,sizeof(buff),"Failed to install plugin.  %s missing.",
                PluginDir.c_str());
        UIAsk("Error",buff,e_AskBox_Error);
        return false;
    }
    PluginFilename=PluginDir;
    PluginFilename+="/";
    PluginFilename+=Info.Filename;
    LoadFilename=ConvertPath2Native(PluginFilename.c_str());
    if(LoadFilename==NULL)
    {
        snprintf(buff,sizeof(buff)-1,"Failed to install plugin.  "
                "\"%s\" failed to filename + path to long.",
                Info.Filename.c_str());
        UIAsk("Error",buff,e_AskBox_Error);
        return false;
    }

    Info.DLLFound=LoadPlugin(LoadFilename,Info.PluginName.c_str());

    /* Add this to the list of plugins */
    m_ExternPlugins.push_back(Info);

    SavePluginList();

    if(Info.PluginClass==e_ExtPluginClass_IODriver)
    {
        /* We just installed a IO Driver we need to tell the IO system
           to rescan so this new entries from this driver show up */
        IOS_ScanForConnections();
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    GetNewExternPluginInfo
 *
 * SYNOPSIS:
 *    bool GetNewExternPluginInfo(const char *Filename,
 *          struct ExternPluginInfo &Info);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename of the .wtp file
 *    Info [O] -- Info about the plugin.
 *
 * FUNCTION:
 *    This function gets info about the .wtp file.  It does not install it.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    InstallNewExternPlugin()
 ******************************************************************************/
bool GetNewExternPluginInfo(const char *Filename,struct ExternPluginInfo &Info)
{
    return LoadInfoAboutExternPlugin(Filename,Info,false);
}

/*******************************************************************************
 * NAME:
 *    LoadInfoAboutExternPlugin
 *
 * SYNOPSIS:
 *    static bool LoadInfoAboutExternPlugin(const char *Filename,
 *          struct ExternPluginInfo &Info,bool InstallDLL);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename and path of the plugin .wtp file to load
 *    Info [O] -- Fill this in with the loaded plugin into
 *    InstallDLL [I] -- If this is true then we copy the DLL to the correct
 *                      installed directory.
 *
 * FUNCTION:
 *    This is a helper function that gets info about a plugin and optionally
 *    install the dll.
 *
 * RETURNS:
 *    true -- things worked out
 *    false -- There was an error.  The user has been prompted.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool LoadInfoAboutExternPlugin(const char *Filename,
        struct ExternPluginInfo &Info,bool InstallDLL)
{
    class RIFF PluginFile(true);
    char ChunkID[5];
    uint32_t ChunkLen;
    bool Opened;
    bool RetValue;
    char buff[200];
    string InstallFilename;
    uint32_t Tmp32;
    uint8_t Tmp8;

    Opened=false;
    try
    {
        InstallFilename="";
        Info.DLLFound=false;

        PluginFile.Open(Filename,e_RIFFOpen_Read,"WTPI");
        Opened=true;

        while(PluginFile.ReadNextDataBlock(ChunkID,&ChunkLen))
        {
            if(strcmp(ChunkID,"FILE")==0)
                ReadStringChunk(PluginFile,InstallFilename,ChunkLen);
            else if(strcmp(ChunkID,"NAME")==0)
                ReadStringChunk(PluginFile,Info.PluginName,ChunkLen);
            else if(strcmp(ChunkID,"DESC")==0)
                ReadStringChunk(PluginFile,Info.Description,ChunkLen);
            else if(strcmp(ChunkID,"CONT")==0)
                ReadStringChunk(PluginFile,Info.Contributors,ChunkLen);
            else if(strcmp(ChunkID,"COPY")==0)
                ReadStringChunk(PluginFile,Info.Copyright,ChunkLen);
            else if(strcmp(ChunkID,"DATE")==0)
                ReadStringChunk(PluginFile,Info.ReleaseDate,ChunkLen);
            else if(strcmp(ChunkID,"VER ")==0)
            {
                PluginFile.Read(&Tmp8,sizeof(Tmp8));
                Info.Version=Tmp8<<24;
                PluginFile.Read(&Tmp8,sizeof(Tmp8));
                Info.Version|=Tmp8<<16;
                PluginFile.Read(&Tmp8,sizeof(Tmp8));
                Info.Version|=Tmp8<<8;
                PluginFile.Read(&Tmp8,sizeof(Tmp8));
                Info.Version|=Tmp8;
            }
            else if(strcmp(ChunkID,"APIV")==0)
            {
                PluginFile.Read(&Tmp8,sizeof(Tmp8));
                Info.APIVersion=Tmp8<<24;
                PluginFile.Read(&Tmp8,sizeof(Tmp8));
                Info.APIVersion|=Tmp8<<16;
                PluginFile.Read(&Tmp8,sizeof(Tmp8));
                Info.APIVersion|=Tmp8<<8;
                PluginFile.Read(&Tmp8,sizeof(Tmp8));
                Info.APIVersion|=Tmp8;
            }
            else if(strcmp(ChunkID,"TYPE")==0)
            {
                PluginFile.Read(&Tmp32,sizeof(Tmp32));
                Info.PluginClass=Tmp32;
            }
            else if(strcmp(ChunkID,"SUBT")==0)
            {
                PluginFile.Read(&Tmp32,sizeof(Tmp32));
                Info.PluginSubClass=Tmp32;
            }
            else if(strcmp(ChunkID,"LIN6")==0 && RunningOS()==e_OS_Linux &&
                    InstallDLL)
            {
                /* Linux 64 bit */
                if(InstallFilename=="")
                    throw("Error in plugin file (chunk order)");
                InstallFilename+=".so";
                Info.Filename=InstallFilename;
                ExternPluginInstallDLL(PluginFile,InstallFilename.c_str(),
                        ChunkLen);
                Info.DLLFound=true;
            }
            else if(strcmp(ChunkID,"WIN6")==0 && RunningOS()==e_OS_Windows &&
                    InstallDLL)
            {
                /* Windows 64 bit */
                if(InstallFilename=="")
                    throw("Error in plugin file (chunk order)");
                InstallFilename+=".dll";
                Info.Filename=InstallFilename;
                ExternPluginInstallDLL(PluginFile,InstallFilename.c_str(),
                        ChunkLen);
                Info.DLLFound=true;
            }
            else if(strcmp(ChunkID,"MACS")==0 && RunningOS()==e_OS_MacOSX &&
                    InstallDLL)
            {
                /* Mac OS */
                if(InstallFilename=="")
                    throw("Error in plugin file (chunk order)");
                InstallFilename+=".so";
                Info.Filename=InstallFilename;
                ExternPluginInstallDLL(PluginFile,InstallFilename.c_str(),
                        ChunkLen);
                Info.DLLFound=true;
            }
            else if(strcmp(ChunkID,"RPI3")==0 && RunningOS()==e_OS_RaspberryPI
                    && RunningExeBits()==32 && InstallDLL)
            {
                /* Raspberry PI 32 bit */
                if(InstallFilename=="")
                    throw("Error in plugin file (chunk order)");
                InstallFilename+=".so";
                Info.Filename=InstallFilename;
                ExternPluginInstallDLL(PluginFile,InstallFilename.c_str(),
                        ChunkLen);
                Info.DLLFound=true;
            }
            else if(strcmp(ChunkID,"RPI6")==0 && RunningOS()==e_OS_RaspberryPI
                    && RunningExeBits()==64 && InstallDLL)
            {
                /* Raspberry PI 64 bit */
                if(InstallFilename=="")
                    throw("Error in plugin file (chunk order)");
                InstallFilename+=".so";
                Info.Filename=InstallFilename;
                ExternPluginInstallDLL(PluginFile,InstallFilename.c_str(),
                        ChunkLen);
                Info.DLLFound=true;
            }
        }
        PluginFile.Close();
        Opened=false;

        RetValue=true;
    }
    catch(const char *Msg)
    {
        snprintf(buff,sizeof(buff),"Failed to install plugin:%s\n",Msg);
        UIAsk("Error",buff,e_AskBox_Error);
        RetValue=false;
    }
    catch(...)
    {
        UIAsk("Error","Failed to install plugin.",e_AskBox_Error);
        RetValue=false;
    }
    if(Opened)
        PluginFile.Close();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    ReadStringChunk
 *
 * SYNOPSIS:
 *    static void ReadStringChunk(class RIFF &PluginFile,std::string &Str,
 *          uint32_t ChunkLen);
 *
 * PARAMETERS:
 *    PluginFile [I] -- The RIFF to load from
 *    Str [O] -- The string to store the read string into
 *    ChunkLen [I] -- The number of bytes to read into this string
 *
 * FUNCTION:
 *    This is a helper function to read a chunk into a string.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    throws errors
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void ReadStringChunk(class RIFF &PluginFile,std::string &Str,
        uint32_t ChunkLen)
{
    char *tmpbuff;
    char buff[100];

    if(ChunkLen<sizeof(buff)-1)
    {
        PluginFile.Read(buff,ChunkLen);
        buff[ChunkLen]=0;
        Str=buff;
    }
    else
    {
        /* We need a tmp buff */
        tmpbuff=(char *)malloc(ChunkLen+1);
        if(tmpbuff==NULL)
            throw("Out of memory");
        PluginFile.Read(buff,ChunkLen);
        tmpbuff[ChunkLen]=0;
        Str=tmpbuff;
        free(tmpbuff);
    }
}

/*******************************************************************************
 * NAME:
 *    ExternPluginInstallDLL
 *
 * SYNOPSIS:
 *    static void ExternPluginInstallDLL(class RIFF &PluginFile,
 *              const char *DescFilename,uint32_t ChunkLen);
 *
 * PARAMETERS:
 *    PluginFile [I] -- The RIFF to load from
 *    DescFilename [I] -- The filename of the DLL to make.  If this already
 *                        exist the we throw an error
 *    ChunkLen [I] -- The number of bytes in this file.
 *
 * FUNCTION:
 *    This function extracts a dll from the plugin install file and writes it
 *    to the dll area.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    throws an error if the file exists (or there is another problem)
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void ExternPluginInstallDLL(class RIFF &PluginFile,
        const char *DescFilename,uint32_t ChunkLen)
{
    uint8_t DataBlock[100];
    uint32_t BytesLeft;
    uint32_t Bytes2Read;
    FILE *in;
    FILE *out;
    string PluginDir;
    string PluginFilename;
    const char *SaveFilename;

    if(!GetAppDataPath(PluginDir))
        throw("App data path unknown.");

    PluginDir+=PLUGIN_DIR;

    /* See if this path exists */
    if(!PathExists(PluginDir.c_str()))
    {
        /* Try making it */
        if(!MakePathDir(PluginDir.c_str()))
        {
            throw("App data path could not be made.");
        }
    }

    PluginFilename=PluginDir;
    PluginFilename+="/";
    PluginFilename+=DescFilename;
    SaveFilename=ConvertPath2Native(PluginFilename.c_str());
    if(SaveFilename==NULL)
        throw("Install path to long");

    /* See if it already exists */
    in=fopen(SaveFilename,"rb");
    if(in!=NULL)
    {
        static char ErrorMsg[100];
        fclose(in);
        snprintf(ErrorMsg,sizeof(ErrorMsg),"File already exists.\nCan not "
                "overwrite \"%s\"",SaveFilename);
        throw(ErrorMsg);
    }

    out=fopen(SaveFilename,"wb");
    if(out==NULL)
    {
        throw("Could not open destination plugin file");
    }

    /* Copy the data into the file */
    BytesLeft=ChunkLen;
    while(BytesLeft>0)
    {
        Bytes2Read=BytesLeft;
        if(Bytes2Read>sizeof(DataBlock))
            Bytes2Read=sizeof(DataBlock);

        PluginFile.Read(DataBlock,Bytes2Read);

        if(fwrite(DataBlock,Bytes2Read,1,out)!=1)
        {
            fclose(out);
            throw("Failed to write to file");
        }

        BytesLeft-=Bytes2Read;
    }
    fclose(out);

    if(!MakeFileExe(SaveFilename))
        throw("Failed to make installed plugin executable");
}

/*******************************************************************************
 * NAME:
 *    PromptAndInstallPlugin
 *
 * SYNOPSIS:
 *    void PromptAndInstallPlugin(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user to select a plugin to install and then
 *    starts the install plugin dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    RunInstallPluginDialog()
 ******************************************************************************/
void PromptAndInstallPlugin(void)
{
    std::string Path;
    std::string Filename;
    std::string PathFilename;

    if(UI_LoadFileReq("Install Plugin",Path,Filename,
            "WhippyTerm Plugins|*.wtp\n"
            "All Files|*",0))
    {
        PathFilename=UI_ConcatFile2Path(Path,Filename);
        RunInstallPluginDialog(PathFilename.c_str());
    }
}

