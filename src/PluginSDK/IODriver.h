/*******************************************************************************
 * FILENAME: IODriver.h
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file is part of the Plugin SDK.  It has things needed for
 *    the IOSystem (IO drivers).
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
 *    Paul Hutchinson (09 Jul 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __IODRIVER_H_
#define __IODRIVER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginUI.h"

/***  DEFINES                          ***/
/* Versions of struct IODriverAPI */
#define IODRIVER_API_VERSION_1          1
#define IODRIVER_API_VERSION_2          2


#define RETERROR_NOBYTES                0
#define RETERROR_DISCONNECT             -1
#define RETERROR_IOERROR                -2
#define RETERROR_BUSY                   -3

/* IODriverInfo.Flags */
#define IODRVINFOFLAG_BLOCKDEV          0x00000001

///* IODriverDetectedInfo.Flags */
#define IODRV_DETECTFLAG_INUSE          0x00000001

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/* !!!! You can only add to this.  Changing it will break the plugins !!!! 
   You can not change any of the sizes either */
struct IODriverDetectedInfo
{
    struct IODriverDetectedInfo *Next;
    uint32_t StructureSize;             // The size of the allocated structure.  Set to sizeof(struct IODriverDetectedInfo) in your plugin
    uint32_t Flags;
    char DeviceUniqueID[512];
    char Name[256];
    char Title[256];
};

struct DriverIOHandle {int PrivateData;};  // Fake type holder
typedef struct DriverIOHandle t_DriverIOHandleType;

typedef struct IOSystemHandle {int PrivateDataHere;} t_IOSystemHandle;  // Fake type holder

typedef enum
{
    e_DataEventCode_BytesAvailable,
    e_DataEventCode_Disconnected,
    e_DataEventCode_Connected,
    e_DataEventCodeMAX
}e_DataEventCodeType;

typedef struct DetectedDevices {int PrivateDataHere;} t_DetectedDevices;    // Fake type holder

typedef struct ConnectionWidgets {int PrivateDataHere;} t_ConnectionWidgetsType;    // Fake type holder

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct IODriverInfo
{
    uint32_t Flags;
    const char *URIHelpString;
};

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct IODriverAPI
{
    /********* Start of IODRIVER_API_VERSION_1 *********/
    PG_BOOL (*Init)(void);
    const struct IODriverInfo *(*GetDriverInfo)(unsigned int *SizeOfInfo);

    PG_BOOL (*InstallPlugin)(void);
    void (*UnInstallPlugin)(void);

    const struct IODriverDetectedInfo *(*DetectDevices)(void);
    void (*FreeDetectedDevices)(const struct IODriverDetectedInfo *Devices);
    PG_BOOL (*GetConnectionInfo)(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);

    t_ConnectionWidgetsType *(*ConnectionOptionsWidgets_AllocWidgets)(t_WidgetSysHandle *WidgetHandle);
    void (*ConnectionOptionsWidgets_FreeWidgets)(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions);
    void (*ConnectionOptionsWidgets_StoreUI)(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
    void (*ConnectionOptionsWidgets_UpdateUI)(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);

    PG_BOOL (*Convert_URI_To_Options)(const char *URI,t_PIKVList *Options,char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,PG_BOOL Update);
    PG_BOOL (*Convert_Options_To_URI)(const char *DeviceUniqueID,t_PIKVList *Options,char *URI,unsigned int MaxURILen);

    t_DriverIOHandleType *(*AllocateHandle)(const char *DeviceUniqueID,t_IOSystemHandle *IOHandle);
    void (*FreeHandle)(t_DriverIOHandleType *DriverIO);
    PG_BOOL (*Open)(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
    void (*Close)(t_DriverIOHandleType *DriverIO);
    int (*Read)(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
    int (*Write)(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
    PG_BOOL (*ChangeOptions)(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
    int (*Transmit)(t_DriverIOHandleType *DriverIO);

    t_ConnectionWidgetsType *(*ConnectionAuxCtrlWidgets_AllocWidgets)(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle);
    void (*ConnectionAuxCtrlWidgets_FreeWidgets)(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConAuxCtrls);
    /********* End of IODRIVER_API_VERSION_1 *********/
    /********* Start of IODRIVER_API_VERSION_2 *********/
    const char *(*GetLastErrorMessage)(t_DriverIOHandleType *DriverIO);
    /********* End of IODRIVER_API_VERSION_2 *********/
};

struct IOS_API
{
    PG_BOOL (*RegisterDriver)(const char *DriverName,const char *BaseURI,const struct IODriverAPI *DriverAPI,int SizeOfDriverAPI);
    const struct PI_UIAPI *(*GetAPI_UI)(void);
    void (*DrvDataEvent)(t_IOSystemHandle *IOHandle,int Code); // Really Code is 'e_DataEventCodeType Code'
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
