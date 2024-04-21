/*******************************************************************************
 * FILENAME: TCPServer_OS_Socket.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the Linux version of the sockets in it.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (22 Feb 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "../TCPServer_Socket.h"
#include "../../TCPServer_Main.h"
#include <windows.h>
#include <winsock.h>
#include <stdlib.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct TCPServer_OurData
{
    t_IOSystemHandle *IOHandle;
    SOCKET ListeningSockFD;
    SOCKET DataSockFD;
    struct sockaddr_in serv_addr;
    WSADATA wsaData;
    bool wsaStarted;
    HANDLE ThreadHandle;
    volatile bool RequestThreadQuit;
    volatile bool ThreadHasQuit;
    volatile bool Opened;
    volatile bool WaitingForConnection;
};

/*** FUNCTION PROTOTYPES      ***/
static DWORD WINAPI TCPServer_OS_PollThread(LPVOID lpParameter);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    TCPServer_AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *TCPServer_AllocateHandle(const char *DeviceUniqueID,
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
t_DriverIOHandleType *TCPServer_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct TCPServer_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct TCPServer_OurData;
        NewData->IOHandle=IOHandle;
        NewData->ListeningSockFD=INVALID_SOCKET;
        NewData->DataSockFD=INVALID_SOCKET;
        NewData->wsaStarted=false;
        NewData->RequestThreadQuit=false;
        NewData->ThreadHasQuit=false;
        NewData->Opened=false;
        NewData->WaitingForConnection=false;

        if(WSAStartup(MAKEWORD(2,2),&NewData->wsaData)!=0)
            throw(0);
        NewData->wsaStarted=true;

        /* Startup the thread for polling if we have data available */
        NewData->ThreadHandle=CreateThread(NULL,0,TCPServer_OS_PollThread,
                NewData,CREATE_SUSPENDED,NULL);
        if(NewData->ThreadHandle==NULL)
            throw(0);

        /* Ok, start the thread */
        ResumeThread(NewData->ThreadHandle);
    }
    catch(...)
    {
        if(NewData!=NULL)
        {
            if(NewData->wsaStarted)
                WSACleanup();
            delete NewData;
        }
        return NULL;
    }

    return (t_DriverIOHandleType *)NewData;
}

/*******************************************************************************
 * NAME:
 *    TCPServer_FreeHandle
 *
 * SYNOPSIS:
 *    void TCPServer_FreeHandle(t_DriverIOHandleType *DriverIO);
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
void TCPServer_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct TCPServer_OurData *OurData=(struct TCPServer_OurData *)DriverIO;

    /* Tell thread to quit */
    OurData->RequestThreadQuit=true;

    /* Wait for the thread to exit */
    while(!OurData->ThreadHasQuit)
        Sleep(1);   // Wait 1 ms

    if(OurData->ListeningSockFD!=INVALID_SOCKET)
        closesocket(OurData->ListeningSockFD);
    if(OurData->DataSockFD!=INVALID_SOCKET)
        closesocket(OurData->DataSockFD);

    if(OurData->wsaStarted)
        WSACleanup();

    delete OurData;
}

/*******************************************************************************
 * NAME:
 *    TCPServer_Open
 *
 * SYNOPSIS:
 *    PG_BOOL TCPServer_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
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
 *    TCPServer_Close(), TCPServer_Read(), TCPServer_Write(),
 *    TCPServer_ChangeOptions()
 ******************************************************************************/
PG_BOOL TCPServer_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct TCPServer_OurData *OurData=(struct TCPServer_OurData *)DriverIO;
    const char *PortStr;
    bool ReuseAddressBool;
    uint16_t Port;
    int opt;
    int SetOptions;
    const char *TmpStr;

    PortStr=g_TCPS_System->KVGetItem(Options,"Port");
    if(PortStr==NULL)
        return false;
    Port=strtoul(PortStr,NULL,10);

    TmpStr=g_TCPS_System->KVGetItem(Options,"ReuseAddress");
    if(TmpStr==NULL)
        TmpStr="1";
    ReuseAddressBool=atoi(TmpStr);

    if((OurData->ListeningSockFD=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
        return false;
    OurData->DataSockFD=INVALID_SOCKET;

    if(ReuseAddressBool)
    {
        SetOptions=0;
        if(ReuseAddressBool)
            SetOptions|=SO_REUSEADDR;

        opt=1;
        if(setsockopt(OurData->ListeningSockFD,SOL_SOCKET,SetOptions,
                (const char*)&opt,sizeof(opt))==SOCKET_ERROR)
        {
            closesocket(OurData->ListeningSockFD);
            OurData->ListeningSockFD=INVALID_SOCKET;
            return false;
        }
    }

    memset(&OurData->serv_addr,0x00,sizeof(OurData->serv_addr));
    OurData->serv_addr.sin_family = AF_INET;
    OurData->serv_addr.sin_addr.s_addr = INADDR_ANY;
    OurData->serv_addr.sin_port = htons(Port);

    if(bind(OurData->ListeningSockFD,(struct sockaddr *)&OurData->serv_addr,
            sizeof(OurData->serv_addr))==SOCKET_ERROR)
    {
        closesocket(OurData->ListeningSockFD);
        OurData->ListeningSockFD=INVALID_SOCKET;
        return false;
    }
    if(listen(OurData->ListeningSockFD,1)==SOCKET_ERROR) // We can only handle 1 connection
    {
        closesocket(OurData->ListeningSockFD);
        OurData->ListeningSockFD=INVALID_SOCKET;
        return false;
    }

    OurData->Opened=true;
    OurData->WaitingForConnection=true;

    g_TCPS_IOSystem->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);

    return true;
}

/*******************************************************************************
 * NAME:
 *    TCPServer_Close
 *
 * SYNOPSIS:
 *    void TCPServer_Close(t_DriverIOHandleType *DriverIO);
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
 *    TCPServer_Open()
 ******************************************************************************/
void TCPServer_Close(t_DriverIOHandleType *DriverIO)
{
    struct TCPServer_OurData *OurData=(struct TCPServer_OurData *)DriverIO;

    if(OurData->ListeningSockFD!=INVALID_SOCKET)
        closesocket(OurData->ListeningSockFD);
    if(OurData->DataSockFD!=INVALID_SOCKET)
        closesocket(OurData->DataSockFD);
    OurData->ListeningSockFD=INVALID_SOCKET;
    OurData->DataSockFD=INVALID_SOCKET;

    OurData->Opened=false;
    OurData->WaitingForConnection=false;

    g_TCPS_IOSystem->DrvDataEvent(OurData->IOHandle,
            e_DataEventCode_Disconnected);
}

/*******************************************************************************
 * NAME:
 *    TCPServer_Read
 *
 * SYNOPSIS:
 *    int TCPServer_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
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
 *    TCPServer_Open(), TCPServer_Write()
 ******************************************************************************/
int TCPServer_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    struct TCPServer_OurData *OurData=(struct TCPServer_OurData *)DriverIO;
    int Byte2Ret;
    struct timeval tv;
    fd_set fds;
    int addrlen;

    if(OurData->ListeningSockFD==INVALID_SOCKET)
        return RETERROR_IOERROR;

    if(OurData->WaitingForConnection)
    {
        /* Ok, we are waiting for connection and we have been
           flaged as having data, means someone connected. */
        addrlen = sizeof(OurData->serv_addr);
        OurData->DataSockFD=accept(OurData->ListeningSockFD,
                (struct sockaddr *)&OurData->serv_addr,&addrlen);
        if(OurData->DataSockFD==INVALID_SOCKET)
            return RETERROR_IOERROR;

        OurData->WaitingForConnection=false;

        return RETERROR_NOBYTES;
    }

    FD_ZERO(&fds);
    FD_SET(OurData->DataSockFD,&fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    Byte2Ret=RETERROR_NOBYTES;

    /* Make sure we have at least 1 byte waiting */
    if(select(OurData->DataSockFD+1,&fds,NULL,NULL,&tv)!=0)
    {
        if(FD_ISSET(OurData->DataSockFD,&fds))
        {
            Byte2Ret=recv(OurData->DataSockFD,(char *)Data,MaxBytes,0);
            if(Byte2Ret==SOCKET_ERROR)
                return RETERROR_IOERROR;
            if(Byte2Ret==0)
            {
                /* 0=connection closed (because we where already told there
                   was data) */
                if(OurData->ListeningSockFD!=INVALID_SOCKET)
                    closesocket(OurData->ListeningSockFD);
                if(OurData->DataSockFD!=INVALID_SOCKET)
                    closesocket(OurData->DataSockFD);
                OurData->ListeningSockFD=INVALID_SOCKET;
                OurData->DataSockFD=INVALID_SOCKET;
                OurData->Opened=false;
                OurData->WaitingForConnection=false;
                g_TCPS_IOSystem->DrvDataEvent(OurData->IOHandle,
                        e_DataEventCode_Disconnected);
                return RETERROR_DISCONNECT;
            }
        }
    }

    return Byte2Ret;
}

/*******************************************************************************
 * NAME:
 *    TCPServer_Write
 *
 * SYNOPSIS:
 *    int TCPServer_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
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
 *    TCPServer_Open(), TCPServer_Read()
 ******************************************************************************/
int TCPServer_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    struct TCPServer_OurData *OurData=(struct TCPServer_OurData *)DriverIO;
    int retVal;
    int BytesSent;
    const uint8_t *OutputPos;

    if(OurData->WaitingForConnection)
        return RETERROR_NOBYTES;

    if(OurData->DataSockFD==INVALID_SOCKET)
        return RETERROR_IOERROR;

    BytesSent=0;
    OutputPos=Data;
    while(BytesSent<Bytes)
    {
        /* Interestingly write might only take SOME of the data, so we loop */
        retVal=send(OurData->DataSockFD,(const char *)OutputPos,
                Bytes-BytesSent,0);
        if(retVal<0)
        {
            switch(errno)
            {
                case EAGAIN:
                case ENOBUFS:
                    return RETERROR_BUSY;
                case EBADF:
                case ECONNABORTED:
                case ECONNREFUSED:
                case ECONNRESET:
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
 *    TCPServer_ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL TCPServer_ChangeOptions(t_DriverIOHandleType *DriverIO,
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
PG_BOOL TCPServer_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
    struct TCPServer_OurData *OurData=(struct TCPServer_OurData *)DriverIO;

    /* If we are already open then we need to close and reopen */
    if(OurData->ListeningSockFD!=INVALID_SOCKET)
        TCPServer_Close(DriverIO);

    return TCPServer_Open(DriverIO,Options);
}

static DWORD WINAPI TCPServer_OS_PollThread(LPVOID lpParameter)
{
    struct TCPServer_OurData *OurData=(struct TCPServer_OurData *)lpParameter;
    fd_set rfds;
    struct timeval tv;
    int retval;

    while(!OurData->RequestThreadQuit)
    {
        if(!OurData->Opened && !OurData->WaitingForConnection)
        {
            Sleep(100);   // Wait 100ms
            continue;
        }

        if(OurData->WaitingForConnection)
        {
            FD_ZERO(&rfds);
            FD_SET(OurData->ListeningSockFD,&rfds);

            /* 1ms timeout */
            tv.tv_sec=0;
            tv.tv_usec=100000;

            retval=select(OurData->ListeningSockFD+1,&rfds,NULL,NULL,&tv);
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
                g_TCPS_IOSystem->DrvDataEvent(OurData->IOHandle,
                        e_DataEventCode_BytesAvailable);

                /* Give the main thead some time to read out all the bytes */
                Sleep(100);   // Wait 100ms
            }
        }
        else
        {
            FD_ZERO(&rfds);
            FD_SET(OurData->DataSockFD,&rfds);

            /* 1ms timeout */
            tv.tv_sec=0;
            tv.tv_usec=100000;

            retval=select(OurData->DataSockFD+1,&rfds,NULL,NULL,&tv);
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
                g_TCPS_IOSystem->DrvDataEvent(OurData->IOHandle,
                        e_DataEventCode_BytesAvailable);

                /* Give the main thead some time to read out all the bytes */
                Sleep(100);   // Wait 100ms
            }
        }
    }

    OurData->ThreadHasQuit=true;

    return 0;
}

bool TCPServer_OSSupports_ReusePort(void)
{
    return false;
}
