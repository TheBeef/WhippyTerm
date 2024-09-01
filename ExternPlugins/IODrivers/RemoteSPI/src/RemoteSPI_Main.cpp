/*******************************************************************************
 * FILENAME: RemoteSPI_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the remote SPI plugin it in.
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
 *    Paul Hutchinson (20 Jun 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "RemoteSPI_Main.h"
#include "PluginSDK/IODriver.h"
#include "PluginSDK/Plugin.h"
#include "OS/RemoteSPI_Socket.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/
#define REMOTESPI_HEADER_BYTE       0xFF
#define REMOTESPI_FOOTER_BYTE       0x77

#define REMOTESPI_URI_PREFIX                    "RSPI"
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      RemoteSPI // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x000B0000
#define DEFAULT_PORT                            2000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct RemoteSPI_ConWidgets
{
    struct PI_TextInput *ServerAddress;
    struct PI_NumberInput *PortNumber;
    struct PI_NumberInput *CS;
    struct PI_NumberInput *Mode;
    struct PI_Checkbox *UseGPIOCS;
    struct PI_NumberInput *SPIDev;
    struct PI_NumberInput *GPIODev;
    struct PI_NumberInput *GPIOPin;
    struct PI_NumberInput *Speed;
    struct PI_NumberInput *Bits;
};

typedef enum
{
    e_RemoteSPIReadState_Header,
    e_RemoteSPIReadState_Cmd,
    e_RemoteSPIReadState_Size1,
    e_RemoteSPIReadState_Size2,
    e_RemoteSPIReadState_Data,
    e_RemoteSPIReadState_Footer,
    e_RemoteSPIReadState_SendUpData,
    e_RemoteSPIReadStateMAX
} e_RemoteSPIReadStateType;

typedef enum
{
    e_RemoteSPICmd_SetParam         =0x00,
    e_RemoteSPICmd_Open             =0x01,
    e_RemoteSPICmd_Close            =0x02,
    e_RemoteSPICmd_WriteData        =0x03,
    e_RemoteSPICmd_ReadData         =0x04,
    e_RemoteSPICmd_OpenStatus       =0x05,
    e_RemoteSPICmd_Error            =0x06,
    e_RemoteSPICmdMAX
} e_RemoteSPICmdType;

typedef enum
{
    e_RemoteSPIErrors_None,
    e_RemoteSPIErrors_Failed2Open,
    e_RemoteSPIErrors_Failed2SetMode,
    e_RemoteSPIErrors_Failed2SetParam,
    e_RemoteSPIErrors_NotOpen,
    e_RemoteSPIErrors_Failed2Write,
    e_RemoteSPIErrorsMAX
} e_RemoteSPIErrorsType;

struct RemoteSPI_OurData
{
    struct RemoteSPI_OSDriverData *DriverData;
    e_RemoteSPIReadStateType ReadState;
    e_RemoteSPICmdType ReadCmd;
    int ReadLen;
    uint8_t ReadBuff[100];
    int BytesInReadBuff;
    uint8_t ReadData[65536];    // Store rx data here until we have the full packet
    int ReadDataLen;
    int ReadDataPos;    // How much have we sent up
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL RemoteSPI_Init(void);
const struct IODriverInfo *RemoteSPI_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *RemoteSPI_DetectDevices(void);
void RemoteSPI_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
t_ConnectionWidgetsType *RemoteSPI_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle);
void RemoteSPI_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle);
void RemoteSPI_ConnectionOptionsWidgets_StoreUI(t_ConnectionWidgetsType *ConOptions,
            t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
            t_PIKVList *Options);
void RemoteSPI_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options);
PG_BOOL RemoteSPI_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL RemoteSPI_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
PG_BOOL RemoteSPI_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);
t_DriverIOHandleType *RemoteSPI_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
void RemoteSPI_FreeHandle(t_DriverIOHandleType *DriverIO);
PG_BOOL RemoteSPI_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
void RemoteSPI_Close(t_DriverIOHandleType *DriverIO);
int RemoteSPI_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes);
int RemoteSPI_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
PG_BOOL RemoteSPI_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options);
static int RemoteSPI_SendMessage(t_DriverIOHandleType *DriverIO,
        e_RemoteSPICmdType cmd,const uint8_t *Data,int DataLen);

/*** VARIABLE DEFINITIONS     ***/
const struct IODriverAPI g_RemoteSPIPluginAPI=
{
    RemoteSPI_Init,
    RemoteSPI_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    RemoteSPI_DetectDevices,
    RemoteSPI_FreeDetectedDevices,
    RemoteSPI_GetConnectionInfo,
    RemoteSPI_ConnectionOptionsWidgets_AllocWidgets,
    RemoteSPI_ConnectionOptionsWidgets_FreeWidgets,
    RemoteSPI_ConnectionOptionsWidgets_StoreUI,
    RemoteSPI_ConnectionOptionsWidgets_UpdateUI,
    RemoteSPI_Convert_URI_To_Options,
    RemoteSPI_Convert_Options_To_URI,
    RemoteSPI_AllocateHandle,
    RemoteSPI_FreeHandle,
    RemoteSPI_Open,
    RemoteSPI_Close,
    RemoteSPI_Read,
    RemoteSPI_Write,
    RemoteSPI_ChangeOptions,
    NULL,                                               // Transmit
};

struct IODriverInfo m_RemoteSPIInfo=
{
    IODRVINFOFLAG_BLOCKDEV
};

const struct IOS_API *g_RSPI_IOSystem;
const struct PI_UIAPI *g_RSPI_UI;
const struct PI_SystemAPI *g_RSPI_System;

static const struct IODriverDetectedInfo g_RSPI_DeviceInfo=
{
    NULL,
    sizeof(struct IODriverDetectedInfo),
    0,                              // Flags
    REMOTESPI_URI_PREFIX,           // DeviceUniqueID
    "RemoteSPI Socket Client",      // Name
    "RemoteSPI",                    // Title
};

/*******************************************************************************
 * NAME:
 *    RemoteSPI_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int RemoteSPI_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        g_RSPI_System=SysAPI;
        g_RSPI_IOSystem=g_RSPI_System->GetAPI_IO();
        g_RSPI_UI=g_RSPI_IOSystem->GetAPI_UI();

        g_RSPI_IOSystem->RegisterDriver("RemoteSPI",REMOTESPI_URI_PREFIX,
                &g_RemoteSPIPluginAPI,sizeof(g_RemoteSPIPluginAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_Init
 *
 * SYNOPSIS:
 *    PG_BOOL RemoteSPI_Init(void);
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
PG_BOOL RemoteSPI_Init(void)
{
    return true;
}

/*******************************************************************************
 * NAME:
 *   RemoteSPI_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*RemoteSPI_GetDriverInfo)(
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
const struct IODriverInfo *RemoteSPI_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_RemoteSPIInfo;
}

/*******************************************************************************
 * NAME:
 *    DetectDevices
 *
 * SYNOPSIS:
 *    const struct IODriverDetectedInfo *DetectDevices(void);
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
const struct IODriverDetectedInfo *RemoteSPI_DetectDevices(void)
{
    return &g_RSPI_DeviceInfo;
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
void RemoteSPI_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_ConnectionOptionsWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionWidgetsType *RemoteSPI_ConnectionOptionsWidgets_AllocWidgets(
 *          t_WidgetSysHandle *WidgetHandle);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to send to the widgets
 *
 * FUNCTION:
 *    This function adds options widgets to a container widget.  These are
 *    options for the connection.  It's things like bit rate, parity, or
 *    any other options the device supports.
 *
 *    The device driver needs to keep handles to the widgets added because it
 *    needs to free them when RemoveNewConnectionOptionsWidgets() called.
 *
 * RETURNS:
 *    The private options data that you want to use.  This is a private
 *    structure that you allocate and then cast to
 *    (t_ConnectionWidgetsType *) when you return.
 *
 * NOTES:
 *    This function must be reentrant.  The system may allocate many sets
 *    of option widgets and free them in any order.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_ConnectionWidgetsType *RemoteSPI_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle)
{
    struct RemoteSPI_ConWidgets *ConWidgets;

    ConWidgets=NULL;
    try
    {
        ConWidgets=new struct RemoteSPI_ConWidgets;

        ConWidgets->ServerAddress=NULL;
        ConWidgets->PortNumber=NULL;
        ConWidgets->CS=NULL;
        ConWidgets->Mode=NULL;
        ConWidgets->UseGPIOCS=NULL;
        ConWidgets->SPIDev=NULL;
        ConWidgets->GPIODev=NULL;
        ConWidgets->GPIOPin=NULL;
        ConWidgets->Speed=NULL;
        ConWidgets->Bits=NULL;

        ConWidgets->ServerAddress=g_RSPI_UI->AddTextInput(WidgetHandle,
                "Remote Server",NULL,NULL);
        if(ConWidgets->ServerAddress==NULL)
            throw(0);

        ConWidgets->PortNumber=g_RSPI_UI->AddNumberInput(WidgetHandle,"Remote Server Port",
                NULL,NULL);
        if(ConWidgets->PortNumber==NULL)
            throw(0);

        g_RSPI_UI->SetNumberInputMinMax(WidgetHandle,
                ConWidgets->PortNumber->Ctrl,1,65535);

        /* SPI Mode */
        ConWidgets->Mode=g_RSPI_UI->AddNumberInput(WidgetHandle,"SPI Mode",
                NULL,NULL);
        if(ConWidgets->Mode==NULL)
            throw(0);
        g_RSPI_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->Mode->Ctrl,
                0,3);

        /* SPI device number */
        ConWidgets->SPIDev=g_RSPI_UI->AddNumberInput(WidgetHandle,"SPI Device",
                NULL,NULL);
        if(ConWidgets->SPIDev==NULL)
            throw(0);
        g_RSPI_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->SPIDev->Ctrl,
                0,255);

        /* Chip select */
        ConWidgets->CS=g_RSPI_UI->AddNumberInput(WidgetHandle,"Chip Select",
                NULL,NULL);
        if(ConWidgets->CS==NULL)
            throw(0);
        g_RSPI_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->CS->Ctrl,
                0,255);

        /* SPI Speed */
        ConWidgets->Speed=g_RSPI_UI->AddNumberInput(WidgetHandle,"Speed (KHz)",
                NULL,NULL);
        if(ConWidgets->Speed==NULL)
            throw(0);
        g_RSPI_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->Speed->Ctrl,
                1,1024);

        /* SPI Bits */
        ConWidgets->Bits=g_RSPI_UI->AddNumberInput(WidgetHandle,"Bits per word",
                NULL,NULL);
        if(ConWidgets->Bits==NULL)
            throw(0);
        g_RSPI_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->Bits->Ctrl,
                5,32);

        /* Use GPIO CS */
        ConWidgets->UseGPIOCS=g_RSPI_UI->AddCheckbox(WidgetHandle,
                "Use GPIO as CS",NULL,NULL);
        if(ConWidgets->UseGPIOCS==NULL)
            throw(0);

        /* GPIO Device */
        ConWidgets->GPIODev=g_RSPI_UI->AddNumberInput(WidgetHandle,"GPIO Device",
                NULL,NULL);
        if(ConWidgets->GPIODev==NULL)
            throw(0);
        g_RSPI_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->GPIODev->Ctrl,
                0,255);

        /* GPIO Pin */
        ConWidgets->GPIOPin=g_RSPI_UI->AddNumberInput(WidgetHandle,"GPIO Pin",
                NULL,NULL);
        if(ConWidgets->GPIOPin==NULL)
            throw(0);
        g_RSPI_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->GPIOPin->Ctrl,
                0,255);
    }
    catch(...)
    {
        if(ConWidgets!=NULL)
        {
            if(ConWidgets->ServerAddress!=NULL)
                g_RSPI_UI->FreeTextInput(WidgetHandle,ConWidgets->ServerAddress);
            if(ConWidgets->PortNumber!=NULL)
                g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);
            if(ConWidgets->CS!=NULL)
                g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->CS);
            if(ConWidgets->Mode!=NULL)
                g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->Mode);
            if(ConWidgets->UseGPIOCS!=NULL)
                g_RSPI_UI->FreeCheckbox(WidgetHandle,ConWidgets->UseGPIOCS);
            if(ConWidgets->SPIDev!=NULL)
                g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->SPIDev);
            if(ConWidgets->GPIODev!=NULL)
                g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->GPIODev);
            if(ConWidgets->GPIOPin!=NULL)
                g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->GPIOPin);
            if(ConWidgets->Speed!=NULL)
                g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->Speed);
            if(ConWidgets->Bits!=NULL)
                g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->Bits);

            delete ConWidgets;
        }
        return NULL;
    }

    return (t_ConnectionWidgetsType *)ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_ConnectionOptionsWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void RemoteSPI_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionWidgetsType *
 *              ConOptions,t_WidgetSysHandle *WidgetHandle);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *          ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *
 * FUNCTION:
 *    Frees the widgets added with ConnectionOptionsWidgets_AllocWidgets()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RemoteSPI_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle)
{
    struct RemoteSPI_ConWidgets *ConWidgets=(struct RemoteSPI_ConWidgets *)ConOptions;

    if(ConWidgets->ServerAddress!=NULL)
        g_RSPI_UI->FreeTextInput(WidgetHandle,ConWidgets->ServerAddress);
    if(ConWidgets->PortNumber!=NULL)
        g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);
    if(ConWidgets->CS!=NULL)
        g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->CS);
    if(ConWidgets->Mode!=NULL)
        g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->Mode);
    if(ConWidgets->UseGPIOCS!=NULL)
        g_RSPI_UI->FreeCheckbox(WidgetHandle,ConWidgets->UseGPIOCS);
    if(ConWidgets->SPIDev!=NULL)
        g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->SPIDev);
    if(ConWidgets->GPIODev!=NULL)
        g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->GPIODev);
    if(ConWidgets->GPIOPin!=NULL)
        g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->GPIOPin);
    if(ConWidgets->Speed!=NULL)
        g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->Speed);
    if(ConWidgets->Bits!=NULL)
        g_RSPI_UI->FreeNumberInput(WidgetHandle,ConWidgets->Bits);

    delete ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_ConnectionOptionsWidgets_StoreUI
 *
 * SYNOPSIS:
 *      void RemoteSPI_ConnectionOptionsWidgets_StoreUI(
 *              t_ConnectionWidgetsType *ConOptions,
 *              t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
 *              t_PIKVList *Options);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *          ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [O] -- The options for this connection.
 *
 * FUNCTION:
 *    This function takes the widgets added with
 *    ConnectionOptionsWidgets_AllocWidgets() and stores them is a key/value pair
 *    list in 'Options'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RemoteSPI_ConnectionOptionsWidgets_StoreUI(t_ConnectionWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct RemoteSPI_ConWidgets *ConWidgets=(struct RemoteSPI_ConWidgets *)ConOptions;
    const char *AddressStr;
    char TmpStr[100];
    uint16_t PortNum;
    int CS;
    int Mode;
    bool UseGPIOCS;
    int SPIDev;
    int GPIODev;
    int GPIOPin;
    int Speed;
    int Bits;

    if(ConWidgets->ServerAddress==NULL || ConWidgets->PortNumber==NULL ||
            ConWidgets->CS==NULL || ConWidgets->Mode==NULL ||
            ConWidgets->UseGPIOCS==NULL || ConWidgets->SPIDev==NULL ||
            ConWidgets->GPIODev==NULL || ConWidgets->GPIOPin==NULL ||
            ConWidgets->Speed==NULL || ConWidgets->Bits==NULL)
    {
        return;
    }

    g_RSPI_System->KVClear(Options);

    AddressStr=g_RSPI_UI->GetTextInputText(WidgetHandle,
            ConWidgets->ServerAddress->Ctrl);
    g_RSPI_System->KVAddItem(Options,"Address",AddressStr);

    PortNum=g_RSPI_UI->GetNumberInputValue(WidgetHandle,
            ConWidgets->PortNumber->Ctrl);
    sprintf(TmpStr,"%d",PortNum);
    g_RSPI_System->KVAddItem(Options,"Port",TmpStr);

    CS=g_RSPI_UI->GetNumberInputValue(WidgetHandle,ConWidgets->CS->Ctrl);
    sprintf(TmpStr,"%d",CS);
    g_RSPI_System->KVAddItem(Options,"ChipSelect",TmpStr);

    Mode=g_RSPI_UI->GetNumberInputValue(WidgetHandle,ConWidgets->Mode->Ctrl);
    sprintf(TmpStr,"%d",Mode);
    g_RSPI_System->KVAddItem(Options,"Mode",TmpStr);

    UseGPIOCS=g_RSPI_UI->IsCheckboxChecked(WidgetHandle,
            ConWidgets->UseGPIOCS->Ctrl);
    sprintf(TmpStr,"%d",UseGPIOCS?1:0);
    g_RSPI_System->KVAddItem(Options,"UseGPIOCS",TmpStr);

    SPIDev=g_RSPI_UI->GetNumberInputValue(WidgetHandle,ConWidgets->SPIDev->Ctrl);
    sprintf(TmpStr,"%d",SPIDev);
    g_RSPI_System->KVAddItem(Options,"SPIDev",TmpStr);

    GPIODev=g_RSPI_UI->GetNumberInputValue(WidgetHandle,ConWidgets->GPIODev->Ctrl);
    sprintf(TmpStr,"%d",GPIODev);
    g_RSPI_System->KVAddItem(Options,"GPIODev",TmpStr);

    GPIOPin=g_RSPI_UI->GetNumberInputValue(WidgetHandle,ConWidgets->GPIOPin->Ctrl);
    sprintf(TmpStr,"%d",GPIOPin);
    g_RSPI_System->KVAddItem(Options,"GPIOPin",TmpStr);

    Speed=g_RSPI_UI->GetNumberInputValue(WidgetHandle,ConWidgets->Speed->Ctrl);
    sprintf(TmpStr,"%d",Speed);
    g_RSPI_System->KVAddItem(Options,"Speed",TmpStr);

    Bits=g_RSPI_UI->GetNumberInputValue(WidgetHandle,ConWidgets->Bits->Ctrl);
    sprintf(TmpStr,"%d",Bits);
    g_RSPI_System->KVAddItem(Options,"Bits",TmpStr);

}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_ConnectionOptionsWidgets_UpdateUI
 *
 * SYNOPSIS:
 *    void RemoteSPI_ConnectionOptionsWidgets_UpdateUI(
 *          t_ConnectionWidgetsType *ConOptions,
 *          t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
 *          t_PIKVList *Options);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *                      ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [I] -- The options for this connection.
 *
 * FUNCTION:
 *    This function takes the widgets added with
 *    ConnectionOptionsWidgets_AllocWidgets() and sets them to the values stored in
 *    the key/value pair list in 'Options'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ConnectionOptionsWidgets_StoreUI()
 ******************************************************************************/
void RemoteSPI_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct RemoteSPI_ConWidgets *ConWidgets=(struct RemoteSPI_ConWidgets *)ConOptions;
    const char *AddressStr;
    const char *PortStr;
    const char *CSStr;
    const char *ModeStr;
    const char *UseGPIOCSStr;
    const char *SPIDevStr;
    const char *GPIODevStr;
    const char *GPIOPinStr;
    const char *SpeedStr;
    const char *BitsStr;
    uint16_t PortNum;
    int CSNum;
    int ModeNum;
    bool UseGPIOCSNum;
    int SPIDevNum;
    int GPIODevNum;
    int GPIOPinNum;
    int SpeedNum;
    int BitsNum;

    if(ConWidgets->ServerAddress==NULL || ConWidgets->PortNumber==NULL ||
            ConWidgets->CS==NULL || ConWidgets->Mode==NULL ||
            ConWidgets->UseGPIOCS==NULL || ConWidgets->SPIDev==NULL ||
            ConWidgets->GPIODev==NULL || ConWidgets->GPIOPin==NULL ||
            ConWidgets->Speed==NULL || ConWidgets->Bits==NULL)
    {
        return;
    }

    AddressStr=g_RSPI_System->KVGetItem(Options,"Address");
    PortStr=g_RSPI_System->KVGetItem(Options,"Port");
    CSStr=g_RSPI_System->KVGetItem(Options,"ChipSelect");
    ModeStr=g_RSPI_System->KVGetItem(Options,"Mode");
    UseGPIOCSStr=g_RSPI_System->KVGetItem(Options,"UseGPIOCS");
    SPIDevStr=g_RSPI_System->KVGetItem(Options,"SPIDev");
    GPIODevStr=g_RSPI_System->KVGetItem(Options,"GPIODev");
    GPIOPinStr=g_RSPI_System->KVGetItem(Options,"GPIOPin");
    SpeedStr=g_RSPI_System->KVGetItem(Options,"Speed");
    BitsStr=g_RSPI_System->KVGetItem(Options,"Bits");

    if(AddressStr==NULL)
        AddressStr="localhost";
    if(PortStr==NULL)
        PortStr="80";
    if(CSStr==NULL)
        CSStr="0";
    if(ModeStr==NULL)
        ModeStr="0";
    if(UseGPIOCSStr==NULL)
        UseGPIOCSStr="0";
    if(SPIDevStr==NULL)
        SPIDevStr="0";
    if(GPIODevStr==NULL)
        GPIODevStr="0";
    if(GPIOPinStr==NULL)
        GPIOPinStr="22";
    if(SpeedStr==NULL)
        SpeedStr="100";
    if(BitsStr==NULL)
        BitsStr="8";

    g_RSPI_UI->SetTextInputText(WidgetHandle,ConWidgets->ServerAddress->Ctrl,
            AddressStr);

    PortNum=atoi(PortStr);
    g_RSPI_UI->SetNumberInputValue(WidgetHandle,ConWidgets->PortNumber->Ctrl,
            PortNum);

    CSNum=atoi(CSStr);
    g_RSPI_UI->SetNumberInputValue(WidgetHandle,ConWidgets->CS->Ctrl,CSNum);

    ModeNum=atoi(ModeStr);
    g_RSPI_UI->SetNumberInputValue(WidgetHandle,ConWidgets->Mode->Ctrl,ModeNum);

    UseGPIOCSNum=atoi(UseGPIOCSStr);
    g_RSPI_UI->SetCheckboxChecked(WidgetHandle,ConWidgets->UseGPIOCS->Ctrl,UseGPIOCSNum);

    SPIDevNum=atoi(SPIDevStr);
    g_RSPI_UI->SetNumberInputValue(WidgetHandle,ConWidgets->SPIDev->Ctrl,SPIDevNum);

    GPIODevNum=atoi(GPIODevStr);
    g_RSPI_UI->SetNumberInputValue(WidgetHandle,ConWidgets->GPIODev->Ctrl,GPIODevNum);

    GPIOPinNum=atoi(GPIOPinStr);
    g_RSPI_UI->SetNumberInputValue(WidgetHandle,ConWidgets->GPIOPin->Ctrl,GPIOPinNum);

    SpeedNum=atoi(SpeedStr);
    g_RSPI_UI->SetNumberInputValue(WidgetHandle,ConWidgets->Speed->Ctrl,SpeedNum);

    BitsNum=atoi(BitsStr);
    g_RSPI_UI->SetNumberInputValue(WidgetHandle,ConWidgets->Bits->Ctrl,BitsNum);
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL RemoteSPI_Convert_Options_To_URI(const char *DeviceUniqueID,
 *          t_PIKVList *Options,char *URI,int MaxURILen);
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
 *    a buffer.
 *
 *    This is in the format of:
 *      "RSPI://localhost:2000/1"
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Convert_URI_To_Options
 ******************************************************************************/
PG_BOOL RemoteSPI_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    const char *AddressStr;
    const char *PortStr;
    const char *CSStr;
    unsigned long Port;

    AddressStr=g_RSPI_System->KVGetItem(Options,"Address");
    PortStr=g_RSPI_System->KVGetItem(Options,"Port");
    CSStr=g_RSPI_System->KVGetItem(Options,"ChipSelect");
    if(AddressStr==NULL || PortStr==NULL || CSStr==NULL)
        return false;

    Port=strtoul(PortStr,NULL,10);

    if(Port==DEFAULT_PORT)
        PortStr="";

    if(strlen(REMOTESPI_URI_PREFIX)+1+strlen(AddressStr)+1+strlen(PortStr)+1>=
            MaxURILen)
    {
        return false;
    }

    strcpy(URI,REMOTESPI_URI_PREFIX);
    strcat(URI,"://");
    strcat(URI,AddressStr);
    if(Port!=DEFAULT_PORT)
    {
        strcat(URI,":");
        strcat(URI,PortStr);
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL RemoteSPI_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
 *          char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
 *          PG_BOOL Update);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to convert to a device ID and options.
 *    Options [O] -- The options for this new connection.  Any options
 *                   that don't come from the URI should be defaults.
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
PG_BOOL RemoteSPI_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    const char *PosStart;
    const char *PortStart;
    const char *CSStart;
    const char *AddressStart;
    unsigned int AddressLen;
    unsigned long Port;
    unsigned long ChipSelect;
    char ServerAddressBuffer[256];
    char CSBuff[100];
    char PortBuff[100];
    string TmpDeviceUniqueIDStr;

    /* Make sure it starts with RSPI:// */
    if(strncasecmp(URI,REMOTESPI_URI_PREFIX "://",
            (sizeof(REMOTESPI_URI_PREFIX)-1)+1+2)!=0)   // +1 for ':' and +2 for '//'
    {
        return false;
    }

    g_RSPI_System->KVClear(Options);

    PosStart=URI;
    PosStart+=sizeof(REMOTESPI_URI_PREFIX)-1;  // -1 because of the \0
    PosStart+=3;    // Slip ://

    AddressStart=PosStart;
// *      "RSPI://localhost:2000/1"
// *      "RSPI://localhost/1"

    /* Get the server address */
    PortStart=strchr(PosStart,':');
    CSStart=strchr(PosStart,'/');

    ChipSelect=0;
    Port=2000;

    if(PortStart!=NULL)
    {
        AddressLen=PortStart-AddressStart;
        PortStart++;    // Slip :
        Port=strtoul(PortStart,NULL,10);
    }
    else if(CSStart!=NULL)
    {
        AddressLen=CSStart-AddressStart;
    }
    else
    {
        AddressLen=strlen(AddressStart);
    }

    if(CSStart!=NULL)
    {
        /* Get chip select */
        ChipSelect=strtoul(PortStart,NULL,10);
    }

    sprintf(PortBuff,"%ld",Port);
    sprintf(CSBuff,"%ld",ChipSelect);

    /* Make sure we have space for this address */
    if(AddressLen>=sizeof(ServerAddressBuffer)-1)
        return false;

    strncpy(ServerAddressBuffer,AddressStart,AddressLen);
    ServerAddressBuffer[AddressLen]=0;

    g_RSPI_System->KVAddItem(Options,"Address",ServerAddressBuffer);
    g_RSPI_System->KVAddItem(Options,"Port",PortBuff);
    g_RSPI_System->KVAddItem(Options,"ChipSelect",CSBuff);

    TmpDeviceUniqueIDStr="";
    TmpDeviceUniqueIDStr+=ServerAddressBuffer;
    if(PortStart!=NULL)
    {
        TmpDeviceUniqueIDStr+=":";
        TmpDeviceUniqueIDStr+=PortBuff;
    }
    if(CSStart!=NULL)
    {
        TmpDeviceUniqueIDStr+="/";
        TmpDeviceUniqueIDStr+=CSBuff;
    }

    if(TmpDeviceUniqueIDStr.length()>=MaxDeviceUniqueIDLen)
        return false;

    strcpy(DeviceUniqueID,TmpDeviceUniqueIDStr.c_str());

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
PG_BOOL RemoteSPI_GetConnectionInfo(const char *DeviceUniqueID,
        t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    const char *AddressStr;
    const char *PortStr;
    string Title;

    /* Fill in defaults */
    strcpy(RetInfo->Name,g_RSPI_DeviceInfo.Name);
    RetInfo->Flags=g_RSPI_DeviceInfo.Flags;

    Title=g_RSPI_DeviceInfo.Title;

    AddressStr=g_RSPI_System->KVGetItem(Options,"Address");
    if(AddressStr!=NULL)
    {
        Title=AddressStr;
        PortStr=g_RSPI_System->KVGetItem(Options,"Port");
        if(PortStr!=NULL)
        {
            Title+=":";
            Title+=PortStr;
        }
    }

    snprintf(RetInfo->Title,sizeof(RetInfo->Title),"%s",Title.c_str());

    return true;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *RemoteSPI_AllocateHandle(const char *DeviceUniqueID,
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
t_DriverIOHandleType *RemoteSPI_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct RemoteSPI_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct RemoteSPI_OurData;

        NewData->ReadState=e_RemoteSPIReadState_Header;
        NewData->BytesInReadBuff=0;
        NewData->ReadDataLen=0;
        NewData->ReadDataPos=0;

        NewData->DriverData=RemoteSPI_OSAllocateHandle(DeviceUniqueID,IOHandle);
        if(NewData->DriverData==NULL)
            throw(0);
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
 *    RemoteSPI_FreeHandle
 *
 * SYNOPSIS:
 *    void RemoteSPI_FreeHandle(t_DriverIOHandleType *DriverIO);
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
void RemoteSPI_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct RemoteSPI_OurData *OurData=(struct RemoteSPI_OurData *)DriverIO;

    RemoteSPI_OSFreeHandle(OurData->DriverData);
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_Open
 *
 * SYNOPSIS:
 *    PG_BOOL RemoteSPI_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
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
 *    RemoteSPI_Close(), RemoteSPI_Read(), RemoteSPI_Write(),
 *    RemoteSPI_ChangeOptions()
 ******************************************************************************/
PG_BOOL RemoteSPI_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct RemoteSPI_OurData *OurData=(struct RemoteSPI_OurData *)DriverIO;
    const char *CSStr;
    const char *ModeStr;
    const char *UseGPIOCSStr;
    const char *SPIDevStr;
    const char *GPIODevStr;
    const char *GPIOPinStr;
    const char *SpeedStr;
    const char *BitsStr;
    uint8_t SendBuff[100];
    uint32_t Speed;

    try
    {
        CSStr=g_RSPI_System->KVGetItem(Options,"ChipSelect");
        ModeStr=g_RSPI_System->KVGetItem(Options,"Mode");
        UseGPIOCSStr=g_RSPI_System->KVGetItem(Options,"UseGPIOCS");
        SPIDevStr=g_RSPI_System->KVGetItem(Options,"SPIDev");
        GPIODevStr=g_RSPI_System->KVGetItem(Options,"GPIODev");
        GPIOPinStr=g_RSPI_System->KVGetItem(Options,"GPIOPin");
        SpeedStr=g_RSPI_System->KVGetItem(Options,"Speed");
        BitsStr=g_RSPI_System->KVGetItem(Options,"Bits");

        if(CSStr==NULL || ModeStr==NULL || UseGPIOCSStr==NULL ||
            SPIDevStr==NULL || GPIODevStr==NULL || GPIOPinStr==NULL ||
            SpeedStr==NULL || BitsStr==NULL)
        {
            return false;
        }

        if(!RemoteSPI_OSOpen(OurData->DriverData,Options))
            return false;

        Speed=strtoul(SpeedStr,NULL,10)*1000;

        /* <SPIMode><UseGPIO><SPIDev><SPIcs><GPIODev><GPIOPin><Speed:4><Bits> */
        SendBuff[0]=strtoul(ModeStr,NULL,10);   // SPIMode
        SendBuff[1]=strtoul(UseGPIOCSStr,NULL,10);   // UseGPIO
        SendBuff[2]=strtoul(SPIDevStr,NULL,10);   // SPIDev
        SendBuff[3]=strtoul(CSStr,NULL,10);     // SPI CS
        SendBuff[4]=strtoul(GPIODevStr,NULL,10);   // GPIO Dev
        SendBuff[5]=strtoul(GPIOPinStr,NULL,10);   // GPIO Pin
        SendBuff[6]=Speed&0xFF;                 // Speed
        SendBuff[7]=(Speed>>8)&0xFF;
        SendBuff[8]=(Speed>>16)&0xFF;
        SendBuff[9]=(Speed>>24)&0xFF;
        SendBuff[10]=strtoul(BitsStr,NULL,10);  // Bits

        if(RemoteSPI_SendMessage(DriverIO,e_RemoteSPICmd_SetParam,SendBuff,11)<0)
            throw(0);
        if(RemoteSPI_SendMessage(DriverIO,e_RemoteSPICmd_Open,NULL,0)<0)
            throw(0);
    }
    catch(...)
    {
        RemoteSPI_OSClose(OurData->DriverData);
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_Close
 *
 * SYNOPSIS:
 *    void RemoteSPI_Close(t_DriverIOHandleType *DriverIO);
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
 *    RemoteSPI_Open()
 ******************************************************************************/
void RemoteSPI_Close(t_DriverIOHandleType *DriverIO)
{
    struct RemoteSPI_OurData *OurData=(struct RemoteSPI_OurData *)DriverIO;

    RemoteSPI_SendMessage(DriverIO,e_RemoteSPICmd_Close,NULL,0);
    RemoteSPI_OSClose(OurData->DriverData);
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_Read
 *
 * SYNOPSIS:
 *    int RemoteSPI_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
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
 *    RemoteSPI_Open(), RemoteSPI_Write()
 ******************************************************************************/
int RemoteSPI_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    struct RemoteSPI_OurData *OurData=(struct RemoteSPI_OurData *)DriverIO;
    int RetBytes;
    uint8_t c;
    int r;
    int RetValue;
    int Bytes2Copy;
    int BytesLeft;

    RetValue=0;
    if(OurData->ReadState==e_RemoteSPIReadState_SendUpData)
    {
        /* We are sending up the data from the last packet */
        BytesLeft=OurData->ReadDataLen-OurData->ReadDataPos;

        if(BytesLeft>0)
        {
            Bytes2Copy=MaxBytes;
            if(Bytes2Copy>BytesLeft)
                Bytes2Copy=BytesLeft;
            memcpy(Data,&OurData->ReadData[OurData->ReadDataPos],Bytes2Copy);
            OurData->ReadDataPos+=Bytes2Copy;
            return Bytes2Copy;
        }

        /* Move on to the next packet */
        OurData->ReadState=e_RemoteSPIReadState_Header;
        RemoteSPI_OSForceDataAvailable(OurData->DriverData,false);
    }

    RetValue=0;
    while(RetValue==0)
    {
        if(OurData->BytesInReadBuff==0)
        {
            /* Get more data */
            RetBytes=RemoteSPI_OSRead(OurData->DriverData,OurData->ReadBuff,
                    sizeof(OurData->ReadBuff));
            if(RetBytes<=0)
                return RetBytes;
            OurData->BytesInReadBuff=RetBytes;
        }

        for(r=0;r<OurData->BytesInReadBuff && RetValue==0;r++)
        {
            c=OurData->ReadBuff[r];
            switch(OurData->ReadState)
            {
                case e_RemoteSPIReadState_Header:
                    if(c==REMOTESPI_HEADER_BYTE)
                        OurData->ReadState=e_RemoteSPIReadState_Cmd;
                break;
                case e_RemoteSPIReadState_Cmd:
                    OurData->ReadCmd=(e_RemoteSPICmdType)c;
                    OurData->ReadState=e_RemoteSPIReadState_Size1;
                break;
                case e_RemoteSPIReadState_Size1:
                    OurData->ReadLen=((uint16_t)c)<<8;
                    OurData->ReadState=e_RemoteSPIReadState_Size2;
                break;
                case e_RemoteSPIReadState_Size2:
                    OurData->ReadLen|=c;
                    OurData->ReadDataLen=0;
                    OurData->ReadState=e_RemoteSPIReadState_Data;
                break;
                case e_RemoteSPIReadState_Data:
                    OurData->ReadData[OurData->ReadDataLen++]=c;
                    if(OurData->ReadDataLen==OurData->ReadLen)
                        OurData->ReadState=e_RemoteSPIReadState_Footer;
                break;
                case e_RemoteSPIReadState_Footer:
                    if(c==REMOTESPI_FOOTER_BYTE)
                    {
                        switch(OurData->ReadCmd)
                        {
                            case e_RemoteSPICmd_ReadData:
                                OurData->ReadState=e_RemoteSPIReadState_Header;
                                OurData->ReadDataPos=0;

                                /* Send up the first block of data */
                                BytesLeft=OurData->ReadDataLen;

                                if(BytesLeft>0)
                                {
                                    Bytes2Copy=MaxBytes;
                                    if(Bytes2Copy>BytesLeft)
                                        Bytes2Copy=BytesLeft;
                                    memcpy(Data,&OurData->ReadData[
                                            OurData->ReadDataPos],Bytes2Copy);
                                    OurData->ReadDataPos+=Bytes2Copy;
                                    RetValue=Bytes2Copy;

                                    OurData->ReadState=
                                            e_RemoteSPIReadState_SendUpData;

                                    /* Force the watching thread to think there
                                       is data */
                                    if(OurData->ReadDataPos<OurData->
                                            ReadDataLen)
                                    {
                                        RemoteSPI_OSForceDataAvailable(OurData->
                                                DriverData,true);
                                    }

                                    /* We will return out after this */
                                }
                            break;
                            case e_RemoteSPICmd_OpenStatus:
                                if(OurData->ReadDataLen>0)
                                {
                                    if(!OurData->ReadData[0])
                                    {
                                        /* The SPI port was closed, close
                                           the connection */
                                        RemoteSPI_OSClose(OurData->DriverData);
                                        return RETERROR_DISCONNECT;
                                    }
                                }
                                OurData->ReadState=e_RemoteSPIReadState_Header;
                            break;
                            case e_RemoteSPICmd_Error:
                                if(OurData->ReadDataLen>0)
                                {
                                    switch(OurData->ReadData[0])
                                    {
                                        case 0:
                                        break;
                                        default:
                                        break;
                                    }
                                    /* Display an error here? */
                                    //`
                                }
                                OurData->ReadState=e_RemoteSPIReadState_Header;
                            break;
                            case e_RemoteSPICmd_SetParam:
                            case e_RemoteSPICmd_Open:
                            case e_RemoteSPICmd_Close:
                            case e_RemoteSPICmd_WriteData:
                            case e_RemoteSPICmdMAX:
                            default:
                                /* Not a command we use, toss */
                                OurData->ReadState=e_RemoteSPIReadState_Header;
                            break;
                        }
                    }
                break;

                default:
                case e_RemoteSPIReadState_SendUpData:
                case e_RemoteSPIReadStateMAX:
                    OurData->ReadState=e_RemoteSPIReadState_Header;
                break;
            }
        }
        /* We need to move the data down so the next time we can continue
           processing */
        memmove(OurData->ReadBuff,&OurData->ReadBuff[r],
                OurData->BytesInReadBuff-r);
        OurData->BytesInReadBuff=OurData->BytesInReadBuff-r;
    }

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_Write
 *
 * SYNOPSIS:
 *    int RemoteSPI_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
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
 *    RemoteSPI_Open(), RemoteSPI_Read()
 ******************************************************************************/
int RemoteSPI_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    return RemoteSPI_SendMessage(DriverIO,e_RemoteSPICmd_WriteData,Data,Bytes);
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL RemoteSPI_ChangeOptions(t_DriverIOHandleType *DriverIO,
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
PG_BOOL RemoteSPI_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
    RemoteSPI_Close(DriverIO);

    return RemoteSPI_Open(DriverIO,Options);
}

/*******************************************************************************
 * NAME:
 *    RemoteSPI_SendMessage
 *
 * SYNOPSIS:
 *    int RemoteSPI_SendMessage(t_DriverIOHandleType *DriverIO,
 *              e_RemoteSPICmdType cmd,const uint8_t *Data,int DataLen);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    cmd [I] -- The command to send
 *    Data [I] -- The data to send (if any)
 *    DataLen [I] -- The number of bytes in 'Data'.
 *
 * FUNCTION:
 *    This function sends a command to client.  The format is:
 *      <Header=0xFF><Command><Len:2><Footer=0x77>
 *
 * RETURNS:
 *    The number of bytes sent minus the headers size (so 'DataLen') or:
 *      RETERROR_NOBYTES -- No bytes was written (0)
 *      RETERROR_DISCONNECT -- This device is no longer open.
 *      RETERROR_IOERROR -- There was an IO error.
 *      RETERROR_BUSY -- The device is currently busy.  Try again later
 *
 * SEE ALSO:
 *    RemoteSPI_OSWrite()
 ******************************************************************************/
static int RemoteSPI_SendMessage(t_DriverIOHandleType *DriverIO,
        e_RemoteSPICmdType cmd,const uint8_t *Data,int DataLen)
{
    struct RemoteSPI_OurData *OurData=(struct RemoteSPI_OurData *)DriverIO;
    uint8_t SendByte;
    uint8_t Len[2];
    int RetCode;

    /* Header */
    SendByte=REMOTESPI_HEADER_BYTE;
    RetCode=RemoteSPI_OSWrite(OurData->DriverData,&SendByte,1);
    if(RetCode<0)
        return RetCode;

    /* Command */
    SendByte=cmd;
    RetCode=RemoteSPI_OSWrite(OurData->DriverData,&SendByte,1);
    if(RetCode<0)
        return RetCode;

    /* Len in big endian (because we are only including headers/footer to make
       it easier for humans to read so we might as well do that for numbers as
       well) */
    Len[0]=(DataLen>>8)&0xFF;
    Len[1]=DataLen&0xFF;
    RetCode=RemoteSPI_OSWrite(OurData->DriverData,(uint8_t *)&Len,2);
    if(RetCode<0)
        return RetCode;

    /* Data */
    RetCode=RemoteSPI_OSWrite(OurData->DriverData,Data,DataLen);
    if(RetCode<0)
        return RetCode;

    /* Footer */
    SendByte=REMOTESPI_FOOTER_BYTE;
    RetCode=RemoteSPI_OSWrite(OurData->DriverData,&SendByte,1);
    if(RetCode<0)
        return RetCode;

    return DataLen;
}
