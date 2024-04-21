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
