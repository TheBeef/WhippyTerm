/*******************************************************************************
 * FILENAME: PluginSystem.h
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
 * HISTORY:
 *    Paul Hutchinson (10 Jul 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __PLUGINSYSTEM_H_
#define __PLUGINSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/
/* Versions of struct PI_SystemAPI */
#define PI_SYSTEM_API_VERSION_1             1

/***  MACROS                           ***/
#ifdef BUILT_IN_PLUGINS // defined in WhippyTerm project
 /* All this to say #define REGISTER_PLUGIN_FUNCTION REGISTER_PLUGIN_FUNCTION_PRIV_NAME ## "_" ## "RegisterPlugin"
    but in code instead of strings */
 #define REGISTER_PLUGIN_FORCE_CAT(x)       x ## _RegisterPlugin
 #define REGISTER_PLUGIN_FORCE_EXPANSION(x) REGISTER_PLUGIN_FORCE_CAT(x)
 #define REGISTER_PLUGIN_FUNCTION           REGISTER_PLUGIN_FORCE_EXPANSION(REGISTER_PLUGIN_FUNCTION_PRIV_NAME)
#else
 #define REGISTER_PLUGIN_FUNCTION RegisterPlugin
#endif

/***  TYPE DEFINITIONS                 ***/
/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct PI_SystemAPI
{
    /********* Start of PI_SYSTEM_API_VERSION_1 *********/
    const struct IOS_API *(*GetAPI_IO)(void);
    const struct DPS_API *(*GetAPI_DataProcessors)(void);
    const struct FTPS_API *(*GetAPI_FileTransfersProtocol)(void);
    void (*KVClear)(t_PIKVList *Handle);
    PG_BOOL (*KVAddItem)(t_PIKVList *Handle,const char *Key,const char *Value);
    const char *(*KVGetItem)(const t_PIKVList *Handle,const char *Key);
    uint32_t (*GetExperimentalID)(void);
    /********* Start of PI_SYSTEM_API_VERSION_1 *********/
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
