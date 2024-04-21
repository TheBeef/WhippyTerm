/*******************************************************************************
 * FILENAME: UDPClient_OS_Socket.cpp
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
#include "../UDPClient_Socket.h"
#include "../../UDPClient_Main.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct UDPClient_OurData
{
    t_IOSystemHandle *IOHandle;
    int SockFD;
    struct sockaddr_in serv_addr;
    uint8_t SendBuffer[65536];  // The buffer to queue outgoing data.  The max size of a UDP packet is 64k so that's what we allocate
    unsigned int BytesInSendBuffer;
};

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UDPClient_AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *UDPClient_AllocateHandle(const char *DeviceUniqueID,
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
t_DriverIOHandleType *UDPClient_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct UDPClient_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct UDPClient_OurData;
        NewData->IOHandle=IOHandle;
        NewData->SockFD=-1;
        NewData->BytesInSendBuffer=0;
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
 *    UDPClient_FreeHandle
 *
 * SYNOPSIS:
 *    void UDPClient_FreeHandle(t_DriverIOHandleType *DriverIO);
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
void UDPClient_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct UDPClient_OurData *OurData=(struct UDPClient_OurData *)DriverIO;

    if(OurData->SockFD>=0)
        close(OurData->SockFD);

    delete OurData;
}

/*******************************************************************************
 * NAME:
 *    UDPClient_Open
 *
 * SYNOPSIS:
 *    PG_BOOL UDPClient_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
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
 *    UDPClient_Close(), UDPClient_Read(), UDPClient_Write(),
 *    UDPClient_ChangeOptions()
 ******************************************************************************/
PG_BOOL UDPClient_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct UDPClient_OurData *OurData=(struct UDPClient_OurData *)DriverIO;
    struct hostent *host;
    struct ip_mreq mreq;
    const char *AddressStr;
    const char *PortStr;
    const char *BroadcastStr;
    const char *MulticastStr;
    const char *MulticastGroupStr;
    int broadcastEnable=1;
    uint16_t Port;
    bool BroadcastBool;
    bool MulticastBool;

    AddressStr=g_UDPC_System->KVGetItem(Options,"Address");
    PortStr=g_UDPC_System->KVGetItem(Options,"Port");
    BroadcastStr=g_UDPC_System->KVGetItem(Options,"Broadcast");
    MulticastStr=g_UDPC_System->KVGetItem(Options,"Multicast");
    MulticastGroupStr=g_UDPC_System->KVGetItem(Options,"MulticastGroup");

    if(AddressStr==NULL || PortStr==NULL || BroadcastStr==NULL ||
            MulticastStr==NULL || MulticastGroupStr==NULL)
    {
        return false;
    }

    Port=strtoul(PortStr,NULL,10);
    BroadcastBool=atoi(BroadcastStr);
    MulticastBool=atoi(MulticastStr);

    if(MulticastBool)
        BroadcastBool=false;

    if((OurData->SockFD=socket(AF_INET,SOCK_DGRAM,0))<0)
        return false;

    if(!MulticastBool)
    {
        host=gethostbyname(AddressStr);
        if(host==NULL)
            return false;
    }

    if(BroadcastBool)
    {
        broadcastEnable=1;
        if(setsockopt(OurData->SockFD,SOL_SOCKET,SO_BROADCAST,&broadcastEnable,
                sizeof(broadcastEnable))!=0)
        {
            close(OurData->SockFD);
            OurData->SockFD=-1;
            return false;
        }
    }
    else if(MulticastBool)
    {
        memset(&mreq,0,sizeof(mreq));
        mreq.imr_interface.s_addr=htonl(INADDR_ANY);
        if(setsockopt(OurData->SockFD,IPPROTO_IP,IP_MULTICAST_IF,
                &mreq,sizeof(mreq))<0)
        {
            close(OurData->SockFD);
            OurData->SockFD=-1;
            return false;
        }
    }

    memset(&OurData->serv_addr,0,sizeof(OurData->serv_addr));
    OurData->serv_addr.sin_family = AF_INET;
    OurData->serv_addr.sin_port = htons(Port);
    if(MulticastBool)
        OurData->serv_addr.sin_addr.s_addr = inet_addr(MulticastGroupStr);
    else
        OurData->serv_addr.sin_addr.s_addr = *(long *)(host->h_addr);
    memset(&(OurData->serv_addr.sin_zero), 0, 8);

    g_UDPC_IOSystem->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);

    return true;
}

/*******************************************************************************
 * NAME:
 *    UDPClient_Close
 *
 * SYNOPSIS:
 *    void UDPClient_Close(t_DriverIOHandleType *DriverIO);
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
 *    UDPClient_Open()
 ******************************************************************************/
void UDPClient_Close(t_DriverIOHandleType *DriverIO)
{
    struct UDPClient_OurData *OurData=(struct UDPClient_OurData *)DriverIO;

    if(OurData->SockFD>=0)
        close(OurData->SockFD);
    OurData->SockFD=-1;

    g_UDPC_IOSystem->DrvDataEvent(OurData->IOHandle,
            e_DataEventCode_Disconnected);
}

/*******************************************************************************
 * NAME:
 *    UDPClient_Read
 *
 * SYNOPSIS:
 *    int UDPClient_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
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
 *    UDPClient_Open(), UDPClient_Write()
 ******************************************************************************/
int UDPClient_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    return 0;   // Nothing to read as this driver is output only
}

/*******************************************************************************
 * NAME:
 *    UDPClient_Write
 *
 * SYNOPSIS:
 *    int UDPClient_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,
 *          int Bytes);
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
 *    UDPClient_Open(), UDPClient_Read()
 ******************************************************************************/
int UDPClient_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,
        int Bytes)
{
    struct UDPClient_OurData *OurData=(struct UDPClient_OurData *)DriverIO;
    int Bytes2Copy;

    if(OurData->SockFD<0)
        return RETERROR_IOERROR;

    /* Queue the data because we send a packet everytime to call send.  The
       main code will need to call UDPClient_Transmit() to send */

    Bytes2Copy=Bytes;
    if(OurData->BytesInSendBuffer+Bytes2Copy>sizeof(OurData->SendBuffer))
        Bytes2Copy=sizeof(OurData->SendBuffer)-OurData->BytesInSendBuffer;

    memcpy(&OurData->SendBuffer[OurData->BytesInSendBuffer],Data,Bytes2Copy);
    OurData->BytesInSendBuffer+=Bytes2Copy;

    return Bytes2Copy;
}

/*******************************************************************************
 * NAME:
 *    UDPClient_ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL UDPClient_ChangeOptions(t_DriverIOHandleType *DriverIO,
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
PG_BOOL UDPClient_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
    struct UDPClient_OurData *OurData=(struct UDPClient_OurData *)DriverIO;

    /* If we are already open then we need to close and reopen */
    if(OurData->SockFD>=0)
        UDPClient_Close(DriverIO);

    return UDPClient_Open(DriverIO,Options);
}

/*******************************************************************************
 * NAME:
 *    UDPClient_Transmit
 *
 * SYNOPSIS:
 *    int UDPClient_Transmit(t_DriverIOHandleType *DriverIO);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *
 * FUNCTION:
 *    This function tells the driver to send any queued data.  This only really
 *    makes sence for block send devices.  Can be set to NULL.
 *
 * RETURNS:
 *    The same as Write()
 *
 * SEE ALSO:
 *    Open(), Write()
 ******************************************************************************/
int UDPClient_Transmit(t_DriverIOHandleType *DriverIO)
{
    struct UDPClient_OurData *OurData=(struct UDPClient_OurData *)DriverIO;
    int BytesSent;

    if(OurData->SockFD<0)
        return RETERROR_IOERROR;

    BytesSent=OurData->BytesInSendBuffer;

    if(sendto(OurData->SockFD,(const char *)OurData->SendBuffer,
            OurData->BytesInSendBuffer,0,
            (const struct sockaddr *)&OurData->serv_addr,
            sizeof(OurData->serv_addr))<0)
    {
        return RETERROR_IOERROR;
    }

    OurData->BytesInSendBuffer=0;

    return BytesSent;
}
