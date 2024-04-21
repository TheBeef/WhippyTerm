/*******************************************************************************
 * FILENAME: DataProcessorsSystem.h
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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __DATAPROCESSORSSYSTEM_H_
#define __DATAPROCESSORSSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"
#include <string>
#include <vector>
#include <list>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

struct DPS_TextProInfo
{
    const char *IDStr;
    const char *DisplayName;
    const char *Tip;
    const char *Help;
};
typedef std::vector<struct DPS_TextProInfo> t_DPS_TextProInfoType;

struct DataProcessor
{
    std::string ProID;
    struct DataProcessorAPI API;
    struct DataProcessorInfo Info;
};

typedef std::list<struct DataProcessor> t_DPSDataProcessorsType;
typedef t_DPSDataProcessorsType::iterator i_DPSDataProcessorsType;

typedef std::vector<t_DataProcessorHandleType *> t_ProcessorsDataType;
typedef t_ProcessorsDataType::iterator i_ProcessorsDataType;

struct ProcessorConData
{
    t_ProcessorsDataType ProcessorsData;
    t_DPSDataProcessorsType DataProcessorsList;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern struct DPS_API g_DPSAPI;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void DPS_Init(void);
bool DPS_AllocProcessorConData(struct ProcessorConData *FData,
        class ConSettings *CustomSettings);
void DPS_FreeProcessorConData(struct ProcessorConData *FData);
bool DPS_ReapplyProcessor2Connection(struct ProcessorConData *FData,
        class ConSettings *CustomSettings);
void DPS_GetListOfTextProcessors(e_DataProcessorClassType ProClass,
        t_DPS_TextProInfoType &RetData);
bool DPS_ProcessorKeyPress(const uint8_t *KeyChar,int KeyCharLen,
        e_UIKeys ExtendedKey,uint8_t Mod);
void DPS_ProcessorIncomingBytes(const uint8_t *inbuff,int bytes);

#endif
