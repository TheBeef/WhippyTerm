/*******************************************************************************
 * FILENAME: RAWFileUpload.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
#include "RAWFileUpload.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      RAWFileUpload // The name to append on the RegisterPlugin() function for built in version
#define SEND_BLOCK_SIZE         1024    // Send 1k at a time
#define NEEDED_MIN_API_VERSION                  0x00080000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct RAWFileUploadData
{
    FILE *FileHandle;
    uint64_t BytesSent;
    bool Done;
};

/*** FUNCTION PROTOTYPES      ***/
static t_FTPHandlerDataType *RAWFileUpload_AllocateData(void);
static void RAWFileUpload_FreeData(t_FTPHandlerDataType *DataHandle);
static PG_BOOL RAWFileUpload_StartUpload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,const char *FilenameWithPath,
        const char *FilenameOnly,uint64_t FileSize,t_PIKVList *Options);
static void RAWFileUpload_AbortUpload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle);
static void RAWFileUpload_Timeout(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle);

/*** VARIABLE DEFINITIONS     ***/
struct FileTransferHandlerAPI m_RAWFileUploadCBs=
{
    RAWFileUpload_AllocateData,
    RAWFileUpload_FreeData,
    NULL,
    NULL,
    NULL,
    RAWFileUpload_StartUpload,
    NULL,
    RAWFileUpload_AbortUpload,
    RAWFileUpload_Timeout,
    NULL,
};

struct FTPHandlerInfo m_RAWFileUpload_Info=
{
    "RAWFileUpload",
    "Send File",
    "Sends a file without using a protocol.  Bytes are just sent.",
    "Sends a file without using a protocol.  Bytes are just sent.",
    FILE_TRANSFER_HANDLER_API_VERSION_1,
    FTPS_API_VERSION_1,
    &m_RAWFileUploadCBs,
    e_FileTransferProtocolMode_Upload,
};

static const struct PI_UIAPI *m_UIAPI;
static const struct PI_SystemAPI *m_System;
static const struct FTPS_API *m_FTPS;

/*******************************************************************************
 * NAME:
 *    RAWFileUpload_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int RAWFileUpload_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
 *          unsigned int Version);
 *
 * PARAMETERS:
 *    SysAPI [I] -- The main API to WhippyTerm
 *    Version [I] -- What version of WhippyTerm is running.  This is used
 *                   to make sure we are compatible.  This is in the
 *                   Major<<24 | Minor<<16 | Patch<<8 | Letter format
 *
 * FUNCTION:
 *    This function registers this plugin with the system.
 *
 * RETURNS:
 *    0 if we support this version of WhippyTerm, and the minimum version
 *    we need if we are not.
 *
 * NOTES:
 *    This function is normally is called from the RegisterPlugin() when
 *    it is being used as a normal plugin.  As a std plugin it is called
 *    from RegisterStdPlugins() instead.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
/* This needs to be extern "C" because it is the main entry point for the
   plugin system */
extern "C"
{
    unsigned int REGISTER_PLUGIN_FUNCTION(const struct PI_SystemAPI *SysAPI,
            unsigned int Version)
    {
        if(Version<NEEDED_MIN_API_VERSION)
            return NEEDED_MIN_API_VERSION;

        m_System=SysAPI;
        m_FTPS=SysAPI->GetAPI_FileTransfersProtocol();
        m_UIAPI=m_FTPS->GetAPI_UI();

        m_FTPS->RegisterFileTransferProtocol(&m_RAWFileUpload_Info);

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    RAWFileUpload_AllocateData
 *
 * SYNOPSIS:
 *    t_FTPHandlerDataType *RAWFileUpload_AllocateData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates any needed data for this input filter.
 *
 * RETURNS:
 *    A pointer to the data, NULL if there was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_FTPHandlerDataType *RAWFileUpload_AllocateData(void)
{
    struct RAWFileUploadData *Data;
    Data=(struct RAWFileUploadData *)malloc(sizeof(struct RAWFileUploadData));
    if(Data==NULL)
        return NULL;

    Data->FileHandle=NULL;
    Data->Done=false;
    Data->BytesSent=0;

    return (t_FTPHandlerDataType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    RAWFileUpload_FreeData
 *
 *  SYNOPSIS:
 *    void RAWFileUpload_FreeData(t_FTPHandlerDataType *DataHandle);
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to free.  This will need to be
 *                      case to your internal data type before you use it.
 *
 *  FUNCTION:
 *    This function frees the memory allocated with AllocateData().
 *
 *  RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RAWFileUpload_FreeData(t_FTPHandlerDataType *DataHandle)
{
    struct RAWFileUploadData *Data=(struct RAWFileUploadData *)DataHandle;

    if(Data->FileHandle!=NULL)
        fclose(Data->FileHandle);

    free(Data);
}

/*******************************************************************************
 * NAME:
 *    RAWFileUpload_StartUpload
 *
 * SYNOPSIS:
 *    PG_BOOL RAWFileUpload_StartUpload(t_FTPSystemData *SysHandle,
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
 ******************************************************************************/
static PG_BOOL RAWFileUpload_StartUpload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,const char *FilenameWithPath,
        const char *FilenameOnly,uint64_t FileSize,t_PIKVList *Options)
{
    struct RAWFileUploadData *Data=(struct RAWFileUploadData *)DataHandle;
    uint8_t Block[SEND_BLOCK_SIZE];
    int Bytes;

    Data->Done=false;
    Data->BytesSent=0;

    Data->FileHandle=fopen(FilenameWithPath,"rb");
    if(Data->FileHandle==NULL)
        return false;

    /* Read the first block to send */
    Bytes=fread(Block,1,SEND_BLOCK_SIZE,Data->FileHandle);
    if(Bytes>0)
    {
        /* Send the block */
        switch(m_FTPS->ULSendData(SysHandle,Block,Bytes))
        {
            case e_FTPS_SendDataRet_Success:
                Data->BytesSent+=Bytes;
                if(Bytes<SEND_BLOCK_SIZE)
                {
                    /* Ok, we are done */
                    Data->Done=true;
                    m_FTPS->ULFinish(SysHandle,false);
                }

            break;
            case e_FTPS_SendDataRet_Fail:
            default:
                Data->Done=true;
                m_FTPS->ULFinish(SysHandle,true);
            break;
            case e_FTPS_SendDataRet_Busy:
                /* Ok, back up to the start of the file and wait for the
                   timeout to resend this block */
                fseek(Data->FileHandle,0,SEEK_SET);
            break;
        }
    }

    if(!Data->Done)
    {
        /* DEBUG PAUL: Not sure how we are going to handle this yet as
           we never rx anything */
        m_FTPS->SetTimeout(SysHandle,1);
    }

    m_FTPS->ULProgress(SysHandle,Data->BytesSent);

    return true;
}

/*******************************************************************************
 * NAME:
 *    RAWFileUpload_AbortUpload
 *
 * SYNOPSIS:
 *    void RAWFileUpload_AbortUpload(t_FTPSystemData *SysHandle,
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
 ******************************************************************************/
static void RAWFileUpload_AbortUpload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle)
{
    struct RAWFileUploadData *Data=(struct RAWFileUploadData *)DataHandle;

    Data->Done=true;
}

/*******************************************************************************
 * NAME:
 *    RAWFileUpload_Timeout
 *
 * SYNOPSIS:
 *    void RAWFileUpload_Timeout(t_FTPSystemData *SysHandle,
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
 ******************************************************************************/
static void RAWFileUpload_Timeout(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle)
{
/* DEBUG PAUL: Send the next block here, we still need to figure out how
   we are going to this for real */

    struct RAWFileUploadData *Data=(struct RAWFileUploadData *)DataHandle;
    uint8_t Block[SEND_BLOCK_SIZE];
    int Bytes;

    if(Data->Done)
        return;

    if(Data->FileHandle==NULL)
        return;

    /* Read the next block to send */
    Bytes=fread(Block,1,SEND_BLOCK_SIZE,Data->FileHandle);
    if(Bytes>0)
    {
        /* Send the block */
        switch(m_FTPS->ULSendData(SysHandle,Block,Bytes))
        {
            case e_FTPS_SendDataRet_Success:
                Data->BytesSent+=Bytes;
                if(Bytes<SEND_BLOCK_SIZE)
                {
                    /* Ok, we are done */
                    Data->Done=true;
                    m_FTPS->ULFinish(SysHandle,false);
                }
            break;
            case e_FTPS_SendDataRet_Fail:
            default:
                Data->Done=true;
                m_FTPS->ULFinish(SysHandle,true);
            break;
            case e_FTPS_SendDataRet_Busy:
                /* Ok, back up and try again on the next timeout */
                fseek(Data->FileHandle,-Bytes,SEEK_CUR);
            break;
            case e_FTPS_SendDataRetMAX:
            break;
        }
    }
    else if(Bytes==0)
    {
        Data->Done=true;
        m_FTPS->ULFinish(SysHandle,false);
    }
    else
    {
        /* Check for an error */
        if(ferror(Data->FileHandle))
        {
            /* Abort */
            Data->Done=true;
            m_FTPS->ULFinish(SysHandle,true);
        }
    }

    m_FTPS->ULProgress(SysHandle,Data->BytesSent);
}

