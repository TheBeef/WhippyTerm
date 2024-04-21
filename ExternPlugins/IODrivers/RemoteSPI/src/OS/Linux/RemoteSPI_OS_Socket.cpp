/*******************************************************************************
 * FILENAME: RemoteSPI_OS_Socket.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the linux version of sockets in it
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
 *    Paul Hutchinson (20 Jun 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "../RemoteSPI_Socket.h"
#include "../../RemoteSPI_Main.h"
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
struct RemoteSPI_OSDriverData
{
    t_IOSystemHandle *IOHandle;
    int SockFD;
    struct sockaddr_in serv_addr;
    pthread_t ThreadInfo;
    volatile bool RequestThreadQuit;
    volatile bool ThreadHasQuit;
    volatile bool Opened;
    volatile bool ForceDataAvailable;
};

/*** FUNCTION PROTOTYPES      ***/
static void *RemoteSPI_OS_PollThread(void *arg);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RemoteSPI_OSAllocateHandle
 *
 * SYNOPSIS:
 *    struct RemoteSPI_OSDriverData *RemoteSPI_OSAllocateHandle(const char *DeviceUniqueID,
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
 *    The second is the struct RemoteSPI_OSDriverData.  This is a handle is allocated
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
struct RemoteSPI_OSDriverData *RemoteSPI_OSAllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct RemoteSPI_OSDriverData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct RemoteSPI_OSDriverData;
        NewData->IOHandle=IOHandle;
        NewData->SockFD=-1;
        NewData->RequestThreadQuit=false;
        NewData->ThreadHasQuit=false;
        NewData->Opened=false;
        NewData->ForceDataAvailable=false;

        /* Startup the thread for polling if we have data available */
        if(pthread_create(&NewData->ThreadInfo,NULL,RemoteSPI_OS_PollThread,
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

    return (struct RemoteSPI_OSDriverData *)NewData;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_OSFreeHandle
 *
 * SYNOPSIS:
 *    void RemoteSPI_OSFreeHandle(struct RemoteSPI_OSDriverData *DriverIO);
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
void RemoteSPI_OSFreeHandle(struct RemoteSPI_OSDriverData *DriverIO)
{
    /* Tell thread to quit */
    DriverIO->RequestThreadQuit=true;

    /* Wait for the thread to exit */
    while(!DriverIO->ThreadHasQuit)
        usleep(1000);   // Wait 1 ms

    if(DriverIO->SockFD>=0)
        close(DriverIO->SockFD);

    delete DriverIO;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_OSOpen
 *
 * SYNOPSIS:
 *    bool RemoteSPI_OSOpen(struct RemoteSPI_OSDriverData *DriverIO,
 *              const t_PIKVList *Options);
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
 *    RemoteSPI_OSClose(), RemoteSPI_OSRead(), RemoteSPI_OSWrite()
 ******************************************************************************/
bool RemoteSPI_OSOpen(struct RemoteSPI_OSDriverData *DriverIO,const t_PIKVList *Options)
{
    struct hostent *host;
    const char *AddressStr;
    const char *PortStr;
    uint16_t Port;

    AddressStr=g_RSPI_System->KVGetItem(Options,"Address");
    PortStr=g_RSPI_System->KVGetItem(Options,"Port");
    if(AddressStr==NULL || PortStr==NULL)
        return false;

    Port=strtoul(PortStr,NULL,10);

    host=gethostbyname(AddressStr);
    if(host==NULL)
    {
        return false;
    }

    if((DriverIO->SockFD=socket(AF_INET,SOCK_STREAM,0))<0)
        return false;

    memset(&DriverIO->serv_addr,'0',sizeof(DriverIO->serv_addr));
    DriverIO->serv_addr.sin_family = AF_INET;
    DriverIO->serv_addr.sin_port = htons(Port);
    DriverIO->serv_addr.sin_addr.s_addr = *(long *)(host->h_addr);
    memset(&(DriverIO->serv_addr.sin_zero), 0, 8);

    if(connect(DriverIO->SockFD,(struct sockaddr *)&DriverIO->serv_addr,
            sizeof(struct sockaddr)) == -1)
    {
        close(DriverIO->SockFD);
        DriverIO->SockFD=-1;
        return false;
    }

    DriverIO->Opened=true;

    g_RSPI_IOSystem->DrvDataEvent(DriverIO->IOHandle,e_DataEventCode_Connected);

    return true;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_OSClose
 *
 * SYNOPSIS:
 *    void RemoteSPI_OSClose(struct RemoteSPI_OSDriverData *DriverIO);
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
 *    RemoteSPI_Open()
 ******************************************************************************/
void RemoteSPI_OSClose(struct RemoteSPI_OSDriverData *DriverIO)
{
    if(DriverIO->SockFD>=0)
        close(DriverIO->SockFD);
    DriverIO->SockFD=-1;

    DriverIO->Opened=false;

    g_RSPI_IOSystem->DrvDataEvent(DriverIO->IOHandle,
            e_DataEventCode_Disconnected);
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_OSRead
 *
 * SYNOPSIS:
 *    int RemoteSPI_OSRead(struct RemoteSPI_OSDriverData *DriverIO,uint8_t *Data,int Bytes);
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
 *    RemoteSPI_OSOpen(), RemoteSPI_OSWrite()
 ******************************************************************************/
int RemoteSPI_OSRead(struct RemoteSPI_OSDriverData *DriverIO,uint8_t *Data,int MaxBytes)
{
    int Byte2Ret;
    struct timeval tv;
    fd_set fds;

    if(DriverIO->SockFD<0)
        return RETERROR_IOERROR;

    FD_ZERO(&fds);
    FD_SET(DriverIO->SockFD,&fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    Byte2Ret=RETERROR_NOBYTES;

    /* Make sure we have at least 1 byte waiting */
    if(select(DriverIO->SockFD+1,&fds,NULL,NULL,&tv)!=0)
    {
        if(FD_ISSET(DriverIO->SockFD,&fds))
        {
            Byte2Ret=read(DriverIO->SockFD,Data,MaxBytes);
            if(Byte2Ret<0)
                return RETERROR_IOERROR;
            if(Byte2Ret==0)
            {
                /* 0=connection closed (because we where already told there
                   was data) */
                if(DriverIO->SockFD>=0)
                    close(DriverIO->SockFD);
                DriverIO->SockFD=-1;
                DriverIO->Opened=false;
                g_RSPI_IOSystem->DrvDataEvent(DriverIO->IOHandle,
                        e_DataEventCode_Disconnected);
                return RETERROR_DISCONNECT;
            }
        }
    }

    return Byte2Ret;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_OSWrite
 *
 * SYNOPSIS:
 *    int RemoteSPI_OSWrite(struct RemoteSPI_OSDriverData *DriverIO,const uint8_t *Data,int Bytes);
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
 *    RemoteSPI_OSOpen(), RemoteSPI_OSRead()
 ******************************************************************************/
int RemoteSPI_OSWrite(struct RemoteSPI_OSDriverData *DriverIO,const uint8_t *Data,
        int Bytes)
{
    int retVal;
    int BytesSent;
    const uint8_t *OutputPos;

    if(DriverIO->SockFD<0)
        return RETERROR_IOERROR;

    BytesSent=0;
    OutputPos=Data;
    while(BytesSent<Bytes)
    {
        /* Interestingly write might only take SOME of the data, so we loop */
        retVal=write(DriverIO->SockFD,OutputPos,Bytes-BytesSent);
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

static void *RemoteSPI_OS_PollThread(void *arg)
{
    struct RemoteSPI_OSDriverData *DriverIO=(struct RemoteSPI_OSDriverData *)arg;
    fd_set rfds;
    struct timeval tv;
    int retval;

    while(!DriverIO->RequestThreadQuit)
    {
        if(!DriverIO->Opened)
        {
            usleep(100000);   // Wait 100ms
            continue;
        }

        FD_ZERO(&rfds);
        FD_SET(DriverIO->SockFD,&rfds);

        /* 1ms timeout */
        tv.tv_sec=0;
        tv.tv_usec=100000;

        if(DriverIO->ForceDataAvailable)
            retval=1;
        else
            retval=select(DriverIO->SockFD+1,&rfds,NULL,NULL,&tv);

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
            g_RSPI_IOSystem->DrvDataEvent(DriverIO->IOHandle,
                    e_DataEventCode_BytesAvailable);

            /* Give the main thead some time to read out all the bytes */
            usleep(100000);   // Wait 100ms
        }
    }

    DriverIO->ThreadHasQuit=true;

    return 0;
}

void RemoteSPI_OSForceDataAvailable(struct RemoteSPI_OSDriverData *DriverIO,
        bool Forced)
{
    DriverIO->ForceDataAvailable=Forced;
}

