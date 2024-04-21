/*******************************************************************************
 * FILENAME: UDPClient_Socket.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the common header file for the socket sub systems of the
 *    UDP client.  There are different versions for different OS's.
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
#ifndef __UDPCLIENT_SOCKET_H_
#define __UDPCLIENT_SOCKET_H_

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
t_DriverIOHandleType *UDPClient_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
void UDPClient_FreeHandle(t_DriverIOHandleType *DriverIO);
PG_BOOL UDPClient_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
int UDPClient_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
int UDPClient_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes);
void UDPClient_Close(t_DriverIOHandleType *DriverIO);
PG_BOOL UDPClient_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options);
int UDPClient_Transmit(t_DriverIOHandleType *DriverIO);

#endif
