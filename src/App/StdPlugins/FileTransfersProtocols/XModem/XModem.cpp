/*******************************************************************************
 * FILENAME: XModem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the driver for an XModem transfer (up and down)
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
 *    Paul Hutchinson (21 Mar 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "XModem.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      XModemUpload // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x02010000  // DEBUG PAUL: This needs to be 2.2

#define XMODEM_MAX_PACKET_SIZE              (3+1024+2)
#define XMODEM_STANDARD_PACKET_SIZE         128
#define XMODEM_LARGE_PACKET_SIZE            1024
#define MAX_START_WAIT_TIME                 90  // You have 90 seconds for the rx to startup
#define XMODEM_MAX_NAKS                     5   // Number of nak's that will abort the tran
#define MAX_PACKET_WAIT_TIME                30  // How long do we wait with no after the last packet before we abort

#define XMODEM_DOWNLOAD_START_TIMEOUT       10  // 10 seconds between start chars
#define XMODEM_DOWNLOAD_START_TRYS_BEFORE_FALLBACK          5   // How many times do we try starting the download before we switch from CRC to Checksum

#define XMODEM_DOWNLOAD_WAIT4QUIET_TIMEOUT          2   // 2 seconds
#define XMODEM_DOWNLOAD_MISSING_PACKET_TIMEOUT      10  // 10 seconds

#define XMODEM_SOH                  1
#define XMODEM_STX                  2
#define XMODEM_EOT                  4
#define XMODEM_ACK                  6
#define XMODEM_NAK                  21
#define XMODEM_CAN                  24

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    /* DO NOT REMOVE / REORDER THIS LIST.  They are stored to disk */
    e_XModemMode_Basic          =0,
    e_XModemMode_CRC            =1,
    e_XModemMode_1K             =2,
    e_XModemModeMAX
} e_XModemModeType;

typedef enum
{
    e_XModemDelayedError_None   =0,
    e_XModemDelayedError_Cancel,
    e_XModemDelayedError_EOT,
    e_XModemDelayedErrorMAX
} e_XModemDelayedErrorType;

/* XModem in a nut shell:
SENDER                                      RECEIVER
                                        <-- C
STX 01 FE Data[1024] CRC CRC            -->
                                        <-- ACK
STX 02 FD Data[1024] CRC CRC            -->
                                        <-- ACK
SOH 03 FC Data[128] CRC CRC             -->
                                        <-- ACK
SOH 04 FB Data[100] CPMEOF[28] CRC CRC  -->
                                        <-- ACK
EOT                                     -->
                                        <-- ACK
*/

struct XModemUploadData
{
    FILE *FileHandle;
    uint32_t PacketNum;
    uint8_t PaddingChar;
    int BlockSize;
    bool UseCRC;
    bool Waiting4Start;
    int StartTimeout;
    int NAKCount;
    bool Done;
    bool Aborted;
    bool Waiting4DoneAck;
    int LastPacketTimeout;
    e_XModemDelayedErrorType DelayedError;
    int MaxStartWaitTime;
    int MaxNaks;
    int MaxPacketWaitTime;
    string ErrorStr;
};

typedef enum
{
    e_XModemDownload_StartOfHeader,
    e_XModemDownload_PacketNum,
    e_XModemDownload_InvPacketNum,
    e_XModemDownload_Data,
    e_XModemDownload_ChecksumCRC1,
    e_XModemDownload_CRC2,
    e_XModemDownload_Flush,
    e_XModemDownloadMAX
} e_XModemDownloadType;

struct XModemDownloadData
{
    FILE *FileHandle;
    uint32_t PacketNum;
    uint16_t ChecksumCRC;
    bool UseCRC;
    bool RequestedUseCRC;
    bool Waiting4Start;
    int StartTimeout;
    int StartsSent;
    int ByteCount;
    int ExpectedByteCount;
    uint64_t BytesRx;
    bool Done;
    bool Aborted;
    bool NAKPacket;
    bool IgnorePacket;
    uint8_t RxBlock[XMODEM_MAX_PACKET_SIZE];
    int DownloadState;
    int LastByteTimeout;
    int TimeSinceLastStartChar;
    int LastPacketTimeout;
    int MaxStartWaitTime;
    int MaxNaks;
    int MaxPacketWaitTime;
    string ErrorStr;
};

struct XModem_Widgets
{
    t_WidgetSysHandle *WidgetHandle;
    struct PI_ComboBox *Padding;
    struct PI_RadioBttnGroup *ModeGroup;
    struct PI_RadioBttn *ModeBttnBasic;
    struct PI_RadioBttn *ModeBttnCRC;
    struct PI_RadioBttn *ModeBttn1K;
    struct PI_NumberInput *MaxStartWaitTime;
    struct PI_NumberInput *MaxNAKPackets;
    struct PI_NumberInput *PacketTimeOut;
};

/*** FUNCTION PROTOTYPES      ***/
static uint16_t XModem_CalcCRC(uint8_t *DataPtr,int Bytes);
static uint8_t XModem_CalcChecksum(uint8_t *DataPtr,int Bytes);
static bool XModem_AllocCommonWidgets(struct XModem_Widgets *Widgets,
        t_WidgetSysHandle *WidgetHandle,t_PIKVList *Options);
static void XModem_FreeCommonWidgets(struct XModem_Widgets *Widgets);
static bool XModem_StoreCommonWidgetsOptions(struct XModem_Widgets *Widgets,t_PIKVList *Options);

t_FTPHandlerDataType *XModemUpload_AllocateData(void);
void XModemUpload_FreeData(t_FTPHandlerDataType *DataHandle);
t_FTPOptionsWidgetsType *XModemUpload_AllocOptionsWidgets(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Options);
void XModemUpload_FreeOptionsWidgets(t_FTPOptionsWidgetsType *FTPOptions);
void XModemUpload_StoreOptions(t_FTPOptionsWidgetsType *FTPOptions,t_PIKVList *Options);
static PG_BOOL XModemUpload_StartUpload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,const char *FilenameWithPath,
        const char *FilenameOnly,uint64_t FileSize,t_PIKVList *Options);
static void XModemUpload_AbortUpload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle);
static void XModemUpload_Timeout(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle);
static PG_BOOL XModemUpload_RxData(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,uint8_t *RxData,uint32_t Bytes);
static const char *XModemUpload_GetLastErrorMsg(t_FTPSystemData *SysHandle,t_FTPHandlerDataType *DataHandle);

t_FTPHandlerDataType *XModemDownload_AllocateData(void);
void XModemDownload_FreeData(t_FTPHandlerDataType *DataHandle);
t_FTPOptionsWidgetsType *XModemDownload_AllocOptionsWidgets(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Options);
void XModemDownload_FreeOptionsWidgets(t_FTPOptionsWidgetsType *FTPOptions);
void XModemDownload_StoreOptions(t_FTPOptionsWidgetsType *FTPOptions,t_PIKVList *Options);
static void XModemDownload_AbortDownload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle);
static void XModemDownload_Timeout(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle);
static PG_BOOL XModemDownload_RxData(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,uint8_t *RxData,uint32_t Bytes);
static PG_BOOL XModemDownload_StartDownload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,t_PIKVList *Options);
static void XModemDownload_Setup4NextPacket(struct XModemDownloadData *Data);
static const char *XModemDownload_GetLastErrorMsg(t_FTPSystemData *SysHandle,t_FTPHandlerDataType *DataHandle);

/*** VARIABLE DEFINITIONS     ***/
struct FileTransferHandlerAPI m_XModemUploadCBs=
{
    XModemUpload_AllocateData,
    XModemUpload_FreeData,
    XModemUpload_AllocOptionsWidgets,
    XModemUpload_FreeOptionsWidgets,
    XModemUpload_StoreOptions,
    XModemUpload_StartUpload,
    NULL,
    XModemUpload_AbortUpload,
    XModemUpload_Timeout,
    XModemUpload_RxData,

    /* V2 */
    XModemUpload_GetLastErrorMsg,
};

struct FileTransferHandlerAPI m_XModemDownloadCBs=
{
    XModemDownload_AllocateData,
    XModemDownload_FreeData,
    XModemDownload_AllocOptionsWidgets,
    XModemDownload_FreeOptionsWidgets,
    XModemDownload_StoreOptions,
    NULL,
    XModemDownload_StartDownload,
    XModemDownload_AbortDownload,
    XModemDownload_Timeout,
    XModemDownload_RxData,

    /* V2 */
    XModemDownload_GetLastErrorMsg,
};

struct FTPHandlerInfo m_XModemUpload_Info=
{
    "XModemUpload",
    "XModem",
    "Sends a file using XModem.",
    "Sends a file using XModem.",
    FILE_TRANSFER_HANDLER_API_VERSION_1,
    FTPS_API_VERSION_1,
    &m_XModemUploadCBs,
    e_FileTransferProtocolMode_Upload,
};

struct FTPHandlerInfo m_XModemDownload_Info=
{
    "XModemDownload",
    "XModem",
    "Receive a file using XModem.",
    "Receive a file using XModem.",
    FILE_TRANSFER_HANDLER_API_VERSION_1,
    FTPS_API_VERSION_1,
    &m_XModemDownloadCBs,
    e_FileTransferProtocolMode_Download,
};

static const struct PI_UIAPI *m_UIAPI;
static const struct PI_SystemAPI *m_System;
static const struct FTPS_API *m_FTPS;

/*******************************************************************************
 * NAME:
 *    URLHighlighter_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int URLHighlighter_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
 *          unsigned int Version);
 *
 * PARAMETERS:
 *    SysAPI [I] -- The main API to WhippyTerm
 *    Version [I] -- What version of WhippyTerm is running.  This is used
 *                   to make sure we are compatible.  This is in the
 *                   Major<<24 | Minor<<16 | Rev<<8 | Patch format
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

        /* If we are have the correct experimental API */
        if(m_System->GetExperimentalID()>0 &&
                m_System->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_FTPS->RegisterFileTransferProtocol(&m_XModemUpload_Info);
        m_FTPS->RegisterFileTransferProtocol(&m_XModemDownload_Info);

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    XModem_CalcCRC
 *
 * SYNOPSIS:
 *    static uint16_t XModem_CalcCRC(uint8_t *DataPtr,int Bytes);
 *
 * PARAMETERS:
 *    DataPtr [I] -- The data to calc the CRC for
 *    Bytes [I] -- The number of bytes in 'DataPtr'
 *
 * FUNCTION:
 *    This function calc's the CRC16 for a block of XModem data.
 *
 * RETURNS:
 *    The CRC for this block.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static uint16_t XModem_CalcCRC(uint8_t *DataPtr,int Bytes)
{
    uint16_t crc;
    int i;

    crc=0;
    while(--Bytes>=0)
    {
        crc=crc^(uint16_t)(*DataPtr)<<8;
        DataPtr++;

        for(i=0;i<8;i++)
        {
            if(crc&0x8000)
                crc=crc<<1^0x1021;
            else
                crc=crc<<1;
        }
    }
    return crc&0xFFFF;
}

/*******************************************************************************
 * NAME:
 *    XModem_CalcChecksum
 *
 * SYNOPSIS:
 *    static uint16_t XModem_CalcChecksum(uint8_t *DataPtr,int Bytes);
 *
 * PARAMETERS:
 *    DataPtr [I] -- The data to calc the checksum for
 *    Bytes [I] -- The number of bytes in 'DataPtr'
 *
 * FUNCTION:
 *    This function calc's the checksum for a block of XModem data.
 *
 * RETURNS:
 *    The CRC for this block.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static uint8_t XModem_CalcChecksum(uint8_t *DataPtr,int Bytes)
{
    uint8_t csum;
    int r;

    csum=0;
    for(r=0;r<Bytes;r++)
        csum+=DataPtr[r];
    return csum;
}

/*******************************************************************************
 * NAME:
 *    XModemUpload_AllocateData
 *
 * SYNOPSIS:
 *    t_FTPHandlerDataType *XModemUpload_AllocateData(void);
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
t_FTPHandlerDataType *XModemUpload_AllocateData(void)
{
    struct XModemUploadData *Data;

    try
    {
        Data=new struct XModemUploadData;
        Data->FileHandle=NULL;
    }
    catch(...)
    {
        return NULL;
    }

    return (t_FTPHandlerDataType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    XModemUpload_FreeData
 *
 *  SYNOPSIS:
 *    void XModemUpload_FreeData(t_FTPHandlerDataType *DataHandle);
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
void XModemUpload_FreeData(t_FTPHandlerDataType *DataHandle)
{
    struct XModemUploadData *Data=(struct XModemUploadData *)DataHandle;

    if(Data->FileHandle!=NULL)
        fclose(Data->FileHandle);

    delete Data;
}

t_FTPOptionsWidgetsType *XModemUpload_AllocOptionsWidgets(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Options)
{
    const char *Value;
    struct XModem_Widgets *Widgets;
    char buff[100];
    int r;

    Widgets=NULL;
    try
    {
        Widgets=new struct XModem_Widgets;
        Widgets->Padding=NULL;
        Widgets->MaxStartWaitTime=NULL;
        Widgets->MaxNAKPackets=NULL;
        Widgets->PacketTimeOut=NULL;

        Widgets->WidgetHandle=WidgetHandle;

        if(!XModem_AllocCommonWidgets(Widgets,WidgetHandle,Options))
            throw(0);

        /* Padding char */
        Widgets->Padding=m_UIAPI->AddComboBox(WidgetHandle,false,
                "Padding Char",NULL,NULL);
        if(Widgets->Padding==NULL)
            throw(0);
        m_UIAPI->ClearComboBox(WidgetHandle,Widgets->Padding->Ctrl);
        for(r=0;r<256;r++)
        {
            sprintf(buff,"%02X",r);
            if(r>=' ' && r<127)
            {
                strcat(buff," (");
                buff[strlen(buff)+1]=0;
                buff[strlen(buff)]=r;
                strcat(buff,")");
            }
            m_UIAPI->AddItem2ComboBox(WidgetHandle,Widgets->Padding->Ctrl,buff,r);
        }

        /* Set things to last used */
        Value=m_System->KVGetItem(Options,"Padding");
        if(Value!=NULL)
        {
            m_UIAPI->SetComboBoxSelectedEntry(WidgetHandle,
                    Widgets->Padding->Ctrl,atoi(Value));
        }
        else
        {
            m_UIAPI->SetComboBoxSelectedEntry(WidgetHandle,
                    Widgets->Padding->Ctrl,26);   // 26 is the default padding char
        }

        Widgets->MaxNAKPackets=m_UIAPI->AddNumberInput(WidgetHandle,
                "Max number of failed blocks",NULL,NULL);
        if(Widgets->MaxNAKPackets==NULL)
            throw(0);
        m_UIAPI->SetNumberInputMinMax(WidgetHandle,
                Widgets->MaxNAKPackets->Ctrl,1,20);
        m_UIAPI->SetNumberInputValue(WidgetHandle,
                Widgets->MaxNAKPackets->Ctrl,XMODEM_MAX_NAKS);
        Value=m_System->KVGetItem(Options,"XMODEM_MAX_NAKS");
        if(Value!=NULL)
        {
            m_UIAPI->SetNumberInputValue(WidgetHandle,
                    Widgets->MaxNAKPackets->Ctrl,atoi(Value));
        }
        else
        {
            m_UIAPI->SetNumberInputValue(WidgetHandle,
                    Widgets->MaxNAKPackets->Ctrl,XMODEM_MAX_NAKS);
        }
    }
    catch(...)
    {
        if(Widgets!=NULL)
        {
            XModem_FreeCommonWidgets(Widgets);

            if(Widgets->MaxNAKPackets!=NULL)
            {
                m_UIAPI->FreeNumberInput(WidgetHandle,Widgets->MaxNAKPackets);
            }

            if(Widgets->Padding!=NULL)
                m_UIAPI->FreeComboBox(WidgetHandle,Widgets->Padding);

            delete Widgets;
        }
        return NULL;
    }

    return (t_FTPOptionsWidgetsType *)Widgets;
}

static bool XModem_AllocCommonWidgets(struct XModem_Widgets *Widgets,
        t_WidgetSysHandle *WidgetHandle,t_PIKVList *Options)
{
    const char *Value;

    Widgets->ModeBttn1K=NULL;
    Widgets->ModeBttnCRC=NULL;
    Widgets->ModeBttnBasic=NULL;
    Widgets->ModeGroup=NULL;
    try
    {
        /* Mode */
        Widgets->ModeGroup=m_UIAPI->AllocRadioBttnGroup(WidgetHandle,"Mode");
        if(Widgets->ModeGroup==NULL)
            throw(0);
        Widgets->ModeBttnBasic=m_UIAPI->AddRadioBttn(WidgetHandle,Widgets->ModeGroup,"XModem",NULL,NULL);
        Widgets->ModeBttnCRC=m_UIAPI->AddRadioBttn(WidgetHandle,Widgets->ModeGroup,"XModem-CRC",NULL,NULL);
        Widgets->ModeBttn1K=m_UIAPI->AddRadioBttn(WidgetHandle,Widgets->ModeGroup,"XModem-1K",NULL,NULL);
        if(Widgets->ModeBttnBasic==NULL)
            throw(0);
        if(Widgets->ModeBttnCRC==NULL)
            throw(0);
        if(Widgets->ModeBttn1K==NULL)
            throw(0);

        /* Set things to last used */
        Value=m_System->KVGetItem(Options,"Mode");
        if(Value!=NULL)
        {
            switch(atoi(Value))
            {
                case e_XModemMode_Basic:
                    m_UIAPI->SetRadioBttnChecked(WidgetHandle,
                            Widgets->ModeBttnBasic,true);
                break;
                case e_XModemMode_CRC:
                default:
                    m_UIAPI->SetRadioBttnChecked(WidgetHandle,
                            Widgets->ModeBttnCRC,true);
                break;
                case e_XModemMode_1K:
                    m_UIAPI->SetRadioBttnChecked(WidgetHandle,
                            Widgets->ModeBttn1K,true);
                break;
            }
        }
        else
        {
            m_UIAPI->SetRadioBttnChecked(WidgetHandle,Widgets->ModeBttnCRC,
                    true);
        }

        Widgets->MaxStartWaitTime=m_UIAPI->AddNumberInput(WidgetHandle,
                "Max start wait time (seconds)",NULL,NULL);
        if(Widgets->MaxStartWaitTime==NULL)
            throw(0);
        m_UIAPI->SetNumberInputMinMax(WidgetHandle,
                Widgets->MaxStartWaitTime->Ctrl,5,120);   // 5s-120s
        m_UIAPI->SetNumberInputValue(WidgetHandle,
                Widgets->MaxStartWaitTime->Ctrl,MAX_START_WAIT_TIME);
        Value=m_System->KVGetItem(Options,"MAX_START_WAIT_TIME");
        if(Value!=NULL)
        {
            m_UIAPI->SetNumberInputValue(WidgetHandle,
                    Widgets->MaxStartWaitTime->Ctrl,atoi(Value));
        }
        else
        {
            m_UIAPI->SetNumberInputValue(WidgetHandle,
                    Widgets->MaxStartWaitTime->Ctrl,MAX_START_WAIT_TIME);
        }

        Widgets->PacketTimeOut=m_UIAPI->AddNumberInput(WidgetHandle,
                "Packet time out (seconds)",NULL,NULL);
        if(Widgets->PacketTimeOut==NULL)
            throw(0);
        m_UIAPI->SetNumberInputMinMax(WidgetHandle,
                Widgets->PacketTimeOut->Ctrl,1,300);
        m_UIAPI->SetNumberInputValue(WidgetHandle,
                Widgets->PacketTimeOut->Ctrl,MAX_PACKET_WAIT_TIME);
        Value=m_System->KVGetItem(Options,"MAX_PACKET_WAIT_TIME");
        if(Value!=NULL)
        {
            m_UIAPI->SetNumberInputValue(WidgetHandle,
                    Widgets->PacketTimeOut->Ctrl,atoi(Value));
        }
        else
        {
            m_UIAPI->SetNumberInputValue(WidgetHandle,
                    Widgets->PacketTimeOut->Ctrl,MAX_PACKET_WAIT_TIME);
        }
    }
    catch(...)
    {
        return false;
    }

    return true;
}

void XModemUpload_FreeOptionsWidgets(t_FTPOptionsWidgetsType *FTPOptions)
{
    struct XModem_Widgets *Widgets=(struct XModem_Widgets *)FTPOptions;

    XModem_FreeCommonWidgets(Widgets);

    if(Widgets->MaxNAKPackets!=NULL)
    {
        m_UIAPI->FreeNumberInput(Widgets->WidgetHandle,
                Widgets->MaxNAKPackets);
    }

    if(Widgets->Padding!=NULL)
        m_UIAPI->FreeComboBox(Widgets->WidgetHandle,Widgets->Padding);
    Widgets->Padding=NULL;

    delete Widgets;
}

static void XModem_FreeCommonWidgets(struct XModem_Widgets *Widgets)
{
    if(Widgets->PacketTimeOut!=NULL)
    {
        m_UIAPI->FreeNumberInput(Widgets->WidgetHandle,
                Widgets->PacketTimeOut);
    }

    if(Widgets->MaxStartWaitTime!=NULL)
    {
        m_UIAPI->FreeNumberInput(Widgets->WidgetHandle,
                Widgets->MaxStartWaitTime);
    }

    if(Widgets->ModeBttn1K!=NULL)
        m_UIAPI->FreeRadioBttn(Widgets->WidgetHandle,Widgets->ModeBttn1K);
    if(Widgets->ModeBttnCRC!=NULL)
        m_UIAPI->FreeRadioBttn(Widgets->WidgetHandle,Widgets->ModeBttnCRC);
    if(Widgets->ModeBttnBasic!=NULL)
        m_UIAPI->FreeRadioBttn(Widgets->WidgetHandle,Widgets->ModeBttnBasic);
    if(Widgets->ModeGroup!=NULL)
        m_UIAPI->FreeRadioBttnGroup(Widgets->WidgetHandle,Widgets->ModeGroup);

    Widgets->PacketTimeOut=NULL;
    Widgets->MaxStartWaitTime=NULL;
    Widgets->ModeBttn1K=NULL;
    Widgets->ModeBttnCRC=NULL;
    Widgets->ModeBttnBasic=NULL;
    Widgets->ModeGroup=NULL;
}

void XModemUpload_StoreOptions(t_FTPOptionsWidgetsType *FTPOptions,t_PIKVList *Options)
{
    struct XModem_Widgets *Widgets=(struct XModem_Widgets *)FTPOptions;
    uintptr_t Value;
    char buff[100];

    if(Widgets->Padding==NULL || Widgets->MaxNAKPackets==NULL)
    {
        return;
    }

    m_System->KVClear(Options);

    if(!XModem_StoreCommonWidgetsOptions(Widgets,Options))
        return;

    Value=m_UIAPI->GetComboBoxSelectedEntry(Widgets->WidgetHandle,
            Widgets->Padding->Ctrl);
    sprintf(buff,"%" PRIuPTR,Value);
    m_System->KVAddItem(Options,"Padding",buff);

    Value=m_UIAPI->GetNumberInputValue(Widgets->WidgetHandle,
            Widgets->MaxNAKPackets->Ctrl);
    sprintf(buff,"%" PRIuPTR,Value);
    m_System->KVAddItem(Options,"XMODEM_MAX_NAKS",buff);

}

static bool XModem_StoreCommonWidgetsOptions(struct XModem_Widgets *Widgets,t_PIKVList *Options)
{
    uintptr_t Value;
    char buff[100];

    if(Widgets->ModeBttnBasic==NULL || Widgets->ModeBttnCRC==NULL ||
            Widgets->ModeBttn1K==NULL || Widgets->MaxStartWaitTime==NULL ||
            Widgets->PacketTimeOut==NULL)
    {
        return false;
    }

    Value=e_XModemMode_CRC;
    if(m_UIAPI->IsRadioBttnChecked(Widgets->WidgetHandle,Widgets->ModeBttnBasic))
        Value=e_XModemMode_Basic;
    if(m_UIAPI->IsRadioBttnChecked(Widgets->WidgetHandle,Widgets->ModeBttnCRC))
        Value=e_XModemMode_CRC;
    if(m_UIAPI->IsRadioBttnChecked(Widgets->WidgetHandle,Widgets->ModeBttn1K))
        Value=e_XModemMode_1K;

    sprintf(buff,"%" PRIuPTR,Value);
    m_System->KVAddItem(Options,"Mode",buff);

    Value=m_UIAPI->GetNumberInputValue(Widgets->WidgetHandle,
            Widgets->MaxStartWaitTime->Ctrl);
    sprintf(buff,"%" PRIuPTR,Value);
    m_System->KVAddItem(Options,"MAX_START_WAIT_TIME",buff);

    Value=m_UIAPI->GetNumberInputValue(Widgets->WidgetHandle,
            Widgets->PacketTimeOut->Ctrl);
    sprintf(buff,"%" PRIuPTR,Value);
    m_System->KVAddItem(Options,"MAX_PACKET_WAIT_TIME",buff);

    return true;
}

/*******************************************************************************
 * NAME:
 *    XModemUpload_StartUpload
 *
 * SYNOPSIS:
 *    PG_BOOL XModemUpload_StartUpload(t_FTPSystemData *SysHandle,
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
static PG_BOOL XModemUpload_StartUpload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,const char *FilenameWithPath,
        const char *FilenameOnly,uint64_t FileSize,t_PIKVList *Options)
{
    struct XModemUploadData *Data=(struct XModemUploadData *)DataHandle;
    e_XModemModeType Mode;
    const char *Value;

    Data->PaddingChar=26;
    Data->PacketNum=0;
    Data->BlockSize=XMODEM_STANDARD_PACKET_SIZE;
    Data->UseCRC=false;
    Data->Waiting4Start=true;
    Data->StartTimeout=0;
    Data->LastPacketTimeout=0;
    Data->Done=false;
    Data->Aborted=false;
    Data->Waiting4DoneAck=false;
    Data->NAKCount=0;
    Data->DelayedError=e_XModemDelayedError_None;
    Data->MaxStartWaitTime=MAX_START_WAIT_TIME;
    Data->MaxNaks=XMODEM_MAX_NAKS;
    Data->MaxPacketWaitTime=MAX_PACKET_WAIT_TIME;

    Mode=e_XModemMode_CRC;
    Value=m_System->KVGetItem(Options,"Mode");
    if(Value!=NULL)
    {
        Mode=(e_XModemModeType)atoi(Value);
        if(Mode>=e_XModemModeMAX)
            return false;
    }

    Value=m_System->KVGetItem(Options,"Padding");
    if(Value!=NULL)
        Data->PaddingChar=atoi(Value);

    Value=m_System->KVGetItem(Options,"MAX_START_WAIT_TIME");
    if(Value!=NULL)
        Data->MaxStartWaitTime=atoi(Value);

    Value=m_System->KVGetItem(Options,"XMODEM_MAX_NAKS");
    if(Value!=NULL)
        Data->MaxNaks=atoi(Value);

    Value=m_System->KVGetItem(Options,"MAX_PACKET_WAIT_TIME");
    if(Value!=NULL)
        Data->MaxPacketWaitTime=atoi(Value);

    Data->FileHandle=fopen(FilenameWithPath,"rb");
    if(Data->FileHandle==NULL)
        return false;

    switch(Mode)
    {
        case e_XModemMode_Basic:
        case e_XModemModeMAX:
            Data->BlockSize=XMODEM_STANDARD_PACKET_SIZE;
            Data->UseCRC=false;
        break;
        case e_XModemMode_CRC:
        default:
            Data->BlockSize=XMODEM_STANDARD_PACKET_SIZE;
            Data->UseCRC=true;
        break;
        case e_XModemMode_1K:
            Data->BlockSize=XMODEM_LARGE_PACKET_SIZE;
            Data->UseCRC=true;
        break;
    }

    /* We need to wait for the rx side to send 'C' or NAK */
    Data->Waiting4Start=true;

    m_FTPS->SetTimeout(SysHandle,1000);

    return true;
}

/*******************************************************************************
 * NAME:
 *    XModemUpload_AbortUpload
 *
 * SYNOPSIS:
 *    void XModemUpload_AbortUpload(t_FTPSystemData *SysHandle,
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
static void XModemUpload_AbortUpload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle)
{
    struct XModemUploadData *Data=(struct XModemUploadData *)DataHandle;
    uint8_t Block[XMODEM_MAX_PACKET_SIZE];
    int SendSize;

    /* Send a whole block to make sure the other side has enough bytes to
       abort */
    SendSize=1;    // Ctrl byte
    SendSize+=Data->BlockSize;
    SendSize+=2;    // Packnum
    if(Data->UseCRC)
        SendSize+=2;    // CRC16
    else
        SendSize+=1;    // checksum

    memset(Block,XMODEM_CAN,SendSize);
    if(m_FTPS->ULSendData(SysHandle,Block,SendSize)!=e_FTPS_SendDataRet_Success)
    {
        /* We failed */
        Data->DelayedError=e_XModemDelayedError_Cancel;
    }

    Data->Done=true;
    Data->Aborted=true;

    if(Data->FileHandle!=NULL)
    {
        fclose(Data->FileHandle);
        Data->FileHandle=NULL;
    }
    Data->ErrorStr="User abort";
}

/*******************************************************************************
 * NAME:
 *    XModemUpload_RxData
 *
 * SYNOPSIS:
 *    static PG_BOOL XModemUpload_RxData(t_FTPSystemData *SysHandle,
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
 ******************************************************************************/
static PG_BOOL XModemUpload_RxData(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,uint8_t *RxData,uint32_t Bytes)
{
    struct XModemUploadData *Data=(struct XModemUploadData *)DataHandle;
    int BytesRead;
    uint32_t b;
    int SendBlockNum;
    uint8_t Block[XMODEM_MAX_PACKET_SIZE];
    uint16_t CRC;
    uint8_t csum;
    int SendSize;

    if(Data->Aborted)
    {
        Block[0]=XMODEM_CAN;
        if(m_FTPS->ULSendData(SysHandle,Block,1)!=e_FTPS_SendDataRet_Success)
            Data->DelayedError=e_XModemDelayedError_Cancel;
        return true;
    }

    if(Data->FileHandle==NULL)
        return false;

    SendBlockNum=-1;    // Don't send anything
    for(b=0;b<Bytes;b++)
    {
        if(Data->Waiting4Start)
        {
            Data->StartTimeout=0;
            if(Data->UseCRC)
            {
                /* Waiting for a 'C' */
                if(RxData[b]=='C')
                {
                    /* Ok, we are ready to start */
                    Data->Waiting4Start=false;
                    SendBlockNum=Data->PacketNum;
                }

                if(RxData[b]==XMODEM_NAK)
                {
                    /* We got an NAK, guess the other side does not support
                       CRC, drop down to the min */
                    Data->UseCRC=false;
                    Data->BlockSize=XMODEM_STANDARD_PACKET_SIZE;

                    Data->Waiting4Start=false;
                    SendBlockNum=Data->PacketNum;
                }
            }
            else
            {
                /* Waiting for a NAK */
                if(RxData[b]==XMODEM_NAK)
                {
                    Data->Waiting4Start=false;
                    SendBlockNum=Data->PacketNum;
                }
            }
        }
        else
        {
            /* See if we where ack'ed or nak'ed */
            if(RxData[b]==XMODEM_ACK)
            {
                if(Data->Waiting4DoneAck)
                {
                    Data->ErrorStr="";
                    m_FTPS->ULFinish(SysHandle,false);
                    return true;    // 'Data' is no longer valid, so we are done
                }
                else if(Data->Done)
                {
                    /* Ok, we are done tell the rx */
                    Block[0]=XMODEM_EOT;
                    if(m_FTPS->ULSendData(SysHandle,Block,1)!=e_FTPS_SendDataRet_Success)
                        Data->DelayedError=e_XModemDelayedError_EOT;
                    Data->Waiting4DoneAck=true;
                }
                else
                {
                    /* Move to the next block */
                    Data->PacketNum++;
                    SendBlockNum=Data->PacketNum;
                    Data->NAKCount=0;

                    m_FTPS->ULProgress(SysHandle,Data->PacketNum*
                            Data->BlockSize);
                }
            }
            if(RxData[b]==XMODEM_NAK)
            {
                if(Data->Waiting4DoneAck)
                {
                    /* Ok, we are done tell them again */
                    Block[0]=XMODEM_EOT;
                    if(m_FTPS->ULSendData(SysHandle,Block,1)!=e_FTPS_SendDataRet_Success)
                        Data->DelayedError=e_XModemDelayedError_EOT;
                    Data->Waiting4DoneAck=true;
                }
                else
                {
                    /* We got nak'ed, resend last block */
                    SendBlockNum=Data->PacketNum;

                    Data->NAKCount++;
                    if(Data->NAKCount>Data->MaxNaks)
                    {
                        memset(Block,XMODEM_CAN,sizeof(Block));
                        if(m_FTPS->ULSendData(SysHandle,Block,sizeof(Block))!=e_FTPS_SendDataRet_Success)
                            Data->DelayedError=e_XModemDelayedError_Cancel;
                        Data->ErrorStr="To many NAK's in a row";
                        m_FTPS->ULFinish(SysHandle,true);
                        return true;    // 'Data' is no longer valid, so we are done
                    }
                }
            }
        }
    }

    if(SendBlockNum>=0)
    {
        Data->LastPacketTimeout=0;

        /* Seek to this packet */
        fseek(Data->FileHandle,SendBlockNum*Data->BlockSize,SEEK_SET);

        SendSize=1;    // Ctrl byte
        SendSize+=Data->BlockSize;
        SendSize+=2;    // Packnum
        if(Data->UseCRC)
            SendSize+=2;    // CRC16
        else
            SendSize+=1;    // checksum

        if(Data->BlockSize==XMODEM_LARGE_PACKET_SIZE)
            Block[0]=XMODEM_STX;
        else
            Block[0]=XMODEM_SOH;
        Block[1]=(Data->PacketNum+1)&0xFF;
        Block[2]=~Block[1];

        /* Load this block into the packet */
        memset(&Block[3],Data->PaddingChar,Data->BlockSize);
        BytesRead=fread(&Block[3],1,Data->BlockSize,Data->FileHandle);

        if(Data->UseCRC)
        {
            CRC=XModem_CalcCRC(&Block[3],Data->BlockSize);
            Block[3+Data->BlockSize+0]=CRC>>8;
            Block[3+Data->BlockSize+1]=CRC&0xFF;
        }
        else
        {
            csum=XModem_CalcChecksum(&Block[3],Data->BlockSize);
            Block[3+Data->BlockSize+0]=csum;
        }
        if(BytesRead>0)
        {
            switch(m_FTPS->ULSendData(SysHandle,Block,SendSize))
            {
                case e_FTPS_SendDataRet_Success:
                    if(BytesRead<Data->BlockSize)
                    {
                        /* Ok, we are done */
                        Data->Done=true;
                    }
                break;
                case e_FTPS_SendDataRet_Fail:
                default:
                    Data->Done=true;
                    Data->ErrorStr="Failed to send the data";
                    m_FTPS->ULFinish(SysHandle,true);
                    return true;    // 'Data' is no longer valid, so we are done
                break;
                case e_FTPS_SendDataRet_Busy:
                    /* Wait for the rx to timeout and nak */
                break;
            }
        }
        else if(BytesRead==0)
        {
            /* We are done */
            Data->Done=true;

            Block[0]=XMODEM_EOT;
            if(m_FTPS->ULSendData(SysHandle,Block,1)!=e_FTPS_SendDataRet_Success)
                Data->DelayedError=e_XModemDelayedError_EOT;
            Data->Waiting4DoneAck=true;
        }
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    XModemUpload_Timeout
 *
 * SYNOPSIS:
 *    static void XModemUpload_Timeout(t_FTPSystemData *SysHandle,
 *          t_FTPHandlerDataType *DataHandle);
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
static void XModemUpload_Timeout(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle)
{
    struct XModemUploadData *Data=(struct XModemUploadData *)DataHandle;
    uint8_t Block[XMODEM_MAX_PACKET_SIZE];
    int SendSize;

    if(Data->Waiting4Start)
    {
        /* Ok, we wait for the rx side to signal us it's ready */
        Data->StartTimeout++;
        if(Data->StartTimeout>Data->MaxStartWaitTime)
        {
            Data->ErrorStr="Timed out waiting to start";
            m_FTPS->ULFinish(SysHandle,true);
            return;    // 'Data' is no longer valid, so we are done
        }
    }
    else
    {
        /* If we wanted to send something but had an error keep trying until
           we get something other and busy */
        switch(Data->DelayedError)
        {
            case e_XModemDelayedError_None:
            case e_XModemDelayedErrorMAX:
            default:
            break;
            case e_XModemDelayedError_Cancel:
                /* Send a whole block to make sure the other side has enough bytes to
                   abort */
                SendSize=1;    // Ctrl byte
                SendSize+=Data->BlockSize;
                SendSize+=2;    // Packnum
                if(Data->UseCRC)
                    SendSize+=2;    // CRC16
                else
                    SendSize+=1;    // checksum
                memset(Block,XMODEM_CAN,SendSize);
                if(m_FTPS->ULSendData(SysHandle,Block,SendSize)!=
                        e_FTPS_SendDataRet_Busy)
                {
                    Data->DelayedError=e_XModemDelayedError_None;
                }
                return;
            break;
            case e_XModemDelayedError_EOT:
                Block[0]=XMODEM_EOT;
                if(m_FTPS->ULSendData(SysHandle,Block,1)!=
                        e_FTPS_SendDataRet_Busy)
                {
                    Data->DelayedError=e_XModemDelayedError_None;
                }
                return;
            break;
        }

        Data->LastPacketTimeout++;
        if(Data->LastPacketTimeout>Data->MaxPacketWaitTime)
        {
            Data->ErrorStr="Timed out waiting for an ACK/NACK";
            m_FTPS->ULFinish(SysHandle,true);
            return;    // 'Data' is no longer valid, so we are done
        }
    }
}

/*******************************************************************************
 * NAME:
 *    XModemUpload_GetLastErrorMsg
 *
 * SYNOPSIS:
 *    const char *XModemUpload_GetLastErrorMsg(t_FTPSystemData *SysHandle,
 *              t_FTPHandlerDataType *DataHandle);
 *
 * PARAMETERS:
 *    SysHandle [I] -- An handle to be passed back to the file transfer protocol
 *                     system through the 'struct FileTransferHandlerAPI' API.
 *    DataHandle [I] -- An handle to the driver's data that was allocated with
 *                      AllocateData().
 *
 * FUNCTION:
 *    This function gets the last error message from the system.  The system
 *    will call then when a function returns an error or abort to find more
 *    details.
 *
 * RETURNS:
 *    A pointer to an error message or NULL if there was no message.  This must
 *    remain valid until the next call to any 'FileTransferHandlerAPI' function.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static const char *XModemUpload_GetLastErrorMsg(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle)
{
    struct XModemUploadData *Data=(struct XModemUploadData *)DataHandle;

    if(Data->ErrorStr=="")
        return NULL;

    return Data->ErrorStr.c_str();
}

////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * NAME:
 *    XModemDownload_AllocateData
 *
 * SYNOPSIS:
 *    t_FTPHandlerDataType *XModemDownload_AllocateData(void);
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
t_FTPHandlerDataType *XModemDownload_AllocateData(void)
{
    struct XModemDownloadData *Data;

    try
    {
        Data=new struct XModemDownloadData;

        Data->FileHandle=NULL;
    }
    catch(...)
    {
        return NULL;
    }

    return (t_FTPHandlerDataType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    XModemDownload_FreeData
 *
 *  SYNOPSIS:
 *    void XModemDownload_FreeData(t_FTPHandlerDataType *DataHandle);
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
void XModemDownload_FreeData(t_FTPHandlerDataType *DataHandle)
{
    struct XModemDownloadData *Data=(struct XModemDownloadData *)DataHandle;

    if(Data->FileHandle!=NULL)
        fclose(Data->FileHandle);

    delete Data;
}

t_FTPOptionsWidgetsType *XModemDownload_AllocOptionsWidgets(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Options)
{
    struct XModem_Widgets *Widgets;

    Widgets=NULL;
    try
    {
        Widgets=new struct XModem_Widgets;
        Widgets->MaxStartWaitTime=NULL;
        Widgets->MaxNAKPackets=NULL;
        Widgets->PacketTimeOut=NULL;

        if(!XModem_AllocCommonWidgets(Widgets,WidgetHandle,Options))
            throw(0);
    }
    catch(...)
    {
        if(Widgets!=NULL)
        {
            XModem_FreeCommonWidgets(Widgets);
            delete Widgets;
        }
        return NULL;
    }

    return (t_FTPOptionsWidgetsType *)Widgets;
}

void XModemDownload_FreeOptionsWidgets(t_FTPOptionsWidgetsType *FTPOptions)
{
    struct XModem_Widgets *Widgets=(struct XModem_Widgets *)FTPOptions;

    XModem_FreeCommonWidgets(Widgets);

    delete Widgets;
}

void XModemDownload_StoreOptions(t_FTPOptionsWidgetsType *FTPOptions,t_PIKVList *Options)
{
    struct XModem_Widgets *Widgets=(struct XModem_Widgets *)FTPOptions;

    m_System->KVClear(Options);

    XModem_StoreCommonWidgetsOptions(Widgets,Options);
}

/*******************************************************************************
 * NAME:
 *    XModemDownload_StartDownload
 *
 * SYNOPSIS:
 *    PG_BOOL XModemDownload_StartDownload(t_FTPSystemData *SysHandle,
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
 ******************************************************************************/
static PG_BOOL XModemDownload_StartDownload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,t_PIKVList *Options)
{
    struct XModemDownloadData *Data=(struct XModemDownloadData *)DataHandle;
    e_XModemModeType Mode;
    const char *Value;
    const char *Filename;
    unsigned char StartChar;

    Data->PacketNum=0;
    Data->UseCRC=false;
    Data->DownloadState=e_XModemDownload_StartOfHeader;
    Data->StartsSent=0;
    Data->StartTimeout=0;
    Data->BytesRx=0;
    Data->Done=false;
    Data->Aborted=false;
    Data->ByteCount=0;
    Data->ExpectedByteCount=0;
    Data->NAKPacket=false;
    Data->IgnorePacket=false;
    Data->LastByteTimeout=0;
    Data->Waiting4Start=true;
    Data->TimeSinceLastStartChar=0;
    Data->LastPacketTimeout=0;
    Data->MaxStartWaitTime=MAX_START_WAIT_TIME;
    Data->MaxNaks=XMODEM_MAX_NAKS;
    Data->MaxPacketWaitTime=MAX_PACKET_WAIT_TIME;
    Data->ErrorStr="";

    Mode=e_XModemMode_CRC;
    Value=m_System->KVGetItem(Options,"Mode");
    if(Value!=NULL)
    {
        Mode=(e_XModemModeType)atoi(Value);
        if(Mode>=e_XModemModeMAX)
            return false;
    }

    Value=m_System->KVGetItem(Options,"MAX_START_WAIT_TIME");
    if(Value!=NULL)
        Data->MaxStartWaitTime=atoi(Value);

    Value=m_System->KVGetItem(Options,"XMODEM_MAX_NAKS");
    if(Value!=NULL)
        Data->MaxNaks=atoi(Value);

    Value=m_System->KVGetItem(Options,"MAX_PACKET_WAIT_TIME");
    if(Value!=NULL)
        Data->MaxPacketWaitTime=atoi(Value);

    Filename=m_FTPS->GetDownloadFilename(SysHandle,NULL);
    if(Filename==NULL)
    {
        /* We aborted */
        return false;
    }

    Data->FileHandle=fopen(Filename,"wb");
    if(Data->FileHandle==NULL)
        return false;

    switch(Mode)
    {
        case e_XModemMode_Basic:
        case e_XModemModeMAX:
            Data->UseCRC=false;
        break;
        case e_XModemMode_CRC:
        default:
            Data->UseCRC=true;
        break;
        case e_XModemMode_1K:
            Data->UseCRC=true;
        break;
    }
    Data->RequestedUseCRC=Data->UseCRC;

    m_FTPS->SetTimeout(SysHandle,1000);

    if(Data->UseCRC)
        StartChar='C';
    else
        StartChar=XMODEM_NAK;
    m_FTPS->DLSendData(SysHandle,&StartChar,1);

    Data->Waiting4Start=true;

    return true;
}

/*******************************************************************************
 * NAME:
 *    XModemDownload_AbortDownload
 *
 * SYNOPSIS:
 *    void XModemDownload_AbortDownload(t_FTPSystemData *SysHandle,
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
static void XModemDownload_AbortDownload(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle)
{
    struct XModemDownloadData *Data=(struct XModemDownloadData *)DataHandle;

    Data->Done=true;
    Data->Aborted=true;

    if(Data->FileHandle!=NULL)
    {
        fclose(Data->FileHandle);
        Data->FileHandle=NULL;
    }

    Data->ErrorStr="User abort";
}

/*******************************************************************************
 * NAME:
 *    XModemDownload_RxData
 *
 * SYNOPSIS:
 *    static PG_BOOL XModemDownload_RxData(t_FTPSystemData *SysHandle,
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
 ******************************************************************************/
static PG_BOOL XModemDownload_RxData(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle,uint8_t *RxData,uint32_t Bytes)
{
    struct XModemDownloadData *Data=(struct XModemDownloadData *)DataHandle;
    uint32_t b;
    uint8_t c;
    bool ProcessPacket;
    uint8_t csum;
    uint16_t CRC;

    if(Data->Aborted || Data->Done)
        return true;

    if(Data->FileHandle==NULL)
        return false;

    for(b=0;b<Bytes;b++)
    {
        c=RxData[b];

        ProcessPacket=false;
        switch(Data->DownloadState)
        {
            case e_XModemDownload_StartOfHeader:
                /* Ignore everything except start */
                if(c==XMODEM_SOH)
                {
                    /* 128 byte block */
                    Data->ExpectedByteCount=XMODEM_STANDARD_PACKET_SIZE;
                    Data->Waiting4Start=false;
                }
                else if(c==XMODEM_STX)
                {
                    /* 1K block */
                    Data->ExpectedByteCount=XMODEM_LARGE_PACKET_SIZE;
                    Data->Waiting4Start=false;
                }
                else if(c==XMODEM_EOT)
                {
                    /* Ok, we are done, ack */
                    c=XMODEM_ACK;
                    m_FTPS->DLSendData(SysHandle,&c,1);

                    Data->ErrorStr="";
                    m_FTPS->DLFinish(SysHandle,false);
                    return true;    // Data has been free'ed we are done
                }
                Data->DownloadState++;
            break;
            case e_XModemDownload_PacketNum:
                /* Ignore packets with the current Packet and the last one */
                if(c==Data->PacketNum)
                {
                    /* We already got this packet, ignore */
                    Data->IgnorePacket=true;
                }
                if(c!=((Data->PacketNum+1)&0xFF))
                {
                    /* Bad Packet Number */
                    Data->NAKPacket=true;
                }
                Data->DownloadState++;
            break;
            case e_XModemDownload_InvPacketNum:
                /* Ignore packets with the current Packet and the last one */
                if(((~c)&0xFF)!=((Data->PacketNum+1)&0xFF))
                {
                    /* Bad Packet Number */
                    Data->NAKPacket=true;
                }
                Data->DownloadState++;
            break;
            case e_XModemDownload_Data:
                Data->RxBlock[Data->ByteCount]=c;
                Data->ByteCount++;
                if(Data->ByteCount>=Data->ExpectedByteCount)
                    Data->DownloadState++;
            break;
            case e_XModemDownload_ChecksumCRC1:
                if(Data->UseCRC)
                {
                    Data->ChecksumCRC=c<<8;
                    Data->DownloadState++;
                }
                else
                {
                    /* Check the checksum */
                    csum=XModem_CalcChecksum(Data->RxBlock,Data->ByteCount);
                    if(c!=csum)
                    {
                        /* Bad block, NAK and try again */
                        Data->NAKPacket=true;
                    }
                    else
                    {
                        ProcessPacket=true;
                    }
                    Data->DownloadState=e_XModemDownload_Flush;
                }
            break;
            case e_XModemDownload_CRC2:
                Data->ChecksumCRC|=c;

                /* Check the CRC */
                CRC=XModem_CalcCRC(Data->RxBlock,Data->ByteCount);
                if(CRC!=Data->ChecksumCRC)
                {
                    /* Bad block, NAK */
                    Data->NAKPacket=true;
                }
                else
                {
                    ProcessPacket=true;
                }
            break;
            case e_XModemDownload_Flush:
            break;
            case e_XModemDownloadMAX:
            default:
            break;
        }
        if(Data->DownloadState>=e_XModemDownloadMAX)
            Data->Aborted=true;

        if(ProcessPacket)
        {
            Data->LastPacketTimeout=0;
            if(!Data->IgnorePacket)
            {
                if(!Data->NAKPacket)
                {
                    Data->BytesRx+=Data->ByteCount;
                    m_FTPS->DLProgress(SysHandle,Data->BytesRx);

                    /* Save packet to disk and move to next packet */
                    if(fwrite(Data->RxBlock,Data->ByteCount,1,
                            Data->FileHandle)!=1)
                    {
                        /* We had a write error */
                        Data->Aborted=true;
                    }
                    else
                    {
                        /* Ack this packet */
                        c=XMODEM_ACK;
                        m_FTPS->DLSendData(SysHandle,&c,1);

                        Data->PacketNum++;
                        XModemDownload_Setup4NextPacket(Data);
                    }
                }
                else
                {
                    /* We will wait for the timeout to have quiet on the line
                       before sending the NAK */
                }
            }
            else
            {
                /* We ignored this packet, setup to rx the packet again 
                   (we need to ACK to say we got it) */
                c=XMODEM_ACK;
                m_FTPS->DLSendData(SysHandle,&c,1);

                XModemDownload_Setup4NextPacket(Data);
            }
        }

        if(Data->Aborted)
        {
            Data->Done=true;
            Data->Aborted=true;

            if(Data->FileHandle!=NULL)
            {
                fclose(Data->FileHandle);
                Data->FileHandle=NULL;
            }
            return true;
        }
        Data->LastByteTimeout=0;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    XModemDownload_Timeout
 *
 * SYNOPSIS:
 *    static void XModemDownload_Timeout(t_FTPSystemData *SysHandle,
 *          t_FTPHandlerDataType *DataHandle);
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
static void XModemDownload_Timeout(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle)
{
    struct XModemDownloadData *Data=(struct XModemDownloadData *)DataHandle;
    unsigned char c;

    if(Data->Waiting4Start)
    {
        Data->TimeSinceLastStartChar++;

        if(Data->TimeSinceLastStartChar>=XMODEM_DOWNLOAD_START_TIMEOUT)
        {
            Data->TimeSinceLastStartChar=0;
            /* Ok, we wait for the tx side to send the first packet.  We keep
               sending the start signal until we get it */
            Data->StartsSent++;

            if(Data->StartsSent>=XMODEM_DOWNLOAD_START_TRYS_BEFORE_FALLBACK)
            {
                if(Data->UseCRC)
                    Data->UseCRC=false;
                else
                    Data->UseCRC=Data->RequestedUseCRC;

                Data->StartsSent=0;
            }

            if(Data->UseCRC)
                c='C';
            else
                c=XMODEM_NAK;
            m_FTPS->DLSendData(SysHandle,&c,1);
        }

        Data->StartTimeout++;
        if(Data->StartTimeout>Data->MaxStartWaitTime)
        {
            Data->ErrorStr="Time out waiting to start";
            m_FTPS->DLFinish(SysHandle,true);
            return;    // Data has been free'ed we are done
        }
    }
    else
    {
        Data->LastByteTimeout++;

        /* Look for quiet */
        if(Data->LastByteTimeout>=XMODEM_DOWNLOAD_WAIT4QUIET_TIMEOUT)
        {
            /* Been x seconds and we haven't gotten any bytes, line is quiet */
            if(!Data->IgnorePacket)
            {
                if(Data->NAKPacket || Data->LastByteTimeout>=
                        XMODEM_DOWNLOAD_MISSING_PACKET_TIMEOUT)
                {
                    /* Ok, we need to nak the last packet */
                    c=XMODEM_NAK;
                    m_FTPS->DLSendData(SysHandle,&c,1);

                    /* Reset for next packet */
                    XModemDownload_Setup4NextPacket(Data);
                }
            }
        }
        Data->LastPacketTimeout++;
        if(Data->LastPacketTimeout>=Data->MaxPacketWaitTime)
        {
            Data->ErrorStr="Time out waiting for packet";
            m_FTPS->DLFinish(SysHandle,true);
            return;    // Data has been free'ed we are done
        }
    }
}

/*******************************************************************************
 * NAME:
 *    XModemDownload_GetLastErrorMsg
 *
 * SYNOPSIS:
 *    const char *XModemDownload_GetLastErrorMsg(t_FTPSystemData *SysHandle,
 *              t_FTPHandlerDataType *DataHandle);
 *
 * PARAMETERS:
 *    SysHandle [I] -- An handle to be passed back to the file transfer protocol
 *                     system through the 'struct FileTransferHandlerAPI' API.
 *    DataHandle [I] -- An handle to the driver's data that was allocated with
 *                      AllocateData().
 *
 * FUNCTION:
 *    This function gets the last error message from the system.  The system
 *    will call then when a function returns an error or abort to find more
 *    details.
 *
 * RETURNS:
 *    A pointer to an error message or NULL if there was no message.  This must
 *    remain valid until the next call to any 'FileTransferHandlerAPI' function.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static const char *XModemDownload_GetLastErrorMsg(t_FTPSystemData *SysHandle,
        t_FTPHandlerDataType *DataHandle)
{
    struct XModemDownloadData *Data=(struct XModemDownloadData *)DataHandle;

    if(Data->ErrorStr=="")
        return NULL;

    return Data->ErrorStr.c_str();
}

static void XModemDownload_Setup4NextPacket(struct XModemDownloadData *Data)
{
    Data->DownloadState=e_XModemDownload_StartOfHeader;
    Data->ByteCount=0;
    Data->ExpectedByteCount=0;
    Data->NAKPacket=false;
    Data->IgnorePacket=false;
    Data->LastByteTimeout=0;
}

