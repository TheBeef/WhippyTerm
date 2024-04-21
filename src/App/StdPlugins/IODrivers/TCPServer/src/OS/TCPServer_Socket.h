/*******************************************************************************
 * FILENAME: TCPServer_Socket.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    OS independant header
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (01 Jul 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __TCPSERVER_SOCKET_H_
#define __TCPSERVER_SOCKET_H_

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
t_DriverIOHandleType *TCPServer_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
void TCPServer_FreeHandle(t_DriverIOHandleType *DriverIO);
PG_BOOL TCPServer_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
int TCPServer_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
int TCPServer_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes);
void TCPServer_Close(t_DriverIOHandleType *DriverIO);
PG_BOOL TCPServer_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options);
bool TCPServer_OSSupports_ReusePort(void);
#endif
