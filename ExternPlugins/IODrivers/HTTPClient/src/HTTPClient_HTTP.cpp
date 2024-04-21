/*******************************************************************************
 * FILENAME: HTTPClient_HTTP.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the common code to handle the HTTP connection.
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
 * CREATED BY:
 *    Paul Hutchinson (10 Jul 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "HTTPClient_Main.h"
#include "HTTPClient_HTTP.h"
#include "OS/HTTPClient_Socket.h"
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    HTTPClient_StartHTTPHandShake
 *
 * SYNOPSIS:
 *    bool HTTPClient_StartHTTPHandShake(t_DriverIOHandleType *DriverIO,
 *              const t_PIKVList *Options,struct HTTPData *HTTPState);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Options [I] -- The options to apply to this connection.
 *    HTTPState [I] -- State machine data for this HTTP connection.
 *
 * FUNCTION:
 *    This function starts the HTTP hand shake.  It sends the first set
 *    of headers.
 *
 * RETURNS:
 *    true -- Things worked
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HTTPClient_StartHTTPHandShake(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options,struct HTTPData *HTTPState)
{
    const char *PathStr;
    const char *AddressStr;
    const char *GenericHeaderStr;
    bool DidUserAgent;

    HTTPState->DoingHeaders=true;
    HTTPState->HeaderEndCount=0;

    DidUserAgent=false;

    AddressStr=g_HC_System->KVGetItem(Options,"Address");
    if(AddressStr==NULL)
        return false;

    PathStr=g_HC_System->KVGetItem(Options,"Path");
    if(PathStr==NULL || *PathStr==0)
        PathStr="/";

    if(HTTPClient_Write(DriverIO,(uint8_t *)"GET ",4)<=0)
        return false;

    if(HTTPClient_Write(DriverIO,(uint8_t *)PathStr,strlen(PathStr))<0)
        return false;

    if(HTTPClient_Write(DriverIO,(uint8_t *)" HTTP/1.1\r\n",11)<=0)
        return false;

    if(HTTPClient_Write(DriverIO,(uint8_t *)"Host: ",6)<=0)
        return false;

    if(HTTPClient_Write(DriverIO,(uint8_t *)AddressStr,strlen(AddressStr))<0)
        return false;

    if(HTTPClient_Write(DriverIO,(uint8_t *)"\r\n",2)<=0)
        return false;

    GenericHeaderStr=g_HC_System->KVGetItem(Options,"GenericHeader1");
    if(GenericHeaderStr!=NULL)
    {
        if(*GenericHeaderStr!=0)
        {
            if(strncmp(GenericHeaderStr,"User-Agent:",11)==0)
                DidUserAgent=true;
            if(HTTPClient_Write(DriverIO,(uint8_t *)GenericHeaderStr,
                    strlen(GenericHeaderStr))<0)
            {
                return false;
            }
            if(HTTPClient_Write(DriverIO,(uint8_t *)"\r\n",2)<=0)
                return false;
        }
    }

    GenericHeaderStr=g_HC_System->KVGetItem(Options,"GenericHeader2");
    if(GenericHeaderStr!=NULL)
    {
        if(*GenericHeaderStr!=0)
        {
            if(strncmp(GenericHeaderStr,"User-Agent:",11)==0)
                DidUserAgent=true;
            if(HTTPClient_Write(DriverIO,(uint8_t *)GenericHeaderStr,
                    strlen(GenericHeaderStr))<0)
            {
                return false;
            }
            if(HTTPClient_Write(DriverIO,(uint8_t *)"\r\n",2)<=0)
                return false;
        }
    }

    GenericHeaderStr=g_HC_System->KVGetItem(Options,"GenericHeader3");
    if(GenericHeaderStr!=NULL)
    {
        if(*GenericHeaderStr!=0)
        {
            if(strncmp(GenericHeaderStr,"User-Agent:",11)==0)
                DidUserAgent=true;
            if(HTTPClient_Write(DriverIO,(uint8_t *)GenericHeaderStr,
                    strlen(GenericHeaderStr))<0)
            {
                return false;
            }
            if(HTTPClient_Write(DriverIO,(uint8_t *)"\r\n",2)<=0)
                return false;
        }
    }

    if(!DidUserAgent)
    {
        if(HTTPClient_Write(DriverIO,(uint8_t *)"User-Agent: Whippy Term\r\n",
                25)<=0)
        {
            return false;
        }
    }

    if(HTTPClient_Write(DriverIO,(uint8_t *)"\r\n",2)<=0)
        return false;

/* Header field to consider:
Authorization
*/

    return true;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_ProcessHTTPHeaders
 *
 * SYNOPSIS:
 *    int HTTPClient_ProcessHTTPHeaders(t_DriverIOHandleType *DriverIO,
 *              struct HTTPData *HTTPState,uint8_t *Data,int BytesRead);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    HTTPState [I] -- State machine data for this HTTP connection.
 *    Data [I] -- The data that was read from the connection
 *    BytesRead [I] -- The number of bytes in 'Data'
 *
 * FUNCTION:
 *    This function is called when a block comes in from the HTTP connection.
 *    It handles the headers.
 *
 * RETURNS:
 *    Number of bytes used from the headers.  Will be 0 if we are no longer
 *    doing headers.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int HTTPClient_ProcessHTTPHeaders(t_DriverIOHandleType *DriverIO,
        struct HTTPData *HTTPState,uint8_t *Data,int BytesRead)
{
    int BytesLeft;
    const uint8_t *Pos;

    if(!HTTPState->DoingHeaders)
        return BytesRead;

    /* Process this block ignoring all headers until we get to the end of the
       headers */
    BytesLeft=BytesRead;
    Pos=Data;
    while(BytesLeft>0)
    {
        if(*Pos=='\n')
        {
            HTTPState->HeaderEndCount++;
            if(HTTPState->HeaderEndCount==2)
            {
                /* 2 new lines = end of the headers */
                HTTPState->DoingHeaders=false;
                BytesLeft--;
                break;
            }
        }
        else if(*Pos!='\r')
        {
            HTTPState->HeaderEndCount=0;
        }
        BytesLeft--;
        Pos++;
    }
    return BytesLeft;
}
