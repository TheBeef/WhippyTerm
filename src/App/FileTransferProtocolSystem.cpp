/*******************************************************************************
 * FILENAME: FileTransferProtocolSystem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the file transfer protocol plug in system in it.
 *
 *    This function mostly handles dealing with the FTP driver plugins.
 *    This is things like registering them, calling different parts, providing
 *    an API for them to call.
 *
 *    It does not provide any of the file transfer login.
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
 *    Paul Hutchinson (16 Mar 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Connections.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/MainApp.h"
#include "App/PluginSupport/KeyValueSupport.h"
#include "App/Settings.h"
#include "App/PluginSupport/PluginUISupport.h"
#include "App/PluginSupport/PluginSystem.h"
#include "UI/UIAsk.h"
#include "UI/UIFileReq.h"
#include "OS/Directorys.h"
#include "Version.h"
#include <string>
#include <string.h>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef std::list<const struct FTPHandlerInfo *> t_FTPHandlersType;
typedef t_FTPHandlersType::iterator i_FTPHandlersType;

struct FTPOptionsData
{
    t_FTPOptionsWidgetsType *FTPOptions;
    i_FTPHandlersType FTPHandler;
};

struct RealFTPData
{
    t_FTPHandlerDataType *HandlerData;
    const struct FileTransferHandlerAPI *HandlerAPI;
    class Connection *Con;
    i_FTPHandlersType FTPHandlerInfo;
};

/*** FUNCTION PROTOTYPES      ***/
static PG_BOOL FTPSPIA_RegisterFileTransferProtocol(const struct FTPHandlerInfo *Info);
static void FTPSPIA_ULProgress(t_FTPSystemData *SysHandle,
        uint64_t BytesTransfered);
static void FTPSPIA_SetTimeout(t_FTPSystemData *SysHandle,uint32_t MSec);
static void FTPSPIA_RestartTimeout(t_FTPSystemData *SysHandle);
static int FTPSPIA_ULSendData(t_FTPSystemData *SysHandle,void *Packet,
        uint32_t Bytes);
static void FTPSPIA_ULFinishUpload(t_FTPSystemData *SysHandle,PG_BOOL Aborted);
static void FTPSPIA_DLProgress(t_FTPSystemData *SysHandle,
        uint64_t BytesTransfered);
static int FTPSPIA_DLSendData(t_FTPSystemData *SysHandle,void *Packet,
        uint32_t Bytes);
static void FTPSPIA_DLFinishDownload(t_FTPSystemData *SysHandle,PG_BOOL Aborted);
static const char *FTPSPIA_GetDownloadFilename(t_FTPSystemData *SysHandle,
        const char *FileNameHint);

static bool FTPS_SetupCurrentHandler(struct RealFTPData *RealFData,
        i_FTPHandlersType Handler);
static void FTPS_FreeCurrentHander(struct RealFTPData *RealFData);

/*** VARIABLE DEFINITIONS     ***/
t_FTPHandlersType m_FTPHandlersList;     // All available data processors

struct FTPS_API g_FTPSAPI=
{
    FTPSPIA_RegisterFileTransferProtocol,
    PIUSDefault_GetDefaultAPI,
    FTPSPIA_SetTimeout,
    FTPSPIA_RestartTimeout,
    FTPSPIA_ULProgress,
    FTPSPIA_ULFinishUpload,
    FTPSPIA_ULSendData,
    FTPSPIA_DLProgress,
    FTPSPIA_DLFinishDownload,
    FTPSPIA_DLSendData,
    FTPSPIA_GetDownloadFilename,
};

/*******************************************************************************
 * NAME:
 *    FTPSPIA_RegisterFileTransfer
 *
 * SYNOPSIS:
 *    PG_BOOL FTPSPIA_RegisterFileTransfer(const struct FileTransferInfo *Info);
 *
 * PARAMETERS:
 *    Info [I] -- Info about this file transfer protocol.  A copy of this
 *                pointer is taken so the data must remain static.
 *                The fields of 'Info' are:
 *          IDStr -- The ID name of the file transfer.  This is a name (without
 *                   spaces) that the system uses to find this file transfer
 8                   handler between sessions.  It needs to be unique for this
 *                   processor.
 *          DisplayName -- The name that is displayed to the user.
 *          CBs -- The callbacks for this file transfer protocol.  See below.
 *
 * FUNCTION:
 *    This function registers a new file transfer protocol.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  The user has been informed.
 *
 * CALLBACKS:
 *==============================================================================
 *    NAME:
 *      AllocateData
 *
 *    SYNOPSIS:
 *      t_FTPHandlerDataType *AllocateData(void);
 *
 *    PARAMETERS:
 *      NONE
 *
 *    FUNCTION:
 *      This function allocates any needed data for this file transfer protocol.
 *
 *    RETURNS:
 *      A pointer to the data, NULL if there was an error.
 *==============================================================================
 *    NAME:
 *      FreeData
 *
 *    SYNOPSIS:
 *      void FreeData(t_FTPHandlerDataType *DataHandle);
 *
 *    PARAMETERS:
 *      DataHandle [I] -- The data handle to free.  This will need to be
 *                        case to your internal data type before you use it.
 *
 *    FUNCTION:
 *      This function frees the memory allocated with AllocateData().
 *
 *    RETURNS:
 *      NONE
 *==============================================================================
 * NAME:
 *    AllocOptionsWidgets
 *
 * SYNOPSIS:
 *    t_FTPOptionsWidgetsType *AllocOptionsWidgets(
 *          t_WidgetSysHandle *WidgetHandle,t_PIKVList *Options);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to send to the widgets
 *    Options [I] -- The options to add widgets for
 *
 * FUNCTION:
 *    This function adds options widgets to a container widget.  These are
 *    options for this file transfer protocol.
 *
 * RETURNS:
 *    The private options data that you want to use.  This is a private
 *    structure that you allocate and then cast to
 *    (t_FTPOptionsWidgetsType *) when you return.  It's up to you what
 *    you want to do with this data (if you do not want to use it return
 *    a fixed int set to 1, and ignore it in FreeOptionsWidgets).  If you
 *    return NULL it is considered an error.
 *
 * NOTES:
 *    This function must be reentrant.  The system may allocate many sets
 *    of option widgets and free them in any order.
 *
 * SEE ALSO:
 *    FreeOptionsWidgets(), StoreOptions()
 *==============================================================================
 * NAME:
 *    FreeOptionsWidgets
 *
 * SYNOPSIS:
 *    void FreeOptionsWidgets(t_FTPOptionsWidgetsType *FTPOptions);
 *
 * PARAMETERS:
 *    FTPOptions [I] -- The options data that was allocated with
 *          AllocOptionsWidgets().
 *
 * FUNCTION:
 *    Frees the widgets added with AllocOptionsWidgets()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocOptionsWidgets()
 *==============================================================================
 * NAME:
 *    StoreOptions
 *
 * SYNOPSIS:
 *    void StoreOptions(t_FTPOptionsWidgetsType *FTPOptions,
 *          t_PIKVList *Options);
 *
 * PARAMETERS:
 *    FTPOptions [I] -- The options data that was allocated with
 *          AllocOptionsWidgets().
 *    Options [O] -- The options for this protocol.
 *
 * FUNCTION:
 *    This function takes the widgets added with AllocOptionsWidgets() and
 *    stores them is a key/value pair list in 'Options'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocOptionsWidgets()
 *==============================================================================
 * NAME:
 *    StartUpload
 *
 * SYNOPSIS:
 *    PG_BOOL StartUpload(t_FTPSystemData *SysHandle,
 *          t_FTPHandlerDataType *DataHandle,const char *FilenameWithPath,
 *          const char *FilenameOnly,uint64_t FileSize,t_PIKVList *Options);
 *
 * PARAMETERS:
 *    SysHandle [I] -- An handle to be passed back to the file transfer protocol
 *                     system through the 'struct FileTransferHandlerAPI' API.
 *    DataHandle [I] -- An handle to the driver's data that was allocated with
 *                      AllocateData().
 *    FilenameWithPath [I] -- The full filename and path
 *    FilenameOnly [I] -- The filename without the path.
 *    FileSize [I] -- The number of bytes in the file to upload
 *    Options [I] -- The options to use for this transfer
 *
 * FUNCTION:
 *    This function is called to start a transfer.  The driver should
 *    init the upload and send the first packet.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    StartDownload
 *
 * SYNOPSIS:
 *    PG_BOOL StartDownload(t_FTPSystemData *SysHandle,
 *          t_FTPHandlerDataType *DataHandle,t_PIKVList *Options);
 *
 * PARAMETERS:
 *    SysHandle [I] -- An handle to be passed back to the file transfer protocol
 *                     system through the 'struct FileTransferHandlerAPI' API.
 *    DataHandle [I] -- An handle to the driver's data that was allocated with
 *                      AllocateData().
 *    Options [I] -- The options to use for this transfer
 *
 * FUNCTION:
 *    This function is called to start a download transfer.  The driver should
 *    init the download and ready for the first packet.
 *
 *    This may also want to call the GetDownloadFilename() function to get the
 *    filename of where to save the data.  However this maybe delayed until
 *    you have the first packet with a filename in it.  You can then
 *    call GetDownloadFilename() with this filename to know what the user
 *    wants to save the file as.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    AbortTransfer
 *
 * SYNOPSIS:
 *    void AbortTransfer(t_FTPSystemData *SysHandle,
 *              t_FTPHandlerDataType *DataHandle);
 *
 * PARAMETERS:
 *    SysHandle [I] -- An handle to be passed back to the file transfer protocol
 *                     system through the 'struct FileTransferHandlerAPI' API.
 *    DataHandle [I] -- An handle to the driver's data that was allocated with
 *                      AllocateData().
 *
 * FUNCTION:
 *    Abort the current transfer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    ULTimeout
 *
 * SYNOPSIS:
 *    void ULTimeout(t_FTPSystemData *SysHandle,
 *              t_FTPHandlerDataType *DataHandle);
 *
 * PARAMETERS:
 *    SysHandle [I] -- An handle to be passed back to the file transfer protocol
 *                     system through the 'struct FileTransferHandlerAPI' API.
 *    DataHandle [I] -- An handle to the driver's data that was allocated with
 *                      AllocateData().
 *
 * FUNCTION:
 *    This function is called when the timeout timer (set with SetTimeout())
 *    goes off.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    RxData
 *
 * SYNOPSIS:
 *    PG_BOOL RxData(t_FTPSystemData *SysHandle,
 *          t_FTPHandlerDataType *DataHandle,uint8_t *RxData,uint32_t Bytes);
 *
 * PARAMETERS:
 *    SysHandle [I] -- An handle to be passed back to the file transfer protocol
 *                     system through the 'struct FileTransferHandlerAPI' API.
 *    DataHandle [I] -- An handle to the driver's data that was allocated with
 *                      AllocateData().
 *    RxData [I] -- A block with the data that was rx'ed in it.
 *    Bytes [I] -- The number of bytes in 'RxData'
 *
 * FUNCTION:
 *    This function is called when new data comes in the connection.
 *
 * RETURNS:
 *    true -- Do not echo the data
 *    false -- Go ahead and echo the data
 *
 * SEE ALSO:
 *    
 *==============================================================================
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL FTPSPIA_RegisterFileTransferProtocol(const struct FTPHandlerInfo *Info)
{
    try
    {
        /* Make sure this handler doesn't need a newer version of our
           API than we can support */
        if(Info->FTPS_APIVersion>FTPS_API_VERSION_1)
        {
            /* Ok, the handler is newer than we are */
            throw("Plugin needs a newer version of " WHIPPYTERM_NAME);
        }

        /* Make sure the handler doesn't use a newer version of their API
           than what we can use (maybe it will work but maybe they need
           whatever the new functions provide) */
        if(Info->FileTransferHandlerAPIVersion>
                FILE_TRANSFER_HANDLER_API_VERSION_1)
        {
            /* Ok, the handler is newer than we are */
            throw("Plugin needs a newer version of " WHIPPYTERM_NAME);
        }

        m_FTPHandlersList.push_back(Info);

        /* Register this plugin with system */
        RegisterPluginWithSystem(Info->IDStr);
    }
    catch(const char *Msg)
    {
        string ErrorMsg;
        ErrorMsg="Failed to register FTP plugin \"";
        ErrorMsg+=Info->DisplayName;
        ErrorMsg+="\"\n";
        ErrorMsg+=Msg;
        UIAsk("Failed",ErrorMsg,e_AskBox_Error,e_AskBttns_Ok);
        return false;
    }
    catch(...)
    {
        string ErrorMsg;
        ErrorMsg="Failed to register FTP plugin \"";
        ErrorMsg+=Info->DisplayName;
        ErrorMsg+="\"";
        UIAsk("Failed",ErrorMsg,e_AskBox_Error,e_AskBttns_Ok);
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    FTPSPIA_SetTimeout
 *
 * SYNOPSIS:
 *    static void FTPSPIA_SetTimeout(t_FTPSystemData *SysHandle,
 *              uint32_t MSec);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *    MSec [I] -- The number of ms before the timeout timer goes off.  Although
 *                this is in ms the actual timer maybe much larger (maybe even
 *                1 second) and very jittery.
 *
 * FUNCTION:
 *    This function sets the timer used for packet timeouts.  Each driver
 *    has an idea of how long it should wait before it considers a packet
 *    has been missed.  This sets the about of time for this.
 *
 *    When the timer goes off the system calls the driver callback function
 *    Timeout().
 *
 *    It's up to the driver to decide what to do with this timer.  Maybe
 *    resend the last packet?
 *
 *    The drive may decide not to use timeout's.  If the driver does not
 *    call this function then timeouts will not be produced.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void FTPSPIA_SetTimeout(t_FTPSystemData *SysHandle,uint32_t MSec)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;

    if(RealFData->Con==NULL)
        return;

    /* Tell the connection to set the timeout */
    RealFData->Con->FileTransSetTimeout(MSec);
}

/*******************************************************************************
 * NAME:
 *    FTPSPIA_RestartTimeout
 *
 * SYNOPSIS:
 *    static void FTPSPIA_RestartTimeout(t_FTPSystemData *SysHandle);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *
 * FUNCTION:
 *    This function is called to restart the timeout timer.  You should
 *    call this every time you send a packet.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void FTPSPIA_RestartTimeout(t_FTPSystemData *SysHandle)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;

    if(RealFData->Con==NULL)
        return;

    /* Tell the connection to set the timeout */
    RealFData->Con->FileTransRestartTimeout();
}

/*******************************************************************************
 * NAME:
 *    FTPSPIA_ULProgress
 *
 * SYNOPSIS:
 *    void FTPSPIA_ULProgress(t_FTPSystemData *SysHandle,
 *              uint64_t BytesTransfered);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *    BytesTransfered [I] -- The number of bytes transfered
 *
 * FUNCTION:
 *    This function updates the progress of the file transfer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void FTPSPIA_ULProgress(t_FTPSystemData *SysHandle,
        uint64_t BytesTransfered)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;

    if(RealFData->Con==NULL)
        return;

    /* Tell the connection that we have progress */
    RealFData->Con->UploadSetNumberOfBytesSent(BytesTransfered);
}

/*******************************************************************************
 * NAME:
 *    FTPSPIA_ULSendData
 *
 * SYNOPSIS:
 *    static int FTPSPIA_ULSendData(t_FTPSystemData *SysHandle,void *Data,
 *              uint32_t Bytes);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *    Data [I] -- A pointer to the buffer that has the packet in it.  These
 *              are the bytes that will be sent.
 *    Bytes [I] -- The number of bytes to send
 *
 * FUNCTION:
 *    This function sends a packet out the connection.  This is normally the
 *    next packet for upload.
 *
 * RETURNS:
 *    e_FTPS_SendDataRet_Success -- Things worked out
 *    e_FTPS_SendDataRet_Fail -- There was an error
 *    e_FTPS_SendDataRet_Busy -- This is a special case where the system has
 *          said it could not currently send the data, but you can try again
 *          later.  You should act as if the packet was not sent (it wasn't)
 *          and wait for your timeout and send it again.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static int FTPSPIA_ULSendData(t_FTPSystemData *SysHandle,void *Data,
        uint32_t Bytes)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;

    if(RealFData->Con==NULL)
        return false;

    switch(RealFData->Con->WriteData((const uint8_t *)Data,Bytes,
            e_ConWriteSource_Upload))
    {
        case e_ConWrite_Success:
            return e_FTPS_SendDataRet_Success;
        break;
        case e_ConWrite_Failed:
            return e_FTPS_SendDataRet_Fail;
        break;
        case e_ConWrite_Busy:
            return e_FTPS_SendDataRet_Busy;
        break;
        case e_ConWrite_Ignored:
        case e_ConWriteMAX:
        default:
        break;
    }

    return e_FTPS_SendDataRet_Fail;
}

/*******************************************************************************
 * NAME:
 *    FTPSPIA_ULFinishUpload
 *
 * SYNOPSIS:
 *    static void FTPSPIA_ULFinishUpload(t_FTPSystemData *SysHandle,
 *              PG_BOOL Aborted);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *    Aborted [I] -- Are we ending because of an abort or because we are done.
 *
 * FUNCTION:
 *    This function is used to tell the system that the driver has finished
 *    the transfer.  This function will free the resources of this transfer
 *    so you can not longer access your t_FTPHandlerDataType data after calling
 *    this function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void FTPSPIA_ULFinishUpload(t_FTPSystemData *SysHandle,PG_BOOL Aborted)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;

    if(RealFData->Con==NULL)
        return;

    RealFData->Con->FinishedUpload(Aborted);

    /* We are done free the resourses */
    FTPS_FreeCurrentHander(RealFData);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * NAME:
 *    FTPSPIA_DLProgress
 *
 * SYNOPSIS:
 *    void FTPSPIA_DLProgress(t_FTPSystemData *SysHandle,
 *              uint64_t BytesTransfered);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *    BytesTransfered [I] -- The number of bytes transfered
 *
 * FUNCTION:
 *    This function updates the progress of the file transfer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void FTPSPIA_DLProgress(t_FTPSystemData *SysHandle,
        uint64_t BytesTransfered)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;

    if(RealFData->Con==NULL)
        return;

    /* Tell the connection that we have progress */
    RealFData->Con->DownloadSetNumberOfBytesRecv(BytesTransfered);
}

/*******************************************************************************
 * NAME:
 *    FTPSPIA_DLSendData
 *
 * SYNOPSIS:
 *    static int FTPSPIA_DLSendData(t_FTPSystemData *SysHandle,void *Data,
 *              uint32_t Bytes);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *    Data [I] -- A pointer to the buffer that has the packet in it.  These
 *              are the bytes that will be sent.
 *    Bytes [I] -- The number of bytes to send
 *
 * FUNCTION:
 *    This function sends a packet out the connection.  This is normally the
 *    next packet for upload.
 *
 * RETURNS:
 *    e_FTPS_SendDataRet_Success -- Things worked out
 *    e_FTPS_SendDataRet_Fail -- There was an error
 *    e_FTPS_SendDataRet_Busy -- This is a special case the system has
 *          said it could not currently send the data, but you can try again
 *          later.  You should act as if the packet was not sent (it wasn't)
 *          and wait for your timeout and send it again.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static int FTPSPIA_DLSendData(t_FTPSystemData *SysHandle,void *Data,
        uint32_t Bytes)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;

    if(RealFData->Con==NULL)
        return false;

    switch(RealFData->Con->WriteData((const uint8_t *)Data,Bytes,
            e_ConWriteSource_Download))
    {
        case e_ConWrite_Success:
            return e_FTPS_SendDataRet_Success;
        break;
        case e_ConWrite_Failed:
            return e_FTPS_SendDataRet_Fail;
        break;
        case e_ConWrite_Busy:
            return e_FTPS_SendDataRet_Busy;
        break;
        case e_ConWrite_Ignored:
        case e_ConWriteMAX:
        default:
        break;
    }

    return e_FTPS_SendDataRet_Fail;
}

/*******************************************************************************
 * NAME:
 *    FTPSPIA_DLFinishDownload
 *
 * SYNOPSIS:
 *    static void FTPSPIA_DLFinishDownload(t_FTPSystemData *SysHandle,
 *              PG_BOOL Aborted);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *    Aborted [I] -- Are we ending because of an abort or because we are done.
 *
 * FUNCTION:
 *    This function is used to tell the system that the driver has finished
 *    the transfer.  This function will free the resources of this transfer
 *    so you can not longer access your t_FTPHandlerDataType data after calling
 *    this function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void FTPSPIA_DLFinishDownload(t_FTPSystemData *SysHandle,PG_BOOL Aborted)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;

    if(RealFData->Con==NULL)
        return;

    RealFData->Con->FinishedDownload(Aborted);

    /* We are done free the resourses */
    FTPS_FreeCurrentHander(RealFData);
}

/*******************************************************************************
 * NAME:
 *    FTPSPIA_GetDownloadFilename
 *
 * SYNOPSIS:
 *    static const char *FTPSPIA_GetDownloadFilename(t_FTPSystemData *SysHandle,
 *              const char *FileNameHint);
 *
 * PARAMETERS:
 *    SysHandle [I] -- The FTP system handle.
 *    FileNameHint [I] -- The filename that was sent from the other side of
 *          the transfer.  This will be presented to the user to edit and select
 *          a path to save the file in.  You can pass NULL for no hint.
 *
 * FUNCTION:
 *    This function prompts the user to select a filename and path to save
 *    the downloading file to.
 *
 * RETURNS:
 *    A buffer with the full path and filename in it for saving.  This pointer
 *    is valid until the transfer is finished, aborted or this function is
 *    called again.
 *
 *    Returns NULL if the user aborts or there is an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static const char *FTPSPIA_GetDownloadFilename(t_FTPSystemData *SysHandle,
        const char *FileNameHint)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)SysHandle;
    string Path;
    string File;
    string FullPath;

    if(RealFData->Con==NULL)
        return NULL;

    if(!UI_SaveFileReq("Select filename to save to",Path,File,"All Files|*",0))
        return NULL;

    FullPath=UI_ConcatFile2Path(Path,File);

    RealFData->Con->SetDownloadFileName(FullPath.c_str());
    return RealFData->Con->GetDownloadFileName();
}


////////////////////////////////////////////////////////////////////////////////
// /\ 'FTPS_API' functions for the plugins
// \/ File transfer functions for whippy main code
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * NAME:
 *    FTPS_Init
 *
 * SYNOPSIS:
 *    void FTPS_Init(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the File Transfer System.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void FTPS_Init(void)
{
}

/*******************************************************************************
 * NAME:
 *    FTPS_GetListOfFTProtocols
 *
 * SYNOPSIS:
 *    void FTPS_GetListOfFTProtocols(e_FileTransferProtocolModeType Mode,
 *          t_FTPS_ProtocolInfoType &RetData);
 *
 * PARAMETERS:
 *    Mode [I] -- What mode of file transfer protocols to return a list of
 *                      e_FileTransferProtocolMode_Upload -- Upload protocols
 *                      e_FileTransferProtocolMode_Download -- Download
 *                              protocols
 *    RetData [O] -- This is filled with info about the file transfer protocols.
 *                   The data return will be a vector of a structure with
 *                   the following fields:
 *                      IDStr -- The identifier string for this file transfer
 *                               protocol.
 *                      DisplayName -- The name to display to the user
 *                      Tip -- A tool tip for this file transfer protocol
 *                      Help -- Help for this file transfer protocol.  This is
 *                              written in a text markup.
 *
 * FUNCTION:
 *    This function gets a list of file transfer protocols available on the
 *    system.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    The order of the returned data will change from time to time so you
 *    must use the 'IDStr' to identify the file transfer protocol not the
 *    index.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void FTPS_GetListOfFTProtocols(e_FileTransferProtocolModeType Mode,
        t_FTPS_ProtocolInfoType &RetData)
{
    i_FTPHandlersType CurFTP;
    struct FTPS_ProtocolInfo NewEntry;

    RetData.clear();

    for(CurFTP=m_FTPHandlersList.begin();CurFTP!=m_FTPHandlersList.end();
            CurFTP++)
    {
        if((*CurFTP)->Mode==Mode)
        {
            NewEntry.IDStr=(*CurFTP)->IDStr;
            NewEntry.DisplayName=(*CurFTP)->DisplayName;
            NewEntry.Tip=(*CurFTP)->Tip;
            NewEntry.Help=(*CurFTP)->Help;

            RetData.push_back(NewEntry);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    FTPS_AllocFTPData
 *
 * SYNOPSIS:
 *    t_FTPData *FTPS_AllocFTPData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates connection data for file transfers when a new
 *    connection is started.
 *
 * RETURNS:
 *    A pointer to the FTP data for this connection.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_FTPData *FTPS_AllocFTPData(void)
{
    struct RealFTPData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct RealFTPData;

        NewData->HandlerData=NULL;
        NewData->HandlerAPI=NULL;
        NewData->Con=NULL;
        NewData->FTPHandlerInfo=m_FTPHandlersList.end();
    }
    catch(...)
    {
        return NULL;
    }

    return (t_FTPData *)NewData;
}

/*******************************************************************************
 * NAME:
 *    FTPS_FreeFTPData
 *
 * SYNOPSIS:
 *    void FTPS_FreeFTPData(t_FTPData *FData);
 *
 * PARAMETERS:
 *    FData [I] -- The FTP connection data to free.
 *
 * FUNCTION:
 *    This function frees any memory allocated in FTPS_AllocFTPData()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    FTPS_AllocFTPData()
 ******************************************************************************/
void FTPS_FreeFTPData(t_FTPData *FData)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)FData;

    delete RealFData;
}

/*******************************************************************************
 * NAME:
 *    FTPS_SetupCurrentHandler
 *
 * SYNOPSIS:
 *    static bool FTPS_SetupCurrentHandler(struct RealFTPData *RealFData,
 *          i_FTPHandlersType Handler);
 *
 * PARAMETERS:
 *    RealFData [I] -- The data for this FTP connection
 *    Handler [I] -- What handler to setup
 *
 * FUNCTION:
 *    This function sets up a handler for uploading / downloading.  It allocates
 *    memory and sets up pointers so you need to free them when you are done
 *    with them.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    FTPS_FreeCurrentHander()
 ******************************************************************************/
static bool FTPS_SetupCurrentHandler(struct RealFTPData *RealFData,
        i_FTPHandlersType Handler)
{
    RealFData->HandlerAPI=(*Handler)->API;
    RealFData->FTPHandlerInfo=Handler;

    RealFData->HandlerData=NULL;
    if(RealFData->HandlerAPI->AllocateData!=NULL)
    {
        RealFData->HandlerData=RealFData->HandlerAPI->AllocateData();
        if(RealFData->HandlerData==NULL)
            return false;
    }

    /* Tell the system we are using this plugin */
    NotePluginInUse((*RealFData->FTPHandlerInfo)->IDStr);

    return true;
}

/*******************************************************************************
 * NAME:
 *    FTPS_FreeCurrentHander
 *
 * SYNOPSIS:
 *    static void FTPS_FreeCurrentHander(struct RealFTPData *RealFData);
 *
 * PARAMETERS:
 *    RealFData [I] -- The data for this FTP connection
 *
 * FUNCTION:
 *    This function frees any resources allocated when the handler was setup.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    FTPS_SetupCurrentHandler()
 ******************************************************************************/
static void FTPS_FreeCurrentHander(struct RealFTPData *RealFData)
{
    if(RealFData->HandlerAPI==NULL)
        return;

    if(RealFData->HandlerAPI->FreeData!=NULL && RealFData->HandlerData!=NULL)
        RealFData->HandlerAPI->FreeData(RealFData->HandlerData);

    /* Tell the system we are no longer using this plugin */
    UnNotePluginInUse((*RealFData->FTPHandlerInfo)->IDStr);

    RealFData->HandlerAPI=NULL;
    RealFData->HandlerData=NULL;
    RealFData->Con=NULL;
    RealFData->FTPHandlerInfo=m_FTPHandlersList.end();
}

/*******************************************************************************
 * NAME:
 *    FTPS_AllocProtocolOptions()
 *
 * SYNOPSIS:
 *    t_ProtocolOptionsDataType *FTPS_AllocProtocolOptions(
 *          const char *ProtocolStrID,t_UIContainerCtrl *ContainerWidget,
 *          t_KVList &OptionsKeyValues);
 *
 * PARAMETERS:
 *    ProtocolStrID [I] -- The file transfer protocol ID string for what
 *                         protocol to get the options for.
 *    ContainerWidget [I] -- The widget to add the controls to
 *    OptionsKeyValues [I] -- The options to set the new widgets to
 *
 * FUNCTION:
 *    This function allocates and connects the options for the requested
 *    protocol to for UI.
 *
 * RETURNS:
 *    A handle to the protocol options.
 *
 * SEE ALSO:
 *    FTPS_FreeProtocolOptions()
 ******************************************************************************/
t_ProtocolOptionsDataType *FTPS_AllocProtocolOptions(
        const char *ProtocolStrID,t_UIContainerCtrl *ContainerWidget,
        t_KVList &OptionsKeyValues)
{
    t_FTPOptionsWidgetsType *FTPOptions;
    struct FTPOptionsData *FTPOptionData;
    i_FTPHandlersType CurFTP;
    const struct FileTransferHandlerAPI *DrvAPI;

    DrvAPI=NULL;
    FTPOptions=NULL;
    try
    {
        /* Find the protocol */
        for(CurFTP=m_FTPHandlersList.begin();
                CurFTP!=m_FTPHandlersList.end();CurFTP++)
        {
            if(strcmp((*CurFTP)->IDStr,ProtocolStrID)==0)
                break;
        }
        if(CurFTP==m_FTPHandlersList.end())
            throw(0);
        DrvAPI=(*CurFTP)->API;

        if(DrvAPI->AllocOptionsWidgets!=NULL)
        {
            FTPOptions=DrvAPI->AllocOptionsWidgets(
                    (t_WidgetSysHandle *)ContainerWidget,
                    PIS_ConvertKVList2PIKVList(OptionsKeyValues));

            if(FTPOptions==NULL)
                throw(false);
        }

        FTPOptionData=new struct FTPOptionsData;
        FTPOptionData->FTPOptions=FTPOptions;
        FTPOptionData->FTPHandler=CurFTP;
    }
    catch(...)
    {
        if(FTPOptions!=NULL && DrvAPI!=NULL)
        {
            if(DrvAPI->FreeOptionsWidgets!=NULL)
                DrvAPI->FreeOptionsWidgets(FTPOptions);
            FTPOptions=NULL;
        }
        UIAsk("Error","Failed to add file transfer protocol options to UI.",
                e_AskBox_Error,e_AskBttns_Ok);
        return NULL;
    }

    return (t_ProtocolOptionsDataType *)FTPOptionData;
}

/*******************************************************************************
 * NAME:
 *    FTPS_FreeProtocolOptions
 *
 * SYNOPSIS:
 *    void FTPS_FreeProtocolOptions(t_ProtocolOptionsDataType *OptionsHandle);
 *
 * PARAMETERS:
 *    OptionsHandle [I] -- The options allocated with
 *                         FTPS_AllocProtocolOptions() to free
 *
 * FUNCTION:
 *    This function frees the options allocated with FTPS_AllocProtocolOptions()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    FTPS_AllocProtocolOptions()
 ******************************************************************************/
void FTPS_FreeProtocolOptions(t_ProtocolOptionsDataType *OptionsHandle)
{
    struct FTPOptionsData *FTPOptionData=(struct FTPOptionsData *)OptionsHandle;
    const struct FileTransferHandlerAPI *DrvAPI;

    try
    {
        DrvAPI=(*FTPOptionData->FTPHandler)->API;

        if(DrvAPI->FreeOptionsWidgets!=NULL)
        {
            DrvAPI->FreeOptionsWidgets(FTPOptionData->FTPOptions);
        }

        delete FTPOptionData;
    }
    catch(...)
    {
        string ErrorMsg;
        ErrorMsg="Failed to free file transfer protocol options from UI.";
        UIAsk("Error",ErrorMsg,e_AskBox_Error,e_AskBttns_Ok);
    }
}

/*******************************************************************************
 * NAME:
 *    FTPS_StoreOptions
 *
 * SYNOPSIS:
 *    void FTPS_StoreOptions(t_ProtocolOptionsDataType *OptionsHandle,
 *          t_KVList &OptionsKeyValues);
 *
 * PARAMETERS:
 *    OptionsHandle [I] -- The handle that was allocated with
 *                              FTPS_AllocProtocolOptions()
 *    OptionsKeyValues [O] -- The options to set to the UI values.
 *
 * FUNCTION:
 *    This function stores the current protocol options from the UI to
 *    a KVList of options.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    FTPS_AllocConnectionOptions()
 ******************************************************************************/
void FTPS_StoreOptions(t_ProtocolOptionsDataType *OptionsHandle,
        t_KVList &OptionsKeyValues)
{
    struct FTPOptionsData *FTPOptionData=(struct FTPOptionsData *)OptionsHandle;
    const struct FileTransferHandlerAPI *DrvAPI;

    DrvAPI=(*FTPOptionData->FTPHandler)->API;

    if(DrvAPI->StoreOptions!=NULL)
    {
        DrvAPI->StoreOptions(FTPOptionData->FTPOptions,
                PIS_ConvertKVList2PIKVList(OptionsKeyValues));
    }
}

/*******************************************************************************
 * NAME:
 *    FTPS_UploadFile
 *
 * SYNOPSIS:
 *    bool FTPS_UploadFile(t_FTPData *FData,class Connection *ParentCon,
 *          const char *Filename,const char *ProtocolID,t_KVList &Options,
 *          uint64_t FileSize);
 *
 * PARAMETERS:
 *    FData [I] -- The data for this FTP connection
 *    ParentCon [I] -- The parent connection that is used for this transfer.
 *                     This is used to give updates to the transfer.
 *    Filename [I] -- The full filename to the file to send
 *    ProtocolID [I] -- What protocol to use for the transfer
 *    Options [I] -- What options to apply to this transfer
 *    FileSize [I] -- The number of bytes in this file to send
 *
 * FUNCTION:
 *    This function starts a file upload using a protocol driver.
 *
 * RETURNS:
 *    true -- Things worked out.  The tranfer has been started
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool FTPS_UploadFile(t_FTPData *FData,class Connection *ParentCon,
        const char *Filename,const char *ProtocolID,t_KVList &Options,
        uint64_t FileSize)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)FData;
    i_FTPHandlersType CurFTP;
    const char *FilenameOnly;

    try
    {
        RealFData->Con=ParentCon;

        /* Find the protocol */
        for(CurFTP=m_FTPHandlersList.begin();CurFTP!=m_FTPHandlersList.end();
                CurFTP++)
        {
            if(strcmp((*CurFTP)->IDStr,ProtocolID)==0)
                break;
        }
        if(CurFTP==m_FTPHandlersList.end())
            throw(0);

        if(!FTPS_SetupCurrentHandler(RealFData,CurFTP))
            throw(0);

        if(RealFData->HandlerAPI->StartUpload==NULL)
        {
            /* Hu? we have driver that can't start an upload? */
            throw(0);
        }

        FilenameOnly=Basename(Filename);

        if(!RealFData->HandlerAPI->StartUpload((t_FTPSystemData *)RealFData,
                RealFData->HandlerData,Filename,FilenameOnly,FileSize,
                PIS_ConvertKVList2PIKVList(Options)))
        {
            throw(0);
        }

        /* Tell the connection that we have 0 progress */
        RealFData->Con->UploadSetNumberOfBytesSent(0);
    }
    catch(...)
    {
        FTPS_FreeCurrentHander(RealFData);

        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    FTPS_DownloadFile
 *
 * SYNOPSIS:
 *    bool FTPS_DownloadFile(t_FTPData *FData,class Connection *ParentCon,
 *          const char *ProtocolID,t_KVList &Options);
 *
 * PARAMETERS:
 *    FData [I] -- The data for this FTP connection
 *    ParentCon [I] -- The parent connection that is used for this transfer.
 *                     This is used to give updates to the transfer.
 *    Filename [I] -- The full filename to the file to send
 *    ProtocolID [I] -- What protocol to use for the transfer
 *    Options [I] -- What options to apply to this transfer
 *    FileSize [I] -- The number of bytes in this file to send
 *
 * FUNCTION:
 *    This function starts a file upload using a protocol driver.
 *
 * RETURNS:
 *    true -- Things worked out.  The tranfer has been started
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool FTPS_DownloadFile(t_FTPData *FData,class Connection *ParentCon,
        const char *ProtocolID,t_KVList &Options)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)FData;
    i_FTPHandlersType CurFTP;

    try
    {
        RealFData->Con=ParentCon;

        /* Find the protocol */
        for(CurFTP=m_FTPHandlersList.begin();CurFTP!=m_FTPHandlersList.end();
                CurFTP++)
        {
            if(strcmp((*CurFTP)->IDStr,ProtocolID)==0)
                break;
        }
        if(CurFTP==m_FTPHandlersList.end())
            throw(0);

        if(!FTPS_SetupCurrentHandler(RealFData,CurFTP))
            throw(0);

        if(RealFData->HandlerAPI->StartDownload==NULL)
        {
            /* Hu? we have driver that can't start an download? */
            throw(0);
        }

        if(!RealFData->HandlerAPI->StartDownload((t_FTPSystemData *)RealFData,
                RealFData->HandlerData,PIS_ConvertKVList2PIKVList(Options)))
        {
            throw(0);
        }

        /* Update the system so it knowns we have 0 bytes tx'ed */
        RealFData->Con->DownloadSetNumberOfBytesRecv(0);
    }
    catch(...)
    {
        FTPS_FreeCurrentHander(RealFData);

        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    FTPS_AbortTransfer
 *
 * SYNOPSIS:
 *    void FTPS_AbortTransfer(t_FTPData *FData);
 *
 * PARAMETERS:
 *    FData [I] -- The data for this FTP connection
 *
 * FUNCTION:
 *    This function aborts a transfer in progress.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void FTPS_AbortTransfer(t_FTPData *FData)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)FData;

    if(RealFData->HandlerAPI->AbortTransfer!=NULL)
    {
        RealFData->HandlerAPI->AbortTransfer((t_FTPSystemData *)RealFData,
                RealFData->HandlerData);
    }

    /* Free the info about this transfer */
    FTPS_FreeCurrentHander(RealFData);
}

/*******************************************************************************
 * NAME:
 *    FTPS_TimeoutTransfer
 *
 * SYNOPSIS:
 *    void FTPS_TimeoutTransfer(t_FTPData *FData);
 *
 * PARAMETERS:
 *    FData [I] -- The data for this FTP connection
 *
 * FUNCTION:
 *    This function is called to send a timeout to the driver.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void FTPS_TimeoutTransfer(t_FTPData *FData)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)FData;

    if(RealFData->HandlerData==NULL)
        return;

    if(RealFData->HandlerAPI->Timeout!=NULL)
    {
        RealFData->HandlerAPI->Timeout((t_FTPSystemData *)RealFData,
                RealFData->HandlerData);
    }
}

/*******************************************************************************
 * NAME:
 *    FTPS_ProcessIncomingBytes
 *
 * SYNOPSIS:
 *    bool FTPS_ProcessIncomingBytes(t_FTPData *FData,uint8_t *Data,int Bytes);
 *
 * PARAMETERS:
 *    FData [I] -- The data for this FTP connection
 *    Data [I] -- The bytes that where read in from the connection
 *    Bytes [I] -- The number of bytes in 'Data'
 *
 * FUNCTION:
 *    This function is called from the connection with bytes that where
 *    read from the connection.  It passes them on to the driver.
 *
 * RETURNS:
 *    true -- The bytes have been used by the driver
 *    false -- Send the bytes to the screen
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool FTPS_ProcessIncomingBytes(t_FTPData *FData,uint8_t *Data,int Bytes)
{
    struct RealFTPData *RealFData=(struct RealFTPData *)FData;

    if(RealFData->HandlerData==NULL)
        return false;

    if(RealFData->HandlerAPI->RxData!=NULL)
    {
        return RealFData->HandlerAPI->RxData((t_FTPSystemData *)RealFData,
                RealFData->HandlerData,Data,Bytes);
    }
    return false;
}

/*******************************************************************************
 * NAME:
 *    FTPS_InformOfNewPluginInstalled
 *
 * SYNOPSIS:
 *    void FTPS_InformOfNewPluginInstalled(const char *PluginIDStr);
 *
 * PARAMETERS:
 *    PluginIDStr [I] -- The ID string for the plugin that was installed
 *
 * FUNCTION:
 *    This function is called any time a new plugin is installed.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This is not called at started when plugin are loaded, just when a new
 *    plugin is installed.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void FTPS_InformOfNewPluginInstalled(const char *PluginIDStr)
{
}

/*******************************************************************************
 * NAME:
 *    FTPS_InformOfPluginUninstalled
 *
 * SYNOPSIS:
 *    void FTPS_InformOfPluginUninstalled(const char *PluginIDStr);
 *
 * PARAMETERS:
 *    PluginIDStr [I] -- The ID string for the plugin that was removed
 *
 * FUNCTION:
 *    This function is called when a plugin is removed from the system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void FTPS_InformOfPluginUninstalled(const char *PluginIDStr)
{
    i_FTPHandlersType CurFTP;

    /* Remove from the list of available FTP handlers */
    for(CurFTP=m_FTPHandlersList.begin();CurFTP!=m_FTPHandlersList.end();
            CurFTP++)
    {
        if(strcmp((*CurFTP)->IDStr,PluginIDStr)==0)
        {
            break;
        }
    }
    if(CurFTP!=m_FTPHandlersList.end())
    {
        m_FTPHandlersList.erase(CurFTP);

        UnRegisterPluginWithSystem(PluginIDStr);
    }
}
