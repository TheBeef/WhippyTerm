/*******************************************************************************
 * FILENAME: FileTransferProtocolSystem.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (16 Mar 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __FILETRANSFERPROTOCOLSYSTEM_H_
#define __FILETRANSFERPROTOCOLSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"
#include "UI/UIControl.h"
#include "App/Util/KeyValue.h"
#include <stdint.h>
#include <list>
#include <vector>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef struct PrivateProtocolOptionsData {int PrivateDataHere;} t_ProtocolOptionsDataType;    // Fake type holder
typedef struct FTPData {int PrivateDataHere;} t_FTPData;    // Fake type holder

struct FTPS_ProtocolInfo
{
    const char *IDStr;
    const char *DisplayName;
    const char *Tip;
    const char *Help;
};
typedef std::vector<struct FTPS_ProtocolInfo> t_FTPS_ProtocolInfoType;
typedef t_FTPS_ProtocolInfoType::iterator i_FTPS_ProtocolInfoType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern struct FTPS_API g_FTPSAPI;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void FTPS_Init(void);
t_FTPData *FTPS_AllocFTPData(void);
void FTPS_FreeFTPData(t_FTPData *FData);
void FTPS_GetListOfFTProtocols(e_FileTransferProtocolModeType Mode,
        t_FTPS_ProtocolInfoType &RetData);
t_ProtocolOptionsDataType *FTPS_AllocProtocolOptions(
        const char *ProtocolStrID,t_UILayoutContainerCtrl *ContainerWidget,
        t_KVList &OptionsKeyValues);
void FTPS_FreeProtocolOptions(t_ProtocolOptionsDataType *OptionsHandle);
void FTPS_StoreOptions(t_ProtocolOptionsDataType *OptionsHandle,
        t_KVList &OptionsKeyValues);
bool FTPS_UploadFile(t_FTPData *FData,class Connection *ParentCon,
        const char *Filename,const char *ProtocolID,t_KVList &Options,
        uint64_t FileSize);
bool FTPS_DownloadFile(t_FTPData *FData,class Connection *ParentCon,
        const char *ProtocolID,t_KVList &Options);
void FTPS_AbortTransfer(t_FTPData *FData);
void FTPS_TimeoutTransfer(t_FTPData *FData);
bool FTPS_ProcessIncomingBytes(t_FTPData *FData,uint8_t *Data,int Bytes);
void FTPS_InformOfNewPluginInstalled(const char *PluginIDStr);
void FTPS_InformOfPluginUninstalled(const char *PluginIDStr);

#endif
