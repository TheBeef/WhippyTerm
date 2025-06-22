/*******************************************************************************
 * FILENAME: TestIODriver.cpp
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
#include "TestIODriver.h"
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
#define TestIODriver_URI_PREFIX                 "TestIO"
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      TestIODriver    // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01001000

#define QUEUE_SIZE                              2000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct TestIODriver_ConWidgets
{
    struct PI_ComboBox *Combox1;
    struct PI_RadioBttnGroup *RadioGroup;
    struct PI_RadioBttn *Radio1;
    struct PI_RadioBttn *Radio2;
    struct PI_Checkbox *Checkbox1;
    struct PI_TextInput *TextInput1;
    struct PI_NumberInput *NumberInput1;
    struct PI_DoubleInput *DoubleInput1;
    struct PI_ColumnViewInput *ColumnViewInput1;
    struct PI_ButtonInput *ButtonInput1;
    struct PI_Indicator *IndicatorInput1;
    struct PI_TextBox *TextBox1;
    struct PI_GroupBox *TestGroupBox;
    struct PI_TextInput *TestTxtBoxInGroupBox;
};

struct TestIODriver_ConAuxWidgets
{
    t_WidgetSysHandle *WidgetHandle;
    struct PI_Checkbox *TestCheckbox;
    struct PI_TextInput *TestTxtBox;
    struct PI_ComboBox *TestComboxBox;
    struct PI_NumberInput *TestNumberBox;
    struct PI_DoubleInput *TestDoubleBox;
    struct PI_RadioBttnGroup *TestRadioBttnGroup;
    struct PI_RadioBttn *TestRadioBttn;
    struct PI_RadioBttn *TestRadioBttn2;
    struct PI_ColumnViewInput *TestColumnView;
    struct PI_ButtonInput *TestButton;
    struct PI_Indicator *TestIndicator;
    struct PI_TextBox *TextBox;
    struct PI_GroupBox *TestGroupBox;
    struct PI_TextInput *TestTxtBoxInGroupBox;
};

typedef list<int> t_TestIODriverLastUsedListType;
typedef t_TestIODriverLastUsedListType::iterator i_TestIODriverLastUsedListType;

struct TestIODriver_OurData
{
    t_IOSystemHandle *IOHandle;
    uint8_t *Queue;
    int QueueSize;
    int BytesInQueue;

    /* Test widgets */
    struct TestIODriver_ConAuxWidgets *CurrentAuxWidgets;
    bool CurrentAState;
    int ACount;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL TestIODriver_Init(void);
const struct IODriverInfo *TestIODriver_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *TestIODriver_DetectDevices(void);
void TestIODriver_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
t_ConnectionWidgetsType *TestIODriver_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle);
void TestIODriver_ConnectionOptionsWidgets_FreeWidgets(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions);
void TestIODriver_ConnectionOptionsWidgets_StoreUI(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
void TestIODriver_ConnectionOptionsWidgets_UpdateUI(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
t_DriverIOHandleType *TestIODriver_AllocateHandle(const char *DeviceUniqueID,t_IOSystemHandle *IOHandle);
PG_BOOL TestIODriver_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL TestIODriver_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
PG_BOOL TestIODriver_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
void TestIODriver_Close(t_DriverIOHandleType *DriverIO);
void TestIODriver_FreeHandle(t_DriverIOHandleType *DriverIO);
int TestIODriver_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
int TestIODriver_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes);
PG_BOOL TestIODriver_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);
t_ConnectionWidgetsType *TestIODriver_ConnectionAuxCtrlWidgets_AllocWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle);
void TestIODriver_ConnectionAuxCtrlWidgets_FreeWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConAuxCtrls);
void TestColumnView_EventCB(const struct PICVEvent *Event,void *UserData);
void TestButton_EventCB(const struct PIButtonEvent *Event,void *UserData);

/*** VARIABLE DEFINITIONS     ***/
const struct IOS_API *m_TestIODriver_IOSAPI;
const struct PI_UIAPI *m_TestIODriver_UIAPI;
const struct PI_SystemAPI *m_TestIODriver_SysAPI;
const struct IODriverAPI g_TestIODriverPluginAPI=
{
    TestIODriver_Init,
    TestIODriver_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    TestIODriver_DetectDevices,
    TestIODriver_FreeDetectedDevices,
    TestIODriver_GetConnectionInfo,
    TestIODriver_ConnectionOptionsWidgets_AllocWidgets,
    TestIODriver_ConnectionOptionsWidgets_FreeWidgets,
    TestIODriver_ConnectionOptionsWidgets_StoreUI,
    TestIODriver_ConnectionOptionsWidgets_UpdateUI,
    TestIODriver_Convert_URI_To_Options,
    TestIODriver_Convert_Options_To_URI,
    TestIODriver_AllocateHandle,
    TestIODriver_FreeHandle,
    TestIODriver_Open,
    TestIODriver_Close,
    TestIODriver_Read,
    TestIODriver_Write,
    NULL,                                           // ChangeOptions
    NULL,                                           // Transmit
    TestIODriver_ConnectionAuxCtrlWidgets_AllocWidgets,
    TestIODriver_ConnectionAuxCtrlWidgets_FreeWidgets,
    /* V2 */
    NULL,   // GetLastErrorMessage
};
struct IODriverInfo m_TestIODriverInfo=
{
    0,
    "<URI>TestIO[Instance]:[Channel]</URI>"
    "<ARG>Instance -- What instance for the loopback to use</ARG>"
    "<ARG>Channel -- What channel on this loopback to use</ARG>"
    "<Example>TestIO7:1</Example>"
};

/*******************************************************************************
 * NAME:
 *    TestIODriver_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int TestIODriver_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        m_TestIODriver_SysAPI=SysAPI;
        m_TestIODriver_IOSAPI=SysAPI->GetAPI_IO();
        m_TestIODriver_UIAPI=m_TestIODriver_IOSAPI->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(m_TestIODriver_SysAPI->GetExperimentalID()>0 &&
                m_TestIODriver_SysAPI->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_TestIODriver_IOSAPI->RegisterDriver("TestIODriver",
                TestIODriver_URI_PREFIX,&g_TestIODriverPluginAPI,
                sizeof(g_TestIODriverPluginAPI));

        return 0;
    }
}

PG_BOOL TestIODriver_Init(void)
{
    return true;
}
/*******************************************************************************
 * NAME:
 *   TestIODriver_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*TestIODriver_GetDriverInfo)(
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
const struct IODriverInfo *TestIODriver_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_TestIODriverInfo;
}

struct TestIODriver_DetectedConnections
{
    int Index;
    string Device[5];
};

const struct IODriverDetectedInfo *TestIODriver_DetectDevices(void)
{
    struct IODriverDetectedInfo *NextEntry;

    NextEntry=new struct IODriverDetectedInfo;
    NextEntry->StructureSize=sizeof(struct IODriverDetectedInfo);
    snprintf(NextEntry->DeviceUniqueID,sizeof(NextEntry->DeviceUniqueID),"TestIO");
    snprintf(NextEntry->Name,sizeof(NextEntry->Name),"TestIO");
    snprintf(NextEntry->Title,sizeof(NextEntry->Title),"TestIO");
    NextEntry->Flags=0;

    NextEntry->Next=NULL;

    return NextEntry;
}

void TestIODriver_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    delete Devices;
}

const char *TestIODriver_MakeConnectionUniqueID(uintptr_t ID)
{
    static char buff[10];
    sprintf(buff,"%" PRIuPTR,ID);
    return buff;
}

uintptr_t TestIODriver_ConvertConnectionUniqueID2DriverID(const char *UniqueID)
{
    return atoi(UniqueID);
}

t_ConnectionWidgetsType *TestIODriver_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle)
{
    struct TestIODriver_ConWidgets *ConWidgets;
    const char *ColumnNames[2]={"One","Two"};

    ConWidgets=NULL;
    try
    {
        ConWidgets=new struct TestIODriver_ConWidgets;
        memset(ConWidgets,0x00,sizeof(struct TestIODriver_ConWidgets));

        /* Combox */
        ConWidgets->Combox1=m_TestIODriver_UIAPI->AddComboBox(WidgetHandle,false,"Combox 1",NULL,NULL);
        if(ConWidgets->Combox1==NULL) throw(0);
        m_TestIODriver_UIAPI->ClearComboBox(WidgetHandle,ConWidgets->Combox1->Ctrl);
        m_TestIODriver_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Combox1->Ctrl,"One",1);
        m_TestIODriver_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Combox1->Ctrl,"Two",2);
        m_TestIODriver_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Combox1->Ctrl,"Three",3);

        /* Radio */
        ConWidgets->RadioGroup=m_TestIODriver_UIAPI->AllocRadioBttnGroup(WidgetHandle,"Radio");
        if(ConWidgets->RadioGroup==NULL) throw(0);
        ConWidgets->Radio1=m_TestIODriver_UIAPI->AddRadioBttn(WidgetHandle,ConWidgets->RadioGroup,"One",NULL,NULL);
        if(ConWidgets->Radio1==NULL) throw(0);
        ConWidgets->Radio2=m_TestIODriver_UIAPI->AddRadioBttn(WidgetHandle,ConWidgets->RadioGroup,"Two",NULL,NULL);
        if(ConWidgets->Radio2==NULL) throw(0);

        /* Checkbox */
        ConWidgets->Checkbox1=m_TestIODriver_UIAPI->AddCheckbox(WidgetHandle,"Checkbox",NULL,NULL);
        if(ConWidgets->Checkbox1==NULL) throw(0);

        /* Text input */
        ConWidgets->TextInput1=m_TestIODriver_UIAPI->AddTextInput(WidgetHandle,"Text Input",NULL,NULL);
        if(ConWidgets->TextInput1==NULL) throw(0);

        /* Number Input */
        ConWidgets->NumberInput1=m_TestIODriver_UIAPI->AddNumberInput(WidgetHandle,"Number Input",NULL,NULL);
        if(ConWidgets->NumberInput1==NULL) throw(0);

        /* Double Input */
        ConWidgets->DoubleInput1=m_TestIODriver_UIAPI->AddDoubleInput(WidgetHandle,"Double Input",NULL,NULL);
        if(ConWidgets->DoubleInput1==NULL) throw(0);

        /* Column View Input */
        ConWidgets->ColumnViewInput1=m_TestIODriver_UIAPI->AddColumnViewInput(WidgetHandle,"Column View",2,ColumnNames,NULL,NULL);
        if(ConWidgets->ColumnViewInput1==NULL) throw(0);
        m_TestIODriver_UIAPI->ColumnViewInputAddRow(WidgetHandle,ConWidgets->ColumnViewInput1->Ctrl);
        m_TestIODriver_UIAPI->ColumnViewInputSetColumnText(WidgetHandle,ConWidgets->ColumnViewInput1->Ctrl,0,0,"One");
        m_TestIODriver_UIAPI->ColumnViewInputSetColumnText(WidgetHandle,ConWidgets->ColumnViewInput1->Ctrl,1,0,"Two");

        /* Button Input */
        ConWidgets->ButtonInput1=m_TestIODriver_UIAPI->AddButtonInput(WidgetHandle,"Button",NULL,NULL);
        if(ConWidgets->ButtonInput1==NULL) throw(0);

        /* Indicator Input */
        ConWidgets->IndicatorInput1=m_TestIODriver_UIAPI->AddIndicator(WidgetHandle,"Indicator");
        if(ConWidgets->IndicatorInput1==NULL) throw(0);

        /* TextBox (display only) */
        ConWidgets->TextBox1=m_TestIODriver_UIAPI->AddTextBox(WidgetHandle,"Textbox","One, Two");
        if(ConWidgets->TextBox1==NULL) throw(0);

        /* Groupbox */
        ConWidgets->TestGroupBox=m_TestIODriver_UIAPI->AddGroupBox(WidgetHandle,"My Group");
        if(ConWidgets->TestGroupBox==NULL) throw(0);

        /* Text input */
        ConWidgets->TestTxtBoxInGroupBox=m_TestIODriver_UIAPI->AddTextInput(ConWidgets->TestGroupBox->GroupWidgetHandle,"Inside Text Input",NULL,NULL);
        if(ConWidgets->TestTxtBoxInGroupBox==NULL) throw(0);
    }
    catch(...)
    {
        if(ConWidgets!=NULL)
        {
            TestIODriver_ConnectionOptionsWidgets_FreeWidgets(WidgetHandle,
                    (t_ConnectionWidgetsType *)ConWidgets);
        }
        return NULL;
    }

    return (t_ConnectionWidgetsType *)ConWidgets;
}

void TestIODriver_ConnectionOptionsWidgets_FreeWidgets(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions)
{
    struct TestIODriver_ConWidgets *ConWidgets=(struct TestIODriver_ConWidgets *)ConOptions;

    if(ConWidgets->TextBox1!=NULL)
        m_TestIODriver_UIAPI->FreeTextBox(WidgetHandle,ConWidgets->TextBox1);
    if(ConWidgets->IndicatorInput1!=NULL)
        m_TestIODriver_UIAPI->FreeIndicator(WidgetHandle,ConWidgets->IndicatorInput1);
    if(ConWidgets->ButtonInput1!=NULL)
        m_TestIODriver_UIAPI->FreeButtonInput(WidgetHandle,ConWidgets->ButtonInput1);
    if(ConWidgets->ColumnViewInput1!=NULL)
        m_TestIODriver_UIAPI->FreeColumnViewInput(WidgetHandle,ConWidgets->ColumnViewInput1);
    if(ConWidgets->DoubleInput1!=NULL)
        m_TestIODriver_UIAPI->FreeDoubleInput(WidgetHandle,ConWidgets->DoubleInput1);
    if(ConWidgets->NumberInput1!=NULL)
        m_TestIODriver_UIAPI->FreeNumberInput(WidgetHandle,ConWidgets->NumberInput1);
    if(ConWidgets->TextInput1!=NULL)
        m_TestIODriver_UIAPI->FreeTextInput(WidgetHandle,ConWidgets->TextInput1);
    if(ConWidgets->Checkbox1!=NULL)
        m_TestIODriver_UIAPI->FreeCheckbox(WidgetHandle,ConWidgets->Checkbox1);
    if(ConWidgets->Radio2!=NULL)
        m_TestIODriver_UIAPI->FreeRadioBttn(WidgetHandle,ConWidgets->Radio2);
    if(ConWidgets->Radio1!=NULL)
        m_TestIODriver_UIAPI->FreeRadioBttn(WidgetHandle,ConWidgets->Radio1);
    if(ConWidgets->RadioGroup!=NULL)
        m_TestIODriver_UIAPI->FreeRadioBttnGroup(WidgetHandle,ConWidgets->RadioGroup);
    if(ConWidgets->Combox1!=NULL)
        m_TestIODriver_UIAPI->FreeComboBox(WidgetHandle,ConWidgets->Combox1);
    if(ConWidgets->TestTxtBoxInGroupBox!=NULL)
        m_TestIODriver_UIAPI->FreeTextInput(ConWidgets->TestGroupBox->GroupWidgetHandle,ConWidgets->TestTxtBoxInGroupBox);
    if(ConWidgets->TestGroupBox!=NULL)
        m_TestIODriver_UIAPI->FreeGroupBox(WidgetHandle,ConWidgets->TestGroupBox);

    delete ConWidgets;
}

void TestIODriver_ConnectionOptionsWidgets_StoreUI(t_WidgetSysHandle *WidgetHandle,
        t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct TestIODriver_ConWidgets *ConWidgets=(struct TestIODriver_ConWidgets *)ConOptions;
    uintptr_t Value;
    char buff[100];

    if(ConWidgets->Combox1==NULL)
        return;

    m_TestIODriver_SysAPI->KVClear(Options);

    Value=m_TestIODriver_UIAPI->GetComboBoxSelectedEntry(WidgetHandle,
            ConWidgets->Combox1->Ctrl);
    sprintf(buff,"%" PRIuPTR,Value);
    m_TestIODriver_SysAPI->KVAddItem(Options,"LoopBackNumber",buff);
}

void TestIODriver_ConnectionOptionsWidgets_UpdateUI(
        t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,
        const char *DeviceUniqueID,t_PIKVList *Options)
{
    struct TestIODriver_ConWidgets *ConWidgets=(struct TestIODriver_ConWidgets *)ConOptions;
    uintptr_t Value;
    const char *LoopBackNumberStr;

    if(ConWidgets->Combox1==NULL)
        return;

    LoopBackNumberStr=m_TestIODriver_SysAPI->KVGetItem(Options,"LoopBackNumber");
    if(LoopBackNumberStr==NULL)
        return;

    Value=atoi(LoopBackNumberStr);

    m_TestIODriver_UIAPI->SetComboBoxSelectedEntry(WidgetHandle,
            ConWidgets->Combox1->Ctrl,Value);
}

t_DriverIOHandleType *TestIODriver_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct TestIODriver_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct TestIODriver_OurData;
        NewData->Queue=NULL;
        NewData->CurrentAuxWidgets=NULL;
        NewData->CurrentAState=false;
        NewData->ACount=0;

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

void TestIODriver_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct TestIODriver_OurData *OurData=(struct TestIODriver_OurData *)DriverIO;

    free(OurData->Queue);

    delete OurData;
}

PG_BOOL TestIODriver_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct TestIODriver_OurData *OurData=(struct TestIODriver_OurData *)DriverIO;

    OurData->BytesInQueue=0;

    m_TestIODriver_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);

    return true;
}

int TestIODriver_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    struct TestIODriver_OurData *OurData=(struct TestIODriver_OurData *)DriverIO;
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

        /* DEBUG PAUL: Test code that toggles the test check box when we see
           an 'a' */
        if(Data[r]=='a')
        {
            if(OurData->CurrentAuxWidgets!=NULL && 
                    OurData->CurrentAuxWidgets->TestCheckbox!=NULL)
            {
                char buff[100];

                OurData->ACount++;
                OurData->CurrentAState=!OurData->CurrentAState;

                m_TestIODriver_UIAPI->SetCheckboxChecked(OurData->CurrentAuxWidgets->WidgetHandle,
                        OurData->CurrentAuxWidgets->TestCheckbox->Ctrl,
                        OurData->CurrentAState);
                sprintf(buff,"%d",OurData->ACount);
                m_TestIODriver_UIAPI->SetComboBoxText(OurData->CurrentAuxWidgets->WidgetHandle,
                        OurData->CurrentAuxWidgets->TestComboxBox->Ctrl,
                        buff);

                m_TestIODriver_UIAPI->SetIndicator(OurData->CurrentAuxWidgets->WidgetHandle,
                        OurData->CurrentAuxWidgets->TestIndicator->Ctrl,OurData->CurrentAState);

            }
        }

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

    m_TestIODriver_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_BytesAvailable);

    return Bytes;
}

int TestIODriver_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    struct TestIODriver_OurData *OurData=(struct TestIODriver_OurData *)DriverIO;
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

void TestIODriver_Close(t_DriverIOHandleType *DriverIO)
{
    struct TestIODriver_OurData *OurData=(struct TestIODriver_OurData *)DriverIO;

    OurData->BytesInQueue=0;

    m_TestIODriver_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Disconnected);

    return;
}

PG_BOOL TestIODriver_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    const char *PosStart;
    char *Pos;
    unsigned long Inst;
    uintptr_t Value;
    char buff[100];

    if(strlen(URI)<(sizeof(TestIODriver_URI_PREFIX)-1)+1+1)  // Prefix (-1 to remove \0) + ':' + number
        return false;

    m_TestIODriver_SysAPI->KVClear(Options);

    PosStart=URI;
    PosStart+=sizeof(TestIODriver_URI_PREFIX)-1;  // -1 because of the \0

    /* We are at the instance number */
    Inst=strtoul(PosStart,&Pos,10);

    if(*Pos!=':')
        return false;   // Malformed URI
    Pos++;  // Skip :

    Value=strtoul(Pos,NULL,10);
    sprintf(buff,"%" PRIuPTR,Value);
    m_TestIODriver_SysAPI->KVAddItem(Options,"LoopBackNumber",buff);

    /* Build the DeviceUniqueID from the Inst */
    sprintf(buff,TestIODriver_URI_PREFIX "%ld",Inst);
    if(MaxDeviceUniqueIDLen<sizeof(buff))
        return false;
    strcpy(DeviceUniqueID,buff);

    return true;
}

PG_BOOL TestIODriver_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    const char *LoopBackNumber;

    LoopBackNumber=m_TestIODriver_SysAPI->KVGetItem(Options,"LoopBackNumber");
    if(LoopBackNumber==NULL)
        LoopBackNumber="";

    strcpy(URI,DeviceUniqueID);
    strcat(URI,":");
    strcat(URI,LoopBackNumber);
    return true;
}

PG_BOOL TestIODriver_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    const char *LoopBackNumberStr;
    int ID;

    if(strlen(DeviceUniqueID)<2)
        return false;
    ID=atoi(&DeviceUniqueID[2]);

    LoopBackNumberStr=m_TestIODriver_SysAPI->KVGetItem(Options,"LoopBackNumber");
    if(LoopBackNumberStr!=NULL)
        snprintf(RetInfo->Title,sizeof(RetInfo->Title),"TestIO%d:%s",ID,LoopBackNumberStr);
    else
        snprintf(RetInfo->Title,sizeof(RetInfo->Title),"TestIO%d",ID);

    snprintf(RetInfo->Name,sizeof(RetInfo->Name),"TestIO %d",ID);
    RetInfo->Flags=0;

    return true;
}

void TestIODriver_TestCheckboxCallBack(const struct PICheckboxEvent *Event,void *UserData)
{
}

//TestIODriver_OurData
//
    struct TestIODriver_ConAuxWidgets *CurrentAuxWidgets;
    bool CurrentAState;

t_ConnectionWidgetsType *TestIODriver_ConnectionAuxCtrlWidgets_AllocWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle)
{
    struct TestIODriver_OurData *OurData=(struct TestIODriver_OurData *)DriverIO;
    struct TestIODriver_ConAuxWidgets *ConAuxWidgets;
    static const char *ColumnNames[]={"One","Two","Three"};

    ConAuxWidgets=NULL;
    try
    {
        ConAuxWidgets=new struct TestIODriver_ConAuxWidgets;
        ConAuxWidgets->TestCheckbox=NULL;
        ConAuxWidgets->TestTxtBox=NULL;
        ConAuxWidgets->WidgetHandle=WidgetHandle;

        ConAuxWidgets->TestCheckbox=m_TestIODriver_UIAPI->AddCheckbox(WidgetHandle,
                "Test checkbox",TestIODriver_TestCheckboxCallBack,ConAuxWidgets);
        if(ConAuxWidgets->TestCheckbox==NULL)
            throw(0);

        ConAuxWidgets->TestTxtBox=m_TestIODriver_UIAPI->AddTextInput(WidgetHandle,
                "Test Text",NULL,NULL);
        if(ConAuxWidgets->TestTxtBox==NULL)
            throw(0);

        ConAuxWidgets->TestComboxBox=m_TestIODriver_UIAPI->AddComboBox(WidgetHandle,
                true,"Combox box",NULL,NULL);

        ConAuxWidgets->TestNumberBox=m_TestIODriver_UIAPI->AddNumberInput(WidgetHandle,
                "Number Input",NULL,NULL);

        ConAuxWidgets->TestDoubleBox=m_TestIODriver_UIAPI->AddDoubleInput(WidgetHandle,
                "Double Input",NULL,NULL);

        ConAuxWidgets->TestRadioBttnGroup=m_TestIODriver_UIAPI->AllocRadioBttnGroup(WidgetHandle,"Group");
        ConAuxWidgets->TestRadioBttn=m_TestIODriver_UIAPI->AddRadioBttn(WidgetHandle,ConAuxWidgets->TestRadioBttnGroup,"Radio Bttn",NULL,NULL);
        ConAuxWidgets->TestRadioBttn2=m_TestIODriver_UIAPI->AddRadioBttn(WidgetHandle,ConAuxWidgets->TestRadioBttnGroup,"Radio Bttn 2",NULL,NULL);

        ConAuxWidgets->TestColumnView=m_TestIODriver_UIAPI->AddColumnViewInput(WidgetHandle,
                "List View",3,ColumnNames,TestColumnView_EventCB,(void *)ConAuxWidgets);

        {
            int x;
            x=m_TestIODriver_UIAPI->ColumnViewInputAddRow(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl);
            m_TestIODriver_UIAPI->ColumnViewInputSetColumnText(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl,0,0,"Test");
            m_TestIODriver_UIAPI->ColumnViewInputSetColumnText(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl,1,0,"B");
            m_TestIODriver_UIAPI->ColumnViewInputSetColumnText(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl,2,0,"C");

            x=m_TestIODriver_UIAPI->ColumnViewInputAddRow(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl);
            m_TestIODriver_UIAPI->ColumnViewInputSetColumnText(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl,0,x,"D");
            m_TestIODriver_UIAPI->ColumnViewInputSetColumnText(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl,1,x,"E");
            m_TestIODriver_UIAPI->ColumnViewInputSetColumnText(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl,2,x,"F");

            m_TestIODriver_UIAPI->ColumnViewInputSelectRow(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl,1);
            m_TestIODriver_UIAPI->ColumnViewInputClearSelection(WidgetHandle,ConAuxWidgets->TestColumnView->Ctrl);
        }

        ConAuxWidgets->TestButton=m_TestIODriver_UIAPI->AddButtonInput(WidgetHandle,
                "Button",TestButton_EventCB,(void *)ConAuxWidgets);

        ConAuxWidgets->TestIndicator=m_TestIODriver_UIAPI->AddIndicator(WidgetHandle,
                "Indicator");

        /* TextBox (display only) */
        ConAuxWidgets->TextBox=m_TestIODriver_UIAPI->AddTextBox(WidgetHandle,"Textbox","My test text.");
        if(ConAuxWidgets->TextBox==NULL) throw(0);

        /* Groupbox */
        ConAuxWidgets->TestGroupBox=m_TestIODriver_UIAPI->AddGroupBox(WidgetHandle,"My Group");
        if(ConAuxWidgets->TestGroupBox==NULL) throw(0);

        /* Text input */
        ConAuxWidgets->TestTxtBoxInGroupBox=m_TestIODriver_UIAPI->AddTextInput(ConAuxWidgets->TestGroupBox->GroupWidgetHandle,"Inside Text Input",NULL,NULL);
        if(ConAuxWidgets->TestTxtBoxInGroupBox==NULL) throw(0);

        OurData->CurrentAuxWidgets=ConAuxWidgets;
    }
    catch(...)
    {
        if(ConAuxWidgets!=NULL)
        {
            if(ConAuxWidgets->TextBox!=NULL)
                m_TestIODriver_UIAPI->FreeTextBox(WidgetHandle,ConAuxWidgets->TextBox);
            if(ConAuxWidgets->TestCheckbox!=NULL)
                m_TestIODriver_UIAPI->FreeCheckbox(WidgetHandle,ConAuxWidgets->TestCheckbox);
            if(ConAuxWidgets->TestTxtBox!=NULL)
                m_TestIODriver_UIAPI->FreeTextInput(WidgetHandle,ConAuxWidgets->TestTxtBox);
            if(ConAuxWidgets->TestComboxBox!=NULL)
                m_TestIODriver_UIAPI->FreeComboBox(WidgetHandle,ConAuxWidgets->TestComboxBox);
            if(ConAuxWidgets->TestNumberBox!=NULL)
                m_TestIODriver_UIAPI->FreeNumberInput(WidgetHandle,ConAuxWidgets->TestNumberBox);
            if(ConAuxWidgets->TestDoubleBox!=NULL)
                m_TestIODriver_UIAPI->FreeDoubleInput(WidgetHandle,ConAuxWidgets->TestDoubleBox);
            if(ConAuxWidgets->TestRadioBttn!=NULL)
                m_TestIODriver_UIAPI->FreeRadioBttn(WidgetHandle,ConAuxWidgets->TestRadioBttn);
            if(ConAuxWidgets->TestRadioBttn2!=NULL)
                m_TestIODriver_UIAPI->FreeRadioBttn(WidgetHandle,ConAuxWidgets->TestRadioBttn2);
            if(ConAuxWidgets->TestRadioBttnGroup!=NULL)
                m_TestIODriver_UIAPI->FreeRadioBttnGroup(WidgetHandle,ConAuxWidgets->TestRadioBttnGroup);
            if(ConAuxWidgets->TestColumnView!=NULL)
                m_TestIODriver_UIAPI->FreeColumnViewInput(WidgetHandle,ConAuxWidgets->TestColumnView);
            if(ConAuxWidgets->TestButton!=NULL)
                m_TestIODriver_UIAPI->FreeButtonInput(WidgetHandle,ConAuxWidgets->TestButton);
            if(ConAuxWidgets->TestIndicator!=NULL)
                m_TestIODriver_UIAPI->FreeIndicator(WidgetHandle,ConAuxWidgets->TestIndicator);
            if(ConAuxWidgets->TestTxtBoxInGroupBox!=NULL)
                m_TestIODriver_UIAPI->FreeTextInput(ConAuxWidgets->TestGroupBox->GroupWidgetHandle,ConAuxWidgets->TestTxtBoxInGroupBox);
            if(ConAuxWidgets->TestGroupBox!=NULL)
                m_TestIODriver_UIAPI->FreeGroupBox(WidgetHandle,ConAuxWidgets->TestGroupBox);

        }
        return NULL;
    }

    return (t_ConnectionWidgetsType *)ConAuxWidgets;
}

void TestIODriver_ConnectionAuxCtrlWidgets_FreeWidgets(t_DriverIOHandleType *DriverIO,t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConAuxCtrls)
{
    struct TestIODriver_OurData *OurData=(struct TestIODriver_OurData *)DriverIO;
    struct TestIODriver_ConAuxWidgets *ConAuxWidgets=(struct TestIODriver_ConAuxWidgets *)ConAuxCtrls;

    if(ConAuxWidgets->TextBox!=NULL)
        m_TestIODriver_UIAPI->FreeTextBox(WidgetHandle,ConAuxWidgets->TextBox);

    if(ConAuxWidgets->TestCheckbox!=NULL)
        m_TestIODriver_UIAPI->FreeCheckbox(WidgetHandle,ConAuxWidgets->TestCheckbox);

    if(ConAuxWidgets->TestTxtBox!=NULL)
        m_TestIODriver_UIAPI->FreeTextInput(WidgetHandle,ConAuxWidgets->TestTxtBox);

    if(ConAuxWidgets->TestComboxBox!=NULL)
        m_TestIODriver_UIAPI->FreeComboBox(WidgetHandle,ConAuxWidgets->TestComboxBox);

    if(ConAuxWidgets->TestNumberBox!=NULL)
        m_TestIODriver_UIAPI->FreeNumberInput(WidgetHandle,ConAuxWidgets->TestNumberBox);

    if(ConAuxWidgets->TestDoubleBox!=NULL)
        m_TestIODriver_UIAPI->FreeDoubleInput(WidgetHandle,ConAuxWidgets->TestDoubleBox);

    if(ConAuxWidgets->TestRadioBttn!=NULL)
        m_TestIODriver_UIAPI->FreeRadioBttn(WidgetHandle,ConAuxWidgets->TestRadioBttn);
    if(ConAuxWidgets->TestRadioBttn2!=NULL)
        m_TestIODriver_UIAPI->FreeRadioBttn(WidgetHandle,ConAuxWidgets->TestRadioBttn2);
    if(ConAuxWidgets->TestRadioBttnGroup!=NULL)
        m_TestIODriver_UIAPI->FreeRadioBttnGroup(WidgetHandle,ConAuxWidgets->TestRadioBttnGroup);

    if(ConAuxWidgets->TestColumnView!=NULL)
        m_TestIODriver_UIAPI->FreeColumnViewInput(WidgetHandle,ConAuxWidgets->TestColumnView);

    if(ConAuxWidgets->TestButton!=NULL)
        m_TestIODriver_UIAPI->FreeButtonInput(WidgetHandle,ConAuxWidgets->TestButton);

    if(ConAuxWidgets->TestIndicator!=NULL)
        m_TestIODriver_UIAPI->FreeIndicator(WidgetHandle,ConAuxWidgets->TestIndicator);

    if(ConAuxWidgets->TestTxtBoxInGroupBox!=NULL)
        m_TestIODriver_UIAPI->FreeTextInput(ConAuxWidgets->TestGroupBox->GroupWidgetHandle,ConAuxWidgets->TestTxtBoxInGroupBox);
    if(ConAuxWidgets->TestGroupBox!=NULL)
        m_TestIODriver_UIAPI->FreeGroupBox(WidgetHandle,ConAuxWidgets->TestGroupBox);

    OurData->CurrentAuxWidgets=NULL;

    delete ConAuxWidgets;
}

void TestColumnView_EventCB(const struct PICVEvent *Event,void *UserData)
{
//    struct TestIODriver_ConAuxWidgets *ConAuxWidgets=(struct TestIODriver_ConAuxWidgets *)UserData;

    switch(Event->EventType)
    {
        case e_PIECV_IndexChanged:
        break;
        case e_PIECVMAX:
        default:
        break;
    }
}

void TestButton_EventCB(const struct PIButtonEvent *Event,void *UserData)
{
//    struct TestIODriver_ConAuxWidgets *ConAuxWidgets=(struct TestIODriver_ConAuxWidgets *)UserData;

    switch(Event->EventType)
    {
        case e_PIEButton_Press:
        break;
        case e_PIEButtonMAX:
        default:
        break;
    }
}

