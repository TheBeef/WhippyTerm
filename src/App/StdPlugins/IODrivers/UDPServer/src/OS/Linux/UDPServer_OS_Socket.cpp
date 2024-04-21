/*******************************************************************************
 * FILENAME: UDPServer_OS_Socket.cpp
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
 *    Paul Hutchinson (15 May 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "../UDPServer_Socket.h"
#include "../../UDPServer_Main.h"
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
struct UDPServer_OurData
{
    t_IOSystemHandle *IOHandle;
    int DataSockFD;
    struct sockaddr_in serv_addr;
    pthread_t ThreadInfo;
    uint8_t ReadBuffer[65536];
    volatile int BytesInBuffer;
    int BufferPos;
    volatile bool RequestThreadQuit;
    volatile bool ThreadHasQuit;
    volatile bool Opened;
};

/*** FUNCTION PROTOTYPES      ***/
static void *UDPServer_OS_PollThread(void *arg);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UDPServer_AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *UDPServer_AllocateHandle(const char *DeviceUniqueID,
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
t_DriverIOHandleType *UDPServer_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct UDPServer_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct UDPServer_OurData;
        NewData->IOHandle=IOHandle;
        NewData->DataSockFD=-1;
        NewData->RequestThreadQuit=false;
        NewData->ThreadHasQuit=false;
        NewData->Opened=false;
        NewData->BytesInBuffer=0;
        NewData->BufferPos=0;

        /* Startup the thread for polling if we have data available */
        if(pthread_create(&NewData->ThreadInfo,NULL,UDPServer_OS_PollThread,
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
 *    UDPServer_FreeHandle
 *
 * SYNOPSIS:
 *    void UDPServer_FreeHandle(t_DriverIOHandleType *DriverIO);
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
void UDPServer_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct UDPServer_OurData *OurData=(struct UDPServer_OurData *)DriverIO;

    /* Tell thread to quit */
    OurData->RequestThreadQuit=true;

    /* Wait for the thread to exit */
    while(!OurData->ThreadHasQuit)
        usleep(1000);   // Wait 1 ms

    if(OurData->DataSockFD>=0)
        close(OurData->DataSockFD);

    delete OurData;
}

/*******************************************************************************
 * NAME:
 *    UDPServer_Open
 *
 * SYNOPSIS:
 *    PG_BOOL UDPServer_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
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
 *    UDPServer_Close(), UDPServer_Read(), UDPServer_Write(),
 *    UDPServer_ChangeOptions()
 ******************************************************************************/
PG_BOOL UDPServer_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct UDPServer_OurData *OurData=(struct UDPServer_OurData *)DriverIO;
    const char *PortStr;
    bool ReuseAddressBool;
    bool ReusePortBool;
    uint16_t Port;
    bool MulticastBool;
    const char *MulticastGroupStr;
    int opt;
    int SetOptions;
    const char *TmpStr;

    PortStr=g_UDPS_System->KVGetItem(Options,"Port");
    if(PortStr==NULL)
        return false;
    Port=strtoul(PortStr,NULL,10);

    TmpStr=g_UDPS_System->KVGetItem(Options,"ReuseAddress");
    if(TmpStr==NULL)
        TmpStr="0";
    ReuseAddressBool=atoi(TmpStr);

    TmpStr=g_UDPS_System->KVGetItem(Options,"ReusePort");
    if(TmpStr==NULL)
        TmpStr="0";
    ReusePortBool=atoi(TmpStr);

    TmpStr=g_UDPS_System->KVGetItem(Options,"Multicast");
    if(TmpStr==NULL)
        TmpStr="0";
    MulticastBool=atoi(TmpStr);

    MulticastGroupStr=g_UDPS_System->KVGetItem(Options,"MulticastGroup");
    if(MulticastBool && MulticastGroupStr==NULL)
        return false;

    if((OurData->DataSockFD=socket(AF_INET,SOCK_DGRAM,0))<0)
        return false;

    if(MulticastBool)
    {
        /* Multicast must be ReuseAddressBool but not ReusePortBool so we force
           them */
        ReuseAddressBool=true;
        ReusePortBool=false;
    }

    if(ReuseAddressBool || ReusePortBool)
    {
        SetOptions=0;
        if(ReuseAddressBool)
            SetOptions|=SO_REUSEADDR;
        if(ReusePortBool)
            SetOptions|=SO_REUSEPORT;

        opt=1;
        if(setsockopt(OurData->DataSockFD,SOL_SOCKET,SetOptions,&opt,
                sizeof(opt)))
        {
            close(OurData->DataSockFD);
            OurData->DataSockFD=-1;
            return false;
        }
    }

    memset(&OurData->serv_addr,0,sizeof(OurData->serv_addr));
    OurData->serv_addr.sin_family = AF_INET;
    if(MulticastBool)
        OurData->serv_addr.sin_addr.s_addr = inet_addr(MulticastGroupStr);
    else
        OurData->serv_addr.sin_addr.s_addr = INADDR_ANY;
    OurData->serv_addr.sin_port = htons(Port);

    if(bind(OurData->DataSockFD,(struct sockaddr *)&OurData->serv_addr,
            sizeof(OurData->serv_addr))<0)
    {
        close(OurData->DataSockFD);
        OurData->DataSockFD=-1;
        return false;
    }

    if(MulticastBool)
    {
        // Request that we join a multicast group
        struct ip_mreq mreq;
        memset(&mreq,0,sizeof(mreq));
        mreq.imr_multiaddr.s_addr=inet_addr(MulticastGroupStr);
        mreq.imr_interface.s_addr=htonl(INADDR_ANY);
        if(setsockopt(OurData->DataSockFD,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,
                sizeof(mreq)))
        {
            close(OurData->DataSockFD);
            OurData->DataSockFD=-1;
            return false;
        }
    }

    OurData->Opened=true;

    g_UDPS_IOSystem->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);

    return true;
}

/*******************************************************************************
 * NAME:
 *    UDPServer_Close
 *
 * SYNOPSIS:
 *    void UDPServer_Close(t_DriverIOHandleType *DriverIO);
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
 *    UDPServer_Open()
 ******************************************************************************/
void UDPServer_Close(t_DriverIOHandleType *DriverIO)
{
    struct UDPServer_OurData *OurData=(struct UDPServer_OurData *)DriverIO;

    if(OurData->DataSockFD>=0)
        close(OurData->DataSockFD);
    OurData->DataSockFD=-1;

    OurData->Opened=false;

    g_UDPS_IOSystem->DrvDataEvent(OurData->IOHandle,
            e_DataEventCode_Disconnected);
}

/*******************************************************************************
 * NAME:
 *    UDPServer_Read
 *
 * SYNOPSIS:
 *    int UDPServer_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
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
 *    UDPServer_Open(), UDPServer_Write()
 ******************************************************************************/
int UDPServer_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    struct UDPServer_OurData *OurData=(struct UDPServer_OurData *)DriverIO;
    int Byte2Ret;
    struct timeval tv;
    fd_set fds;

    if(OurData->DataSockFD<0)
        return RETERROR_IOERROR;

    if(OurData->BytesInBuffer==0)
    {
        /* No buffered so load next message */
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
                Byte2Ret=read(OurData->DataSockFD,OurData->ReadBuffer,
                        sizeof(OurData->ReadBuffer));
                if(Byte2Ret<0)
                    return RETERROR_IOERROR;
                OurData->BytesInBuffer=Byte2Ret;
                OurData->BufferPos=0;
            }
        }
    }
    if(OurData->BytesInBuffer>0)
    {
        /* Copy data from the buffer */
        Byte2Ret=OurData->BytesInBuffer;
        if(Byte2Ret>MaxBytes)
            Byte2Ret=MaxBytes;
        memcpy(Data,&OurData->ReadBuffer[OurData->BufferPos],Byte2Ret);
        OurData->BufferPos+=Byte2Ret;
        OurData->BytesInBuffer-=Byte2Ret;
    }

    return Byte2Ret;
}

/*******************************************************************************
 * NAME:
 *    UDPServer_Write
 *
 * SYNOPSIS:
 *    int UDPServer_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
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
 *    UDPServer_Open(), UDPServer_Read()
 ******************************************************************************/
int UDPServer_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    /* We are a read only socket (we have no idea who to send to) */
    return RETERROR_NOBYTES;
}

/*******************************************************************************
 * NAME:
 *    UDPServer_ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL UDPServer_ChangeOptions(t_DriverIOHandleType *DriverIO,
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
PG_BOOL UDPServer_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
    struct UDPServer_OurData *OurData=(struct UDPServer_OurData *)DriverIO;

    /* If we are already open then we need to close and reopen */
    if(OurData->DataSockFD>=0)
        UDPServer_Close(DriverIO);

    return UDPServer_Open(DriverIO,Options);
}

static void *UDPServer_OS_PollThread(void *arg)
{
    struct UDPServer_OurData *OurData=(struct UDPServer_OurData *)arg;
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

        if(OurData->BytesInBuffer>0)
        {
            /* Data available */
            g_UDPS_IOSystem->DrvDataEvent(OurData->IOHandle,
                    e_DataEventCode_BytesAvailable);

            /* Give the main thead some time to read out all the bytes */
            usleep(100000);   // Wait 100ms
            continue;
        }

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
            g_UDPS_IOSystem->DrvDataEvent(OurData->IOHandle,
                    e_DataEventCode_BytesAvailable);

            /* Give the main thead some time to read out all the bytes */
            usleep(100000);   // Wait 100ms
        }
    }

    OurData->ThreadHasQuit=true;

    return 0;
}

/*******************************************************************************
 * NAME:
 *    OSSupportsReusePort
 *
 * SYNOPSIS:
 *    bool OSSupportsReusePort(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns if the OS supports SO_REUSEPORT.
 *
 * RETURNS:
 *    true -- OS supports SO_REUSEPORT
 *    false -- No OS support.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool OSSupportsReusePort(void)
{
    return true;
}
