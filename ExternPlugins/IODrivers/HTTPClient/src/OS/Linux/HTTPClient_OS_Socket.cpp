/*******************************************************************************
 * FILENAME: HTTPClient_OS_Socket.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the Linux version of the socket IO in it.
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
#include "../HTTPClient_Socket.h"
#include "../../HTTPClient_Main.h"
#include "../../HTTPClient_HTTP.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct HTTPClient_OurData
{
    struct HTTPData HTTPState;
    t_IOSystemHandle *IOHandle;
    int SockFD;
    struct sockaddr_in serv_addr;
    pthread_t ThreadInfo;
    volatile bool RequestThreadQuit;
    volatile bool ThreadHasQuit;
    volatile bool Opened;
};

/*** FUNCTION PROTOTYPES      ***/
static void *HTTPClient_OS_PollThread(void *arg);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    HTTPClient_AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *HTTPClient_AllocateHandle(const char *DeviceUniqueID,
 *          t_IOSystemHandle *IOHandle);
 *
 * PARAMETERS:
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    IOHandle [I] -- A handle to the IO system.  This is used when talking
 *                    the IO system (just store it and pass it went needed).
 *
 * FUNCTION:
 *    This function allocates a connection to the device.
 *
 *    The system uses two different objects to talk in/out of a device driver.
 *
 *    The first is the Detect Device ID.  This is used to assign a value to
 *    all the devices detected on the system.  This is just a number (that
 *    is big enough to store a pointer).  The might be the comport number
 *    (COM1 and COM2 might be 1,2) or a pointer to a string with the
 *    path to the OS device driver (or really anything the device driver wants
 *    to use).
 *    This is used by the system to know what device out of all the available
 *    devices it is talk about.
 *    These may be allocated when the system wants a list of devices, or it
 *    may not be allocated at all.
 *
 *    The second is the t_DriverIOHandleType.  This is a handle is allocated
 *    when the user opens a new connection (new tab).  It contains any needed
 *    data for a connection.  This is not the same as opening the connection
 *    (which actually opens the device with the OS).
 *    This may include things like allocating buffers, a place to store the
 *    handle returned when the driver actually opens the device with the OS
 *    and anything else the driver needs.
 *
 * RETURNS:
 *    Newly allocated data for this connection or NULL on error.
 *
 * SEE ALSO:
 *    ConvertConnectionUniqueID2DriverID(), DetectNextConnection(),
 *    FreeHandle()
 ******************************************************************************/
t_DriverIOHandleType *HTTPClient_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct HTTPClient_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct HTTPClient_OurData;
        NewData->IOHandle=IOHandle;
        NewData->SockFD=-1;
        NewData->RequestThreadQuit=false;
        NewData->ThreadHasQuit=false;
        NewData->Opened=false;

        /* Startup the thread for polling if we have data available */
        if(pthread_create(&NewData->ThreadInfo,NULL,HTTPClient_OS_PollThread,
                NewData)<0)
        {
            throw(0);
        }

    }
    catch(...)
    {
        if(NewData!=NULL)
            delete NewData;
        return NULL;
    }

    return (t_DriverIOHandleType *)NewData;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_FreeHandle
 *
 * SYNOPSIS:
 *    void HTTPClient_FreeHandle(t_DriverIOHandleType *DriverIO);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *
 * FUNCTION:
 *    This function frees the data allocated with AllocateHandle().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocateHandle()
 ******************************************************************************/
void HTTPClient_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct HTTPClient_OurData *OurData=(struct HTTPClient_OurData *)DriverIO;

    /* Tell thread to quit */
    OurData->RequestThreadQuit=true;

    /* Wait for the thread to exit */
    while(!OurData->ThreadHasQuit)
        usleep(1000);   // Wait 1 ms

    if(OurData->SockFD>=0)
        close(OurData->SockFD);

    delete OurData;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_Open
 *
 * SYNOPSIS:
 *    PG_BOOL HTTPClient_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Options [I] -- The options to apply to this connection.
 *
 * FUNCTION:
 *    This function opens the OS device.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    HTTPClient_Close(), HTTPClient_Read(), HTTPClient_Write(),
 *    HTTPClient_ChangeOptions()
 ******************************************************************************/
PG_BOOL HTTPClient_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct HTTPClient_OurData *OurData=(struct HTTPClient_OurData *)DriverIO;
    struct hostent *host;
//    int flags;
    const char *AddressStr;
    const char *PortStr;
    uint16_t Port;

    AddressStr=g_HC_System->KVGetItem(Options,"Address");
    PortStr=g_HC_System->KVGetItem(Options,"Port");
    if(AddressStr==NULL || PortStr==NULL)
        return false;

    Port=strtoul(PortStr,NULL,10);

    host=gethostbyname(AddressStr);
    if(host==NULL)
    {
        return false;
    }

    if((OurData->SockFD=socket(AF_INET,SOCK_STREAM,0))<0)
        return false;

//    /* Switch to nonblocking */
//    flags=fcntl(OurData->SockFD,F_GETFL,0);
//    if(flags<0)
//        flags=0;
//    flags|=O_NONBLOCK;
//    fcntl(OurData->SockFD,F_SETFL,flags);

    memset(&OurData->serv_addr,'0',sizeof(OurData->serv_addr));
    OurData->serv_addr.sin_family = AF_INET;
    OurData->serv_addr.sin_port = htons(Port);
    OurData->serv_addr.sin_addr.s_addr = *(long *)(host->h_addr);
    memset(&(OurData->serv_addr.sin_zero), 0, 8);

    if(connect(OurData->SockFD,(struct sockaddr *)&OurData->serv_addr,
            sizeof(struct sockaddr)) == -1)
    {
        close(OurData->SockFD);
        OurData->SockFD=-1;
        return false;
    }

    OurData->Opened=true;

    if(!HTTPClient_StartHTTPHandShake(DriverIO,Options,&OurData->HTTPState))
    {
        HTTPClient_Close(DriverIO);
        return false;
    }

    g_HC_IOSystem->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);

    return true;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_Close
 *
 * SYNOPSIS:
 *    void HTTPClient_Close(t_DriverIOHandleType *DriverIO);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *
 * FUNCTION:
 *    This function closes a connection that was opened with Open()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    HTTPClient_Open()
 ******************************************************************************/
void HTTPClient_Close(t_DriverIOHandleType *DriverIO)
{
    struct HTTPClient_OurData *OurData=(struct HTTPClient_OurData *)DriverIO;

    if(OurData->SockFD>=0)
        close(OurData->SockFD);
    OurData->SockFD=-1;

    OurData->Opened=false;

    g_HC_IOSystem->DrvDataEvent(OurData->IOHandle,
            e_DataEventCode_Disconnected);
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_Read
 *
 * SYNOPSIS:
 *    int HTTPClient_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Data [I] -- A buffer to store the data that was read.
 *    Bytes [I] -- The max number of bytes that can be stored in 'Data'
 *
 * FUNCTION:
 *    This function reads data from the device and stores it in 'Data'
 *
 * RETURNS:
 *    The number of bytes that was read or:
 *      RETERROR_NOBYTES -- No bytes was read (0)
 *      RETERROR_DISCONNECT -- This device is no longer open.
 *      RETERROR_IOERROR -- There was an IO error.
 *      RETERROR_BUSY -- The device is currently busy.  Try again later
 *
 * SEE ALSO:
 *    HTTPClient_Open(), HTTPClient_Write()
 ******************************************************************************/
int HTTPClient_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    struct HTTPClient_OurData *OurData=(struct HTTPClient_OurData *)DriverIO;
    int Byte2Ret;
    int BytesLeft;
    struct timeval tv;
    fd_set fds;

    if(OurData->SockFD<0)
        return RETERROR_IOERROR;

    FD_ZERO(&fds);
    FD_SET(OurData->SockFD,&fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    Byte2Ret=RETERROR_NOBYTES;

    /* Make sure we have at least 1 byte waiting */
    if(select(OurData->SockFD+1,&fds,NULL,NULL,&tv)!=0)
    {
        if(FD_ISSET(OurData->SockFD,&fds))
        {
            Byte2Ret=read(OurData->SockFD,Data,MaxBytes);
            if(Byte2Ret<0)
                return RETERROR_IOERROR;
            if(Byte2Ret==0)
            {
                /* 0=connection closed (because we where already told there
                   was data) */
                if(OurData->SockFD>=0)
                    close(OurData->SockFD);
                OurData->SockFD=-1;
                OurData->Opened=false;
                g_HC_IOSystem->DrvDataEvent(OurData->IOHandle,
                        e_DataEventCode_Disconnected);
                return RETERROR_DISCONNECT;
            }
            else
            {
                BytesLeft=HTTPClient_ProcessHTTPHeaders(DriverIO,
                        &OurData->HTTPState,Data,Byte2Ret);
                if(BytesLeft!=Byte2Ret && BytesLeft!=0)
                {
                    /* Ok, we need to move the extra bytes down in the buffer
                       and return that number */
                    memcpy(Data,&Data[Byte2Ret-BytesLeft],BytesLeft);
                }
                Byte2Ret=BytesLeft;
            }
        }
    }

    return Byte2Ret;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_Write
 *
 * SYNOPSIS:
 *    int HTTPClient_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Data [I] -- The data to write to the device.
 *    Bytes [I] -- The number of bytes to write.
 *
 * FUNCTION:
 *    This function writes (sends) data to the device.
 *
 * RETURNS:
 *    The number of bytes written or:
 *      RETERROR_NOBYTES -- No bytes was written (0)
 *      RETERROR_DISCONNECT -- This device is no longer open.
 *      RETERROR_IOERROR -- There was an IO error.
 *      RETERROR_BUSY -- The device is currently busy.  Try again later
 *
 * SEE ALSO:
 *    HTTPClient_Open(), HTTPClient_Read()
 ******************************************************************************/
int HTTPClient_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    struct HTTPClient_OurData *OurData=(struct HTTPClient_OurData *)DriverIO;
    int retVal;
    int BytesSent;
    const uint8_t *OutputPos;

    if(OurData->SockFD<0)
        return RETERROR_IOERROR;

    BytesSent=0;
    OutputPos=Data;
    while(BytesSent<Bytes)
    {
        /* Interestingly write might only take SOME of the data, so we loop */
        retVal=write(OurData->SockFD,OutputPos,Bytes-BytesSent);
        if(retVal<0)
        {
            switch(errno)
            {
                case EAGAIN:
                case ENOBUFS:
                    return RETERROR_BUSY;
                case EBADF:
                case EBADFD:
                case ECONNABORTED:
                case ECONNREFUSED:
                case ECONNRESET:
                case EHOSTDOWN:
                case EHOSTUNREACH:
                case ENETDOWN:
                case ENETRESET:
                case ENETUNREACH:
                    return RETERROR_DISCONNECT;
                default:
                    return RETERROR_IOERROR;
            }
        }
        BytesSent+=retVal;
        OutputPos+=retVal;
    }

    return BytesSent;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL HTTPClient_ChangeOptions(t_DriverIOHandleType *DriverIO,
 *          const t_PIKVList *Options);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Options [I] -- The options to apply to this connection.
 *
 * FUNCTION:
 *    This function changes the connection options on an open device.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    Open()
 ******************************************************************************/
PG_BOOL HTTPClient_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
    struct HTTPClient_OurData *OurData=(struct HTTPClient_OurData *)DriverIO;

    /* If we are already open then we need to close and reopen */
    if(OurData->SockFD>=0)
        HTTPClient_Close(DriverIO);

    return HTTPClient_Open(DriverIO,Options);
}

static void *HTTPClient_OS_PollThread(void *arg)
{
    struct HTTPClient_OurData *OurData=(struct HTTPClient_OurData *)arg;
    fd_set rfds;
    struct timeval tv;
    int retval;

    while(!OurData->RequestThreadQuit)
    {
        if(!OurData->Opened)
        {
            usleep(100000);   // Wait 100ms
            continue;
        }

        FD_ZERO(&rfds);
        FD_SET(OurData->SockFD,&rfds);

        /* 1ms timeout */
        tv.tv_sec=0;
        tv.tv_usec=100000;

        retval=select(OurData->SockFD+1,&rfds,NULL,NULL,&tv);
        if(retval==0)
        {
            /* Timeout */
        }
        else if(retval==-1)
        {
            /* Error */
        }
        else
        {
            /* Data available */
            g_HC_IOSystem->DrvDataEvent(OurData->IOHandle,
                    e_DataEventCode_BytesAvailable);

            /* Give the main thead some time to read out all the bytes */
            usleep(100000);   // Wait 100ms
        }
    }

    OurData->ThreadHasQuit=true;

    return 0;
}
