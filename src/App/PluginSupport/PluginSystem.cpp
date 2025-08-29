/*******************************************************************************
 * FILENAME: PluginSystem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 21 Aug 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (21 Aug 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "PluginSystem.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/DataProcessorsSystem.h"
#include "App/IOSystem.h"
#include "App/MainWindow.h"
#include "App/PluginSupport/ExternPluginsSystem.h"
#include "App/StdPlugins/RegisterStdPlugins.h"
#include <map>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct PluginInfo
{
    int InUseCount;
    struct DLLHandle *ExternPluginHandle;
};

typedef map<string,struct PluginInfo> t_PluginList;
typedef t_PluginList::iterator i_PluginList;

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
t_PluginList m_PluginList;

/*******************************************************************************
 * NAME:
 *    InitPluginSystem
 *
 * SYNOPSIS:
 *    void InitPluginSystem(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the plugin system.  It will register built in
 *    plugins and call the extern plugin system to register external plugins.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void InitPluginSystem(void)
{
    RegisterExternPlugins();
    RegisterStdPlugins();

    IOS_InitPlugins();
}

/*******************************************************************************
 * NAME:
 *    RegisterPluginWithSystem
 *
 * SYNOPSIS:
 *    void RegisterPluginWithSystem(const char *IDStr);
 *
 * PARAMETERS:
 *    IDStr [I] -- The string that identifies this plugin
 *
 * FUNCTION:
 *    This function registers a plugin with the plugin system.  This is done
 *    by the subsystem then a plugin registeres it's self with them.  This
 *    will allow others in the system find this plugin later.
 *
 * RETURNS:
 *    ??
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RegisterPluginWithSystem(const char *IDStr)
{
    struct PluginInfo NewPluginInfo;

    NewPluginInfo.InUseCount=0;
    NewPluginInfo.ExternPluginHandle=GetCurrentExternPluginHandle();

    /* Add this plugin to the list of installed plugins */
    m_PluginList.insert(make_pair(IDStr,NewPluginInfo));
}

/*******************************************************************************
 * NAME:
 *    UnRegisterPluginWithSystem
 *
 * SYNOPSIS:
 *    void UnRegisterPluginWithSystem(const char *IDStr);
 *
 * PARAMETERS:
 *    IDStr [I] -- The string that identifies this plugin
 *
 * FUNCTION:
 *    This function removes a plugin from the system (used when uninstalling
 *    a plugin)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UnRegisterPluginWithSystem(const char *IDStr)
{
    i_PluginList Plugin;

    Plugin=m_PluginList.find(IDStr);
    if(Plugin==m_PluginList.end())
        return;
    m_PluginList.erase(Plugin);
}

/*******************************************************************************
 * NAME:
 *    NotePluginInUse
 *
 * SYNOPSIS:
 *    void NotePluginInUse(const char *IDStr);
 *
 * PARAMETERS:
 *    IDStr [I] -- The ID for this plugin
 *
 * FUNCTION:
 *    This function notes that a plugin is now in use.  Any plugin that is
 *    in use can not removed or changed while it's in use.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UnNotePluginInUse()
 ******************************************************************************/
void NotePluginInUse(const char *IDStr)
{
    i_PluginList Plugin;
    Plugin=m_PluginList.find(IDStr);
    if(Plugin==m_PluginList.end())
        return;
    Plugin->second.InUseCount++;
}

/*******************************************************************************
 * NAME:
 *    UnNotePluginInUse
 *
 * SYNOPSIS:
 *    void UnNotePluginInUse(const char *IDStr);
 *
 * PARAMETERS:
 *    IDStr [I] -- The ID for this plugin
 *
 * FUNCTION:
 *    This function is called when a plugin is no longer being used.  You
 *    call NotePluginInUse() every time a plugin is used and this function
 *    every time you are done with it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    NotePluginInUse()
 ******************************************************************************/
void UnNotePluginInUse(const char *IDStr)
{
    i_PluginList Plugin;
    Plugin=m_PluginList.find(IDStr);
    if(Plugin==m_PluginList.end())
        return;
    Plugin->second.InUseCount--;
    if(Plugin->second.InUseCount<0)
    {
        /* Hu? */
        Plugin->second.InUseCount=0;
    }
}

/*******************************************************************************
 * NAME:
 *    IsPluginInUse
 *
 * SYNOPSIS:
 *    bool IsPluginInUse(const char *IDStr);
 *
 * PARAMETERS:
 *    IDStr [I] -- The ID for this plugin
 *
 * FUNCTION:
 *    This function checks if a plugin is in use or not.
 *
 * RETURNS:
 *    true -- Plugin is in use
 *    false -- Plugin is not being used
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool IsPluginInUse(struct ExternPluginInfo *ExPlugin)
{
    i_PluginList Plugin;

    for(Plugin=m_PluginList.begin();Plugin!=m_PluginList.end();Plugin++)
    {
        if(Plugin->second.ExternPluginHandle==ExPlugin->DLLHandle)
        {
            if(Plugin->second.InUseCount>0)
                return true;
        }
    }
    return false;
}

/*******************************************************************************
 * NAME:
 *    InformOfNewPluginInstalled
 *
 * SYNOPSIS:
 *    void InformOfNewPluginInstalled(struct ExternPluginInfo *Info);
 *
 * PARAMETERS:
 *    Info [I] -- The info about this plugin (from the .wtp file)
 *
 * FUNCTION:
 *    This function is called when a new plugin is installed in the system.
 *    This is not when a plugin is loaded, but instead when a plug is first
 *    installed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void InformOfNewPluginInstalled(struct ExternPluginInfo *Info)
{
    i_PluginList Plugin;

    for(Plugin=m_PluginList.begin();Plugin!=m_PluginList.end();Plugin++)
    {
        if(Plugin->second.ExternPluginHandle==Info->DLLHandle)
        {
            /* Tell all the areas that this plugin has been installed */
            FTPS_InformOfNewPluginInstalled(Plugin->first.c_str());
            IOS_InformOfNewPluginInstalled(Plugin->first.c_str());
            DPS_InformOfNewPluginInstalled(Plugin->first.c_str());
            MW_InformOfNewPluginInstalled(Plugin->first.c_str());
        }
    }
}

/*******************************************************************************
 * NAME:
 *    InformOfPluginUninstalled
 *
 * SYNOPSIS:
 *    void InformOfPluginUninstalled(struct ExternPluginInfo *Info);
 *
 * PARAMETERS:
 *    Info [I] -- The info about this plugin (from the .wtp file)
 *
 * FUNCTION:
 *    This function calls different parts of the system to tell them that
 *    a plugin has been uninstalled.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void InformOfPluginUninstalled(struct ExternPluginInfo *Info)
{
    i_PluginList Plugin;

    if(Info->DLLHandle==NULL)
        return;

    Plugin=m_PluginList.begin();
    while(Plugin!=m_PluginList.end())
    {
        if(Plugin->second.ExternPluginHandle==Info->DLLHandle)
        {
            /* Because the main window may need to release the resources
               used by the plugin we tell it we are about to uninstall this
               plugin (we will tell it we are done later) */
            MW_InformOfPluginAboutToUninstall(Plugin->first.c_str());

            /* Tell all the areas that this plugin has been installed */
            FTPS_InformOfPluginUninstalled(Plugin->first.c_str());
            IOS_InformOfPluginUninstalled(Plugin->first.c_str());
            DPS_InformOfPluginUninstalled(Plugin->first.c_str());
            MW_InformOfPluginUninstalled(Plugin->first.c_str());

            /* List likely changed, start again */
            Plugin=m_PluginList.begin();
            continue;
        }
        Plugin++;
    }
}
