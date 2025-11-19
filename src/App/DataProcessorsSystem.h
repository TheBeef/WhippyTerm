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
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __DATAPROCESSORSSYSTEM_H_
#define __DATAPROCESSORSSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Util/KeyValue.h"
#include "PluginSDK/Plugin.h"
#include "UI/UIControl.h"
#include <string>
#include <vector>
#include <list>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct DPS_ProInfo
{
    const char *IDStr;
    const char *DisplayName;
    const char *Tip;
    const char *Help;
    const struct DataProcessor *ProcessorData;
};
typedef std::vector<struct DPS_ProInfo> t_DPS_ProInfoType;

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
    class ConSettings *Settings;
};

typedef enum
{
    e_DataProPlugSettingsFn_SetCurrentTabName,
    e_DataProPlugSettingsFn_AddNewTab,
    e_DataProPlugSettingsFnMAX
} e_DataProPlugSettingsFnType;

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
void DPS_GetListOfTextProcessors(e_TextDataProcessorClassType TxtClass,
        t_DPS_ProInfoType &RetData);
void DPS_GetListOfBinaryProcessors(e_BinaryDataProcessorClassType BinClass,
        t_DPS_ProInfoType &RetData);
void DPS_GetDataProcessorPluginList(t_DPS_ProInfoType &RetData);
uint_fast32_t DPS_GetDataProcessorPluginCount(void);
bool DPS_ProcessorKeyPress(struct ProcessorConData *FData,const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,uint8_t Mod);
void DPS_ProcessorIncomingBytes(struct ProcessorConData *FData,
        const uint8_t *inbuff,int bytes,bool DoAutoLF,bool DoAutoCR);
void DPS_ProcessorOutGoingBytes(struct ProcessorConData *FData,const uint8_t *outbuff,int bytes);
const struct DataProcessor *DPS_GetProcessorsInfo(const char *IDStr);

bool DPS_DoesPluginHaveSettings(const char *IDStr);
void DPS_PluginSettings_SetActiveCtrls(void *(*GuiCtrl)(e_DataProPlugSettingsFnType Fn,void *Arg1,void *Arg2));
t_DataProSettingsWidgetsType *DPS_PluginSettings_AddWidgets(class ConSettings *Settings,const char *IDStr,t_UILayoutContainerCtrl *Cont);
void DPS_PluginSettings_FreeWidgets(const char *IDStr,t_DataProSettingsWidgetsType *PrivData);
void DPS_PluginSettings_SetSettingsFromWidgets(class ConSettings *Settings,const char *IDStr,t_DataProSettingsWidgetsType *PrivData);
void DPS_PrunePluginSettings(class ConSettings *Settings);
void DPS_PluginSettings_Load(class ConSettings *Settings,const char *IDStr);
void DPS_InformOfNewPluginInstalled(const char *PluginIDStr);
void DPS_InformOfPluginUninstalled(const char *PluginIDStr);

#endif
