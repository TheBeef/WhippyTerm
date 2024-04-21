/*******************************************************************************
 * FILENAME: RemoteSPI_Socket.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the common header file for the socket sub systems of the
 *    remote SPI.  There are different versions for different OS's.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (20 Jun 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __REMOTESPI_SOCKET_H_
#define __REMOTESPI_SOCKET_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"
#include <string.h>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct RemoteSPI_OSDriverData;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
struct RemoteSPI_OSDriverData *RemoteSPI_OSAllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
void RemoteSPI_OSFreeHandle(struct RemoteSPI_OSDriverData *DriverIO);
bool RemoteSPI_OSOpen(struct RemoteSPI_OSDriverData *DriverIO,const t_PIKVList *Options);
void RemoteSPI_OSClose(struct RemoteSPI_OSDriverData *DriverIO);
int RemoteSPI_OSRead(struct RemoteSPI_OSDriverData *DriverIO,uint8_t *Data,int MaxBytes);
int RemoteSPI_OSWrite(struct RemoteSPI_OSDriverData *DriverIO,const uint8_t *Data,
        int Bytes);
void RemoteSPI_OSForceDataAvailable(struct RemoteSPI_OSDriverData *DriverIO,
        bool Forced);


#endif
