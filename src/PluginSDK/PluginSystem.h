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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
    const struct IOS_API *(*GetAPI_IO)(void);
    const struct DPS_API *(*GetAPI_DataProcessors)(void);
    const struct FTPS_API *(*GetAPI_FileTransfersProtocol)(void);
    void (*KVClear)(t_PIKVList *Handle);
    PG_BOOL (*KVAddItem)(t_PIKVList *Handle,const char *Key,const char *Value);
    const char *(*KVGetItem)(const t_PIKVList *Handle,const char *Key);
    void (*WriteData)(const uint8_t *Data,int Bytes);       // DEBUG PAUL: This feels wrong, should be moved to DPS_API????
//    PG_BOOL (*LoadKVList)(int StorageArea,const char *PluginName,const char *DataName,t_PIKVList *Handle);
//    PG_BOOL (*SaveKVList)(int StorageArea,const char *PluginName,const char *DataName,const t_PIKVList *Handle);
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
