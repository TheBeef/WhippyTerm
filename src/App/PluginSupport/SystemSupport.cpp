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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
#include "App/PluginSupport/SystemSupport.h"
#include "App/PluginSupport/PluginUISupport.h"
#include "App/PluginSupport/KeyValueSupport.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
const struct IOS_API *PISys_GetAPI_IO(void);
const struct DPS_API *PISys_GetAPI_DataProcessors(void);
const struct FTPS_API *PISys_GetAPI_FileTransferProtocol(void);

/*** VARIABLE DEFINITIONS     ***/
const struct PI_SystemAPI g_PISystemAPI=
{
    PISys_GetAPI_IO,
    PISys_GetAPI_DataProcessors,
    PISys_GetAPI_FileTransferProtocol,
    PI_KVClear,
    PI_KVAddItem,
    PI_KVGetItem,
    Con_WriteData,
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
 *    A pointer to the Data Processors System API.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct FTPS_API *PISys_GetAPI_FileTransferProtocol(void)
{
    return &g_FTPSAPI;
}
//
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
