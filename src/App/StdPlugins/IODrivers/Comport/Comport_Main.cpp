/*******************************************************************************
 * FILENAME: Comport_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the driver for comports on Linux and Windows.
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
#include "PluginSDK/Plugin.h"
#include "Comport_Main.h"
#include "Comport_ConnectionOptions.h"
#include "OS/Comport_Serial.h"
#include "UI/UIDebug.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      Comport // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x000B0000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL Comport_Init(void);
const struct IODriverInfo *Comport_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *Comport_DetectDevices(void);
void Comport_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
static void Comport_DTRCheckboxCallBack(const struct PICheckboxEvent *Event,void *UserData);
static void Comport_RTSCheckboxCallBack(const struct PICheckboxEvent *Event,void *UserData);
static void Comport_SendBreakButton(const struct PIButtonEvent *Event,void *UserData);
static void Comport_InfoClearButton(const struct PIButtonEvent *Event,void *UserData);

/*** VARIABLE DEFINITIONS     ***/
const struct IODriverAPI g_ComportPluginAPI=
{
    Comport_Init,
    Comport_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    Comport_DetectDevices,
    Comport_FreeDetectedDevices,
    Comport_GetConnectionInfo,
    Comport_ConnectionOptionsWidgets_AllocWidgets,
    Comport_ConnectionOptionsWidgets_FreeWidgets,
    Comport_ConnectionOptionsWidgets_StoreUI,
    Comport_ConnectionOptionsWidgets_UpdateUI,
    Comport_Convert_URI_To_Options,
    Comport_Convert_Options_To_URI,
    Comport_AllocateHandle,
    Comport_FreeHandle,
    Comport_Open,
    Comport_Close,
    Comport_Read,
    Comport_Write,
    Comport_ChangeOptions,
    NULL,                                               // Transmit
    Comport_ConnectionAuxCtrlWidgets_AllocWidgets,
    Comport_ConnectionAuxCtrlWidgets_FreeWidgets,
};

struct IODriverInfo m_ComportInfo=
{
    0
};

const struct IOS_API *g_CP_IOSystem;
const struct PI_UIAPI *g_CP_UI;
const struct PI_SystemAPI *g_CP_System;

/*******************************************************************************
 * NAME:
 *    Comport_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int Comport_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
 *          unsigned int Version);
 *
 * PARAMETERS:
 *    SysAPI [I] -- The main API to WhippyTerm
 *    Version [I] -- What version of WhippyTerm is running.  This is used
 *                   to make sure we are compatible.  This is in the
 *                   Major<<24 | Minor<<16 | Rev<<8 | Patch
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

        g_CP_System=SysAPI;
        g_CP_IOSystem=g_CP_System->GetAPI_IO();
        g_CP_UI=g_CP_IOSystem->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(g_CP_System->GetExperimentalID()>0 &&
                g_CP_System->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        g_CP_IOSystem->RegisterDriver("Comport",COMPORT_URI_PREFIX,
                &g_ComportPluginAPI,sizeof(g_ComportPluginAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    Comport_Init
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_Init(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's anything needed for the plugin.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  The main system will tell the user that
 *             the plugin failed and remove this plugin from the list of
 *             available plugins.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL Comport_Init(void)
{
    return true;
}

/*******************************************************************************
 * NAME:
 *   Comport_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*Comport_GetDriverInfo)(
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
const struct IODriverInfo *Comport_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_ComportInfo;
}

/*******************************************************************************
 * NAME:
 *    DetectDevices
 *
 * SYNOPSIS:
 *    struct IODriverDetectedInfo *DetectDevices(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function detects different devices for this driver.  It will
 *    allocate a linked list of detected devices filling in a
 *    'struct IODriverDetectedInfo' structure for each detected device.
 *
 *    The 'struct IODriverDetectedInfo' has the following fields:
 *      Next -- A pointer to the next entry in the list or NULL if this was
 *              the last entry.
 *      StructureSize -- The size of the allocated structure.  This must be
 *              set to sizeof(struct IODriverDetectedInfo)
 *      Flags [I] -- What flags apply to this device:
 *                   IODRV_DETECTFLAG_INUSE -- The devices has been detected
 *                          as in use.
 *      DeviceUniqueID -- This is a string that can be used to identify this
 *                        particular delected device later.
 *                        This maybe stored to the disk so it needs to
 *                        be built in such a away as you can extract the
 *                        device again.
 *                        For example the new connection system uses this to
 *                        store the options for this connection into session
 *                        file so it can restore it next time this device is
 *                        selected.
 *      Name [I] -- The string used for the user to select this device.  This
 *                  should be recognisable on it's own to the user as what
 *                  driver this goes with as the system only shows this driver
 *                  without modifying it.
 *      Title -- The string that has a short name for this device.
 *               This will be used for titles of windows, tab's and the like.
 *
 * RETURNS:
 *    The first node in the linked list or NULL if there where no devices
 *    detected or an error.
 *
 * SEE ALSO:
 *    FreeDetectedDevices()
 ******************************************************************************/
const struct IODriverDetectedInfo *Comport_DetectDevices(void)
{
    t_OSComportListType DevList;
    i_OSComportListType d;
    struct IODriverDetectedInfo *First;
    struct IODriverDetectedInfo *NewInfo;
    const char *CopyFrom;

    try
    {
        /* Scan for the OS devices */
        if(!Comport_OS_GetSerialPortList(DevList))
            throw(0);

        First=NULL;
        for(d=DevList.begin();d!=DevList.end();d++)
        {
            NewInfo=new struct IODriverDetectedInfo;
            NewInfo->StructureSize=sizeof(struct IODriverDetectedInfo);
            NewInfo->Flags=0;

            strncpy(NewInfo->DeviceUniqueID,d->DriverName.c_str(),
                    sizeof(NewInfo->DeviceUniqueID)-1);
            NewInfo->DeviceUniqueID[sizeof(NewInfo->DeviceUniqueID)-1]=0;

            if(d->FullName.empty())
                CopyFrom=d->DriverName.c_str();
            else
                CopyFrom=d->FullName.c_str();
            strncpy(NewInfo->Name,CopyFrom,sizeof(NewInfo->Name)-1);
            NewInfo->Name[sizeof(NewInfo->Name)-1]=0;

            if(d->ShortName.empty())
                CopyFrom=d->FullName.c_str();
            else
                CopyFrom=d->ShortName.c_str();
            strncpy(NewInfo->Title,CopyFrom,sizeof(NewInfo->Title)-1);
            NewInfo->Title[sizeof(NewInfo->Title)-1]=0;

            NewInfo->Next=First;
            First=NewInfo;
        }
    }
    catch(...)
    {
        Comport_FreeDetectedDevices(First);

        First=NULL;
    }
    return First;
}

/*******************************************************************************
 * NAME:
 *    FreeDetectedDevices
 *
 * SYNOPSIS:
 *    void FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
 *
 * PARAMETERS:
 *    Devices [I] -- The linked list to free
 *
 * FUNCTION:
 *    This function frees all the links in the linked list allocated with
 *    DetectDevices()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Comport_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    struct IODriverDetectedInfo *Tmp;

    while(Devices!=NULL)
    {
        Tmp=Devices->Next;
        delete Devices;
        Devices=Tmp;
    }
}


void Comport_DefaultPortOptions(struct ComportPortOptions *Options)
{
    Options->BitRate=9600;
    Options->DataBits=e_ComportDataBits_8;
    Options->Parity=e_ComportParity_none;
    Options->StopBits=e_ComportStopBits_1;
    Options->FlowControl=e_ComportFlowControl_None;
}

void Comport_ConvertFromKVList(struct ComportPortOptions *Options,const t_PIKVList *KVList)
{
    const char *BitRateValue;
    const char *DataBitsValue;
    const char *ParityValue;
    const char *StopBitsValue;
    const char *FlowControlValue;

    Comport_DefaultPortOptions(Options);

    BitRateValue=g_CP_System->KVGetItem(KVList,"BitRate");
    DataBitsValue=g_CP_System->KVGetItem(KVList,"DataBits");
    ParityValue=g_CP_System->KVGetItem(KVList,"Parity");
    StopBitsValue=g_CP_System->KVGetItem(KVList,"StopBits");
    FlowControlValue=g_CP_System->KVGetItem(KVList,"FlowControl");

    if(BitRateValue!=NULL)
        Options->BitRate=strtol(BitRateValue,NULL,10);

    if(DataBitsValue!=NULL)
    {
        if(strcasecmp(DataBitsValue,"7")==0)
            Options->DataBits=e_ComportDataBits_7;
        else
            Options->DataBits=e_ComportDataBits_8;
    }

    if(ParityValue!=NULL)
    {
        if(strcasecmp(ParityValue,"odd")==0)
            Options->Parity=e_ComportParity_odd;
        else if(strcasecmp(ParityValue,"even")==0)
            Options->Parity=e_ComportParity_even;
        else if(strcasecmp(ParityValue,"mark")==0)
            Options->Parity=e_ComportParity_mark;
        else if(strcasecmp(ParityValue,"space")==0)
            Options->Parity=e_ComportParity_space;
        else
            Options->Parity=e_ComportParity_none;
    }

    if(StopBitsValue!=NULL)
    {
        if(strcasecmp(StopBitsValue,"2")==0)
            Options->StopBits=e_ComportStopBits_2;
        else
            Options->StopBits=e_ComportStopBits_1;
    }

    if(FlowControlValue!=NULL)
    {
        if(strcasecmp(FlowControlValue,"XOnXOff")==0)
            Options->FlowControl=e_ComportFlowControl_XONXOFF;
        else if(strcasecmp(FlowControlValue,"HW")==0)
            Options->FlowControl=e_ComportFlowControl_Hardware;
        else
            Options->FlowControl=e_ComportFlowControl_None;
    }
}

void Comport_Convert2KVList(const struct ComportPortOptions *Options,
        t_PIKVList *KVList)
{
    char buff[100];
    const char *cstr;

    g_CP_System->KVClear(KVList);
    sprintf(buff,"%d",Options->BitRate);
    g_CP_System->KVAddItem(KVList,"BitRate",buff);

    cstr="8";
    switch(Options->DataBits)
    {
        case e_ComportDataBits_7:
            cstr="7";
        break;
        case e_ComportDataBits_8:
            cstr="8";
        break;
        case e_ComportDataBitsMAX:
        default:
        break;
    }
    g_CP_System->KVAddItem(KVList,"DataBits",cstr);

    cstr="none";
    switch(Options->Parity)
    {
        case e_ComportParity_odd:
            cstr="odd";
        break;
        case e_ComportParity_even:
            cstr="even";
        break;
        case e_ComportParity_space:
            cstr="space";
        break;
        case e_ComportParity_mark:
            cstr="mark";
        break;
        case e_ComportParity_none:
            cstr="none";
        break;
        case e_ComportParityMAX:
        default:
        break;
    }
    g_CP_System->KVAddItem(KVList,"Parity",cstr);

    cstr="1";
    switch(Options->StopBits)
    {
        case e_ComportStopBits_2:
            cstr="2";
        break;
        case e_ComportStopBits_1:
            cstr="1";
        break;
        case e_ComportStopBitsMAX:
        default:
        break;
    }
    g_CP_System->KVAddItem(KVList,"StopBits",cstr);

    cstr="none";
    switch(Options->FlowControl)
    {
        case e_ComportFlowControl_XONXOFF:
            cstr="XOnXOff";
        break;
        case e_ComportFlowControl_Hardware:
            cstr="HW";
        break;
        case e_ComportFlowControl_None:
            cstr="none";
        break;
        case e_ComportFlowControlMAX:
        default:
        break;
    }
    g_CP_System->KVAddItem(KVList,"FlowControl",cstr);
}

/*******************************************************************************
 * NAME:
 *    Comport_SetOptionsFromURI
 *
 * SYNOPSIS:
 *    bool Comport_SetOptionsFromURI(const char *OptionsURIStart,
 *          t_PIKVList *KVList,bool Update);
 *
 * PARAMETERS:
 *    OptionsURIStart [I] -- The URI string set to the first byte of the
 *          baud rate.
 *    KVList [O] -- The KV list to fill with options.
 *    Update [I] -- If this is true then we are updating 'Options'.  If
 *                  false then you should default 'Options' before you
 *                  fill in the options.
 *
 * FUNCTION:
 *    This function helps the URI builder to process the com options.
 *
 * RETURNS:
 *    true -- All ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Comport_SetOptionsFromURI(const char *OptionsURIStart,t_PIKVList *KVList,
        bool Update)
{
    const char *PosStart;
    const char *PosEnd;
    unsigned long Baud;
    char DataBitsChar;
    char ParityChar;
    char StopBitChar;
    struct ComportPortOptions Options;

    /* If we are updating then read the values out of the KVList, else
       default */
    if(Update)
        Comport_ConvertFromKVList(&Options,KVList);
    else
        Comport_DefaultPortOptions(&Options);

    PosStart=OptionsURIStart;

    /* Find the next , (end of baud, start of Data Bits) */
    PosEnd=PosStart;
    while(*PosEnd!=',' && *PosEnd!=0)
        PosEnd++;
    if(*PosEnd==0)
        return false;
    Baud=strtoul(PosStart,NULL,10);

    /* Data bits */
    PosStart=PosEnd+1;
    DataBitsChar=*PosStart;

    /* Parity */
    PosStart++;
    if(*PosStart!=',')
        return false;
    PosStart++;
    ParityChar=*PosStart;

    /* Stop Bits */
    PosStart++;
    if(*PosStart!=',')
        return false;
    PosStart++;
    StopBitChar=*PosStart;

    Options.BitRate=Baud;
    if(DataBitsChar=='7')
        Options.DataBits=e_ComportDataBits_7;
    else
        Options.DataBits=e_ComportDataBits_8;

    if(ParityChar=='n' || ParityChar=='N')
        Options.Parity=e_ComportParity_none;
    if(ParityChar=='o' || ParityChar=='O')
        Options.Parity=e_ComportParity_odd;
    if(ParityChar=='e' || ParityChar=='E')
        Options.Parity=e_ComportParity_even;

    if(StopBitChar=='2')
        Options.StopBits=e_ComportStopBits_2;
    else
        Options.StopBits=e_ComportStopBits_1;

    Comport_Convert2KVList(&Options,KVList);

    return true;
}

struct Comport_ConAuxWidgets *Comport_AllocAuxWidgets(t_DriverIOHandleType *IOHandle,t_WidgetSysHandle *WidgetHandle)
{
    struct Comport_ConAuxWidgets *ConAuxWidgets;
    static const char *InfoViewColumnsNames[]={"Event"};

    ConAuxWidgets=NULL;
    try
    {
        ConAuxWidgets=new struct Comport_ConAuxWidgets;
        ConAuxWidgets->IOHandle=IOHandle;
        ConAuxWidgets->WidgetHandle=WidgetHandle;

        /* DTR -> DSR */
        ConAuxWidgets->DTR_Checkbox=g_CP_UI->AddCheckbox(WidgetHandle,
                "DTR",Comport_DTRCheckboxCallBack,ConAuxWidgets);
        if(ConAuxWidgets->DTR_Checkbox==NULL)
            throw(0);
        g_CP_UI->SetCheckboxChecked(ConAuxWidgets->WidgetHandle,
                ConAuxWidgets->DTR_Checkbox->Ctrl,true);

        /* RTS -> CTS */
        ConAuxWidgets->RTS_Checkbox=g_CP_UI->AddCheckbox(WidgetHandle,
                "RTS",Comport_RTSCheckboxCallBack,ConAuxWidgets);
        if(ConAuxWidgets->RTS_Checkbox==NULL)
            throw(0);
        g_CP_UI->SetCheckboxChecked(ConAuxWidgets->WidgetHandle,
                ConAuxWidgets->RTS_Checkbox->Ctrl,true);

        ConAuxWidgets->DSR_Indicator=g_CP_UI->AddIndicator(WidgetHandle,"DSR");
        if(ConAuxWidgets->DSR_Indicator==NULL)
            throw(0);
        ConAuxWidgets->CTS_Indicator=g_CP_UI->AddIndicator(WidgetHandle,"CTS");
        if(ConAuxWidgets->CTS_Indicator==NULL)
            throw(0);

        ConAuxWidgets->CD_Indicator=g_CP_UI->AddIndicator(WidgetHandle,"CD");
        if(ConAuxWidgets->CD_Indicator==NULL)
            throw(0);
        ConAuxWidgets->RI_Indicator=g_CP_UI->AddIndicator(WidgetHandle,"RI");
        if(ConAuxWidgets->RI_Indicator==NULL)
            throw(0);

        ConAuxWidgets->SendBreakButton=g_CP_UI->AddButtonInput(WidgetHandle,
                "Send Break",Comport_SendBreakButton,ConAuxWidgets);
        if(ConAuxWidgets->SendBreakButton==NULL)
            throw(0);

        ConAuxWidgets->InfoView=g_CP_UI->AddColumnViewInput(WidgetHandle,
                "Logs",1,InfoViewColumnsNames,NULL,NULL);
        if(ConAuxWidgets->InfoView==NULL)
            throw(0);

        ConAuxWidgets->InfoClearButton=g_CP_UI->AddButtonInput(WidgetHandle,
                "Clear",Comport_InfoClearButton,ConAuxWidgets);
        if(ConAuxWidgets->InfoClearButton==NULL)
            throw(0);
    }
    catch(...)
    {
        if(ConAuxWidgets!=NULL)
            Comport_FreeAuxWidgets(WidgetHandle,ConAuxWidgets);
        ConAuxWidgets=NULL;
    }

    return ConAuxWidgets;
}

void Comport_FreeAuxWidgets(t_WidgetSysHandle *WidgetHandle,struct Comport_ConAuxWidgets *ConAuxWidgets)
{
    if(ConAuxWidgets->DTR_Checkbox!=NULL)
        g_CP_UI->FreeCheckbox(WidgetHandle,ConAuxWidgets->DTR_Checkbox);
    if(ConAuxWidgets->RTS_Checkbox!=NULL)
        g_CP_UI->FreeCheckbox(WidgetHandle,ConAuxWidgets->RTS_Checkbox);
    if(ConAuxWidgets->CD_Indicator!=NULL)
        g_CP_UI->FreeIndicator(WidgetHandle,ConAuxWidgets->CD_Indicator);
    if(ConAuxWidgets->RI_Indicator!=NULL)
        g_CP_UI->FreeIndicator(WidgetHandle,ConAuxWidgets->RI_Indicator);
    if(ConAuxWidgets->DSR_Indicator!=NULL)
        g_CP_UI->FreeIndicator(WidgetHandle,ConAuxWidgets->DSR_Indicator);
    if(ConAuxWidgets->CTS_Indicator!=NULL)
        g_CP_UI->FreeIndicator(WidgetHandle,ConAuxWidgets->CTS_Indicator);
    if(ConAuxWidgets->SendBreakButton!=NULL)
        g_CP_UI->FreeButtonInput(WidgetHandle,ConAuxWidgets->SendBreakButton);
    if(ConAuxWidgets->InfoView!=NULL)
        g_CP_UI->FreeColumnViewInput(WidgetHandle,ConAuxWidgets->InfoView);
    if(ConAuxWidgets->InfoClearButton!=NULL)
        g_CP_UI->FreeButtonInput(WidgetHandle,ConAuxWidgets->InfoClearButton);

    delete ConAuxWidgets;
}

void Comport_NotifyOfModemBitsChange(struct Comport_ConAuxWidgets *ConAuxWidgets,bool CD, bool RI, bool DSR, bool CTS)
{
    /* Update the UI here */
    g_CP_UI->SetIndicator(ConAuxWidgets->WidgetHandle,ConAuxWidgets->CD_Indicator->Ctrl,CD);
    g_CP_UI->SetIndicator(ConAuxWidgets->WidgetHandle,ConAuxWidgets->RI_Indicator->Ctrl,RI);
    g_CP_UI->SetIndicator(ConAuxWidgets->WidgetHandle,ConAuxWidgets->DSR_Indicator->Ctrl,DSR);
    g_CP_UI->SetIndicator(ConAuxWidgets->WidgetHandle,ConAuxWidgets->CTS_Indicator->Ctrl,CTS);
}

void Comport_DTRCheckboxCallBack(const struct PICheckboxEvent *Event,void *UserData)
{
    struct Comport_ConAuxWidgets *ConAuxWidgets=(struct Comport_ConAuxWidgets *)UserData;

    switch(Event->EventType)
    {
        case e_PIECheckbox_Changed:
            Comport_UpdateDTR(ConAuxWidgets->IOHandle,Event->Checked);
        break;
        case e_PIECheckboxMAX:
        default:
        break;
    }
}

void Comport_RTSCheckboxCallBack(const struct PICheckboxEvent *Event,void *UserData)
{
    struct Comport_ConAuxWidgets *ConAuxWidgets=(struct Comport_ConAuxWidgets *)UserData;

    switch(Event->EventType)
    {
        case e_PIECheckbox_Changed:
            Comport_UpdateRTS(ConAuxWidgets->IOHandle,Event->Checked);
        break;
        case e_PIECheckboxMAX:
        default:
        break;
    }
}

void Comport_SendBreakButton(const struct PIButtonEvent *Event,void *UserData)
{
    struct Comport_ConAuxWidgets *ConAuxWidgets=(struct Comport_ConAuxWidgets *)UserData;

    switch(Event->EventType)
    {
        case e_PIEButton_Press:
            Comport_SendBreak(ConAuxWidgets->IOHandle);
        break;
        case e_PIEButtonMAX:
        default:
        break;
    }
}

void Comport_InfoClearButton(const struct PIButtonEvent *Event,void *UserData)
{
    struct Comport_ConAuxWidgets *ConAuxWidgets=(struct Comport_ConAuxWidgets *)UserData;

    switch(Event->EventType)
    {
        case e_PIEButton_Press:
            g_CP_UI->ColumnViewInputClear(ConAuxWidgets->WidgetHandle,
                    ConAuxWidgets->InfoView->Ctrl);
        break;
        case e_PIEButtonMAX:
        default:
        break;
    }
}

bool Comport_ReadAuxDTRCheckbox(struct Comport_ConAuxWidgets *ConAuxWidgets)
{
    return g_CP_UI->IsCheckboxChecked(ConAuxWidgets->WidgetHandle,ConAuxWidgets->DTR_Checkbox->Ctrl);
}

bool Comport_ReadAuxRTSCheckbox(struct Comport_ConAuxWidgets *ConAuxWidgets)
{
    return g_CP_UI->IsCheckboxChecked(ConAuxWidgets->WidgetHandle,ConAuxWidgets->RTS_Checkbox->Ctrl);
}

void Comport_AddLogMsg(struct Comport_ConAuxWidgets *ConAuxWidgets,const char *Msg)
{
    int NewRow;

    NewRow=g_CP_UI->ColumnViewInputAddRow(ConAuxWidgets->WidgetHandle,ConAuxWidgets->InfoView->Ctrl);
    g_CP_UI->ColumnViewInputSetColumnText(ConAuxWidgets->WidgetHandle,ConAuxWidgets->InfoView->Ctrl,0,NewRow,Msg);
}
