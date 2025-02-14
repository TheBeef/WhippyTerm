/*******************************************************************************
 * FILENAME: Comport_OS_Serial.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    // Format: "COM64:9600,n,8,1"
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "../Comport_Serial.h"
#include "../../Comport_Main.h"
#include <string>
#include <stdbool.h>
#include <windows.h>
#include <stdio.h>
#include <SetupAPI.h>

using namespace std;

/*** DEFINES                  ***/
#define INBUFFER_GROW_SIZE                      10000   // We grow by 10k at a time
#define INBUFFER_MAX_SIZE                       1000000 // Cap at 1M

//#define INBUFFER_GROW_SIZE                      100   // We grow by 10k at a time
//#define INBUFFER_MAX_SIZE                       10000 // Cap at 1M

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct OpenComportInfo
{
    t_IOSystemHandle *DriverIO;
    string DriverName;
    HANDLE hComm;
    HANDLE ThreadMutex;
    HANDLE ThreadHandle;
    uint8_t *InBuffer;
    unsigned int InBufferSize;
    unsigned int InBufferHead;
    unsigned int InBufferTail;
    volatile bool RequestThreadQuit;
    volatile bool ThreadHasQuit;
    volatile bool Opened;
    struct Comport_ConAuxWidgets *AuxWidgets;
    bool RTSSet;
    bool DTRSet;
    DWORD LastModemBits;
    volatile DWORD ModemBits;
    volatile DWORD CommErrors;
    DWORD LastCommErrors;
};

/*** FUNCTION PROTOTYPES      ***/
static DWORD WINAPI Comport_OS_PollThread(LPVOID lpParameter);
static bool Comport_OS_ConfigPort(struct OpenComportInfo *ComInfo,
        uint32_t BitRate,e_ComportDataBitsType DataBits,
        e_ComportParityType Parity,e_ComportStopBitsType StopBits,
        e_ComportFlowControlType FlowControl);

/*** VARIABLE DEFINITIONS     ***/

bool Comport_OS_GetSerialPortList(t_OSComportListType &List)
{
    bool RetValue;
    string DevName;
    i_OSComportListType FoundPorts;
    struct ComportInfo NewComportInfo;
    GUID WorkingGuid;
    DWORD dwRequiredSize;
    DWORD DeviceIndex;
    SP_DEVINFO_DATA DeviceInfoData;
    HDEVINFO DeviceInfoSet;
    char szFriendlyName[MAX_PATH];
    char szPortName[MAX_PATH];
    HKEY hKey;
    DWORD dwReqSize;

    DeviceInfoSet=NULL;
    hKey=NULL;
    RetValue=false;
    try
    {
        /* SetupAPI method */
        DeviceInfoData.cbSize=sizeof(SP_DEVINFO_DATA);

        if(!SetupDiClassGuidsFromNameA("PORTS",(LPGUID)&WorkingGuid,1,
                                      &dwRequiredSize))
        {
            throw(0);
        }

        DeviceInfoSet=SetupDiGetClassDevsA(&WorkingGuid,NULL,NULL,
                DIGCF_PRESENT|DIGCF_PROFILE);
        if(DeviceInfoSet==INVALID_HANDLE_VALUE)
            throw(0);

        for(DeviceIndex=0;SetupDiEnumDeviceInfo(DeviceInfoSet,DeviceIndex,
                &DeviceInfoData);DeviceIndex++)
        {
            if(!SetupDiGetDeviceRegistryPropertyA(DeviceInfoSet,
                                                &DeviceInfoData,
                                                SPDRP_FRIENDLYNAME,
                                                NULL,
                                                (BYTE *)szFriendlyName,
                                                sizeof(szFriendlyName),
                                                NULL))
            {
                /* Skip */
                continue;
            }

            hKey=SetupDiOpenDevRegKey(DeviceInfoSet,&DeviceInfoData,
                    DICS_FLAG_GLOBAL,0,DIREG_DEV,KEY_READ);
            if(hKey)
            {
                dwReqSize=sizeof(szPortName);
                if(RegQueryValueExA(hKey,"PortName",NULL,NULL,
                        (LPBYTE)&szPortName,&dwReqSize)!=ERROR_SUCCESS)
                {
                    /* Skip */
                    RegCloseKey(hKey);
                    continue;
                }

                RegCloseKey(hKey);
            }

            /* We have to name it "\\.\COMxx" for comports over 9 because...
               Windows... */
            DevName="\\\\.\\";
            DevName+=szPortName;

            /* See if we found this port already */
            for(FoundPorts=List.begin();FoundPorts!=List.end();FoundPorts++)
            {
                if(DevName==FoundPorts->DriverName)
                    break;
            }

            if(FoundPorts==List.end())
            {
                /* Didn't find it, add it */
                NewComportInfo.DriverName=DevName;
                NewComportInfo.FullName=szFriendlyName;
                NewComportInfo.ShortName=szPortName;
                List.push_back(NewComportInfo);
            }
        }

        RetValue=true;
    }
    catch(...)
    {
        RetValue=false;
    }

    if(DeviceInfoSet!=NULL)
        SetupDiDestroyDeviceInfoList(DeviceInfoSet);

    return RetValue;
}

bool Comport_OS_SerialPortBusy(const std::string &DriverName)
{
    HANDLE hComm;
//    string OpenName;

    /* See if we can open it */
//    OpenName="\\\\.\\";
//    OpenName+=DriverName;

    hComm=CreateFileA(DriverName.c_str(),GENERIC_READ|GENERIC_WRITE,0,0,
            OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);
    if(hComm==INVALID_HANDLE_VALUE)
        return true;

    CloseHandle(hComm);

    return false;
}

/*******************************************************************************
 * NAME:
 *    Comport_AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *Comport_AllocateHandle(const char *DeviceUniqueID,
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
t_DriverIOHandleType *Comport_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    string filename;
    struct OpenComportInfo *NewComInfo;

    NewComInfo=NULL;
    try
    {
        NewComInfo=new(struct OpenComportInfo);
        if(NewComInfo==NULL)
            throw(0);

        NewComInfo->hComm=INVALID_HANDLE_VALUE;
        NewComInfo->DriverIO=IOHandle;
        NewComInfo->ThreadMutex=NULL;
        NewComInfo->DriverName=DeviceUniqueID;
        NewComInfo->RequestThreadQuit=false;
        NewComInfo->ThreadHasQuit=false;
        NewComInfo->Opened=false;
        NewComInfo->InBuffer=(uint8_t *)malloc(INBUFFER_GROW_SIZE);
        NewComInfo->InBufferSize=INBUFFER_GROW_SIZE;
        NewComInfo->InBufferHead=0;
        NewComInfo->InBufferTail=0;
        NewComInfo->LastModemBits=0;
        NewComInfo->ModemBits=0;
        NewComInfo->AuxWidgets=NULL;
        NewComInfo->CommErrors=0;
        NewComInfo->LastCommErrors=0;

        NewComInfo->ThreadMutex=CreateMutex(NULL,FALSE,NULL);
        if(NewComInfo->ThreadMutex==NULL)
            throw(0);

        NewComInfo->ThreadHandle=CreateThread(NULL,0,Comport_OS_PollThread,
                NewComInfo,CREATE_SUSPENDED,NULL);
        if(NewComInfo->ThreadHandle==NULL)
            throw(0);

        /* Ok, start the thread */
        ResumeThread(NewComInfo->ThreadHandle);
    }
    catch(...)
    {
        if(NewComInfo!=NULL)
        {
            if(NewComInfo->ThreadMutex!=NULL)
                CloseHandle(NewComInfo->ThreadMutex);

            delete NewComInfo;
        }

        return NULL;
    }

    return (t_DriverIOHandleType *)NewComInfo;
}

/*******************************************************************************
 * NAME:
 *    Comport_FreeHandle
 *
 * SYNOPSIS:
 *    void Comport_FreeHandle(t_DriverIOHandleType *DriverIO);
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
void Comport_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    /* Tell thread to quit */
    ComInfo->RequestThreadQuit=true;

    /* Wait for the thread to exit */
    while(!ComInfo->ThreadHasQuit)
        Sleep(1);   // Wait 1 ms

    if(ComInfo->hComm!=INVALID_HANDLE_VALUE)
        CloseHandle(ComInfo->hComm);

    delete ComInfo;
}

/*******************************************************************************
 * NAME:
 *    Comport_Open
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_Open(t_DriverIOHandleType *DriverIO,
 *          const t_PIKVList *Options);
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
 *    Close(), Read(), Write()
 ******************************************************************************/
PG_BOOL Comport_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;
    struct ComportPortOptions PortOptions;

    ComInfo->hComm=CreateFileA(ComInfo->DriverName.c_str(),
            GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
    if(ComInfo->hComm==INVALID_HANDLE_VALUE)
        return false;

    Comport_ConvertFromKVList(&PortOptions,Options);

    if(!Comport_OS_ConfigPort(ComInfo,PortOptions.BitRate,
            PortOptions.DataBits,PortOptions.Parity,
            PortOptions.StopBits,PortOptions.FlowControl))
    {
        CloseHandle(ComInfo->hComm);
        return false;
    }

    ComInfo->RTSSet=true;
    ComInfo->DTRSet=true;
    EscapeCommFunction(ComInfo->hComm,SETRTS);
    EscapeCommFunction(ComInfo->hComm,SETDTR);

    g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,e_DataEventCode_Connected);

    ComInfo->Opened=true;

    return true;
}

/*******************************************************************************
 * NAME:
 *    Comport_ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_ChangeOptions(t_DriverIOHandleType *DriverIO,
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
PG_BOOL Comport_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;
    struct ComportPortOptions PortOptions;

    Comport_ConvertFromKVList(&PortOptions,Options);

    if(!Comport_OS_ConfigPort(ComInfo,PortOptions.BitRate,
            PortOptions.DataBits,PortOptions.Parity,
            PortOptions.StopBits,PortOptions.FlowControl))
    {
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    Comport_Close
 *
 * SYNOPSIS:
 *    void Comport_Close(t_DriverIOHandleType *DriverIO);
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
 *    Open()
 ******************************************************************************/
void Comport_Close(t_DriverIOHandleType *DriverIO)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    if(ComInfo->hComm!=INVALID_HANDLE_VALUE)
        CloseHandle(ComInfo->hComm);
    ComInfo->hComm=INVALID_HANDLE_VALUE;

    ComInfo->Opened=false;

    g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,e_DataEventCode_Disconnected);
}

/*******************************************************************************
 * NAME:
 *    Comport_Read
 *
 * SYNOPSIS:
 *    int Comport_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
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
 *    Open(), Write()
 ******************************************************************************/
int Comport_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;
    int ReadBytes;
    bool HaveMutex;
    DWORD TmpModemBits;
    DWORD TmpErrorBits;

    HaveMutex=false;
    try
    {
        /* Grab the com port */
        WaitForSingleObject(ComInfo->ThreadMutex,INFINITE);
        HaveMutex=true;

        if(ComInfo->ModemBits!=ComInfo->LastModemBits)
        {
            /* Update the indicators */
            TmpModemBits=ComInfo->ModemBits;
            Comport_NotifyOfModemBitsChange(ComInfo->AuxWidgets,
                    TmpModemBits&MS_RLSD_ON,TmpModemBits&MS_RING_ON,
                    TmpModemBits&MS_DSR_ON,TmpModemBits&MS_CTS_ON);
            ComInfo->LastModemBits=ComInfo->ModemBits;
        }

        if(ComInfo->CommErrors!=ComInfo->LastCommErrors)
        {
            /* Update the any errors we support */
            TmpErrorBits=ComInfo->CommErrors;
            if(ComInfo->CommErrors&CE_BREAK &&
                    !(ComInfo->LastCommErrors&CE_BREAK))
            {
                Comport_AddLogMsg(ComInfo->AuxWidgets,"BREAK");
            }
            if(ComInfo->CommErrors&CE_FRAME &&
                    !(ComInfo->LastCommErrors&CE_FRAME))
            {
                Comport_AddLogMsg(ComInfo->AuxWidgets,"Framing error");
            }
            if(ComInfo->CommErrors&CE_RXPARITY &&
                    !(ComInfo->LastCommErrors&CE_RXPARITY))
            {
                Comport_AddLogMsg(ComInfo->AuxWidgets,"Parity error");
            }
            if(ComInfo->CommErrors&CE_RXOVER &&
                    !(ComInfo->LastCommErrors&CE_RXOVER))
            {
                Comport_AddLogMsg(ComInfo->AuxWidgets,"Overrun error");
            }
            if(ComInfo->CommErrors&CE_OVERRUN &&
                    !(ComInfo->LastCommErrors&CE_OVERRUN))
            {
                Comport_AddLogMsg(ComInfo->AuxWidgets,"Overrun error");
            }
            ComInfo->LastCommErrors=ComInfo->CommErrors;
        }

        ReadBytes=RETERROR_NOBYTES;

        if(ComInfo->InBufferTail!=ComInfo->InBufferHead)
        {
            /* We have bytes available */
            if(ComInfo->InBufferHead>=ComInfo->InBufferTail)
            {
                ReadBytes=ComInfo->InBufferHead-ComInfo->InBufferTail;
            }
            else
            {
                ReadBytes=ComInfo->InBufferSize-ComInfo->InBufferTail;
            }
            if(ReadBytes>Bytes)
                ReadBytes=Bytes;

            memcpy(Data,&ComInfo->InBuffer[ComInfo->InBufferTail],ReadBytes);
            ComInfo->InBufferTail+=ReadBytes;
            if(ComInfo->InBufferTail>=ComInfo->InBufferSize)
                ComInfo->InBufferTail=0;
        }

        ReleaseMutex(ComInfo->ThreadMutex);
        HaveMutex=false;
    }
    catch(...)
    {
        if(HaveMutex)
            ReleaseMutex(ComInfo->ThreadMutex);
        ReadBytes=-1;
    }
    return ReadBytes;
}

/*******************************************************************************
 * NAME:
 *    Comport_Write
 *
 * SYNOPSIS:
 *    int Comport_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,
 *              int Bytes);
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
 *    Open(), Read()
 ******************************************************************************/
int Comport_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;
    int RetBytes;
    DWORD dwWritten;
    bool HaveMutex;

    try
    {
        /* Grab the com port */
        WaitForSingleObject(ComInfo->ThreadMutex,INFINITE);
        HaveMutex=true;

        if(!WriteFile(ComInfo->hComm,Data,Bytes,&dwWritten,NULL))
            throw(0);
        RetBytes=dwWritten;

        ReleaseMutex(ComInfo->ThreadMutex);
        HaveMutex=false;
    }
    catch(...)
    {
        if(HaveMutex)
            ReleaseMutex(ComInfo->ThreadMutex);
        RetBytes=-1;
    }
    return RetBytes;
}

/*******************************************************************************
 * NAME:
 *    ConnectionAuxCtrlWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionWidgetsType *ConnectionAuxCtrlWidgets_AllocWidgets(
 *          t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    WidgetHandle [I] -- The handle to send to the widgets
 *
 * FUNCTION:
 *    This function adds aux control widgets to the aux control tab in the
 *    main window.  The aux controls are for extra controls that do things /
 *    display things going on with the driver.  For example there are things
 *    like the status of the CTS line and to set the RTS line.
 *
 *    The device driver needs to keep handles to the widgets added because it
 *    needs to free them when ConnectionAuxCtrlWidgets_FreeWidgets() called.
 *
 * RETURNS:
 *    The private options data that you want to use.  This is a private
 *    structure that you allocate and then cast to
 *    (t_ConnectionAuxCtrlWidgetsType *) when you return.
 *
 * NOTES:
 *    These widgets can only be accessed in the main thread.  They are not
 *    thread safe.
 *
 * SEE ALSO:
 *    ConnectionAuxCtrlWidgets_FreeWidgets()
 ******************************************************************************/
t_ConnectionWidgetsType *Comport_ConnectionAuxCtrlWidgets_AllocWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    ComInfo->AuxWidgets=Comport_AllocAuxWidgets(DriverIO,WidgetHandle);

    return (t_ConnectionWidgetsType *)ComInfo->AuxWidgets;
}

/*******************************************************************************
 * NAME:
 *    ConnectionAuxCtrlWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void ConnectionAuxCtrlWidgets_FreeWidgets(t_DriverIOHandleType *DriverIO,
 *          t_WidgetSysHandle *WidgetHandle,
 *          t_ConnectionWidgetsType *ConAuxCtrls);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    WidgetHandle [I] -- The handle to send to the widgets
 *    ConAuxCtrls [I] -- The aux controls data that was allocated with
 *          ConnectionAuxCtrlWidgets_AllocWidgets().
 *
 * FUNCTION:
 *    Frees the widgets added with ConnectionAuxCtrlWidgets_AllocWidgets()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ConnectionAuxCtrlWidgets_AllocWidgets()
 ******************************************************************************/
void Comport_ConnectionAuxCtrlWidgets_FreeWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConAuxCtrls)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    Comport_FreeAuxWidgets(WidgetHandle,(struct Comport_ConAuxWidgets *)ConAuxCtrls);
    ComInfo->AuxWidgets=NULL;
}

void Comport_UpdateDTR(t_DriverIOHandleType *DriverIO,bool DTR)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    if(ComInfo->hComm==INVALID_HANDLE_VALUE)
        return;

    EscapeCommFunction(ComInfo->hComm,DTR?SETDTR:CLRDTR);
    ComInfo->DTRSet=DTR;
}

void Comport_UpdateRTS(t_DriverIOHandleType *DriverIO,bool RTS)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    if(ComInfo->hComm==INVALID_HANDLE_VALUE)
        return;

    EscapeCommFunction(ComInfo->hComm,RTS?SETRTS:CLRRTS);
    ComInfo->RTSSet=RTS;
}

void Comport_SendBreak(t_DriverIOHandleType *DriverIO)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    if(ComInfo->hComm==INVALID_HANDLE_VALUE)
        return;

    SetCommBreak(ComInfo->hComm);
    Sleep(100);
    ClearCommBreak(ComInfo->hComm);
}

/*******************************************************************************
 * NAME:
 *    Comport_OS_ConfigPort
 *
 * SYNOPSIS:
 *    static bool Comport_OS_ConfigPort(struct OpenComportInfo *ComInfo,
 *          uint32_t BitRate,e_ComportDataBitsType DataBits,
 *          e_ComportParityType Parity,e_ComportStopBitsType StopBits,
 *          e_ComportFlowControlType FlowControl);
 *
 * PARAMETERS:
 *    ComInfo [I] -- The handle to this connection
 *    BitRate [I] -- The bit rate
 *    DataBits [I] -- The number of data bits
 *    Parity [I] -- The parity
 *    StopBits [I] -- The number of stop bits
 *    FlowControl [I] -- The flow control
 *
 * FUNCTION:
 *    This function sets the comport options.
 *
 * RETURNS:
 *    true -- Thing worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool Comport_OS_ConfigPort(struct OpenComportInfo *ComInfo,
        uint32_t BitRate,e_ComportDataBitsType DataBits,
        e_ComportParityType Parity,e_ComportStopBitsType StopBits,
        e_ComportFlowControlType FlowControl)
{
    DCB dcb;
    COMMTIMEOUTS timeouts;

    try
    {
        FillMemory(&dcb, sizeof(dcb), 0);
        dcb.DCBlength = sizeof(dcb);
        if(!BuildCommDCB("9600,n,8,1", &dcb))
        {
            return false;
        }

        dcb.BaudRate=BitRate;
        dcb.fBinary=true;

        switch(Parity)
        {
            case e_ComportParity_none:
                dcb.fParity=false;
                dcb.Parity=NOPARITY;
            break;
            case e_ComportParity_odd:
                dcb.fParity=true;
                dcb.Parity=ODDPARITY;
            break;
            case e_ComportParity_even:
                dcb.fParity=true;
                dcb.Parity=EVENPARITY;
            break;
            case e_ComportParity_mark:
                dcb.fParity=true;
                dcb.Parity=MARKPARITY;
            break;
            case e_ComportParity_space:
                dcb.fParity=true;
                dcb.Parity=SPACEPARITY;
            break;
            case e_ComportParityMAX:
            default:
                throw(0);
        }

        dcb.fOutxDsrFlow=false;
        dcb.fDtrControl=DTR_CONTROL_DISABLE;
        dcb.fNull=false;
        dcb.fRtsControl=RTS_CONTROL_ENABLE;
        dcb.fTXContinueOnXoff=false;
        dcb.fOutX=false;
        dcb.fInX=false;
        dcb.fOutxCtsFlow=false;
        dcb.fAbortOnError=false;

        switch(FlowControl)
        {
            case e_ComportFlowControl_None:
            break;
            case e_ComportFlowControl_XONXOFF:
                dcb.fTXContinueOnXoff=true;
                dcb.fOutX=true;
                dcb.fInX=true;
            break;
            case e_ComportFlowControl_Hardware:
                dcb.fOutxCtsFlow=true;
                dcb.fRtsControl=RTS_CONTROL_TOGGLE;
            break;
            case e_ComportFlowControlMAX:
            default:
                throw(0);
        }

        switch(DataBits)
        {
            case e_ComportDataBits_7:
                dcb.ByteSize=7;
            break;
            case e_ComportDataBits_8:
                dcb.ByteSize=8;
            break;
            case e_ComportDataBitsMAX:
            default:
                throw(0);
        }

        switch(StopBits)
        {
            case e_ComportStopBits_1:
                dcb.StopBits=ONESTOPBIT;
            break;
            case e_ComportStopBits_2:
                dcb.StopBits=TWOSTOPBITS;
            break;
            case e_ComportStopBitsMAX:
            default:
                throw(0);
        }

        if(!SetCommState(ComInfo->hComm,&dcb))
            return false;

        /* Restore RTS and DTR lines */
        EscapeCommFunction(ComInfo->hComm,ComInfo->RTSSet?SETRTS:CLRRTS);
        EscapeCommFunction(ComInfo->hComm,ComInfo->DTRSet?SETDTR:CLRDTR);

        GetCommTimeouts(ComInfo->hComm,&timeouts);

        timeouts.ReadIntervalTimeout=0;
        timeouts.ReadTotalTimeoutMultiplier=0;
        timeouts.ReadTotalTimeoutConstant=1;
        timeouts.WriteTotalTimeoutMultiplier=0;
        timeouts.WriteTotalTimeoutConstant=1000;

        if(!SetCommTimeouts(ComInfo->hComm,&timeouts))
            return false;
    }
    catch(...)
    {
        return false;
    }

    return true;
}

static DWORD WINAPI Comport_OS_PollThread(LPVOID lpParameter)
{
    struct OpenComportInfo *ComInfo;
    DWORD Errors;
    COMSTAT Stat;
    BOOL Ret;
    unsigned int MaxBytes;
    DWORD BytesRead;
    uint8_t *NewBuffer;
    unsigned int NextHead;
    unsigned int Bytes2Copy;
    DWORD dwModemStatus;

    ComInfo=(struct OpenComportInfo *)lpParameter;

    while(!ComInfo->RequestThreadQuit)
    {
        if(!ComInfo->Opened)
        {
            Sleep(1);   // Wait 1ms
            continue;
        }

        /* Grab com port while we wait for incoming bytes */
        WaitForSingleObject(ComInfo->ThreadMutex,INFINITE);
        Ret=ClearCommError(ComInfo->hComm,&Errors,&Stat);

        /* Check the Line Status */
        if(Ret)
        {
            if(Errors!=ComInfo->CommErrors)
            {
                ComInfo->CommErrors=Errors;

                /* Data available */
                g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,
                        e_DataEventCode_BytesAvailable);
            }
        }

        /* Read any bytes into our InBuffer */
        if(Ret && Stat.cbInQue>0)
        {
            /* Data available */
            g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,
                    e_DataEventCode_BytesAvailable);

            /* Check if we are full */
            NextHead=ComInfo->InBufferHead+1;
            if(NextHead>=ComInfo->InBufferSize)
                NextHead=0;

            if(NextHead==ComInfo->InBufferTail)
            {
                /* We are full, make more space */
                if(ComInfo->InBufferSize>=INBUFFER_MAX_SIZE)
                {
                    /* We don't have any room and we can't make more, wait */
                    ReleaseMutex(ComInfo->ThreadMutex);
                    Sleep(1);   // Wait 1ms
                    continue;
                }
                NewBuffer=(uint8_t *)malloc(ComInfo->InBufferSize+
                        INBUFFER_GROW_SIZE);
                if(NewBuffer==NULL)
                {
                    /* We couldn't get more memory, wait and try again */
                    ReleaseMutex(ComInfo->ThreadMutex);
                    Sleep(1);   // Wait 1ms
                    continue;
                }
                /* We need to copy the old data over to the new buffer */
                if(ComInfo->InBufferHead>ComInfo->InBufferTail)
                {
                    Bytes2Copy=ComInfo->InBufferHead-ComInfo->InBufferTail;
                    memcpy(NewBuffer,&ComInfo->InBuffer[ComInfo->InBufferTail],
                            Bytes2Copy);
                    ComInfo->InBufferTail=0;
                    ComInfo->InBufferHead=Bytes2Copy;
                }
                else
                {
                    Bytes2Copy=ComInfo->InBufferSize-ComInfo->InBufferTail;
                    memcpy(NewBuffer,&ComInfo->InBuffer[ComInfo->InBufferTail],
                            Bytes2Copy);
                    memcpy(&NewBuffer[Bytes2Copy],ComInfo->InBuffer,
                            ComInfo->InBufferHead);
                    ComInfo->InBufferTail=0;
                    ComInfo->InBufferHead+=Bytes2Copy;
                }
                free(ComInfo->InBuffer);
                ComInfo->InBuffer=NewBuffer;
                ComInfo->InBufferSize+=INBUFFER_GROW_SIZE;
            }

            if(ComInfo->InBufferHead>=ComInfo->InBufferTail)
            {
                MaxBytes=ComInfo->InBufferSize-ComInfo->InBufferHead;
                if(ComInfo->InBufferTail==0)
                    MaxBytes--;
            }
            else
            {
                MaxBytes=ComInfo->InBufferTail-ComInfo->InBufferHead-1;
            }

            if(!ReadFile(ComInfo->hComm,&ComInfo->InBuffer[ComInfo->InBufferHead],
                    MaxBytes,&BytesRead,NULL))
            {
                /* DEBUG PAUL: We need to flag an error */
            }

            ComInfo->InBufferHead+=BytesRead;
            if(ComInfo->InBufferHead>=ComInfo->InBufferSize)
                ComInfo->InBufferHead=0;
        }

        /* Check the Line Status */
        if(GetCommModemStatus(ComInfo->hComm,&dwModemStatus))
        {
            if(dwModemStatus!=ComInfo->ModemBits)
            {
                ComInfo->ModemBits=dwModemStatus;

                /* Data available */
                g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,
                        e_DataEventCode_BytesAvailable);
            }
        }

        ReleaseMutex(ComInfo->ThreadMutex);

        Sleep(1);   // Wait 1ms
    }

    ComInfo->ThreadHasQuit=true;

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Comport_Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
 *          char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
 *          PG_BOOL Update);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to convert to a device ID and options.
 *    Options [O] -- The options for this new connection.
 *    DeviceUniqueID [O] -- The unique ID for this device build from the 'URI'
 *    MaxDeviceUniqueIDLen [I] -- The max length of the buffer for
 *          'DeviceUniqueID'
 *    Update [I] -- If this is true then we are updating 'Options'.  If
 *                  false then you should default 'Options' before you
 *                  fill in the options.
 *
 * FUNCTION:
 *    This function converts a a URI string into a unique ID and options for
 *    a connection to be opened.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL Comport_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    const char *PosStart;
    const char *StartOfComPortNum;
    const char *EndOfComPortNum;
    string DevName;

    // Format: "COM64:9600,n,8,1"

    if(strlen(URI)<(sizeof(COMPORT_URI_PREFIX)-1))  // Prefix (-1 to remove \0)
        return false;

    /* Make sure it starts with COM: */
    if(strncasecmp(URI,COMPORT_URI_PREFIX,(sizeof(COMPORT_URI_PREFIX)-1))!=0)
        return false;

    PosStart=URI;
    PosStart+=sizeof(COMPORT_URI_PREFIX)-1;

    StartOfComPortNum=PosStart;

    /* Should be a comport number */
    while(*PosStart>='0' && *PosStart<='9')
        PosStart++;

    /* We should be on a colon */
    if(*PosStart!=':')
        return false;

    EndOfComPortNum=PosStart;

    PosStart+=1;    // move past the :

    /* Make sure there are 3 commons */
    /* Move to the baud rate */
    if(!Comport_SetOptionsFromURI(PosStart,Options,Update))
        return false;

    /* Build the DeviceUniqueID */

    /* We have to name it "\\.\COMxx" for comports over 9 because...
       Windows... */
    DevName="\\\\.\\COM";
    DevName.append(StartOfComPortNum,EndOfComPortNum-StartOfComPortNum);

    if(DevName.length()>=MaxDeviceUniqueIDLen)
        return false;
    strcpy(DeviceUniqueID,DevName.c_str());

    return true;
}

/*******************************************************************************
 * NAME:
 *    Comport_Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_Convert_Options_To_URI(const char *DeviceUniqueID,
 *          t_PIKVList *Options,char *URI,unsigned int MaxURILen);
 *
 * PARAMETERS:
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [I] -- The options for this connection.
 *    URI [O] -- A buffer to fill with the URI for this connection.
 *    MaxURILen [I] -- The size of the 'URI' buffer.
 *
 * FUNCTION:
 *    This function builds a URI for the device and options and fills it into
 *    a buffer
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Comport_Convert_URI_To_Options()
 ******************************************************************************/
PG_BOOL Comport_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    struct ComportPortOptions PortOptions;
    char buff[100];
    char DataBitsChar;
    char ParityChar;
    char StopBitsChar;

    Comport_ConvertFromKVList(&PortOptions,Options);

    if(strlen(DeviceUniqueID)<8)
        return false;

    if(sizeof(COMPORT_URI_PREFIX)+1+strlen(&DeviceUniqueID[7])>=MaxURILen-1)
        return false;

    strcpy(URI,COMPORT_URI_PREFIX);
    strcat(URI,&DeviceUniqueID[7]);
    strcat(URI,":");

    DataBitsChar='?';
    switch(PortOptions.DataBits)
    {
        case e_ComportDataBits_7:
            DataBitsChar='7';
        break;
        case e_ComportDataBits_8:
            DataBitsChar='8';
        break;
        case e_ComportDataBitsMAX:
        default:
        break;
    }

    ParityChar='?';
    switch(PortOptions.Parity)
    {
        case e_ComportParity_none:
            ParityChar='n';
        break;
        case e_ComportParity_odd:
            ParityChar='o';
        break;
        case e_ComportParity_even:
            ParityChar='e';
        break;
        case e_ComportParity_mark:
            ParityChar='m';
        break;
        case e_ComportParity_space:
            ParityChar='s';
        break;
        case e_ComportParityMAX:
        default:
        break;
    }

    StopBitsChar='?';
    switch(PortOptions.StopBits)
    {
        case e_ComportStopBits_1:
            StopBitsChar='1';
        break;
        case e_ComportStopBits_2:
            StopBitsChar='2';
        break;
        case e_ComportStopBitsMAX:
        default:
        break;
    }

    sprintf(buff,"%d,%c,%c,%c",PortOptions.BitRate,DataBitsChar,ParityChar,
            StopBitsChar);

    if(strlen(URI)+strlen(buff)>=MaxURILen-1)
        return false;
    strcat(URI,buff);

    return true;
}

/*******************************************************************************
 * NAME:
 *    GetConnectionInfo
 *
 * SYNOPSIS:
 *    PG_BOOL GetConnectionInfo(const char *DeviceUniqueID,
 *              struct IODriverDetectedInfo *RetInfo);
 *
 * PARAMETERS:
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    RetInfo [O] -- The structure to fill in with info about this device.
 *                   See DetectDevices() for a description of this structure.
 *                   The 'Next' must be set to NULL.
 *
 * FUNCTION:
 *    Get info about this connection.  This info is used at different times
 *    for different things in the system.
 *
 * RETURNS:
 *    true -- 'RetInfo' has been filled in.
 *    false -- There was an error in getting the info.
 *
 * SEE ALSO:
 *    DetectDevices()
 ******************************************************************************/
PG_BOOL Comport_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,
        struct IODriverDetectedInfo *RetInfo)
{
    static string Name;
    bool RetValue;
    string DevName;
    i_OSComportListType FoundPorts;
    struct ComportInfo NewComportInfo;
    GUID WorkingGuid;
    DWORD dwRequiredSize;
    DWORD DeviceIndex;
    SP_DEVINFO_DATA DeviceInfoData;
    HDEVINFO DeviceInfoSet;
    char szFriendlyName[MAX_PATH];
    char szPortName[MAX_PATH];
    HKEY hKey;
    DWORD dwReqSize;
    const char *p;
    int ComPortNum;

    DeviceInfoSet=NULL;
    hKey=NULL;
    RetValue=false;
    try
    {
        /* Fill in defaults */
        if(strlen(DeviceUniqueID)<7)
            throw(0);
        p=DeviceUniqueID;
        p+=7;   // Slip the "\\.\COM"

        ComPortNum=atoi(p);

        snprintf(RetInfo->Name,sizeof(RetInfo->Name),"COM%d",ComPortNum);
        snprintf(RetInfo->Title,sizeof(RetInfo->Title),"COM%d",ComPortNum);
        RetInfo->Flags=0;

        /* The only way to get the name is to search all the comports */

        /* SetupAPI method */
        DeviceInfoData.cbSize=sizeof(SP_DEVINFO_DATA);

        if(!SetupDiClassGuidsFromNameA("PORTS",(LPGUID)&WorkingGuid,1,
                                      &dwRequiredSize))
        {
            throw(0);
        }

        DeviceInfoSet=SetupDiGetClassDevsA(&WorkingGuid,NULL,NULL,
                DIGCF_PRESENT|DIGCF_PROFILE);
        if(DeviceInfoSet==INVALID_HANDLE_VALUE)
            throw(0);

        for(DeviceIndex=0;SetupDiEnumDeviceInfo(DeviceInfoSet,DeviceIndex,
                &DeviceInfoData);DeviceIndex++)
        {
            if(!SetupDiGetDeviceRegistryPropertyA(DeviceInfoSet,
                                                &DeviceInfoData,
                                                SPDRP_FRIENDLYNAME,
                                                NULL,
                                                (BYTE *)szFriendlyName,
                                                sizeof(szFriendlyName),
                                                NULL))
            {
                /* Skip */
                continue;
            }

            hKey=SetupDiOpenDevRegKey(DeviceInfoSet,&DeviceInfoData,
                    DICS_FLAG_GLOBAL,0,DIREG_DEV,KEY_READ);
            if(hKey)
            {
                dwReqSize=sizeof(szPortName);
                if(RegQueryValueExA(hKey,"PortName",NULL,NULL,
                        (LPBYTE)&szPortName,&dwReqSize)!=ERROR_SUCCESS)
                {
                    /* Skip */
                    RegCloseKey(hKey);
                    continue;
                }

                RegCloseKey(hKey);
            }

            /* We have to name it "\\.\COMxx" for comports over 9 because...
               Windows... */
            DevName="\\\\.\\";
            DevName+=szPortName;

            if(strcmp(DevName.c_str(),DeviceUniqueID)==0)
            {
                /* We found it */
                snprintf(RetInfo->Name,sizeof(RetInfo->Name),"%s",szFriendlyName);
                snprintf(RetInfo->Title,sizeof(RetInfo->Title),"%s",szPortName);
                RetInfo->Flags=0;
                break;
            }
        }
        RetValue=true;
    }
    catch(...)
    {
        RetValue=false;
    }

    if(DeviceInfoSet!=NULL)
        SetupDiDestroyDeviceInfoList(DeviceInfoSet);

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    Comport_GetURIHelpString
 *
 * SYNOPSIS:
 *    void Comport_CustomizeComportInfo(struct IODriverInfo *ComportInfo);
 *
 * PARAMETERS:
 *    ComportInfo [I/O] -- The comport info to customize.
 *
 * FUNCTION:
 *    This function changes any of the comport info that is needed for this
 *    version of the OS.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Comport_CustomizeComportInfo(struct IODriverInfo *ComportInfo)
{
    ComportInfo->URIHelpString=
            "<h3>FORMAT</h3>"
            "<p style='margin-left:60px;text-indent: -30px;'>"
            "COM[Port]:[Bit Rate],[Data Bits],[Parity],[Stop Bits]"
            "</p>"
            "<h3>WHERE</h3>"
            "<p style='margin-left:60px;text-indent: -30px;'>"
            "Port -- Which com port to use for this connection."
            "</p>"
            "<p style='margin-left:60px;text-indent: -30px;'>"
            "Bit Rate -- The speed to use for this connection."
            "</p>"
            "<p style='margin-left:60px;text-indent: -30px;'>"
            "Data Bits -- The number of data bits for this connection.  "
            "Supported values are 7 or 8 bits."
            "</p>"
            "<p style='margin-left:60px;text-indent: -30px;'>"
            "Parity -- What parity to use for this connection.  Supported "
            "values are n=none, o=odd, e=even, m=mark, s=space."
            "</p>"
            "<p style='margin-left:60px;text-indent: -30px;'>"
            "Stop Bits -- How many stop bits to use.  Supported values are "
            "1 or 2 stop bits."
            "</p>"
            "<h3>EXAMPLE</h3>"
            "<p style='margin-left:60px;text-indent: -30px;'>"
            "COM1:9600,8,n,1"
            "</p>";
}

