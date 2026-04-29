/*******************************************************************************
 * FILENAME: VersionCheckSystem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the version check system.  This system will connect to the
 *    web site and check if there is a new version available or not.
 *
 * COPYRIGHT:
 *    Copyright 23 Apr 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (23 Apr 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "VersionCheckSystem.h"
#include "OS/Sockets.h"
#include "OS/System.h"
#include "Version.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*** DEFINES                  ***/
#define SOURCE_WEB_URL                      "/VersionCheck/Read.php"
//#define SOURCE_WEB_SITE                     "whippyterm.com"
//#define SOURCE_WEB_SITE_PORT                80
//#define SOURCE_WEB_SITE                     "localhost"
#define SOURCE_WEB_SITE                     "example.com"
#define SOURCE_WEB_SITE_PORT                10008

#define MAXREPLYBUFFSIZE                    16000   /* We expect the reply from the server to be less than 16k.  If it's longer then we can't check the version (a typical size is more like 500 bytes) */
#define MAX_WAIT_FOR_DATA_TIMEOUT           5       /* We wait up to 5 seconds to get a reply from the server */

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
struct OSSocket *m_VCheckSocket;
static int8_t m_Maj,m_Min,m_Rev,m_Patch;

/*******************************************************************************
 * NAME:
 *    InitVersionCheckSystem
 *
 * SYNOPSIS:
 *    void InitVersionCheckSystem(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the version check system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void InitVersionCheckSystem(void)
{
//    uint8_t Major,Minor,Rev,Patch;
//
//    if(OpenConnection2WebSite())
//    {
//        if(ReadLatestVersionFromWebSite())
//        {
//            CheckLatestVersionFromWebSite(&Major,&Minor,&Rev,&Patch);
//        }
//        CloseConnection2WebSite();
//    }
}

/*******************************************************************************
 * NAME:
 *    ShutDownVersionCheckSystem
 *
 * SYNOPSIS:
 *    void ShutDownVersionCheckSystem(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shuts down the version check system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void ShutDownVersionCheckSystem(void)
{
}

/*******************************************************************************
 * NAME:
 *    OpenConnection2WebSite
 *
 * SYNOPSIS:
 *    bool OpenConnection2WebSite(volatile bool *AbortFlag);
 *
 * PARAMETERS:
 *    AbortFlag [I] -- This is a pointer to a bool that is used to abort the
 *                     connect().  Set this to false before calling
 *                     this function.  This function will then check this
 *                     var while attempting to connent to the server.  If
 *                     it is set to true then this function is aborted
 *                     and returns fail.
 *
 * FUNCTION:
 *    This function opens the connection to the web site used to check the
 *    version of the program.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error opening the website.
 *
 * NOTES:
 *    This is a blocking call
 *
 * SEE ALSO:
 *    ReadLatestVersionFromWebSite(), ReadLatestVersionFromWebSite(),
 *    CheckLatestVersionFromWebSite(), CloseConnection2WebSite()
 ******************************************************************************/
bool OpenConnection2WebSite(volatile bool *AbortFlag)
{
    m_VCheckSocket=OpenSocket(SOURCE_WEB_SITE,SOURCE_WEB_SITE_PORT,AbortFlag);
    if(m_VCheckSocket==NULL)
        return false;

    return true;
}

/*******************************************************************************
 * NAME:
 *    ReadLatestVersionFromWebSite
 *
 * SYNOPSIS:
 *    bool ReadLatestVersionFromWebSite(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sends a request out to the web site to read the lastest
 *    version of WhippyTerm.  It will only check for the lastest version of
 *    this build (Windows/Linux/etc...).
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * NOTES:
 *    You must have an open connection before calling this function.
 *
 * SEE ALSO:
 *    OpenConnection2WebSite()
 ******************************************************************************/
bool ReadLatestVersionFromWebSite(void)
{
    char buff[1000];
    int Bytes;
    int OSClassID;
    char *ReplyBuff;
    unsigned int Timeout;
    unsigned int InsertPos;
    char *Line;
    char *p;
    bool RetValue;
    char *StatusCode;
    char *HeaderKey;
    char *HeaderValue;
    std::string OS_ID;
    std::string Version;

    ReplyBuff=NULL;
    try
    {
        if(m_VCheckSocket==NULL)
            throw(0);

        ReplyBuff=(char *)malloc(MAXREPLYBUFFSIZE);
        if(ReplyBuff==NULL)
            throw(0);

        switch(RunningOS())
        {
            case e_OS_Linux:
                OSClassID=0;
            break;
            case e_OS_Windows:
                OSClassID=1;
            break;
            case e_OS_MacOSX:
                OSClassID=2;
            break;
            case e_OS_RaspberryPI:
                OSClassID=3;
            break;
            case e_OSMAX:
            default:
                throw(0);
            break;
        }

        if(!GetOS_IDStrings(OS_ID,Version))
        {
            OS_ID="unknown";
            Version="";
        }

        snprintf(buff,sizeof(buff),"GET " SOURCE_WEB_URL "?osclass=%d&os=%s&v=%s&b=%d&cr=%d&wt=%s HTTP/1.1\r\n"
                "Host: %s:%d\r\n"
                "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                "Connection: close\r\n"
                "\r\n"
                "\r\n",OSClassID,OS_ID.c_str(),Version.c_str(),RunningExeBits(),
                        0/* Current update poll rate from settings*/,WHIPPYTERM_VERSION_STR,
                        SOURCE_WEB_SITE,SOURCE_WEB_SITE_PORT);

        if(!SendSocket(m_VCheckSocket,buff,strlen(buff)))
            throw(0);

        /* Parse the reply */
        Timeout=0;
        InsertPos=0;
        for(;;)
        {
            Bytes=ReadSocket(m_VCheckSocket,&ReplyBuff[InsertPos],
                    MAXREPLYBUFFSIZE-InsertPos-1);
            if(Bytes==0)
            {
                /* 1 Second timeout */
                Timeout++;
                if(Timeout>=MAX_WAIT_FOR_DATA_TIMEOUT)
                {
                    /* Maybe the server just didn't hang up, count it as done */
                    break;
                }
            }
            else if(Bytes==-1)
            {
                /* Ok, the connection was closed, we are done */
                break;
            }
            else if(Bytes<0)
            {
                /* An error */
                throw(0);
            }
            else
            {
                /* Got data */
                InsertPos+=Bytes;
                if(InsertPos>=MAXREPLYBUFFSIZE-1)
                {
                    /* To big of reply */
                    throw(0);
                }
            }
        }
        ReplyBuff[InsertPos++]=0;

        /* First line is the status code */
        Line=ReplyBuff;
        p=Line;
        StatusCode=NULL;
        while(*p!=0)
        {
            if(*p=='\r')
                *p=0;
            if(*p=='\n')
            {
                *p=0;
                break;
            }
            if(StatusCode==NULL && *p==' ')
            {
                /* Status code is after the first space */
                StatusCode=p+1;
            }
            p++;
        }
        Line=p+1;

        if(StatusCode==NULL)
            throw(0);

        if(atoi(StatusCode)!=200)
            throw(0);

        /* Parse the headers */
        while(*Line!=0)
        {
            HeaderKey=Line;
            HeaderValue=NULL;
            p=Line;
            while(*p!=0)
            {
                if(*p=='\r')
                    *p=0;
                if(*p=='\n')
                {
                    *p=0;
                    break;
                }
                if(HeaderValue==NULL && *p==':')
                {
                    HeaderValue=p+1;
                    *p=0;
                }
                p++;
            }

            /* Ok we have the header line */
            if(*Line==0)
            {
                /* Blank line */
                /* We are done with the headers */
                Line=p+1;
                break;
            }
            else
            {
                /* Check this header */
                /* We don't actually use the headers so... */
            }
            /* Move to the next line */
            Line=p+1;
        }

        /* Body */
        /* The body should just be a version number */
        p=Line;
        while(*p==' ')
            p++;
        m_Maj=-1;
        m_Min=-1;
        m_Rev=-1;
        m_Patch=-1;
        m_Maj=strtoul(p,&p,10);
        if(*p=='.')
        {
            m_Min=strtoul(p+1,&p,10);
            if(*p=='.')
            {
                m_Rev=strtoul(p+1,&p,10);
                if(*p=='.')
                {
                    m_Patch=strtoul(p+1,&p,10);
                }
            }
        }

        if(m_Maj==-1 || m_Min==-1 || m_Rev==-1 || m_Patch==-1)
            throw(0);

        RetValue=true;
    }
    catch(...)
    {
        RetValue=false;
    }

    free(ReplyBuff);

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    CheckLatestVersionFromWebSite
 *
 * SYNOPSIS:
 *    bool CheckLatestVersionFromWebSite(uint8_t *Major,uint8_t *Minor,
 *              uint8_t *Rev,uint8_t *Patch);
 *
 * PARAMETERS:
 *    Major [O] -- The major part of the lastest version
 *    Minor [O] -- The minor part of the lastest version
 *    Rev [O] -- The rev part of the lastest version
 *    Patch [O] -- The patch part of the lastest version
 *
 * FUNCTION:
 *    This function checks the result from ReadLatestVersionFromWebSite() and
 *    compares it to the current version running.
 *
 * RETURNS:
 *    true -- A new version is available
 *    false -- The currently running version is the latest (or at least
 *             newer than the version available on the web site).
 *
 * NOTES:
 *    ReadLatestVersionFromWebSite() must have been called before this function
 *    is called.
 *
 * SEE ALSO:
 *    Version.h for Maj/Min/Rev/Patch info, OpenConnection2WebSite()
 ******************************************************************************/
bool CheckLatestVersionFromWebSite(uint8_t *Major,uint8_t *Minor,uint8_t *Rev,
        uint8_t *Patch)
{
    uint32_t FullVer;

    *Major=m_Maj;
    *Minor=m_Min;
    *Rev=m_Rev;
    *Patch=m_Patch;

    FullVer=(m_Maj<<24)|(m_Min<<16)|(m_Rev<<8)|(m_Patch);

    if(FullVer>WHIPPYTERM_VERSION)
        return true;
    return false;
}

/*******************************************************************************
 * NAME:
 *    CloseConnection2WebSite
 *
 * SYNOPSIS:
 *    void CloseConnection2WebSite(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function closes the connection to the web site.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    OpenConnection2WebSite()
 ******************************************************************************/
void CloseConnection2WebSite(void)
{
    if(m_VCheckSocket!=NULL)
        CloseSocket(m_VCheckSocket);
    m_VCheckSocket=NULL;
}

