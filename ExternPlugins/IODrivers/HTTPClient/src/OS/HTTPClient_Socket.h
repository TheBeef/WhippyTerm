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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
