/*******************************************************************************
 * FILENAME: SystemSupport.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
#include "App/IOSystem.h"
#include "App/Connections.h"
#include "App/ConnectionsGlobal.h"
#include "App/DataProcessorsSystem.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/ScriptingSystem.h"
#include "App/PluginSupport/ExperimentalID.h"
#include "App/PluginSupport/KeyValueSupport.h"
#include "App/PluginSupport/PluginUISupport.h"
#include "App/PluginSupport/SystemSupport.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
const struct IOS_API *PISys_GetAPI_IO(void);
const struct DPS_API *PISys_GetAPI_DataProcessors(void);
const struct FTPS_API *PISys_GetAPI_FileTransferProtocol(void);
static uint32_t PISys_GetExperimentalID(void);
const struct ScriptingSystem_API *PISys_GetAPI_Scripting(void);

/*** VARIABLE DEFINITIONS     ***/
const struct PI_SystemAPI g_PISystemAPI=
{
    PISys_GetAPI_IO,
    PISys_GetAPI_DataProcessors,
    PISys_GetAPI_FileTransferProtocol,
    PI_KVClear,
    PI_KVAddItem,
    PI_KVGetItem,
    PISys_GetExperimentalID,
    PISys_GetAPI_Scripting,
};

/*******************************************************************************
 * NAME:
 *    PISys_GetAPI_IO
 *
 * SYNOPSIS:
 *    const struct IOS_API *PISys_GetAPI_IO(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets access to the IO System API.
 *
 * RETURNS:
 *    A pointer to the IO System API.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct IOS_API *PISys_GetAPI_IO(void)
{
    return &g_IOS_API;
}

/*******************************************************************************
 * NAME:
 *    PISys_GetAPI_DataProcessors
 *
 * SYNOPSIS:
 *    const struct DPS_API *PISys_GetAPI_DataProcessors(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets access to the Data Processors System API.
 *
 * RETURNS:
 *    A pointer to the Data Processors System API.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct DPS_API *PISys_GetAPI_DataProcessors(void)
{
    return &g_DPSAPI;
}

/*******************************************************************************
 * NAME:
 *    PISys_GetAPI_FileTransferProtocol
 *
 * SYNOPSIS:
 *    const struct FTPS_API *PISys_GetAPI_FileTransferProtocol(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets access to the File Transfer System API.
 *
 * RETURNS:
 *    A pointer to the File Transfer System API.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct FTPS_API *PISys_GetAPI_FileTransferProtocol(void)
{
    return &g_FTPSAPI;
}

/*******************************************************************************
 * NAME:
 *    PISys_GetExperimentalID
 *
 * SYNOPSIS:
 *    static uint32_t PISys_GetExperimentalID(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the ID of experimental build that is running.  Plugins
 *    can use this function to check if you are running on a official release
 *    build (OFFICIAL_RELEASE = 0) or a dev build (OFFICIAL_RELEASE = 1).
 *
 *    The ID returned identifies what version of the dev build is running.
 *    The experimental ID will be 0 when running an official release instead
 *    of a dev build (in which case the plugin should not be using experimental
 *    API calls).
 *
 *    The ID is changed by 1 every time there is a plugin API change (and
 *    reset to 0 when an official is made).  The plugin can use this number
 *    to check if the API will have needed functions.  The plugin must
 *    check this number and the main version to know what API functions are
 *    available).
 *
 *    So for example:
 *    struct MainAPI
 *    {
 *        // Available in version 1.0
 *        int (*DoThing)(void);
 *        // Available in version 1.1
 *        int (*DoMore)(void);
 *        // New API being worked on and will become available in 1.2
 *        int (*DoLess)(void);
 *    }
 *    int ExperimentalID=2;
 *
 *    In this case version 1.0 had DoThing() as it's API function, when
 *    version 1.1 was released DoMore() was added, and now we are working on
 *    the next version.  While we where working on it DoLess() was added
 *    and plugins where made that used it.
 *
 *    Then new functions are added.  The ExperimentalID is inc'ed every time
 *    there is a change:
 *    struct MainAPI
 *    {
 *        // Available in version 1.0
 *        int (*DoThing)(void);
 *        // Available in version 1.1
 *        int (*DoMore)(void);
 *        // New API being worked on and will become available in 1.2
 *        int (*DoLess)(void);
 *        int (*DoIt)(void);
 *        int (*DoNothing)(void);
 *        int (*JustDoIt)(void);
 *    }
 *    int ExperimentalID=5;
 *
 *    This then continues, but at some point there is a need to break the old
 *    API.  At this point the ExperimentalID is reset back to 1 to invalidate
 *    the API (you can also invalid it back to the last working version).
 *    For example:
 *    struct MainAPI
 *    {
 *        // Available in version 1.0
 *        int (*DoThing)(void);
 *        // Available in version 1.1
 *        int (*DoMore)(void);
 *        // New API being worked on and will become available in 1.2
 *        int (*DoLess)(void);
 *        int (*DoIt)(int ABC);
 *        int (*DoNothing)(void);
 *        int (*JustDoIt)(void);
 *    }
 *    int ExperimentalID=2;
 *
 *    Dev plugins should check if the experimental ID is bigger than the
 *    version they expect.  If it isn't then they should abort.
 *
 *    So the plugin should have test code like:
 *      unsigned int REGISTER_PLUGIN_FUNCTION(const struct PI_SystemAPI *SysAPI,
 *              unsigned int Version)
 *      {
 *          if(Version<0x01020000)
 *              return NEEDED_MIN_API_VERSION;
 *
 *          if(SysAPI->GetExperimentalID()>0)
 *          {
 *              // We are running an experimental build.  Check if we can
 *              // work with this API
 *              if(SysAPI->GetExperimentalID()<5)
 *              {
 *                  // Return a fail without including the needed version number
 *                  return 0xFFFFFFFF;
 *              }
 *          }
 *
 *          ...
 *
 *      }
 *
 * RETURNS:
 *    0 -- This is an official release version running.  You should not use
 *         unofficial API calls.
 *    >0 -- A dev version is running.  You should can call unofficial APIs as
 *          long you support correct version of them (this number matchs
 *          what you expect).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static uint32_t PISys_GetExperimentalID(void)
{
    return EXPERIMENTAL_PLUGIN_API_ID;
}

/*******************************************************************************
 * NAME:
 *    PISys_GetAPI_Scripting
 *
 * SYNOPSIS:
 *    const struct ScriptingSystem_API *PISys_GetAPI_Scripting(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets access to the Scripting System API.
 *
 * RETURNS:
 *    A pointer to the Scripting System API.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct ScriptingSystem_API *PISys_GetAPI_Scripting(void)
{
    return &g_ScriptingAPI;
}

///*******************************************************************************
// * NAME:
// *    PISys_LoadKVList
// *
// * SYNOPSIS:
// *    PG_BOOL PISys_LoadKVList(int StorageArea,const char *Name,
// *          t_PIKVList *Handle);
// *
// * PARAMETERS:
// *    StorageArea [I] -- Where to store the list.  Supported values:
// *                          e_SysStoreLVListArea_Session -- Use the session file
// *                          e_SysStoreLVListArea_Settings -- Use the settings
// *                              file.
// *    Name [I] -- 
// *
// * FUNCTION:
// *    This function loads a LVList from an area.
// *
// * RETURNS:
// *    true -- Data was found and loaded
// *    false -- The data was not found or there was an error
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
//PG_BOOL PISys_LoadKVList(int StorageArea,const char *Name,t_PIKVList *Handle)
//{
//}
//
///*******************************************************************************
// * NAME:
// *    PISys_SaveKVList
// *
// * SYNOPSIS:
// *    PG_BOOL PISys_SaveKVList(int StorageArea,const char *Name,
// *              const t_PIKVList *Handle)
// *
// * PARAMETERS:
// *    StorageArea [I] -- What storage are to save into.
// *    Name [I] -- 
// *
// * FUNCTION:
// *    
// *
// * RETURNS:
// *    
// *
// * NOTES:
// *    
// *
// * LIMITATIONS:
// *    
// *
// * EXAMPLE:
// *    
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
//PG_BOOL PISys_SaveKVList(int StorageArea,const char *Name,
//        const t_PIKVList *Handle)
//{
//}
//
