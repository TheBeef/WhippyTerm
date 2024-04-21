/*******************************************************************************
 * FILENAME: UDPClient_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UDP client plugin it in.
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
 *    Paul Hutchinson (15 May 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UDPClient_Main.h"
#include "PluginSDK/IODriver.h"
#include "PluginSDK/Plugin.h"
#include "OS/UDPClient_Socket.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/*** DEFINES                  ***/
#define UDPCLIENT_URI_PREFIX                    "UDP"
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      UDPClient // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x00080000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct UDPClient_ConWidgets
{
    t_WidgetSysHandle *WidgetHandle;
    struct PI_TextInput *ServerAddress;
    struct PI_NumberInput *PortNumber;
    struct PI_Checkbox *Broadcast;
    struct PI_Checkbox *Multicast;
    struct PI_TextInput *MulticastGroup;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL UDPClient_Init(void);
const struct IODriverInfo *UDPClient_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *UDPClient_DetectDevices(void);
void UDPClient_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
t_ConnectionOptionsWidgetsType *UDPClient_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle);
void UDPClient_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle);
void UDPClient_ConnectionOptionsWidgets_StoreUI(t_ConnectionOptionsWidgetsType *ConOptions,
            t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
            t_PIKVList *Options);
void UDPClient_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options);
PG_BOOL UDPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL UDPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
void UDPClient_MulticastCheckboxEventCB(const struct PICheckboxEvent *Event,void *UserData);
PG_BOOL UDPClient_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);

/*** VARIABLE DEFINITIONS     ***/
const struct IODriverAPI g_UDPClientPluginAPI=
{
    UDPClient_Init,
    UDPClient_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    UDPClient_DetectDevices,
    UDPClient_FreeDetectedDevices,
    UDPClient_GetConnectionInfo,
    UDPClient_ConnectionOptionsWidgets_AllocWidgets,
    UDPClient_ConnectionOptionsWidgets_FreeWidgets,
    UDPClient_ConnectionOptionsWidgets_StoreUI,
    UDPClient_ConnectionOptionsWidgets_UpdateUI,
    UDPClient_Convert_URI_To_Options,
    UDPClient_Convert_Options_To_URI,
    UDPClient_AllocateHandle,
    UDPClient_FreeHandle,
    UDPClient_Open,
    UDPClient_Close,
    UDPClient_Read,
    UDPClient_Write,
    UDPClient_ChangeOptions,
    UDPClient_Transmit,
};

struct IODriverInfo m_UDPClientInfo=
{
    IODRVINFOFLAG_BLOCKDEV
};

const struct IOS_API *g_UDPC_IOSystem;
const struct PI_UIAPI *g_UDPC_UI;
const struct PI_SystemAPI *g_UDPC_System;

static const struct IODriverDetectedInfo g_UDPC_DeviceInfo=
{
    NULL,
    sizeof(struct IODriverDetectedInfo),
    0,                      // Flags
    UDPCLIENT_URI_PREFIX,   // DeviceUniqueID
    "UDP Socket Client",    // Name
    "UDPClient",            // Title
};

/*******************************************************************************
 * NAME:
 *    UDPClient_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int UDPClient_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        g_UDPC_System=SysAPI;
        g_UDPC_IOSystem=g_UDPC_System->GetAPI_IO();
        g_UDPC_UI=g_UDPC_IOSystem->GetAPI_UI();

        g_UDPC_IOSystem->RegisterDriver("UDPClient",UDPCLIENT_URI_PREFIX,
                &g_UDPClientPluginAPI,sizeof(g_UDPClientPluginAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    UDPClient_Init
 *
 * SYNOPSIS:
 *    PG_BOOL UDPClient_Init(void);
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
PG_BOOL UDPClient_Init(void)
{
    return true;
}

/*******************************************************************************
 * NAME:
 *   UDPClient_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*UDPClient_GetDriverInfo)(
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
const struct IODriverInfo *UDPClient_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_UDPClientInfo;
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
const struct IODriverDetectedInfo *UDPClient_DetectDevices(void)
{
    return &g_UDPC_DeviceInfo;
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
void UDPClient_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    UDPClient_ConnectionOptionsWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionOptionsWidgetsType *UDPClient_ConnectionOptionsWidgets_AllocWidgets(
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
 *    (t_ConnectionOptionsWidgetsType *) when you return.
 *
 * NOTES:
 *    This function must be reentrant.  The system may allocate many sets
 *    of option widgets and free them in any order.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_ConnectionOptionsWidgetsType *UDPClient_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle)
{
    struct UDPClient_ConWidgets *ConWidgets;

    ConWidgets=NULL;
    try
    {
        ConWidgets=new struct UDPClient_ConWidgets;

        ConWidgets->WidgetHandle=WidgetHandle;

        ConWidgets->ServerAddress=g_UDPC_UI->AddTextInput(WidgetHandle,
                "Server",NULL,NULL);
        if(ConWidgets->ServerAddress==NULL)
            throw(0);

        ConWidgets->PortNumber=g_UDPC_UI->AddNumberInput(WidgetHandle,"Port",
                NULL,NULL);
        if(ConWidgets->PortNumber==NULL)
            throw(0);

        g_UDPC_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->PortNumber->Ctrl,
                1,65535);

        ConWidgets->Broadcast=g_UDPC_UI->AddCheckbox(WidgetHandle,
                "Broadcast",NULL,NULL);
        if(ConWidgets->Broadcast==NULL)
            throw(0);

        ConWidgets->Multicast=g_UDPC_UI->AddCheckbox(WidgetHandle,
                "Multicast",UDPClient_MulticastCheckboxEventCB,ConWidgets);
        if(ConWidgets->Multicast==NULL)
            throw(0);

        ConWidgets->MulticastGroup=g_UDPC_UI->AddTextInput(WidgetHandle,
                "Multicast Group IP",NULL,NULL);
        if(ConWidgets->MulticastGroup==NULL)
            throw(0);
    }
    catch(...)
    {
        if(ConWidgets!=NULL)
        {
            if(ConWidgets->ServerAddress!=NULL)
                g_UDPC_UI->FreeTextInput(WidgetHandle,ConWidgets->ServerAddress);
            if(ConWidgets->PortNumber!=NULL)
                g_UDPC_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);
            if(ConWidgets->Broadcast!=NULL)
                g_UDPC_UI->FreeCheckbox(WidgetHandle,ConWidgets->Broadcast);
            if(ConWidgets->Multicast!=NULL)
                g_UDPC_UI->FreeCheckbox(WidgetHandle,ConWidgets->Multicast);
            if(ConWidgets->MulticastGroup!=NULL)
                g_UDPC_UI->FreeTextInput(WidgetHandle,ConWidgets->MulticastGroup);

            delete ConWidgets;
        }
        return NULL;
    }

    return (t_ConnectionOptionsWidgetsType *)ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    UDPClient_ConnectionOptionsWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void UDPClient_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *
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
void UDPClient_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle)
{
    struct UDPClient_ConWidgets *ConWidgets=(struct UDPClient_ConWidgets *)ConOptions;

    if(ConWidgets->ServerAddress!=NULL)
        g_UDPC_UI->FreeTextInput(WidgetHandle,ConWidgets->ServerAddress);
    if(ConWidgets->PortNumber!=NULL)
        g_UDPC_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);
    if(ConWidgets->Broadcast!=NULL)
        g_UDPC_UI->FreeCheckbox(WidgetHandle,ConWidgets->Broadcast);
    if(ConWidgets->Multicast!=NULL)
        g_UDPC_UI->FreeCheckbox(WidgetHandle,ConWidgets->Multicast);
    if(ConWidgets->MulticastGroup!=NULL)
        g_UDPC_UI->FreeTextInput(WidgetHandle,ConWidgets->MulticastGroup);

    delete ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    UDPClient_ConnectionOptionsWidgets_StoreUI
 *
 * SYNOPSIS:
 *      void UDPClient_ConnectionOptionsWidgets_StoreUI(
 *              t_ConnectionOptionsWidgetsType *ConOptions,
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
void UDPClient_ConnectionOptionsWidgets_StoreUI(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct UDPClient_ConWidgets *ConWidgets=(struct UDPClient_ConWidgets *)ConOptions;
    const char *AddressStr;
    char PortStr[100];
    char buff[100];
    uint16_t PortNum;
    bool BroadcastBool;
    bool MulticastBool;
    const char *MulticastGroupStr;

    if(ConWidgets->ServerAddress==NULL || ConWidgets->PortNumber==NULL ||
            ConWidgets->Broadcast==NULL || ConWidgets->Multicast==NULL ||
            ConWidgets->MulticastGroup==NULL)
    {
        return;
    }

    g_UDPC_System->KVClear(Options);

    AddressStr=g_UDPC_UI->GetTextInputText(WidgetHandle,
            ConWidgets->ServerAddress->Ctrl);
    g_UDPC_System->KVAddItem(Options,"Address",AddressStr);

    PortNum=g_UDPC_UI->GetNumberInputValue(WidgetHandle,
            ConWidgets->PortNumber->Ctrl);
    sprintf(PortStr,"%d",PortNum);
    g_UDPC_System->KVAddItem(Options,"Port",PortStr);

    BroadcastBool=g_UDPC_UI->IsCheckboxChecked(WidgetHandle,
            ConWidgets->Broadcast->Ctrl);
    sprintf(buff,"%d",BroadcastBool);
    g_UDPC_System->KVAddItem(Options,"Broadcast",buff);

    MulticastBool=g_UDPC_UI->IsCheckboxChecked(WidgetHandle,
            ConWidgets->Multicast->Ctrl);
    sprintf(buff,"%d",MulticastBool);
    g_UDPC_System->KVAddItem(Options,"Multicast",buff);

    MulticastGroupStr=g_UDPC_UI->GetTextInputText(WidgetHandle,
            ConWidgets->MulticastGroup->Ctrl);
    g_UDPC_System->KVAddItem(Options,"MulticastGroup",MulticastGroupStr);
}

/*******************************************************************************
 * NAME:
 *    UDPClient_ConnectionOptionsWidgets_UpdateUI
 *
 * SYNOPSIS:
 *    void UDPClient_ConnectionOptionsWidgets_UpdateUI(
 *          t_ConnectionOptionsWidgetsType *ConOptions,
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
void UDPClient_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct UDPClient_ConWidgets *ConWidgets=(struct UDPClient_ConWidgets *)ConOptions;
    const char *AddressStr;
    const char *PortStr;
    const char *BroadcastStr;
    const char *MulticastStr;
    const char *MulticastGroupStr;
    uint16_t PortNum;
    bool BroadcastBool;
    bool MulticastBool;

    if(ConWidgets->ServerAddress==NULL || ConWidgets->PortNumber==NULL ||
            ConWidgets->Broadcast==NULL || ConWidgets->Multicast==NULL ||
            ConWidgets->MulticastGroup==NULL)
    {
        return;
    }

    AddressStr=g_UDPC_System->KVGetItem(Options,"Address");
    PortStr=g_UDPC_System->KVGetItem(Options,"Port");
    BroadcastStr=g_UDPC_System->KVGetItem(Options,"Broadcast");
    MulticastStr=g_UDPC_System->KVGetItem(Options,"Multicast");
    MulticastGroupStr=g_UDPC_System->KVGetItem(Options,"MulticastGroup");

    if(AddressStr==NULL)
        AddressStr="localhost";
    if(PortStr==NULL)
        PortStr="1234";
    if(BroadcastStr==NULL)
        BroadcastStr="0";
    if(MulticastStr==NULL)
        MulticastStr="0";
    MulticastBool=atoi(MulticastStr);
    if(MulticastGroupStr==NULL)
        MulticastGroupStr="";

    g_UDPC_UI->SetTextInputText(WidgetHandle,ConWidgets->ServerAddress->Ctrl,
            AddressStr);

    PortNum=atoi(PortStr);
    g_UDPC_UI->SetNumberInputValue(WidgetHandle,ConWidgets->PortNumber->Ctrl,
            PortNum);

    BroadcastBool=atoi(BroadcastStr);
    g_UDPC_UI->SetCheckboxChecked(WidgetHandle,ConWidgets->Broadcast->Ctrl,
            BroadcastBool);

    g_UDPC_UI->SetCheckboxChecked(WidgetHandle,ConWidgets->Multicast->Ctrl,
            MulticastBool);
    g_UDPC_UI->SetTextInputText(WidgetHandle,ConWidgets->MulticastGroup->Ctrl,
            MulticastGroupStr);
    g_UDPC_UI->EnableTextInput(WidgetHandle,ConWidgets->MulticastGroup->Ctrl,
            MulticastBool);
    g_UDPC_UI->EnableCheckbox(ConWidgets->WidgetHandle,
            ConWidgets->Broadcast->Ctrl,!MulticastBool);
    g_UDPC_UI->EnableTextInput(WidgetHandle,ConWidgets->ServerAddress->Ctrl,
            !MulticastBool);
}

/*******************************************************************************
 * NAME:
 *    UDPClient_Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL UDPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
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
 *      "UDP://localhost:80"
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Convert_URI_To_Options
 ******************************************************************************/
PG_BOOL UDPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    const char *AddressStr;
    const char *PortStr;

    AddressStr=g_UDPC_System->KVGetItem(Options,"Address");
    PortStr=g_UDPC_System->KVGetItem(Options,"Port");
    if(AddressStr==NULL || PortStr==NULL)
        return false;

    if(strlen(UDPCLIENT_URI_PREFIX)+1+strlen(AddressStr)+1+strlen(PortStr)+1>=
            MaxURILen)
    {
        return false;
    }

    strcpy(URI,UDPCLIENT_URI_PREFIX);
    strcat(URI,"://");
    strcat(URI,AddressStr);
    strcat(URI,":");
    strcat(URI,PortStr);

    return true;
}

/*******************************************************************************
 * NAME:
 *    UDPClient_Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL UDPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
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
PG_BOOL UDPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    const char *PosStart;
    const char *PortStart;
    const char *AddressStart;
    unsigned int AddressLen;
    unsigned long Port;
    char ServerAddressBuffer[256];
    char buff[100];

    /* Make sure it starts with UDP:// */
    if(strncasecmp(URI,UDPCLIENT_URI_PREFIX "://",
            (sizeof(UDPCLIENT_URI_PREFIX)-1)+1+2)!=0)   // +1 for ':' and +2 for '//'
    {
        return false;
    }

    g_UDPC_System->KVClear(Options);

    PosStart=URI;
    PosStart+=sizeof(UDPCLIENT_URI_PREFIX)-1;  // -1 because of the \0
    PosStart+=3;    // Slip ://

    AddressStart=PosStart;

    /* Get the server address */
    PortStart=strchr(PosStart,':');
    if(PortStart==NULL)
        return false;
    AddressLen=PortStart-AddressStart;
    PortStart++;    // Slip :

    /* Make sure we have space for this address */
    if(AddressLen>=sizeof(ServerAddressBuffer)-1)
        return false;

    strncpy(ServerAddressBuffer,AddressStart,AddressLen);
    ServerAddressBuffer[AddressLen]=0;

    Port=strtoul(PortStart,NULL,10);

    g_UDPC_System->KVAddItem(Options,"Address",ServerAddressBuffer);
    g_UDPC_System->KVAddItem(Options,"Port",PortStart);

    if(strlen(ServerAddressBuffer)+strlen(PortStart)+1>=MaxDeviceUniqueIDLen)
        return false;

    strcpy(DeviceUniqueID,ServerAddressBuffer);
    strcat(DeviceUniqueID,":");
    sprintf(buff,"%ld",Port);
    strcat(DeviceUniqueID,buff);

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
PG_BOOL UDPClient_GetConnectionInfo(const char *DeviceUniqueID,
        t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    const char *AddressStr;
    const char *PortStr;
    string Title;

    /* Fill in defaults */
    strcpy(RetInfo->Name,g_UDPC_DeviceInfo.Name);
    RetInfo->Flags=g_UDPC_DeviceInfo.Flags;

    Title=g_UDPC_DeviceInfo.Title;

    AddressStr=g_UDPC_System->KVGetItem(Options,"Address");
    if(AddressStr!=NULL)
    {
        Title=AddressStr;
        PortStr=g_UDPC_System->KVGetItem(Options,"Port");
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
 *    UDPClient_MulticastCheckboxEventCB
 *
 * SYNOPSIS:
 *    void UDPClient_MulticastCheckboxEventCB(const struct PICheckboxEvent *Event,
 *          void *UserData);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here
 *    UserData [I] -- Our 'ConWidgets' pointer (for our widgets)
 *
 * FUNCTION:
 *    This function is called when there is an event from the Multicast checkbox
 *    It handles turn on/off the MulticastGroup input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UDPClient_MulticastCheckboxEventCB(const struct PICheckboxEvent *Event,
        void *UserData)
{
    struct UDPClient_ConWidgets *ConWidgets=(struct UDPClient_ConWidgets *)UserData;

    switch(Event->EventType)
    {
        case e_PIECheckbox_Changed:
            g_UDPC_UI->EnableTextInput(ConWidgets->WidgetHandle,
                    ConWidgets->MulticastGroup->Ctrl,Event->Checked);
            g_UDPC_UI->EnableCheckbox(ConWidgets->WidgetHandle,
                    ConWidgets->Broadcast->Ctrl,!Event->Checked);
            g_UDPC_UI->EnableTextInput(ConWidgets->WidgetHandle,
                    ConWidgets->ServerAddress->Ctrl,!Event->Checked);
        break;
        case e_PIECheckboxMAX:
        default:
        break;
    }
}

