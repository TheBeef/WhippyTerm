/*******************************************************************************
 * FILENAME: Comport_OS_Serial.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (24 Feb 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "../Comport_Serial.h"
#include "../../Comport_Main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

#warning TBD

bool Comport_OS_GetSerialPortList(t_OSComportListType &List)
{
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
    return NULL;
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
 *    Close(), Read(), Write(), ChangeOptions()
 ******************************************************************************/
PG_BOOL Comport_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    return false;
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
    return false;
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
    return RETERROR_IOERROR;
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
    return RETERROR_IOERROR;
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
    return false;
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
    return false;
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
    return false;
}
