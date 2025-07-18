/*******************************************************************************
 * FILENAME: UDPServer_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is a whippy term plugin for listening on a UDP socketed for
 *    connections.
 *
 *    This plugin uses the URI format:
 *      UDPS:port
 *    Example:
 *      UDPS:80
 *      UDPS:8080
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
#include "UDPServer_Main.h"
#include "PluginSDK/IODriver.h"
#include "PluginSDK/Plugin.h"
#include "OS/UDPServer_Socket.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/
#define UDPSERVER_URI_PREFIX                    "UDPS"
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      UDPServer // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct UDPServer_DetectedConnections
{
    int Junk;
};

struct UDPServer_ConWidgets
{
    t_WidgetSysHandle *WidgetHandle;
    struct PI_NumberInput *PortNumber;
    struct PI_Checkbox *ReuseAddress;
    struct PI_Checkbox *ReusePort;
    struct PI_Checkbox *Multicast;
    struct PI_TextInput *MulticastGroup;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL UDPServer_Init(void);
const struct IODriverInfo *UDPServer_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *UDPServer_DetectDevices(void);
void UDPServer_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
t_ConnectionWidgetsType *UDPServer_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle);
void UDPServer_ConnectionOptionsWidgets_FreeWidgets(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions);
void UDPServer_ConnectionOptionsWidgets_StoreUI(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
void UDPServer_ConnectionOptionsWidgets_UpdateUI(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
PG_BOOL UDPServer_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL UDPServer_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
void UDPServer_MulticastCheckboxEventCB(const struct PICheckboxEvent *Event,void *UserData);
PG_BOOL UDPServer_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);

/*** VARIABLE DEFINITIONS     ***/
const struct IODriverAPI g_UDPServerPluginAPI=
{
    UDPServer_Init,
    UDPServer_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    UDPServer_DetectDevices,
    UDPServer_FreeDetectedDevices,
    UDPServer_GetConnectionInfo,
    UDPServer_ConnectionOptionsWidgets_AllocWidgets,
    UDPServer_ConnectionOptionsWidgets_FreeWidgets,
    UDPServer_ConnectionOptionsWidgets_StoreUI,
    UDPServer_ConnectionOptionsWidgets_UpdateUI,
    UDPServer_Convert_URI_To_Options,
    UDPServer_Convert_Options_To_URI,
    UDPServer_AllocateHandle,
    UDPServer_FreeHandle,
    UDPServer_Open,
    UDPServer_Close,
    UDPServer_Read,
    UDPServer_Write,
    UDPServer_ChangeOptions,
    NULL,                                               // Transmit
    NULL,                                               // ConnectionAuxCtrlWidgets_AllocWidgets
    NULL,                                               // ConnectionAuxCtrlWidgets_FreeWidgets
    /* V2 */
    NULL,                                               // GetLastErrorMessage
#warning Add extra error info
};
extern const struct IODriverAPI g_UDPServerPluginAPI;

struct IODriverInfo m_UDPServerInfo=
{
    0,
    "<URI>UDPS://[host]:[port]</URI>"
    "<ARG>host -- The TCP/IP interface to listen on</ARG>"
    "<ARG>port -- The port to listen on</ARG>"
    "<Example>UDPS://localhost:80</Example>"
};

const struct IOS_API *g_UDPS_IOSystem;
const struct PI_UIAPI *g_UDPS_UI;
const struct PI_SystemAPI *g_UDPS_System;

static const struct IODriverDetectedInfo g_UDPS_DeviceInfo=
{
    NULL,
    sizeof(struct IODriverDetectedInfo),
    0,                      // Flags
    UDPSERVER_URI_PREFIX,   // DeviceUniqueID
    "UDP Socket Server",    // Name
    "UDPServer",            // Title
};

/*******************************************************************************
 * NAME:
 *    UDPServer_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int UDPServer_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        g_UDPS_System=SysAPI;
        g_UDPS_IOSystem=g_UDPS_System->GetAPI_IO();
        g_UDPS_UI=g_UDPS_IOSystem->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(g_UDPS_System->GetExperimentalID()>0 &&
                g_UDPS_System->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        g_UDPS_IOSystem->RegisterDriver("UDPServer",UDPSERVER_URI_PREFIX,
                &g_UDPServerPluginAPI,sizeof(g_UDPServerPluginAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    UDPServer_Init
 *
 * SYNOPSIS:
 *    PG_BOOL UDPServer_Init(void);
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
PG_BOOL UDPServer_Init(void)
{
    return true;
}

/*******************************************************************************
 * NAME:
 *   UDPServer_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*UDPServer_GetDriverInfo)(
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
const struct IODriverInfo *UDPServer_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_UDPServerInfo;
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
const struct IODriverDetectedInfo *UDPServer_DetectDevices(void)
{
    return &g_UDPS_DeviceInfo;
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
void UDPServer_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    UDPServer_ConnectionOptionsWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionWidgetsType *UDPServer_ConnectionOptionsWidgets_AllocWidgets(
 *          t_WidgetSysHandle *WidgetHandle);
 *
 * PARAMETERS:
 *    Devices [I] -- The handle to the private data allocated with
 *                   AllocDetectDevices().  Cast this to your private data type.
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
t_ConnectionWidgetsType *UDPServer_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle)
{
    struct UDPServer_ConWidgets *ConWidgets;

    ConWidgets=NULL;
    try
    {
        ConWidgets=new struct UDPServer_ConWidgets;
        ConWidgets->PortNumber=NULL;
        ConWidgets->ReusePort=NULL;
        ConWidgets->ReuseAddress=NULL;
        ConWidgets->Multicast=NULL;
        ConWidgets->MulticastGroup=NULL;

        ConWidgets->WidgetHandle=WidgetHandle;

        ConWidgets->PortNumber=g_UDPS_UI->AddNumberInput(WidgetHandle,"Port",
                NULL,NULL);
        if(ConWidgets->PortNumber==NULL)
            throw(0);

        g_UDPS_UI->SetNumberInputMinMax(WidgetHandle,
                ConWidgets->PortNumber->Ctrl,1,65535);

        ConWidgets->ReuseAddress=g_UDPS_UI->AddCheckbox(WidgetHandle,
                "SO_REUSEADDR",NULL,NULL);
        if(ConWidgets->ReuseAddress==NULL)
            throw(0);

        if(OSSupportsReusePort())
        {
            ConWidgets->ReusePort=g_UDPS_UI->AddCheckbox(WidgetHandle,
                    "SO_REUSEPORT",NULL,NULL);
            if(ConWidgets->PortNumber==NULL)
                throw(0);
        }

        ConWidgets->Multicast=g_UDPS_UI->AddCheckbox(WidgetHandle,
                "Multicast",UDPServer_MulticastCheckboxEventCB,ConWidgets);
        if(ConWidgets->Multicast==NULL)
            throw(0);

        ConWidgets->MulticastGroup=g_UDPS_UI->AddTextInput(WidgetHandle,
                "Multicast Group IP",NULL,NULL);
        if(ConWidgets->MulticastGroup==NULL)
            throw(0);
    }
    catch(...)
    {
        if(ConWidgets!=NULL)
        {
            if(ConWidgets->PortNumber!=NULL)
                g_UDPS_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);
            if(ConWidgets->ReuseAddress!=NULL)
                g_UDPS_UI->FreeCheckbox(WidgetHandle,ConWidgets->ReuseAddress);
            if(ConWidgets->ReusePort!=NULL)
                g_UDPS_UI->FreeCheckbox(WidgetHandle,ConWidgets->ReusePort);
            if(ConWidgets->Multicast!=NULL)
                g_UDPS_UI->FreeCheckbox(WidgetHandle,ConWidgets->Multicast);
            if(ConWidgets->MulticastGroup!=NULL)
                g_UDPS_UI->FreeTextInput(WidgetHandle,ConWidgets->MulticastGroup);

            delete ConWidgets;
        }
        return NULL;
    }

    return (t_ConnectionWidgetsType *)ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    UDPServer_ConnectionOptionsWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void UDPServer_ConnectionOptionsWidgets_FreeWidgets(
 *              t_WidgetSysHandle *WidgetHandle,
 *              t_ConnectionWidgetsType *ConOptions);
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
void UDPServer_ConnectionOptionsWidgets_FreeWidgets(
        t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions)
{
    struct UDPServer_ConWidgets *ConWidgets=(struct UDPServer_ConWidgets *)ConOptions;

    if(ConWidgets->PortNumber!=NULL)
        g_UDPS_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);
    if(ConWidgets->ReuseAddress!=NULL)
        g_UDPS_UI->FreeCheckbox(WidgetHandle,ConWidgets->ReuseAddress);
    if(ConWidgets->Multicast!=NULL)
        g_UDPS_UI->FreeCheckbox(WidgetHandle,ConWidgets->Multicast);
    if(ConWidgets->ReusePort!=NULL)
        g_UDPS_UI->FreeCheckbox(WidgetHandle,ConWidgets->ReusePort);
    if(ConWidgets->MulticastGroup!=NULL)
        g_UDPS_UI->FreeTextInput(WidgetHandle,ConWidgets->MulticastGroup);

    delete ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    UDPServer_ConnectionOptionsWidgets_StoreUI
 *
 * SYNOPSIS:
 *      void UDPServer_ConnectionOptionsWidgets_StoreUI(
 *              t_WidgetSysHandle *WidgetHandle,
 *              t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,
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
void UDPServer_ConnectionOptionsWidgets_StoreUI(t_WidgetSysHandle *WidgetHandle,
        t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct UDPServer_ConWidgets *ConWidgets=(struct UDPServer_ConWidgets *)ConOptions;
    char buff[100];
    uint16_t PortNum;
    bool ReuseAddressBool;
    bool ReusePortBool;
    bool MulticastBool;
    const char *MulticastGroupStr;

    if(ConWidgets->PortNumber==NULL || ConWidgets->ReuseAddress==NULL ||
            ConWidgets->Multicast==NULL || ConWidgets->MulticastGroup==NULL)
    {
        return;
    }

    g_UDPS_System->KVClear(Options);

    PortNum=g_UDPS_UI->GetNumberInputValue(WidgetHandle,
            ConWidgets->PortNumber->Ctrl);
    sprintf(buff,"%d",PortNum);
    g_UDPS_System->KVAddItem(Options,"Port",buff);

    ReuseAddressBool=g_UDPS_UI->IsCheckboxChecked(WidgetHandle,
            ConWidgets->ReuseAddress->Ctrl);
    sprintf(buff,"%d",ReuseAddressBool);
    g_UDPS_System->KVAddItem(Options,"ReuseAddress",buff);

    if(ConWidgets->ReusePort!=NULL)
    {
        ReusePortBool=g_UDPS_UI->IsCheckboxChecked(WidgetHandle,
                ConWidgets->ReusePort->Ctrl);
        sprintf(buff,"%d",ReusePortBool);
        g_UDPS_System->KVAddItem(Options,"ReusePort",buff);
    }

    MulticastBool=g_UDPS_UI->IsCheckboxChecked(WidgetHandle,
            ConWidgets->Multicast->Ctrl);
    sprintf(buff,"%d",MulticastBool);
    g_UDPS_System->KVAddItem(Options,"Multicast",buff);

    MulticastGroupStr=g_UDPS_UI->GetTextInputText(WidgetHandle,
            ConWidgets->MulticastGroup->Ctrl);
    g_UDPS_System->KVAddItem(Options,"MulticastGroup",MulticastGroupStr);
}

/*******************************************************************************
 * NAME:
 *    UDPServer_ConnectionOptionsWidgets_UpdateUI
 *
 * SYNOPSIS:
 *    void UDPServer_ConnectionOptionsWidgets_UpdateUI(
 *          t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,
 *          const char *DeviceUniqueID,t_PIKVList *Options);
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
void UDPServer_ConnectionOptionsWidgets_UpdateUI(
        t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,
        const char *DeviceUniqueID,t_PIKVList *Options)
{
    struct UDPServer_ConWidgets *ConWidgets=(struct UDPServer_ConWidgets *)ConOptions;
    const char *PortStr;
    uint16_t PortNum;
    bool ReuseAddressBool;
    bool ReusePortBool;
    bool MulticastBool;
    const char *TmpStr;
    const char *MulticastGroupStr;

    if(ConWidgets->PortNumber==NULL || ConWidgets->ReuseAddress==NULL ||
            ConWidgets->Multicast==NULL || ConWidgets->MulticastGroup==NULL)
    {
        return;
    }

    PortStr=g_UDPS_System->KVGetItem(Options,"Port");
    if(PortStr==NULL)
        PortStr="1234";
    PortNum=atoi(PortStr);

    TmpStr=g_UDPS_System->KVGetItem(Options,"ReuseAddress");
    if(TmpStr==NULL)
        TmpStr="0";
    ReuseAddressBool=atoi(TmpStr);
    
    ReusePortBool=0;
    if(ConWidgets->ReusePort!=NULL)
    {
        TmpStr=g_UDPS_System->KVGetItem(Options,"ReusePort");
        if(TmpStr==NULL)
            TmpStr="0";
        ReusePortBool=atoi(TmpStr);
    }

    TmpStr=g_UDPS_System->KVGetItem(Options,"Multicast");
    if(TmpStr==NULL)
        TmpStr="0";
    MulticastBool=atoi(TmpStr);

    MulticastGroupStr=g_UDPS_System->KVGetItem(Options,"MulticastGroup");
    if(MulticastGroupStr==NULL)
        MulticastGroupStr="";

    g_UDPS_UI->SetNumberInputValue(WidgetHandle,ConWidgets->PortNumber->Ctrl,
            PortNum);
    g_UDPS_UI->SetCheckboxChecked(WidgetHandle,ConWidgets->ReuseAddress->Ctrl,
            ReuseAddressBool);
    g_UDPS_UI->SetCheckboxChecked(WidgetHandle,ConWidgets->Multicast->Ctrl,
            MulticastBool);
    g_UDPS_UI->SetTextInputText(WidgetHandle,ConWidgets->MulticastGroup->Ctrl,
            MulticastGroupStr);
    g_UDPS_UI->EnableTextInput(WidgetHandle,ConWidgets->MulticastGroup->Ctrl,
            MulticastBool);
    g_UDPS_UI->EnableCheckbox(ConWidgets->WidgetHandle,
            ConWidgets->ReuseAddress->Ctrl,!MulticastBool);

    if(ConWidgets->ReusePort!=NULL)
    {
        g_UDPS_UI->SetCheckboxChecked(WidgetHandle,ConWidgets->ReusePort->Ctrl,
                ReusePortBool);
        g_UDPS_UI->EnableCheckbox(ConWidgets->WidgetHandle,
                ConWidgets->ReusePort->Ctrl,!MulticastBool);
    }
}

/*******************************************************************************
 * NAME:
 *    UDPServer_Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL UDPServer_Convert_Options_To_URI(const char *DeviceUniqueID,
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
 *      "UDPS://localhost:80"
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Convert_URI_To_Options
 ******************************************************************************/
PG_BOOL UDPServer_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    const char *PortStr;

    PortStr=g_UDPS_System->KVGetItem(Options,"Port");
    if(PortStr==NULL)
        return false;

    if(strlen(UDPSERVER_URI_PREFIX)+strlen(PortStr)+1>=MaxURILen)
        return false;

    strcpy(URI,UDPSERVER_URI_PREFIX);
    strcat(URI,":");
    strcat(URI,PortStr);

    return true;
}

/*******************************************************************************
 * NAME:
 *    UDPServer_Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL UDPServer_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
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
PG_BOOL UDPServer_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    const char *PortStart;
    unsigned long Port;
    char buff[100];

    /* Make sure it starts with UDP:// */
    if(strncasecmp(URI,UDPSERVER_URI_PREFIX ":",
            (sizeof(UDPSERVER_URI_PREFIX)-1)+1)!=0)   // +1 for ':'
    {
        return false;
    }

    g_UDPS_System->KVClear(Options);

    PortStart=URI;
    PortStart+=sizeof(UDPSERVER_URI_PREFIX)-1;  // -1 because of the \0
    PortStart+=1;    // Slip :

    Port=strtoul(PortStart,NULL,10);

    g_UDPS_System->KVAddItem(Options,"Port",PortStart);

    sprintf(buff,"%ld",Port);
    if(strlen(buff)>=MaxDeviceUniqueIDLen)
        return false;
    strcpy(DeviceUniqueID,buff);

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
PG_BOOL UDPServer_GetConnectionInfo(const char *DeviceUniqueID,
        t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    const char *PortStr;
    string Title;

    /* Fill in defaults */
    strcpy(RetInfo->Name,g_UDPS_DeviceInfo.Name);
    RetInfo->Flags=g_UDPS_DeviceInfo.Flags;

    Title=g_UDPS_DeviceInfo.Title;
    PortStr=g_UDPS_System->KVGetItem(Options,"Port");
    if(PortStr!=NULL)
    {
        Title+=":";
        Title+=PortStr;
    }

    snprintf(RetInfo->Title,sizeof(RetInfo->Title),"%s",Title.c_str());

    return true;
}

/*******************************************************************************
 * NAME:
 *    UDPServer_MulticastCheckboxEventCB
 *
 * SYNOPSIS:
 *    void UDPServer_MulticastCheckboxEventCB(const struct PICheckboxEvent *Event,
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
void UDPServer_MulticastCheckboxEventCB(const struct PICheckboxEvent *Event,
        void *UserData)
{
    struct UDPServer_ConWidgets *ConWidgets=(struct UDPServer_ConWidgets *)UserData;

    switch(Event->EventType)
    {
        case e_PIECheckbox_Changed:
            g_UDPS_UI->EnableTextInput(ConWidgets->WidgetHandle,
                    ConWidgets->MulticastGroup->Ctrl,Event->Checked);
            g_UDPS_UI->EnableCheckbox(ConWidgets->WidgetHandle,
                    ConWidgets->ReuseAddress->Ctrl,!Event->Checked);
            if(ConWidgets->ReusePort!=NULL)
            {
                g_UDPS_UI->EnableCheckbox(ConWidgets->WidgetHandle,
                        ConWidgets->ReusePort->Ctrl,!Event->Checked);
            }
        break;
        case e_PIECheckboxMAX:
        default:
        break;
    }
}

