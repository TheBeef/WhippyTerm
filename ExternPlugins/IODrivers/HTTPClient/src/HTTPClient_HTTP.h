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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
