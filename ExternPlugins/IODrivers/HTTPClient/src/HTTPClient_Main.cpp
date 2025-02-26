/*******************************************************************************
 * FILENAME: HTTPClient_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has a HTTP client plugin in it.
 *
 *    This plugin uses the URI format:
 *      HTTP://Domain/Path:Port
 *    Example:
 *      HTTP://localhost:80
 *      HTTP://google.com/search?abc=1:8080
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
 *    Paul Hutchinson (10 Jul 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "HTTPClient_Main.h"
#include "PluginSDK/IODriver.h"
#include "PluginSDK/Plugin.h"
#include "OS/HTTPClient_Socket.h"
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/
#define HTTPCLIENT_URI_PREFIX                   "HTTP"
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      HTTPClient // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct HTTPClient_ConWidgets
{
    struct PI_TextInput *ServerAddress;
    struct PI_NumberInput *PortNumber;
    struct PI_TextInput *Path;
    struct PI_TextInput *GenericHeader1;
    struct PI_TextInput *GenericHeader2;
    struct PI_TextInput *GenericHeader3;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL HTTPClient_Init(void);
const struct IODriverInfo *HTTPClient_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *HTTPClient_DetectDevices(void);
void HTTPClient_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
t_ConnectionWidgetsType *HTTPClient_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle);
void HTTPClient_ConnectionOptionsWidgets_FreeWidgets(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions);
void HTTPClient_ConnectionOptionsWidgets_StoreUI(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
void HTTPClient_ConnectionOptionsWidgets_UpdateUI(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
PG_BOOL HTTPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL HTTPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
PG_BOOL HTTPClient_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);

/*** VARIABLE DEFINITIONS     ***/
const struct IODriverAPI g_HTTPClientPluginAPI=
{
    HTTPClient_Init,
    HTTPClient_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    HTTPClient_DetectDevices,
    HTTPClient_FreeDetectedDevices,
    HTTPClient_GetConnectionInfo,
    HTTPClient_ConnectionOptionsWidgets_AllocWidgets,
    HTTPClient_ConnectionOptionsWidgets_FreeWidgets,
    HTTPClient_ConnectionOptionsWidgets_StoreUI,
    HTTPClient_ConnectionOptionsWidgets_UpdateUI,
    HTTPClient_Convert_URI_To_Options,
    HTTPClient_Convert_Options_To_URI,
    HTTPClient_AllocateHandle,
    HTTPClient_FreeHandle,
    HTTPClient_Open,
    HTTPClient_Close,
    HTTPClient_Read,
    HTTPClient_Write,
    HTTPClient_ChangeOptions,
    NULL,                                                   // Transmit
};

struct IODriverInfo m_HTTPClientInfo=
{
    0,
    "<h3>FORMAT</h3>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "HTTP://[host][:port][/path]"
    "</p>"
    "<h3>WHERE</h3>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "host -- The server to connect to"
    "</p>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "port -- The port number to connect to.  If this is provided then port 80 "
    "will be used."
    "</p>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "path -- The path to send to the server of the page to load.  This can "
    "include a query (?)."
    "</p>"
    "<h3>EXAMPLE</h3>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "HTTP://localhost:2000/test.php?one=1"
    "</p>"
};

const struct IOS_API *g_HC_IOSystem;
const struct PI_UIAPI *g_HC_UI;
const struct PI_SystemAPI *g_HC_System;

static const struct IODriverDetectedInfo g_HC_DeviceInfo=
{
    NULL,
    sizeof(struct IODriverDetectedInfo),
    0,                      // Flags
    HTTPCLIENT_URI_PREFIX,  // DeviceUniqueID
    "HTTP Socket Client",   // Name
    "HTTPClient",           // Title
};


/*******************************************************************************
 * NAME:
 *    URLHighlighter_HTTPClient
 *
 * SYNOPSIS:
 *    unsigned int URLHighlighter_HTTPClient(const struct PI_SystemAPI *SysAPI,
 *          int Version);
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

        g_HC_System=SysAPI;
        g_HC_IOSystem=g_HC_System->GetAPI_IO();
        g_HC_UI=g_HC_IOSystem->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(g_HC_System->GetExperimentalID()>0 &&
                g_HC_System->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        g_HC_IOSystem->RegisterDriver("HTTPClient",HTTPCLIENT_URI_PREFIX,
                &g_HTTPClientPluginAPI,sizeof(g_HTTPClientPluginAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_Init
 *
 * SYNOPSIS:
 *    PG_BOOL HTTPClient_Init(void);
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
PG_BOOL HTTPClient_Init(void)
{
    return true;
}
/*******************************************************************************
 * NAME:
 *   HTTPClient_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*HTTPClient_GetDriverInfo)(
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
const struct IODriverInfo *HTTPClient_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_HTTPClientInfo;
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
const struct IODriverDetectedInfo *HTTPClient_DetectDevices(void)
{
    return &g_HC_DeviceInfo;
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
void HTTPClient_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_ConnectionOptionsWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionWidgetsType *HTTPClient_ConnectionOptionsWidgets_AllocWidgets(
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
t_ConnectionWidgetsType *HTTPClient_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle)
{
    struct HTTPClient_ConWidgets *ConWidgets;

    ConWidgets=NULL;
    try
    {
        ConWidgets=new struct HTTPClient_ConWidgets;

        ConWidgets->ServerAddress=NULL;
        ConWidgets->PortNumber=NULL;
        ConWidgets->Path=NULL;
        ConWidgets->GenericHeader1=NULL;
        ConWidgets->GenericHeader2=NULL;
        ConWidgets->GenericHeader3=NULL;

        ConWidgets->ServerAddress=g_HC_UI->AddTextInput(WidgetHandle,
                "Server",NULL,NULL);
        if(ConWidgets->ServerAddress==NULL)
            throw(0);

        ConWidgets->PortNumber=g_HC_UI->AddNumberInput(WidgetHandle,"Port",
                NULL,NULL);
        if(ConWidgets->PortNumber==NULL)
            throw(0);

        g_HC_UI->SetNumberInputMinMax(WidgetHandle,ConWidgets->PortNumber->Ctrl,
                1,65535);

        ConWidgets->Path=g_HC_UI->AddTextInput(WidgetHandle,
                "Path",NULL,NULL);
        if(ConWidgets->Path==NULL)
            throw(0);

        ConWidgets->GenericHeader1=g_HC_UI->AddTextInput(WidgetHandle,
                "Extra Header",NULL,NULL);
        if(ConWidgets->GenericHeader1==NULL)
            throw(0);

        ConWidgets->GenericHeader2=g_HC_UI->AddTextInput(WidgetHandle,
                "Extra Header",NULL,NULL);
        if(ConWidgets->GenericHeader2==NULL)
            throw(0);

        ConWidgets->GenericHeader3=g_HC_UI->AddTextInput(WidgetHandle,
                "Extra Header",NULL,NULL);
        if(ConWidgets->GenericHeader3==NULL)
            throw(0);
    }
    catch(...)
    {
        if(ConWidgets!=NULL)
        {
            if(ConWidgets->ServerAddress!=NULL)
                g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->ServerAddress);
            if(ConWidgets->PortNumber!=NULL)
                g_HC_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);
            if(ConWidgets->Path!=NULL)
                g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->Path);
            if(ConWidgets->GenericHeader1!=NULL)
                g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->GenericHeader1);
            if(ConWidgets->GenericHeader2!=NULL)
                g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->GenericHeader2);
            if(ConWidgets->GenericHeader3!=NULL)
                g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->GenericHeader3);

            delete ConWidgets;
        }
        return NULL;
    }

    return (t_ConnectionWidgetsType *)ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_ConnectionOptionsWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void HTTPClient_ConnectionOptionsWidgets_FreeWidgets(
 *          t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions);
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
void HTTPClient_ConnectionOptionsWidgets_FreeWidgets(
        t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions)
{
    struct HTTPClient_ConWidgets *ConWidgets=(struct HTTPClient_ConWidgets *)ConOptions;

    if(ConWidgets->ServerAddress!=NULL)
        g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->ServerAddress);
    if(ConWidgets->PortNumber!=NULL)
        g_HC_UI->FreeNumberInput(WidgetHandle,ConWidgets->PortNumber);
    if(ConWidgets->Path!=NULL)
        g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->Path);
    if(ConWidgets->GenericHeader1!=NULL)
        g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->GenericHeader1);
    if(ConWidgets->GenericHeader2!=NULL)
        g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->GenericHeader2);
    if(ConWidgets->GenericHeader3!=NULL)
        g_HC_UI->FreeTextInput(WidgetHandle,ConWidgets->GenericHeader3);

    delete ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_ConnectionOptionsWidgets_StoreUI
 *
 * SYNOPSIS:
 *      void HTTPClient_ConnectionOptionsWidgets_StoreUI(
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
void HTTPClient_ConnectionOptionsWidgets_StoreUI(
        t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,
        const char *DeviceUniqueID,t_PIKVList *Options)
{
    struct HTTPClient_ConWidgets *ConWidgets=(struct HTTPClient_ConWidgets *)ConOptions;
    const char *AddressStr;
    char PortStr[100];
    uint16_t PortNum;
    const char *PathStr;
    const char *GenericHeaderStr;

    if(ConWidgets->ServerAddress==NULL || ConWidgets->PortNumber==NULL ||
            ConWidgets->Path==NULL || ConWidgets->GenericHeader1==NULL ||
            ConWidgets->GenericHeader2==NULL ||
            ConWidgets->GenericHeader3==NULL)
    {
        return;
    }

    g_HC_System->KVClear(Options);

    AddressStr=g_HC_UI->GetTextInputText(WidgetHandle,
            ConWidgets->ServerAddress->Ctrl);
    g_HC_System->KVAddItem(Options,"Address",AddressStr);

    PortNum=g_HC_UI->GetNumberInputValue(WidgetHandle,
            ConWidgets->PortNumber->Ctrl);
    sprintf(PortStr,"%d",PortNum);
    g_HC_System->KVAddItem(Options,"Port",PortStr);

    PathStr=g_HC_UI->GetTextInputText(WidgetHandle,
            ConWidgets->Path->Ctrl);
    g_HC_System->KVAddItem(Options,"Path",PathStr);

    GenericHeaderStr=g_HC_UI->GetTextInputText(WidgetHandle,
            ConWidgets->GenericHeader1->Ctrl);
    g_HC_System->KVAddItem(Options,"GenericHeader1",GenericHeaderStr);

    GenericHeaderStr=g_HC_UI->GetTextInputText(WidgetHandle,
            ConWidgets->GenericHeader2->Ctrl);
    g_HC_System->KVAddItem(Options,"GenericHeader2",GenericHeaderStr);

    GenericHeaderStr=g_HC_UI->GetTextInputText(WidgetHandle,
            ConWidgets->GenericHeader3->Ctrl);
    g_HC_System->KVAddItem(Options,"GenericHeader3",GenericHeaderStr);
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_ConnectionOptionsWidgets_UpdateUI
 *
 * SYNOPSIS:
 *    void HTTPClient_ConnectionOptionsWidgets_UpdateUI(
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
void HTTPClient_ConnectionOptionsWidgets_UpdateUI(
        t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,
        const char *DeviceUniqueID,t_PIKVList *Options)
{
    struct HTTPClient_ConWidgets *ConWidgets=(struct HTTPClient_ConWidgets *)ConOptions;
    const char *AddressStr;
    const char *PortStr;
    uint16_t PortNum;
    const char *PathStr;

    if(ConWidgets->ServerAddress==NULL || ConWidgets->PortNumber==NULL ||
            ConWidgets->Path==NULL)
    {
        return;
    }

    AddressStr=g_HC_System->KVGetItem(Options,"Address");
    PortStr=g_HC_System->KVGetItem(Options,"Port");
    PathStr=g_HC_System->KVGetItem(Options,"Path");

    if(AddressStr==NULL)
        AddressStr="localhost";
    if(PortStr==NULL)
        PortStr="80";

    g_HC_UI->SetTextInputText(WidgetHandle,ConWidgets->ServerAddress->Ctrl,
            AddressStr);

    PortNum=atoi(PortStr);
    g_HC_UI->SetNumberInputValue(WidgetHandle,ConWidgets->PortNumber->Ctrl,
            PortNum);

    g_HC_UI->SetTextInputText(WidgetHandle,ConWidgets->Path->Ctrl,PathStr);
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL HTTPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
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
 *      "HTTP://localhost:80"
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Convert_URI_To_Options
 ******************************************************************************/
PG_BOOL HTTPClient_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    const char *AddressStr;
    const char *PortStr;
    const char *PathStr;

    AddressStr=g_HC_System->KVGetItem(Options,"Address");
    PortStr=g_HC_System->KVGetItem(Options,"Port");
    PathStr=g_HC_System->KVGetItem(Options,"Path");
    if(AddressStr==NULL || PortStr==NULL || PathStr==NULL)
        return false;

    if(strlen(HTTPCLIENT_URI_PREFIX)+1+strlen(AddressStr)+1+
            strlen(PathStr)+1+strlen(PortStr)+1>=MaxURILen)
    {
        return false;
    }

    strcpy(URI,HTTPCLIENT_URI_PREFIX);
    strcat(URI,"://");
    strcat(URI,AddressStr);
    if(strcmp(PortStr,"80")!=0)
    {
        strcat(URI,":");
        strcat(URI,PortStr);
    }
    if(strcmp(PathStr,"/")!=0)
        strcat(URI,PathStr);

    return true;
}

/*******************************************************************************
 * NAME:
 *    HTTPClient_Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL HTTPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
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
PG_BOOL HTTPClient_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    char *Pos;
    char FirstPathChar;
    char *Path;
    const char *PortStr;
    char *SrvAddress;
    char *URICopy;
    uint16_t PortNum;
    char PortBuff[100];

    /* Make sure it starts with HTTP:// */
    if(strncasecmp(URI,HTTPCLIENT_URI_PREFIX "://",
            (sizeof(HTTPCLIENT_URI_PREFIX)-1)+2)!=0)   // +2 for '//'
    {
        return false;
    }

    URICopy=(char *)malloc(strlen(URI)+1);
    if(URICopy==NULL)
        return false;
    strcpy(URICopy,URI);

    /* Pull apart the URI */
    Pos=URICopy;
    Pos+=sizeof(HTTPCLIENT_URI_PREFIX)-1;  // -1 because of the \0
    Pos+=3;    // Slip ://

    SrvAddress=Pos;
    Path=NULL;
    PortStr=NULL;
    FirstPathChar='/';

    while(*Pos!=0)
    {
        if(*Pos=='/' || *Pos=='?' || *Pos=='#')
        {
            /* Start of path, copy the address */
            Path=Pos;

            FirstPathChar=*Pos; // Save this so we can restore it later
            *Pos=0;

            /* Scan until we find the port or the end */
            do
            {
                Pos++;
            } while(*Pos!=0 && *Pos!=':');

            continue;
        }
        if(*Pos==':')
        {
            /* Start of port */
            *Pos=0;
            PortStr=Pos+1;
        }
        Pos++;
    }

    if(*SrvAddress==0)
    {
        free(URICopy);
        return false;
    }
    if(PortStr==NULL)
        PortStr="80";

    g_HC_System->KVClear(Options);
    g_HC_System->KVAddItem(Options,"Address",SrvAddress);
    g_HC_System->KVAddItem(Options,"Port",PortStr);

    /* Must be after the SrvAddress has been copied because we unstring it
       here */
    if(Path==NULL)
    {
        Path=(char *)"/";
    }
    else
    {
        /* Restore the first char of the path */
        *Path=FirstPathChar;
    }
    g_HC_System->KVAddItem(Options,"Path",Path);

    PortNum=atoi(PortStr);
    sprintf(PortBuff,"%d",PortNum);

    if(strlen(SrvAddress)+strlen(PortBuff)+1>=MaxDeviceUniqueIDLen)
    {
        free(URICopy);
        return false;
    }

    strcpy(DeviceUniqueID,HTTPCLIENT_URI_PREFIX);

    free(URICopy);

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
PG_BOOL HTTPClient_GetConnectionInfo(const char *DeviceUniqueID,
        t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    const char *AddressStr;
    const char *PortStr;
    string Title;

    /* Fill in defaults */
    strcpy(RetInfo->Name,g_HC_DeviceInfo.Name);
    RetInfo->Flags=0;

    Title=g_HC_DeviceInfo.Title;

    AddressStr=g_HC_System->KVGetItem(Options,"Address");
    if(AddressStr!=NULL)
    {
        Title=AddressStr;
        PortStr=g_HC_System->KVGetItem(Options,"Port");
        if(PortStr!=NULL)
        {
            if(atoi(PortStr)!=80)
            {
                Title+=":";
                Title+=PortStr;
            }
        }
    }

    snprintf(RetInfo->Title,sizeof(RetInfo->Title),"%s",Title.c_str());

    return true;
}
