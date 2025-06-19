/*******************************************************************************
 * FILENAME: FileTransferProtocol.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the plug in support for file transfers (upload / download)
 *    in it.
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
#ifndef __FILETRANSFER_H_
#define __FILETRANSFER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginTypes.h"
#include "PluginUI.h"
#include <stdint.h>

/***  DEFINES                          ***/
/* Versions of struct FTPHandlerInfo */
#define FILE_TRANSFER_HANDLER_API_VERSION_1             1

/* Versions of struct FTPS_API */
#define FTPS_API_VERSION_1                              1

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_FileTransferProtocolMode_Upload,
    e_FileTransferProtocolMode_Download,
    e_FileTransferProtocolModeMAX
} e_FileTransferProtocolModeType;

struct FTPHandlerData {int AllPrivate;};  // Fake type holder
typedef struct FTPHandlerData t_FTPHandlerDataType;

typedef struct FTPOptionsWidgets {int PrivateDataHere;} t_FTPOptionsWidgetsType;    // Fake type holder

typedef struct FTPSystemData {int PrivateDataHere;} t_FTPSystemData;    // Fake type holder

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct FileTransferHandlerAPI
{
    /********* Start of FILE_TRANSFER_HANDLER_API_VERSION_1 *********/
    t_FTPHandlerDataType *(*AllocateData)(void);
    void (*FreeData)(t_FTPHandlerDataType *DataHandle);
    t_FTPOptionsWidgetsType *(*AllocOptionsWidgets)(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Options);
    void (*FreeOptionsWidgets)(t_FTPOptionsWidgetsType *FTPOptions);
    void (*StoreOptions)(t_FTPOptionsWidgetsType *FTPOptions,t_PIKVList *Options);
    PG_BOOL (*StartUpload)(t_FTPSystemData *SysHandle,t_FTPHandlerDataType *DataHandle,const char *FilenameWithPath,const char *FilenameOnly,uint64_t FileSize,t_PIKVList *Options);
    PG_BOOL (*StartDownload)(t_FTPSystemData *SysHandle,t_FTPHandlerDataType *DataHandle,t_PIKVList *Options);
    void (*AbortTransfer)(t_FTPSystemData *SysHandle,t_FTPHandlerDataType *DataHandle);
    void (*Timeout)(t_FTPSystemData *SysHandle,t_FTPHandlerDataType *DataHandle);
    PG_BOOL (*RxData)(t_FTPSystemData *SysHandle,t_FTPHandlerDataType *DataHandle,uint8_t *Data,uint32_t Bytes);
    /********* End of FILE_TRANSFER_HANDLER_API_VERSION_1 *********/
};

struct FTPHandlerInfo
{
    const char *IDStr;
    const char *DisplayName;
    const char *Tip;
    const char *Help;
    uint8_t FileTransferHandlerAPIVersion;          /* The version of struct FileTransferHandlerAPI your FTP handler supports */
    uint8_t FTPS_APIVersion;                        /* The version of struct FTPS_API your FTP handler expects */
    const struct FileTransferHandlerAPI *API;
    e_FileTransferProtocolModeType Mode;
};

typedef enum
{
    e_FTPS_SendDataRet_Success,
    e_FTPS_SendDataRet_Fail,
    e_FTPS_SendDataRet_Busy,
    e_FTPS_SendDataRetMAX
} e_FTPS_SendDataRetType;

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct FTPS_API
{
    /********* Start of FTPS_API_VERSION_1 *********/
    PG_BOOL (*RegisterFileTransferProtocol)(const struct FTPHandlerInfo *Info);
    const struct PI_UIAPI *(*GetAPI_UI)(void);
    void (*SetTimeout)(t_FTPSystemData *SysHandle,uint32_t MSec);
    void (*RestartTimeout)(t_FTPSystemData *SysHandle);
    void (*ULProgress)(t_FTPSystemData *SysHandle,uint64_t BytesTransfered);
    void (*ULFinish)(t_FTPSystemData *SysHandle,PG_BOOL Aborted);
    int (*ULSendData)(t_FTPSystemData *SysHandle,void *Data,uint32_t Bytes);
    void (*DLProgress)(t_FTPSystemData *SysHandle,uint64_t BytesTransfered);
    void (*DLFinish)(t_FTPSystemData *SysHandle,PG_BOOL Aborted);
    int (*DLSendData)(t_FTPSystemData *SysHandle,void *Data,uint32_t Bytes);
    const char *(*GetDownloadFilename)(t_FTPSystemData *SysHandle,const char *FileNameHint);
    /********* End of FTPS_API_VERSION_1 *********/
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
