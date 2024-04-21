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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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

#endif
