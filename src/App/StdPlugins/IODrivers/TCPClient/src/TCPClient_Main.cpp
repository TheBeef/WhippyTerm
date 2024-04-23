/*******************************************************************************
 * FILENAME: TCPClient_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the TCP client plugin it in.
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
 *    Paul Hutchinson (20 Jun 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "TCPClient_Main.h"
#include "PluginSDK/IODriver.h"
#include "PluginSDK/Plugin.h"
#include "OS/TCPClient_Socket.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/*** DEFINES                  ***/
#define TCPCLIENT_URI_PREFIX                    "TCP"
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      TCPClient // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x000B0000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct TCPClient_ConWidgets
{
    struct PI_TextInput *ServerAddress;
    struct PI_NumberInput *PortNumber;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL TCPClient_Init(void);
const struct IODriverInfo *TCPClient_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *TCPClient_DetectDevices(void);
void TCPClient_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
t_ConnectionOptionsWidgetsType *TCPClient_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle);
void TCPClient_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle);
void TCPClient_ConnectionOptionsWidgets_StoreUI(t_ConnectionOptionsWidgetsType *ConOptions,
            t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
            t_PIKVList *Options);
void TCPClient_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options);
PG_BOOL TCPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL TCPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
PG_BOOL TCPClient_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);

/*** VARIABLE DEFINITIONS     ***/
const struct IODriverAPI g_TCPClientPluginAPI=
{
    TCPClient_Init,
    TCPClient_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    TCPClient_DetectDevices,
    TCPClient_FreeDetectedDevices,
    TCPClient_GetConnectionInfo,
    TCPClient_ConnectionOptionsWidgets_AllocWidgets,
    TCPClient_ConnectionOptionsWidgets_FreeWidgets,
    TCPClient_ConnectionOptionsWidgets_StoreUI,
    TCPClient_ConnectionOptionsWidgets_UpdateUI,
    TCPClient_Convert_URI_To_Options,
    TCPClient_Convert_Options_To_URI,
    TCPClient_AllocateHandle,
    TCPClient_FreeHandle,
    TCPClient_Open,
    TCPClient_Close,
    TCPClient_Read,
    TCPClient_Write,
    TCPClient_ChangeOptions,
    NULL,                                               // Transmit
};

struct IODriverInfo m_TCPClientInfo=
{
    0
};

const struct IOS_API *g_TCPC_IOSystem;
const struct PI_UIAPI *g_TCPC_UI;
const struct PI_SystemAPI *g_TCPC_System;

static const struct IODriverDetectedInfo g_TCPC_DeviceInfo=
{
    NULL,
    sizeof(struct IODriverDetectedInfo),
    0,                      // Flags
    TCPCLIENT_URI_PREFIX,   // DeviceUniqueID
    "TCP Socket Client",    // Name
    "TCPClient",            // Title
};

/*******************************************************************************
 * NAME:
 *    TCPClient_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int TCPClient_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        g_TCPC_System=SysAPI;
        g_TCPC_IOSystem=g_TCPC_System->GetAPI_IO();
        g_TCPC_UI=g_TCPC_IOSystem->GetAPI_UI();

        g_TCPC_IOSystem->RegisterDriver("TCPClient",TCPCLIENT_URI_PREFIX,
                &g_TCPClientPluginAPI,sizeof(g_TCPClientPluginAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    TCPClient_Init
 *
 * SYNOPSIS:
 *    PG_BOOL TCPClient_Init(void);
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
PG_BOOL TCPClient_Init(void)
{
    return true;
}

/*******************************************************************************
 * NAME:
 *   TCPClient_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*TCPClient_GetDriverInfo)(
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
const struct IODriverInfo *TCPClient_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_TCPClientInfo;
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
const struct IODriverDetectedInfo *TCPClient_DetectDevices(void)
{
    return &g_TCPC_DeviceInfo;
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
void TCPClient_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    TCPClient_ConnectionOptionsWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionOptionsWidgetsType *TCPClient_ConnectionOptionsWidgets_AllocWidgets(
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
 *    (t_ConnectionOptionsWidgetsType *) when you return.
 *
 * NOTES:
 *    This function must be reentrant.  The system may allocate many sets
 *    of option widgets and free them in any order.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_ConnectionOptionsWidgetsType *TCPClient_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle)
{
    struct TCPClient_ConWidgets *ConWidgets;

    ConWidgets=NULL;
    try
    {
        ConWidgets=new struct TCPClient_ConWidgets;

        ConWidgets->ServerAddress=g_TCPC_UI->AddTextInput(WidgetHandle,
                "Server",NULL,NULL);
        if(ConWidgets->ServerAddress==NULL)
            throw(0);

        ConWidgets->PortNumber=g_TCPC_UI->AddNumberInput(WidgetHandle,"Port",
                NULL,NULL);
        if(ConWidgets->PortNumber==NULL)
            throw(0);

        g_TCPC_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->PortNumber->Ctrl,
                1,65535);
    }
    catch(...)
    {
        if(ConWidgets!=NULL)
        {
            if(ConWidgets->ServerAddress!=NULL)
                g_TCPC_UI->FreeTextInput(WidgetHandle,ConWidgets->ServerAddress);
            if(ConWidgets->PortNumber!=NULL)
                g_TCPC_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);

            delete ConWidgets;
        }
        return NULL;
    }

    return (t_ConnectionOptionsWidgetsType *)ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    TCPClient_ConnectionOptionsWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void TCPClient_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *
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
void TCPClient_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle)
{
    struct TCPClient_ConWidgets *ConWidgets=(struct TCPClient_ConWidgets *)ConOptions;

    if(ConWidgets->ServerAddress!=NULL)
        g_TCPC_UI->FreeTextInput(WidgetHandle,ConWidgets->ServerAddress);
    if(ConWidgets->PortNumber!=NULL)
        g_TCPC_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);

    delete ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    TCPClient_ConnectionOptionsWidgets_StoreUI
 *
 * SYNOPSIS:
 *      void TCPClient_ConnectionOptionsWidgets_StoreUI(
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
void TCPClient_ConnectionOptionsWidgets_StoreUI(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct TCPClient_ConWidgets *ConWidgets=(struct TCPClient_ConWidgets *)ConOptions;
    const char *AddressStr;
    char PortStr[100];
    uint16_t PortNum;

    if(ConWidgets->ServerAddress==NULL || ConWidgets->PortNumber==NULL)
        return;

    g_TCPC_System->KVClear(Options);

    AddressStr=g_TCPC_UI->GetTextInputText(WidgetHandle,
            ConWidgets->ServerAddress->Ctrl);
    g_TCPC_System->KVAddItem(Options,"Address",AddressStr);

    PortNum=g_TCPC_UI->GetNumberInputValue(WidgetHandle,
            ConWidgets->PortNumber->Ctrl);
    sprintf(PortStr,"%d",PortNum);
    g_TCPC_System->KVAddItem(Options,"Port",PortStr);
}

/*******************************************************************************
 * NAME:
 *    TCPClient_ConnectionOptionsWidgets_UpdateUI
 *
 * SYNOPSIS:
 *    void TCPClient_ConnectionOptionsWidgets_UpdateUI(
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
void TCPClient_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct TCPClient_ConWidgets *ConWidgets=(struct TCPClient_ConWidgets *)ConOptions;
    const char *AddressStr;
    const char *PortStr;
    uint16_t PortNum;

    if(ConWidgets->ServerAddress==NULL || ConWidgets->PortNumber==NULL)
        return;

    AddressStr=g_TCPC_System->KVGetItem(Options,"Address");
    PortStr=g_TCPC_System->KVGetItem(Options,"Port");

    if(AddressStr==NULL)
        AddressStr="localhost";
    if(PortStr==NULL)
        PortStr="80";

    g_TCPC_UI->SetTextInputText(WidgetHandle,ConWidgets->ServerAddress->Ctrl,
            AddressStr);

    PortNum=atoi(PortStr);
    g_TCPC_UI->SetNumberInputValue(WidgetHandle,ConWidgets->PortNumber->Ctrl,
            PortNum);
}

/*******************************************************************************
 * NAME:
 *    TCPClient_Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL TCPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
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
 *      "TCP://localhost:80"
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Convert_URI_To_Options
 ******************************************************************************/
PG_BOOL TCPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    const char *AddressStr;
    const char *PortStr;

    AddressStr=g_TCPC_System->KVGetItem(Options,"Address");
    PortStr=g_TCPC_System->KVGetItem(Options,"Port");
    if(AddressStr==NULL || PortStr==NULL)
        return false;

    if(strlen(TCPCLIENT_URI_PREFIX)+1+strlen(AddressStr)+1+strlen(PortStr)+1>=
            MaxURILen)
    {
        return false;
    }

    strcpy(URI,TCPCLIENT_URI_PREFIX);
    strcat(URI,"://");
    strcat(URI,AddressStr);
    strcat(URI,":");
    strcat(URI,PortStr);

    return true;
}

/*******************************************************************************
 * NAME:
 *    TCPClient_Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL TCPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
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
PG_BOOL TCPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    const char *PosStart;
    const char *PortStart;
    const char *AddressStart;
    unsigned int AddressLen;
    char ServerAddressBuffer[256];

    /* Make sure it starts with TCP:// */
    if(strncasecmp(URI,TCPCLIENT_URI_PREFIX "://",
            (sizeof(TCPCLIENT_URI_PREFIX)-1)+1+2)!=0)   // +1 for ':' and +2 for '//'
    {
        return false;
    }

    g_TCPC_System->KVClear(Options);

    PosStart=URI;
    PosStart+=sizeof(TCPCLIENT_URI_PREFIX)-1;  // -1 because of the \0
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

    g_TCPC_System->KVAddItem(Options,"Address",ServerAddressBuffer);
    g_TCPC_System->KVAddItem(Options,"Port",PortStart);

    if(strlen(ServerAddressBuffer)+strlen(PortStart)+1>=MaxDeviceUniqueIDLen)
        return false;

    strcpy(DeviceUniqueID,TCPCLIENT_URI_PREFIX);

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
PG_BOOL TCPClient_GetConnectionInfo(const char *DeviceUniqueID,
        t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    const char *AddressStr;
    const char *PortStr;
    string Title;

    /* Fill in defaults */
    strcpy(RetInfo->Name,g_TCPC_DeviceInfo.Name);
    RetInfo->Flags=g_TCPC_DeviceInfo.Flags;

    Title=g_TCPC_DeviceInfo.Title;

    AddressStr=g_TCPC_System->KVGetItem(Options,"Address");
    if(AddressStr!=NULL)
    {
        Title=AddressStr;
        PortStr=g_TCPC_System->KVGetItem(Options,"Port");
        if(PortStr!=NULL)
        {
            Title+=":";
            Title+=PortStr;
        }
    }

    snprintf(RetInfo->Title,sizeof(RetInfo->Title),"%s",Title.c_str());

    return true;
}
