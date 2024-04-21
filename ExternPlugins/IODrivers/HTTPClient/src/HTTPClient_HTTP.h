/*******************************************************************************
 * FILENAME: HTTPClient_HTTP.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file.
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
#ifndef __HTTPCLIENT_HTTP_H_
#define __HTTPCLIENT_HTTP_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "OS/HTTPClient_Socket.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct HTTPData
{
    bool DoingHeaders;
    int HeaderEndCount;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool HTTPClient_StartHTTPHandShake(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options,struct HTTPData *HTTPState);
int HTTPClient_ProcessHTTPHeaders(t_DriverIOHandleType *DriverIO,
        struct HTTPData *HTTPState,uint8_t *Data,int BytesRead);

#endif
