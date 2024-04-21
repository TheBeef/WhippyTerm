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
#define NEEDED_MIN_API_VERSION                  0x00080000

#define QUEUE_SIZE              2000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct LB2_ConWidgets
{
    struct PI_ComboBox *Loopback;
};

typedef list<int> t_LB2LastUsedListType;
typedef t_LB2LastUsedListType::iterator i_LB2LastUsedListType;

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
t_ConnectionOptionsWidgetsType *LB2_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle);
void LB2_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *ConOptions,t_WidgetSysHandle *WidgetHandle);
void LB2_ConnectionOptionsWidgets_StoreUI(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options);
void LB2_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options);
t_DriverIOHandleType *LB2_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
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
    LB2_ConnectionOptionsWidgets_AllocWidgets,
    LB2_ConnectionOptionsWidgets_FreeWidgets,
    LB2_ConnectionOptionsWidgets_StoreUI,
    LB2_ConnectionOptionsWidgets_UpdateUI,
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
};
struct IODriverInfo m_LB2Info=
{
    0
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

        m_LB2_SysAPI=SysAPI;
        m_LB2_IOSAPI=SysAPI->GetAPI_IO();
        m_LB2_UIAPI=m_LB2_IOSAPI->GetAPI_UI();

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
    for(r=0;r<5;r++)
    {
        ID=rand()%10;

        NextEntry=new struct IODriverDetectedInfo;
        NextEntry->StructureSize=sizeof(struct IODriverDetectedInfo);
        snprintf(NextEntry->DeviceUniqueID,sizeof(NextEntry->DeviceUniqueID),"LB%d",ID);
        snprintf(NextEntry->Name,sizeof(NextEntry->Name),"Loopback %d",ID);
        snprintf(NextEntry->Title,sizeof(NextEntry->Title),"LB%d",ID);
        NextEntry->Flags=0;
        if(ID&1)
            NextEntry->Flags|=IODRV_DETECTFLAG_INUSE;

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

t_ConnectionOptionsWidgetsType *LB2_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle)
{
    struct LB2_ConWidgets *ConWidgets;

    ConWidgets=NULL;
    try
    {
        ConWidgets=new struct LB2_ConWidgets;

        ConWidgets->Loopback=m_LB2_UIAPI->AddComboBox(WidgetHandle,false,
                "Loop Back Channel",NULL,NULL);

        if(ConWidgets->Loopback==NULL)
            throw(0);

        m_LB2_UIAPI->ClearComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"1",1);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"2",2);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"3",3);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"4",4);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"5",5);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"6",6);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"7",7);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"8",8);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"9",9);
        m_LB2_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Loopback->Ctrl,"10",10);
    }
    catch(...)
    {
        if(ConWidgets!=NULL)
        {
            if(ConWidgets->Loopback!=NULL)
                m_LB2_UIAPI->FreeComboBox(WidgetHandle,ConWidgets->Loopback);

            delete ConWidgets;
        }
        return NULL;
    }

    return (t_ConnectionOptionsWidgetsType *)ConWidgets;
}

void LB2_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle)
{
    struct LB2_ConWidgets *ConWidgets=(struct LB2_ConWidgets *)ConOptions;

    if(ConWidgets->Loopback!=NULL)
        m_LB2_UIAPI->FreeComboBox(WidgetHandle,ConWidgets->Loopback);

    delete ConWidgets;
}

void LB2_ConnectionOptionsWidgets_StoreUI(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct LB2_ConWidgets *ConWidgets=(struct LB2_ConWidgets *)ConOptions;
    uintptr_t Value;
    char buff[100];

    if(ConWidgets->Loopback==NULL)
        return;

    m_LB2_SysAPI->KVClear(Options);

    Value=m_LB2_UIAPI->GetComboBoxSelectedEntry(WidgetHandle,
            ConWidgets->Loopback->Ctrl);
    sprintf(buff,"%" PRIuPTR,Value);
    m_LB2_SysAPI->KVAddItem(Options,"LoopBackNumber",buff);
}

void LB2_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct LB2_ConWidgets *ConWidgets=(struct LB2_ConWidgets *)ConOptions;
    uintptr_t Value;
    const char *LoopBackNumberStr;

    if(ConWidgets->Loopback==NULL)
        return;

    LoopBackNumberStr=m_LB2_SysAPI->KVGetItem(Options,"LoopBackNumber");
    if(LoopBackNumberStr==NULL)
        return;

    Value=atoi(LoopBackNumberStr);

    m_LB2_UIAPI->SetComboBoxSelectedEntry(WidgetHandle,
            ConWidgets->Loopback->Ctrl,Value);
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

//        if(Data[r]=='\r')
//        {
//            if(OurData->BytesInQueue+OutBytes+1>=OurData->QueueSize)
//                return RETERROR_IOERROR;
//
//            *Insert++='\n';
//            OutBytes++;
//        }
    }

//    if(OurData->BytesInQueue+Bytes>=OurData->QueueSize)
//        return RETERROR_IOERROR;
//
//    memcpy(&OurData->Queue[OurData->BytesInQueue],Data,Bytes);
//
//    OurData->BytesInQueue+=Bytes;

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
    uintptr_t Value;
    char buff[100];

    if(strlen(URI)<(sizeof(LB2_URI_PREFIX)-1)+1+1)  // Prefix (-1 to remove \0) + ':' + number
        return false;

    m_LB2_SysAPI->KVClear(Options);

    PosStart=URI;
    PosStart+=sizeof(LB2_URI_PREFIX)-1;  // -1 because of the \0

    /* We are at the instance number */
    Inst=strtoul(PosStart,&Pos,10);

    if(*Pos!=':')
        return false;   // Malformed URI
    Pos++;  // Skip :

    Value=strtoul(Pos,NULL,10);
    sprintf(buff,"%" PRIuPTR,Value);
    m_LB2_SysAPI->KVAddItem(Options,"LoopBackNumber",buff);

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
    const char *LoopBackNumber;

    LoopBackNumber=m_LB2_SysAPI->KVGetItem(Options,"LoopBackNumber");
    if(LoopBackNumber==NULL)
        LoopBackNumber="";

    strcpy(URI,DeviceUniqueID);
    strcat(URI,":");
    strcat(URI,LoopBackNumber);
    return true;
}

PG_BOOL LB2_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    const char *LoopBackNumberStr;
    int ID;

    if(strlen(DeviceUniqueID)<2)
        return false;
    ID=atoi(&DeviceUniqueID[2]);

    LoopBackNumberStr=m_LB2_SysAPI->KVGetItem(Options,"LoopBackNumber");
    if(LoopBackNumberStr!=NULL)
        snprintf(RetInfo->Title,sizeof(RetInfo->Title),"LB%d:%s",ID,LoopBackNumberStr);
    else
        snprintf(RetInfo->Title,sizeof(RetInfo->Title),"LB%d",ID);

    snprintf(RetInfo->Name,sizeof(RetInfo->Name),"Loopback %d",ID);
    RetInfo->Flags=0;

    return true;
}
