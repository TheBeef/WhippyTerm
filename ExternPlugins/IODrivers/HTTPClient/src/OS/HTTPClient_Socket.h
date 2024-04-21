/*******************************************************************************
 * FILENAME: HTTPClient_Socket.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the common header file for the socket sub systems of the
 *    TCP client.  There are different versions for different OS's.
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
 *    Paul Hutchinson (10 Jul 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __HTTPCLIENT_SOCKET_H_
#define __HTTPCLIENT_SOCKET_H_

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
t_DriverIOHandleType *HTTPClient_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
void HTTPClient_FreeHandle(t_DriverIOHandleType *DriverIO);
PG_BOOL HTTPClient_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
int HTTPClient_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
int HTTPClient_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes);
void HTTPClient_Close(t_DriverIOHandleType *DriverIO);
PG_BOOL HTTPClient_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options);

#endif
