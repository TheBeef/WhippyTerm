/*******************************************************************************
 * FILENAME: UDPServer_Socket.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    OS independant header
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (15 May 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __UDPSERVER_SOCKET_H_
#define __UDPSERVER_SOCKET_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"
#include <string.h>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
t_DriverIOHandleType *UDPServer_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
void UDPServer_FreeHandle(t_DriverIOHandleType *DriverIO);
PG_BOOL UDPServer_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
int UDPServer_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
int UDPServer_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes);
void UDPServer_Close(t_DriverIOHandleType *DriverIO);
PG_BOOL UDPServer_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options);

bool OSSupportsReusePort(void);

#endif
