/*******************************************************************************
 * FILENAME: TCPClient_OS_Socket.cpp
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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (20 Jun 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "../TCPClient_Socket.h"
#include "../../TCPClient_Main.h"
#include <windows.h>
#include <winsock.h>
#include <stdlib.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct TCPClient_OurData
{
    t_IOSystemHandle *IOHandle;
    SOCKET SockFD;
    struct sockaddr_in serv_addr;
    WSADATA wsaData;
    bool wsaStarted;
    HANDLE ThreadHandle;
    volatile bool RequestThreadQuit;
    volatile bool ThreadHasQuit;
    volatile bool Opened;
};

/*** FUNCTION PROTOTYPES      ***/
static DWORD WINAPI TCPClient_OS_PollThread(LPVOID lpParameter);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    TCPClient_AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *TCPClient_AllocateHandle(const char *DeviceUniqueID,
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
t_DriverIOHandleType *TCPClient_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct TCPClient_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct TCPClient_OurData;
        NewData->IOHandle=IOHandle;
        NewData->SockFD=INVALID_SOCKET;
        NewData->wsaStarted=false;
        NewData->RequestThreadQuit=false;
        NewData->ThreadHasQuit=false;
        NewData->Opened=false;

        if(WSAStartup(MAKEWORD(2,2),&NewData->wsaData)!=0)
            throw(0);
        NewData->wsaStarted=true;

        /* Startup the thread for polling if we have data available */
        NewData->ThreadHandle=CreateThread(NULL,0,TCPClient_OS_PollThread,
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
 *    TCPClient_FreeHandle
 *
 * SYNOPSIS:
 *    void TCPClient_FreeHandle(t_DriverIOHandleType *DriverIO);
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
void TCPClient_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct TCPClient_OurData *OurData=(struct TCPClient_OurData *)DriverIO;

    /* Tell thread to quit */
    OurData->RequestThreadQuit=true;

    /* Wait for the thread to exit */
    while(!OurData->ThreadHasQuit)
        Sleep(1);   // Wait 1 ms

    if(OurData->SockFD!=INVALID_SOCKET)
        closesocket(OurData->SockFD);

    if(OurData->wsaStarted)
        WSACleanup();

    delete OurData;
}

/*******************************************************************************
 * NAME:
 *    TCPClient_Open
 *
 * SYNOPSIS:
 *    PG_BOOL TCPClient_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
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
 *    TCPClient_Close(), TCPClient_Read(), TCPClient_Write(),
 *    TCPClient_ChangeOptions()
 ******************************************************************************/
PG_BOOL TCPClient_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct TCPClient_OurData *OurData=(struct TCPClient_OurData *)DriverIO;
    struct hostent *host;
    const char *AddressStr;
    const char *PortStr;
    uint16_t Port;

    AddressStr=g_TCPC_System->KVGetItem(Options,"Address");
    PortStr=g_TCPC_System->KVGetItem(Options,"Port");
    if(AddressStr==NULL || PortStr==NULL)
        return false;

    Port=strtoul(PortStr,NULL,10);

    host=gethostbyname(AddressStr);
    if(host==NULL)
        return false;

    if((OurData->SockFD=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
        return false;

    memset(&OurData->serv_addr,0x00,sizeof(OurData->serv_addr));
    OurData->serv_addr.sin_family = AF_INET;
    OurData->serv_addr.sin_port = htons(Port);
    OurData->serv_addr.sin_addr.s_addr = *(long *)(host->h_addr);
    memset(&(OurData->serv_addr.sin_zero), 0, 8);

    if(connect(OurData->SockFD,(struct sockaddr *)&OurData->serv_addr,
            sizeof(struct sockaddr)) == SOCKET_ERROR)
    {
        closesocket(OurData->SockFD);
        OurData->SockFD=INVALID_SOCKET;
        return false;
    }

    OurData->Opened=true;

    g_TCPC_IOSystem->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);

    return true;
}

/*******************************************************************************
 * NAME:
 *    TCPClient_Close
 *
 * SYNOPSIS:
 *    void TCPClient_Close(t_DriverIOHandleType *DriverIO);
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
 *    TCPClient_Open()
 ******************************************************************************/
void TCPClient_Close(t_DriverIOHandleType *DriverIO)
{
    struct TCPClient_OurData *OurData=(struct TCPClient_OurData *)DriverIO;

    if(OurData->SockFD!=INVALID_SOCKET)
        closesocket(OurData->SockFD);
    OurData->SockFD=INVALID_SOCKET;

    OurData->Opened=false;

    g_TCPC_IOSystem->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Disconnected);
}

/*******************************************************************************
 * NAME:
 *    TCPClient_Read
 *
 * SYNOPSIS:
 *    int TCPClient_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
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
 *    TCPClient_Open(), TCPClient_Write()
 ******************************************************************************/
int TCPClient_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    struct TCPClient_OurData *OurData=(struct TCPClient_OurData *)DriverIO;
    int Byte2Ret;
    struct timeval tv;
    fd_set fds;

    if(OurData->SockFD==INVALID_SOCKET)
        return RETERROR_IOERROR;

    FD_ZERO(&fds);
    FD_SET(OurData->SockFD,&fds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    Byte2Ret=RETERROR_NOBYTES;

    /* Make sure we have at least 1 byte waiting */
    if(select(OurData->SockFD+1,&fds,NULL,NULL,&tv)!=SOCKET_ERROR)
    {
        if(FD_ISSET(OurData->SockFD,&fds))
        {
            Byte2Ret=recv(OurData->SockFD,(char *)Data,MaxBytes,0);
            if(Byte2Ret==SOCKET_ERROR)
                return RETERROR_IOERROR;
            if(Byte2Ret==0)
            {
                /* 0=connection closed (because we where already told there
                   was data) */
                if(OurData->SockFD!=INVALID_SOCKET)
                    closesocket(OurData->SockFD);
                OurData->SockFD=INVALID_SOCKET;
                OurData->Opened=false;
                g_TCPC_IOSystem->DrvDataEvent(OurData->IOHandle,
                        e_DataEventCode_Disconnected);
                return RETERROR_DISCONNECT;
            }
        }
    }

    return Byte2Ret;
}

/*******************************************************************************
 * NAME:
 *    TCPClient_Write
 *
 * SYNOPSIS:
 *    int TCPClient_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
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
 *    TCPClient_Open(), TCPClient_Read()
 ******************************************************************************/
int TCPClient_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    struct TCPClient_OurData *OurData=(struct TCPClient_OurData *)DriverIO;
    int retVal;
    int BytesSent;
    const uint8_t *OutputPos;

    if(OurData->SockFD==INVALID_SOCKET)
        return RETERROR_IOERROR;

    BytesSent=0;
    OutputPos=Data;
    while(BytesSent<Bytes)
    {
        /* Interestingly write might only take SOME of the data, so we loop */
        retVal=send(OurData->SockFD,(const char *)OutputPos,Bytes-BytesSent,0);
        if(retVal==SOCKET_ERROR)
        {
            switch(WSAGetLastError())
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
 *    TCPClient_ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL TCPClient_ChangeOptions(t_DriverIOHandleType *DriverIO,
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
PG_BOOL TCPClient_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
//    struct TCPClient_OurData *OurData=(struct TCPClient_OurData *)DriverIO;
//
//    /* If we are already open then we need to close and reopen */
//    if(OurData->SockFD!=INVALID_SOCKET)
//        TCPClient_Close(DriverIO);
//
//    return TCPClient_Open(DriverIO,Options);

    /* We don't currently support any options */
    return true;
}

static DWORD WINAPI TCPClient_OS_PollThread(LPVOID lpParameter)
{
    struct TCPClient_OurData *OurData=(struct TCPClient_OurData *)lpParameter;
    fd_set rfds;
    struct timeval tv;
    int retval;

    while(!OurData->RequestThreadQuit)
    {
        if(!OurData->Opened)
        {
            Sleep(100);   // Wait 100ms
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
            g_TCPC_IOSystem->DrvDataEvent(OurData->IOHandle,
                    e_DataEventCode_BytesAvailable);

            /* Give the main thead some time to read out all the bytes */
            Sleep(100);   // Wait 100ms
        }
    }

    OurData->ThreadHasQuit=true;

    return 0;
}
