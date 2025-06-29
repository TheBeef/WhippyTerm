/*******************************************************************************
 * FILENAME: IOSystem.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
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
#ifndef __IOSYSTEM_H_
#define __IOSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"
#include "UI/UIControl.h"
#include "Util/KeyValue.h"
#include <string>
#include <vector>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct ConnectionInfoList
{
    std::string UniqueID;
    std::string Name;
    std::string Title;
    bool InUse;
    bool BlockDevice;
    struct ConnectionInfoList *Next;
};

struct DriverInfoList
{
    std::string Name;
    std::string DriverURIHelpString;
    struct DriverInfoList *Next;
};

typedef struct PrivateConnectionOptionsData {int PrivateDataHere;} t_ConnectionOptionsDataType;    // Fake type holder
typedef struct PrivateConnectionAuxCtrlsDataType {int PrivateDataHere;} t_ConnectionAuxCtrlsDataType;    // Fake type holder

typedef enum
{
    e_IOSysIOError_Success,
    e_IOSysIOError_GenericIO,
    e_IOSysIOError_Disconnect,
    e_IOSysIOError_Busy,
    e_IOSysIOErrorMAX
} e_IOSysIOErrorType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern const struct IOS_API g_IOS_API;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void IOS_Init(void);
void IOS_InitPlugins(void);
void IOS_InitNewlyInstalledPlugin(struct ExternPluginInfo *Info);
void IOS_Shutdown(void);

void IOS_ScanForConnections(void);
struct ConnectionInfoList *IOS_GetListOfAvailableConnections(void);
void IOS_FreeListOfAvailableConnections(struct ConnectionInfoList *CInfo);
bool IOS_MakeURIFromDetectedCon(struct ConnectionInfoList *CInfoEntry,
        const t_KVList &Options,std::string &URI);
struct ConnectionInfoList *IOS_FindConnectionFromDetectedID(
        struct ConnectionInfoList *CInfo,const char *UniqueID);

t_ConnectionOptionsDataType *IOS_AllocConnectionOptions(
        struct ConnectionInfoList *CInfoEntry,
        t_UIContainerCtrl *ContainerWidget,t_KVList &OptionsKeyValues,
        void (*UIChanged)(void *UserData),void *UserData);
t_ConnectionOptionsDataType *IOS_AllocConnectionOptionsFromUniqueID(
        const char *UniqueID,t_UIContainerCtrl *ContainerWidget,
        t_KVList &OptionsKeyValues,void (*UIChanged)(void *UserData),
        void *UserData);
void IOS_FreeConnectionOptions(t_ConnectionOptionsDataType *ConOptionsHandle);
void IOS_StoreConnectionOptions(t_ConnectionOptionsDataType *ConOptionsHandle,
        t_KVList &OptionsKeyValues);
void IOS_SetUI2ConnectionOptions(t_ConnectionOptionsDataType *ConOptionsHandle,
        t_KVList &OptionsKeyValues);

t_ConnectionAuxCtrlsDataType *IOS_AllocConnectionAuxCtrls(t_IOSystemHandle *Handle,t_UIContainerCtrl *ContainerWidget);
void IOS_FreeConnectionAuxCtrls(t_ConnectionAuxCtrlsDataType *ConAuxCtrlsHandle);
void IOS_ConnectionAuxCtrlsShow(t_ConnectionAuxCtrlsDataType *ConAuxCtrlsHandle,bool Show);

bool IOS_GetUniqueIDFromURI(const char *URI,std::string &UniqueID);
bool IOS_GetURIFromUniqueID(const char *UniqueID,const t_KVList &Options,
        std::string &URI);
bool IOS_GetConnectionInfo(const char *UniqueID,const t_KVList &Options,
        struct ConnectionInfoList *Info);

t_IOSystemHandle *IOS_AllocIOSystemHandle(const char *UniqueID,uintptr_t ID);
t_IOSystemHandle *IOS_AllocIOSystemHandleFromURI(const char *URI,uintptr_t ID);
void IOS_FreeIOSystemHandle(t_IOSystemHandle *Handle);

bool IOS_Open(t_IOSystemHandle *Handle);
bool IOS_SetConnectionOptions(t_IOSystemHandle *Handle,const t_KVList &Options);
void IOS_GetConnectionOptions(t_IOSystemHandle *Handle,t_KVList &Options);
e_IOSysIOErrorType IOS_WriteData(t_IOSystemHandle *Handle,const uint8_t *Data,int Bytes);
int IOS_ReadData(t_IOSystemHandle *Handle,uint8_t *Data,int MaxBytes);
void IOS_Close(t_IOSystemHandle *Handle);
void IOS_GetUniqueID(t_IOSystemHandle *Handle,std::string &UniqueID);
void IOS_InformOfNewDataEvent(t_IOSystemHandle *IOHandle);
bool IOS_GetDeviceURI(t_IOSystemHandle *Handle,std::string &URI);
bool IOS_UpdateOptionsFromURI(const char *URI,t_KVList &Options);
e_IOSysIOErrorType IOS_TransmitQueuedData(t_IOSystemHandle *Handle);
const char *IOS_GetLastErrorMessage(t_IOSystemHandle *Handle);

int IOS_Ask(const char *Message,int Type);

struct DriverInfoList *IOS_GetListOfDrivers(void);
void IOS_FreeDriverInfoList(struct DriverInfoList *List);

#endif
