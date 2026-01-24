/*******************************************************************************
 * FILENAME: LB2_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "LB2_Main.h"
#include "PluginSDK/Plugin.h"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <list>

using namespace std;

#ifndef PRIuPTR
#define PRIuPTR "d"
#endif

/*** DEFINES                  ***/
#define LB2_URI_PREFIX          "LB"
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      TestLB // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

#define QUEUE_SIZE              2000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct LB2_ConWidgets
{
    struct PI_ComboBox *Loopback;
};

struct LB2_OurData
{
    t_IOSystemHandle *IOHandle;
    uint8_t *Queue;
    int QueueSize;
    int BytesInQueue;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL LB2_Init(void);
const struct IODriverInfo *LB2_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *LB2_DetectDevices(void);
void LB2_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
t_DriverIOHandleType *LB2_AllocateHandle(const char *DeviceUniqueID,t_IOSystemHandle *IOHandle);
PG_BOOL LB2_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL LB2_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
PG_BOOL LB2_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
void LB2_Close(t_DriverIOHandleType *DriverIO);
void LB2_FreeHandle(t_DriverIOHandleType *DriverIO);
int LB2_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
int LB2_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes);
PG_BOOL LB2_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);
t_ConnectionWidgetsType *LB2_ConnectionAuxCtrlWidgets_AllocWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle);
void LB2_ConnectionAuxCtrlWidgets_FreeWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConAuxCtrls);

/*** VARIABLE DEFINITIONS     ***/
const struct IOS_API *m_LB2_IOSAPI;
const struct PI_UIAPI *m_LB2_UIAPI;
const struct PI_SystemAPI *m_LB2_SysAPI;
const struct IODriverAPI g_LB2PluginAPI=
{
    LB2_Init,
    LB2_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    LB2_DetectDevices,
    LB2_FreeDetectedDevices,
    LB2_GetConnectionInfo,
    NULL,
    NULL,
    NULL,
    NULL,
    LB2_Convert_URI_To_Options,
    LB2_Convert_Options_To_URI,
    LB2_AllocateHandle,
    LB2_FreeHandle,
    LB2_Open,
    LB2_Close,
    LB2_Read,
    LB2_Write,
    NULL,                                           // ChangeOptions
    NULL,                                           // Transmit
    NULL,
    NULL,
    /* V2 */
    NULL,   // GetLastErrorMessage
    /* V3 */
    NULL,   // AllocSettingsWidgets
    NULL,   // FreeSettingsWidgets
    NULL,   // SetSettingsFromWidgets
    NULL,   // ApplySettings
};
struct IODriverInfo m_LB2Info=
{
    0,
    "<URI>LB[Instance]:[Channel]</URI>"
    "<ARG>Instance -- What instance for the loopback to use</ARG>"
    "<ARG>Channel -- What channel on this loopback to use</ARG>"
    "<Example>LB7:1</Example>"
};

/*******************************************************************************
 * NAME:
 *    LB2_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int LB2_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        m_LB2_SysAPI=SysAPI;
        m_LB2_IOSAPI=SysAPI->GetAPI_IO();
        m_LB2_UIAPI=m_LB2_IOSAPI->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(m_LB2_SysAPI->GetExperimentalID()>0 &&
                m_LB2_SysAPI->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_LB2_IOSAPI->RegisterDriver("TestLB",LB2_URI_PREFIX,
                &g_LB2PluginAPI,sizeof(g_LB2PluginAPI));

        return 0;
    }
}

PG_BOOL LB2_Init(void)
{
    return true;
}
/*******************************************************************************
 * NAME:
 *   LB2_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*LB2_GetDriverInfo)(
 *              unsigned int *SizeOfInfo);
 *
 * PARAMETERS:
 *   SizeOfInfo [O] -- The size of 'struct IODriverInfo'.  This is used
 *                     for forward / backward compatibility.
 *
 * FUNCTION:
 *   This function gets info about the plugin.
 *
 * RETURNS:
 *   NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct IODriverInfo *LB2_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_LB2Info;
}

struct LB2_DetectedConnections
{
    int Index;
    string Device[5];
};

const struct IODriverDetectedInfo *LB2_DetectDevices(void)
{
    struct IODriverDetectedInfo *First;
    struct IODriverDetectedInfo *NextEntry;
    int r;
    int ID;

    First=NULL;
    for(r=0;r<9;r++)
    {
        ID=r+1;

        NextEntry=new struct IODriverDetectedInfo;
        NextEntry->StructureSize=sizeof(struct IODriverDetectedInfo);
        snprintf(NextEntry->DeviceUniqueID,sizeof(NextEntry->DeviceUniqueID),"LB%d",ID);
        snprintf(NextEntry->Name,sizeof(NextEntry->Name),"Loopback %d",ID);
        snprintf(NextEntry->Title,sizeof(NextEntry->Title),"LB%d",ID);
        NextEntry->Flags=0;

        NextEntry->Next=First;
        First=NextEntry;
    }

    return First;
}

void LB2_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    struct IODriverDetectedInfo *Tmp;
    while(Devices!=NULL)
    {
        Tmp=Devices->Next;
        delete Devices;
        Devices=Tmp;
    }
}

const char *LB2_MakeConnectionUniqueID(uintptr_t ID)
{
    static char buff[10];
    sprintf(buff,"%" PRIuPTR,ID);
    return buff;
}

uintptr_t LB2_ConvertConnectionUniqueID2DriverID(const char *UniqueID)
{
    return atoi(UniqueID);
}

t_DriverIOHandleType *LB2_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct LB2_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct LB2_OurData;
        NewData->Queue=NULL;

        NewData->IOHandle=IOHandle;

        NewData->Queue=(uint8_t *)malloc(QUEUE_SIZE);
        if(NewData->Queue==NULL)
            throw(0);
        NewData->QueueSize=QUEUE_SIZE;
        NewData->BytesInQueue=0;
    }
    catch(...)
    {
        if(NewData->Queue!=NULL)
            free(NewData->Queue);
        if(NewData!=NULL)
            delete NewData;
        return NULL;
    }

    return (t_DriverIOHandleType *)NewData;
}

void LB2_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct LB2_OurData *OurData=(struct LB2_OurData *)DriverIO;

    free(OurData->Queue);

    delete OurData;
}

PG_BOOL LB2_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct LB2_OurData *OurData=(struct LB2_OurData *)DriverIO;

    OurData->BytesInQueue=0;

    m_LB2_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);

    return true;
}

int LB2_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    struct LB2_OurData *OurData=(struct LB2_OurData *)DriverIO;
    int r;
    int OutBytes;
    uint8_t *Insert;

    OutBytes=0;
    Insert=&OurData->Queue[OurData->BytesInQueue];
    for(r=0;r<Bytes;r++)
    {
        if(OurData->BytesInQueue+OutBytes+1>=OurData->QueueSize)
            return RETERROR_IOERROR;

        *Insert++=Data[r];
        OutBytes++;
    }

    OurData->BytesInQueue+=OutBytes;

    m_LB2_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_BytesAvailable);

    return Bytes;
}

int LB2_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    struct LB2_OurData *OurData=(struct LB2_OurData *)DriverIO;
    int Byte2Ret;

    Byte2Ret=MaxBytes;
    if(Byte2Ret>OurData->BytesInQueue)
        Byte2Ret=OurData->BytesInQueue;

    memcpy(Data,OurData->Queue,Byte2Ret);

    if(Byte2Ret!=OurData->BytesInQueue)
    {
        memcpy(OurData->Queue,&OurData->Queue[Byte2Ret],OurData->BytesInQueue-Byte2Ret);
        OurData->BytesInQueue-=Byte2Ret;
    }
    else
    {
        OurData->BytesInQueue=0;
    }

    return Byte2Ret;
}

void LB2_Close(t_DriverIOHandleType *DriverIO)
{
    struct LB2_OurData *OurData=(struct LB2_OurData *)DriverIO;

    OurData->BytesInQueue=0;

    m_LB2_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Disconnected);

    return;
}

PG_BOOL LB2_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    const char *PosStart;
    char *Pos;
    unsigned long Inst;
    char buff[100];

    if(strlen(URI)<(sizeof(LB2_URI_PREFIX)-1))  // Prefix (-1 to remove \0)
        return false;

    m_LB2_SysAPI->KVClear(Options);

    PosStart=URI;
    PosStart+=sizeof(LB2_URI_PREFIX)-1;  // -1 because of the \0

    /* We are at the instance number */
    Inst=strtoul(PosStart,&Pos,10);

    /* Build the DeviceUniqueID from the Inst */
    sprintf(buff,LB2_URI_PREFIX "%ld",Inst);
    if(MaxDeviceUniqueIDLen<sizeof(buff))
        return false;
    strcpy(DeviceUniqueID,buff);

    return true;
}

PG_BOOL LB2_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    strcpy(URI,DeviceUniqueID);

    return true;
}

PG_BOOL LB2_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    int ID;

    if(strlen(DeviceUniqueID)<2)
        return false;

    ID=atoi(&DeviceUniqueID[2]);
    snprintf(RetInfo->Title,sizeof(RetInfo->Title),"LB%d",ID);
    snprintf(RetInfo->Name,sizeof(RetInfo->Name),"Loopback %d",ID);
    RetInfo->Flags=0;

    return true;
}
