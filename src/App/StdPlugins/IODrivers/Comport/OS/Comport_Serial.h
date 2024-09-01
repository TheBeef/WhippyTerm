/*******************************************************************************
 * FILENAME: Comport_Serial.h
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
#ifndef __COMPORT_SERIAL_H_
#define __COMPORT_SERIAL_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"
#include <list>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct ComportInfo
{
    std::string DriverName;
    std::string FullName;
    std::string ShortName;
};

typedef std::list<struct ComportInfo> t_OSComportListType;
typedef t_OSComportListType::iterator i_OSComportListType;
struct OSComportHandle {int x;};
typedef struct OSComportHandle t_OSComportHandle;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool Comport_OS_GetSerialPortList(t_OSComportListType &List);
bool Comport_OS_SerialPortBusy(const std::string &DriverName);
t_DriverIOHandleType *Comport_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
void Comport_FreeHandle(t_DriverIOHandleType *DriverIO);
PG_BOOL Comport_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
void Comport_Close(t_DriverIOHandleType *DriverIO);
int Comport_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
int Comport_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
PG_BOOL Comport_ChangeOptions(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
PG_BOOL Comport_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL Comport_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
PG_BOOL Comport_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);
t_ConnectionWidgetsType *Comport_ConnectionAuxCtrlWidgets_AllocWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle);
void Comport_ConnectionAuxCtrlWidgets_FreeWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConAuxCtrls);
void Comport_UpdateDTR(t_DriverIOHandleType *DriverIO,bool DTR);
void Comport_UpdateRTS(t_DriverIOHandleType *DriverIO,bool RTS);
void Comport_SendBreak(t_DriverIOHandleType *DriverIO);
bool Comport_ReadAuxDTRCheckbox(struct Comport_ConAuxWidgets *ConAuxWidgets);
bool Comport_ReadAuxRTSCheckbox(struct Comport_ConAuxWidgets *ConAuxWidgets);
void Comport_AddLogMsg(struct Comport_ConAuxWidgets *ConAuxWidgets,const char *Msg);

#endif
