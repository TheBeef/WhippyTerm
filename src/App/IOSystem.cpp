/*******************************************************************************
 * FILENAME: IOSystem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the system that handles all the IO drives in it.
 *    It lets the main code control all the different drivers from one
 *    place.
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
#include "App/IOSystem.h"
#include "App/PluginSupport/PluginUISupport.h"
#include "App/PluginSupport/SystemSupport.h"
#include "App/PluginSupport/KeyValueSupport.h"
#include "App/ConnectionsGlobal.h"
#include "PluginSDK/Plugin.h"
#include "PluginSupport/PluginUISupport.h"
#include "ThirdParty/strnatcmp/strnatcmp.h"
#include "UI/UIPlugins.h"
#include "UI/UIAsk.h"
#include "UI/UISystem.h"
#include "UI/UIDebug.h"
#include <algorithm>
#include <string>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

/*** DEFINES                  ***/
#define MAX_DEVICES_FOR_SCAN    100 // The max number of devices we will read from the detect code
#define MAX_UNIQUE_ID_LEN       100

#define DATAEVENTQUEUE_SIZE                         30

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct IODriverAvailCon
{
    string DeviceUniqueID;
    string Name;
    string Title;
    bool InUse;
};
typedef list<struct IODriverAvailCon> t_IODriverAvailCon;
typedef t_IODriverAvailCon::iterator i_IODriverAvailCon;

struct IODriver
{
    string DriverName;
    string BaseURI;
    struct IODriverAPI API;
    t_IODriverAvailCon AvailableConnections;
    struct IODriverInfo Info;
};
typedef list<struct IODriver> t_IODriverListType;
typedef t_IODriverListType::iterator i_IODriverListType;

struct ConnectionInfoInternal
{
    struct ConnectionInfoList CInfo;

    /* Private data */
    i_IODriverListType drv;
    i_IODriverAvailCon con;
};

struct IOSystemDrvHandle
{
    uintptr_t ID;
    struct IODriver *IOdrv;
    string DeviceUniqueID;
    t_KVList Options;
    bool DrvOpen;
    t_DriverIOHandleType *DriverData;
    t_UIMutex *DataEventMutex;
    e_DataEventCodeType *DataEventQueue;
    int DataEventHead;
    int DataEventTail;
    bool DataAvailable;
};

typedef list<t_IOSystemHandle *> t_ActiveHandlesListType;
typedef t_ActiveHandlesListType::iterator i_ActiveHandlesListType;

struct COD_WidgetData
{
    struct PI_ComboBox *ComboBox;
    struct PI_TextInput *TextInput;
    struct PI_NumberInput *NumberInput;
    struct PI_DoubleInput *DoubleInput;
    struct PI_Checkbox *CheckboxInput;
    struct PI_RadioBttn *RadioBttnInput;
    struct ConnectionOptionsData *ConOptionData;
    void *UserData;
    void (*EventCB)(const struct PICBEvent *Event,void *UserData);
    void (*CheckboxEventCB)(const struct PICheckboxEvent *Event,void *UserData);
    void (*RadioBttnEventCB)(const struct PIRBEvent *Event,void *UserData);
    struct COD_WidgetData *Next;
};

struct ConnectionOptionsData
{
    t_ConnectionOptionsWidgetsType *ConOptions;
    i_IODriverListType drv;
    string DeviceUniqueID;
    t_UIContainerCtrl *ContainerWidget;
    struct COD_WidgetData *WidgetExtraData;
    void (*UIChangedCB)(void *UserData);
    void *UserData;
};

/*** FUNCTION PROTOTYPES      ***/
static PG_BOOL IOS_RegisterDriver(const char *DriverName,const char *BaseURI,
        const struct IODriverAPI *DriverAPI,int SizeOfDriverAPI);
static void IOS_DrvDataEvent(t_IOSystemHandle *IOHandle,int Code);
static void IOS_UnEscUniqueID(string &EscUniqueID,string &UniqueID);
static void IOS_EscUniqueID(const char *UniqueID,string &EscUniqueID);
static void IOS_EscUniqueID(string &UniqueID,string &EscUniqueID);
static void IOS_CombineID(const char *DriverName,const char *DeviceUniqueID,string &UniqueID);
static void IOS_SplitID(const char *ID,string &DriverName,string &DeviceUniqueID);
static const struct PI_UIAPI *IOS_Get_IOSystemUI_API(void);
static struct PI_ComboBox *IOS_AddComboBox(t_WidgetSysHandle *WidgetHandle,
        PG_BOOL UserEditable,const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
static void IOS_FreeComboBox(t_WidgetSysHandle *WidgetHandle,
        struct PI_ComboBox *UICtrl);
static void IOS_ComboBoxEventHandler(const struct PICBEvent *Event,
        void *UserData);
struct PI_TextInput *IOS_AddTextInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void IOS_FreeTextInput(t_WidgetSysHandle *WidgetHandle,struct PI_TextInput *UICtrl);
static void IOS_TextInputEventHandler(const struct PICBEvent *Event,
        void *UserData);
struct PI_NumberInput *IOS_AddNumberInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void IOS_FreeNumberInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_NumberInput *UICtrl);
static void IOS_NumberInputEventHandler(const struct PICBEvent *Event,
        void *UserData);
static struct PI_DoubleInput *IOS_AddDoubleInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
static void IOS_FreeDoubleInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_DoubleInput *UICtrl);
static void IOS_DoubleInputEventHandler(const struct PICBEvent *Event,
        void *UserData);
static struct PI_Checkbox *IOS_AddCheckboxInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),
        void *UserData);
static void IOS_CheckboxInputEventHandler(const struct PICheckboxEvent *Event,
        void *UserData);
static void IOS_FreeCheckboxInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_Checkbox *UICtrl);
static t_PI_RadioBttnGroup *IOS_AllocRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
        const char *Label);
static void IOS_RadioBttnInputEventHandler(const struct PIRBEvent *Event,
        void *UserData);
static void IOS_FreeRadioBttnInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_RadioBttn *UICtrl);
static struct PI_RadioBttn *IOS_AddRadioBttnInput(t_WidgetSysHandle *WidgetHandle,
        t_PI_RadioBttnGroup *RBGroup,const char *Label,
        void (*EventCB)(const struct PIRBEvent *Event,void *UserData),
        void *UserData);
static void IOS_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
        t_PI_RadioBttnGroup *UICtrl);

/*** VARIABLE DEFINITIONS     ***/
const struct IOS_API g_IOS_API=
{
    IOS_RegisterDriver,
    IOS_Get_IOSystemUI_API,
    IOS_DrvDataEvent,
};

static struct PI_UIAPI IOS_UIAPI=
{
    IOS_AddComboBox,
    IOS_FreeComboBox,
    PIUSDefault_ClearComboBox,
    PIUSDefault_AddItem2ComboBox,
    PIUSDefault_SetComboBoxSelectedEntry,
    PIUSDefault_GetComboBoxSelectedEntry,
    IOS_AllocRadioBttnGroup,
    IOS_FreeRadioBttnGroup,
    IOS_AddRadioBttnInput,
    IOS_FreeRadioBttnInput,
    PIUSDefault_IsRadioBttnChecked,
    PIUSDefault_SetRadioBttnChecked,
    PIUSDefault_GetComboBoxText,
    PIUSDefault_SetComboBoxText,
    IOS_AddCheckboxInput,
    IOS_FreeCheckboxInput,
    PIUSDefault_IsCheckboxChecked,
    PIUSDefault_SetCheckboxChecked,
    IOS_AddTextInput,
    IOS_FreeTextInput,
    PIUSDefault_GetTextInputText,
    PIUSDefault_SetTextInputText,

    IOS_AddNumberInput,
    IOS_FreeNumberInput,
    PIUSDefault_GetNumberInputValue,
    PIUSDefault_SetNumberInputValue,
    PIUSDefault_SetNumberInputMinMax,

    IOS_AddDoubleInput,
    IOS_FreeDoubleInput,
    PIUSDefault_GetDoubleInputValue,
    PIUSDefault_SetDoubleInputValue,
    PIUSDefault_SetDoubleInputMinMax,
    PIUSDefault_SetDoubleInputDecimals,

    PIUSDefault_EnableComboBox,
    PIUSDefault_EnableRadioBttn,
    PIUSDefault_EnableCheckbox,
    PIUSDefault_EnableTextInput,
    PIUSDefault_EnableNumberInput,
    PIUSDefault_EnableDoubleInput,
};

bool m_NeverScanned4Connections;
static t_IODriverListType m_IODriverList;
t_ActiveHandlesListType m_ActiveHandlesList;    // A list of active handles

/*******************************************************************************
 * NAME:
 *    IOS_RegisterDriver
 *
 * SYNOPSIS:
 *    PG_BOOL IOS_RegisterDriver(const char *DriverName,const char *BaseURI,
 *          const struct IODriver *DriverAPI,int SizeOfDriverAPI);
 *
 * PARAMETERS:
 *    DriverName [I] -- A Unique Name for this driver.  This just be only
 *                      A-Z a-z and 0-9.  This may also be shown to the user.
 *    BaseURI [I] -- The base URL string for this driver.  Only letters
 *                   are valid for this string (A-Z a-z).
 *    DriverAPI [I] -- The API to this driver.  This is a list of function
 *                     pointers.  See below for a list of each member (under
 *                     CALLBACKS).
 *    SizeOfDriverAPI [I] -- The sizeof the 'DriverAPI' structure the plugin
 *                           is using.  This is used for forward / backward
 *                           compatibility.
 *
 * FUNCTION:
 *    This function is called from a plugin IO driver.  It registers the
 *    plugin with the IO system.
 *
 * CALLBACKS:
 *==============================================================================
 * NAME:
 *    Init
 *
 * SYNOPSIS:
 *    PG_BOOL Init(void);
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
 *==============================================================================
 * NAME:
 *   GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*GetDriverInfo)(unsigned int *SizeOfInfo);
 *
 * PARAMETERS:
 *   SizeOfInfo [O] -- The size of 'struct IODriverInfo'.  This is used
 *                     for forward / backward compatibility.
 *
 * FUNCTION:
 *   This function gets info about the plugin.  'IODriverInfo' has
 *   the following fields:
 *           Flags -- Attributes about this driver.
 *                       Supported flags:
 *                           
 *
 * RETURNS:
 *   NONE
 *==============================================================================
 * NAME:
 *    InstallPlugin (TBD)
 *
 * SYNOPSIS:
 *    PG_BOOL InstallPlugin(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when a plugin is about to installed on the
 *    system.  This is before the plugin has actually being copied, or
 *    added to the system.
 *
 *    This function is use for any plugin that needs to install files or
 *    change system settings when the plugin is installed (copying drivers,
 *    changing registry,etc).
 *
 * RETURNS:
 *    true -- The plugin successfully installed it needed resources.
 *    false -- This plugin failed to install the needed resources.  The
 *             install of the plugin will be aborted.
 *
 * NOTES:
 *    Most plugin will not need this and should set this to NULL.
 *
 * SEE ALSO:
 *    UnInstallPlugin()
 *==============================================================================
 * NAME:
 *    UnInstallPlugin (TBD)
 *
 * SYNOPSIS:
 *    void UnInstallPlugin(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the user uninstalls a plugin.  It should
 *    remove any resources that where installed with InstallPlugin()
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    Most plugin will not need this and should set this to NULL.
 *
 * SEE ALSO:
 *    InstallPlugin()
 *==============================================================================
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
 *==============================================================================
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
 *==============================================================================
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
 *==============================================================================
 * NAME:
 *    ConnectionOptionsWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionOptionsWidgetsType *ConnectionOptionsWidgets_AllocWidgets(
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
 *    ConnectionOptionsWidgets_UpdateUI(), ConnectionOptionsWidgets_StoreUI()
 *==============================================================================
 * NAME:
 *    ConnectionOptionsWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *
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
 *==============================================================================
 * NAME:
 *    ConnectionOptionsWidgets_StoreUI
 *
 * SYNOPSIS:
 *    void ConnectionOptionsWidgets_StoreUI(
 *          t_ConnectionOptionsWidgetsType *ConOptions,
 *          t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
 *          t_PIKVList *Options);
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
 *    ConnectionOptionsWidgets_UpdateUI()
 *==============================================================================
 * NAME:
 *    ConnectionOptionsWidgets_UpdateUI
 *
 * SYNOPSIS:
 *    void ConnectionOptionsWidgets_UpdateUI(
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
 *    ConnectionOptionsWidgets_AllocWidgets() and sets them to the values
 *    stored in the key/value pair list in 'Options'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ConnectionOptionsWidgets_StoreUI()
 *==============================================================================
 * NAME:
 *    Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
 *          char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
 *          PG_BOOL Update);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to convert to a device ID and options.
 *    Options [O] -- The options for this new connection.
 *    DeviceUniqueID [O] -- The unique ID for this device build from the 'URI'
 *    MaxDeviceUniqueIDLen [I] -- The max length of the buffer for
 *          'DeviceUniqueID'
 *    Update [I] -- If this is true then we are updating 'Options'.  If
 *                  false then you should default 'Options' before you
 *                  fill in the options.
 *
 * FUNCTION:
 *    This function converts a URI string into a unique ID and options for
 *    a connection to be opened.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL Convert_Options_To_URI(const char *DeviceUniqueID,
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
 *    This function builds a URI for the device unique ID and options and
 *    returns that in a buffer.
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Convert_URI_To_Options
 *==============================================================================
 * NAME:
 *    AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *AllocateHandle(const char *DeviceUniqueID,
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
 *==============================================================================
 * NAME:
 *    FreeHandle
 *
 * SYNOPSIS:
 *    void FreeHandle(t_DriverIOHandleType *DriverIO);
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
 *==============================================================================
 * NAME:
 *    Open
 *
 * SYNOPSIS:
 *    PG_BOOL Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
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
 *    Close(), Read(), Write(), ChangeOptions()
 *==============================================================================
 * NAME:
 *    Close
 *
 * SYNOPSIS:
 *    void Close(t_DriverIOHandleType *DriverIO);
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
 *    Open()
 *==============================================================================
 * NAME:
 *    Read
 *
 * SYNOPSIS:
 *    int Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
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
 *    Open(), Write()
 *==============================================================================
 * NAME:
 *    Write
 *
 * SYNOPSIS:
 *    int Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
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
 *    Open(), Read()
 *==============================================================================
 * NAME:
 *    ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL ChangeOptions(t_DriverIOHandleType *DriverIO,
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
 *==============================================================================
 * NAME:
 *    Transmit
 *
 * SYNOPSIS:
 *    int Transmit(t_DriverIOHandleType *DriverIO);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *
 * FUNCTION:
 *    This function tells the driver to send any queued data.  This only really
 *    makes sence for block send devices.  Can be set to NULL.
 *
 * RETURNS:
 *    The same as Write()
 *
 * SEE ALSO:
 *    Open(), Write()
 *==============================================================================
 *
 * RETURNS:
 *    true -- Registration worked
 *    false -- There was an error.  The user has been informed.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL IOS_RegisterDriver(const char *DriverName,const char *BaseURI,
        const struct IODriverAPI *DriverAPI,int SizeOfDriverAPI)
{
    struct IODriver NewDriver;
    const char *pos;
    unsigned int Size2Copy;
    unsigned int InfoSize;
    const struct IODriverInfo *Info;

    try
    {
        /* Check for bad chars in the driver name */
        for(pos=DriverName;*pos!=0;pos++)
        {
            if((*pos<'1' || *pos>'9') &&
                    (*pos<'a' || *pos>'z') &&
                    (*pos<'A' || *pos>'Z'))
            {
                throw(0);
            }
        }

        /* Check for bad chars in the Base URI */
        for(pos=BaseURI;*pos!=0;pos++)
        {
            if((*pos<'a' || *pos>'z') &&
                    (*pos<'A' || *pos>'Z'))
            {
                throw(0);
            }
        }

        NewDriver.DriverName=DriverName;
        NewDriver.BaseURI=BaseURI;
        memset(&NewDriver.API,0x00,sizeof(NewDriver.API));
        Size2Copy=SizeOfDriverAPI;
        if(Size2Copy>sizeof(NewDriver.API))
            Size2Copy=sizeof(NewDriver.API);
        memcpy(&NewDriver.API,DriverAPI,Size2Copy);

        /* Make sure we have the min API */
        if(NewDriver.API.Convert_URI_To_Options==NULL ||
                NewDriver.API.Convert_Options_To_URI==NULL ||
                NewDriver.API.GetDriverInfo==NULL ||
                NewDriver.API.AllocateHandle==NULL ||
                NewDriver.API.FreeHandle==NULL ||
                NewDriver.API.Open==NULL ||
                NewDriver.API.Close==NULL ||
                NewDriver.API.Read==NULL ||
                NewDriver.API.Write==NULL ||
                NewDriver.API.DetectDevices==NULL ||
                NewDriver.API.GetConnectionInfo==NULL ||
                NewDriver.API.Init==NULL)

        {
            throw(0);
        }

        /* Get info about this plugin */
        Info=NewDriver.API.GetDriverInfo(&InfoSize);
        memset(&NewDriver.Info,0x00,sizeof(NewDriver.Info));
        Size2Copy=InfoSize;
        if(Size2Copy>sizeof(NewDriver.Info))
            Size2Copy=sizeof(NewDriver.Info);
        memcpy(&NewDriver.Info,Info,Size2Copy);

        /* Uppercase the BaseURI */
        transform(NewDriver.BaseURI.begin(),NewDriver.BaseURI.end(),
                NewDriver.BaseURI.begin(),::toupper);

        m_IODriverList.push_back(NewDriver);
    }
    catch(...)
    {
        char buff[100];

        sprintf(buff,"Failed to register IO plugin \"%s\".",DriverName);
        UIAsk("Error",buff,e_AskBox_Error,e_AskBttns_Ok);

        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    IOS_Get_IOSystemUI_API
 *
 * SYNOPSIS:
 *    static const struct PI_UIAPI *IOS_Get_IOSystemUI_API(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the UI used with the IO system.
 *
 * RETURNS:
 *    A pointer to the IO system version of the 'PI_UIAPI' structure.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static const struct PI_UIAPI *IOS_Get_IOSystemUI_API(void)
{
    return &IOS_UIAPI;
}

/*******************************************************************************
 * NAME:
 *    IOS_Init
 *
 * SYNOPSIS:
 *    void IOS_Init(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the IO system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void IOS_Init(void)
{
    m_IODriverList.clear();
    m_NeverScanned4Connections=true;

/* DEBUG PAUL: Reload the list from session */
}

/*******************************************************************************
 * NAME:
 *    IOS_Shutdown
 *
 * SYNOPSIS:
 *    void IOS_Shutdown(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the system is shutting down.  It frees
 *    any memory used by this IO system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void IOS_Shutdown(void)
{
}

/*******************************************************************************
 * NAME:
 *    IOS_InitPlugins
 *
 * SYNOPSIS:
 *    void IOS_InitPlugins(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles init'ing of stuff related to the plugin's.  This
 *    can't be done in normal init because the plugin's haven't been
 *    registered yet.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void IOS_InitPlugins(void)
{
    i_IODriverListType drv;

    /* Ask each driver to fill in supported connections */
    for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
    {
        if(drv->API.Init!=NULL && !drv->API.Init())
        {
            /* Ok, we couldn't init this plugin, remove it from the list */
            m_IODriverList.erase(drv);
            drv=m_IODriverList.begin();
        }
    }

/* DEBUG PAUL: Add setting for "force rescan at startup" */
    IOS_ScanForConnections();
}

/*******************************************************************************
 * NAME:
 *    IOS_ScanForConnections
 *
 * SYNOPSIS:
 *    void IOS_ScanForConnections(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function will scan and make a list of connections that can be
 *    made.  This function will force a rescan of the connections.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void IOS_ScanForConnections(void)
{
    i_IODriverListType drv;
    struct IODriverAvailCon NewCon;
    int FoundDevices;
    const struct IODriverDetectedInfo *DetectedDevices;
    const struct IODriverDetectedInfo *DetDev;

    /* Ask each driver to fill in supported connections */
    for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
    {
        DetectedDevices=drv->API.DetectDevices();

        drv->AvailableConnections.clear();
        FoundDevices=0;
        for(DetDev=DetectedDevices;DetDev!=NULL;DetDev=DetDev->Next)
        {
            /* For now we just check that the plugin filled in the
               size.  In the future we can use this to expand the
               structure without breaking older plugins. */
            if(DetDev->StructureSize!=sizeof(struct IODriverDetectedInfo))
                continue;

            NewCon.DeviceUniqueID=DetDev->DeviceUniqueID;
            NewCon.Name=DetDev->Name;
            NewCon.Title=DetDev->Title;
            NewCon.InUse=false;
            if(DetDev->Flags&IODRV_DETECTFLAG_INUSE)
                NewCon.InUse=true;

            try
            {
                drv->AvailableConnections.push_back(NewCon);
            }
            catch(...)
            {
                continue;
            }

            FoundDevices++;
            if(FoundDevices>MAX_DEVICES_FOR_SCAN)
                break;
        }

        /* Plugin doesn't have to provide a free function because they
           can just return a static list */
        if(drv->API.FreeDetectedDevices!=NULL)
            drv->API.FreeDetectedDevices(DetectedDevices);
        DetectedDevices=NULL;
    }
    m_NeverScanned4Connections=false;
}

/*******************************************************************************
 * NAME:
 *    IOS_GetListOfAvailableConnections
 *
 * SYNOPSIS:
 *    struct ConnectionInfoList *IOS_GetListOfAvailableConnections(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates a list of available connections.  This function
 *    will return a linked list of 'struct ConnectionInfoList' with a
 *    sorted list of connections that are available on the system.
 *
 * RETURNS:
 *    A linked list of available connections or NULL if there was an error.
 *
 * NOTES:
 *    You must free the list with IOS_FreeListOfAvailableConnections()
 *
 *    If you call IOS_ScanForConnections() then this list is no longer valid.
 *
 * SEE ALSO:
 *    IOS_FreeListOfAvailableConnections(), IOS_ScanForConnections()
 ******************************************************************************/
struct ConnectionInfoList *IOS_GetListOfAvailableConnections(void)
{
    i_IODriverListType drv;
    i_IODriverAvailCon con;
    struct ConnectionInfoInternal *ListOfCon;
    struct ConnectionInfoInternal *NewCon;
    struct ConnectionInfoInternal *SearchCon;
    struct ConnectionInfoInternal *PrevCon;

    try
    {
        if(m_NeverScanned4Connections)
            IOS_ScanForConnections();

        ListOfCon=NULL;
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
        {
            for(con=drv->AvailableConnections.begin();
                    con!=drv->AvailableConnections.end();con++)
            {
                NewCon=new struct ConnectionInfoInternal;
                IOS_CombineID(drv->DriverName.c_str(),
                        con->DeviceUniqueID.c_str(),NewCon->CInfo.UniqueID);
                NewCon->CInfo.Name=con->Name;
                NewCon->CInfo.Title=con->Title;
                NewCon->CInfo.InUse=con->InUse;
                NewCon->CInfo.BlockDevice=
                        drv->Info.Flags&IODRVINFOFLAG_BLOCKDEV;
                NewCon->CInfo.Next=NULL;
                NewCon->drv=drv;
                NewCon->con=con;

                /* Find where this entry should fit */
                PrevCon=NULL;
                for(SearchCon=ListOfCon;SearchCon!=NULL;
                        SearchCon=(struct ConnectionInfoInternal *)
                        SearchCon->CInfo.Next)
                {
                    if(strnatcasecmp(SearchCon->CInfo.Name.c_str(),
                            con->Name.c_str())>=0)
                    {
                        break;
                    }
                    PrevCon=SearchCon;
                }
                if(SearchCon==NULL)
                {
                    /* Add to the end */
                    if(PrevCon==NULL)
                    {
                        /* Empty list */
                        ListOfCon=NewCon;
                    }
                    else
                    {
                        NewCon->CInfo.Next=PrevCon->CInfo.Next;
                        PrevCon->CInfo.Next=(struct ConnectionInfoList *)NewCon;
                    }
                }
                else
                {
                    /* SearchCon is our insert point */
                    if(PrevCon==NULL)
                    {
                        /* Top of list */
                        NewCon->CInfo.Next=(struct ConnectionInfoList *)ListOfCon;
                        ListOfCon=NewCon;
                    }
                    else
                    {
                        NewCon->CInfo.Next=PrevCon->CInfo.Next;
                        PrevCon->CInfo.Next=(struct ConnectionInfoList *)NewCon;
                    }
                }
            }
        }
    }
    catch(...)
    {
        return NULL;
    }
    return (struct ConnectionInfoList *)ListOfCon;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeListOfAvailableConnections
 *
 * SYNOPSIS:
 *    void IOS_FreeListOfAvailableConnections(struct ConnectionInfoList *CInfo);
 *
 * PARAMETERS:
 *    CInfo [I] -- The list of connections allocated with
 *                 IOS_GetListOfAvailableConnections()
 *
 * FUNCTION:
 *    This function frees the list of connections allocated with
 *    IOS_GetListOfAvailableConnections().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_GetListOfAvailableConnections()
 ******************************************************************************/
void IOS_FreeListOfAvailableConnections(struct ConnectionInfoList *CInfo)
{
    struct ConnectionInfoInternal *ListOfCon=(struct ConnectionInfoInternal *)CInfo;
    struct ConnectionInfoInternal *Current;
    struct ConnectionInfoInternal *Next;

    for(Current=ListOfCon;Current!=NULL;)
    {
        Next=(struct ConnectionInfoInternal *)Current->CInfo.Next;
        delete Current;
        Current=Next;
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_MakeURIFromDetectedCon
 *
 * SYNOPSIS:
 *    bool IOS_MakeURIFromDetectedCon(
 *          struct ConnectionInfoList *CInfoEntry,
 *          const t_KVList &Options,std::string &URI);
 *
 * PARAMETERS:
 *    CInfoEntry [I] -- The connection entry to get the unique id for
 *    Options [I] -- The options for this connection.
 *    URI [O] -- The URI for this connection.
 *
 * FUNCTION:
 *    This function gets a unique ID string for a connection.  This unique
 *    string can be stored and used to find this connection again.  The
 *    unique ID is based partly on the detected ID and the options.
 *
 * RETURNS:
 *    true -- Built URI for this connection.
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool IOS_MakeURIFromDetectedCon(struct ConnectionInfoList *CInfoEntry,
        const t_KVList &Options,std::string &URI)
{
    struct ConnectionInfoInternal *ConInfo=(struct ConnectionInfoInternal *)CInfoEntry;
    string DriverName;
    string DeviceUniqueID;
    char URIBuffer[MAX_URI_LENGTH];
    bool Success;

    try
    {
        URI="";

        IOS_SplitID(CInfoEntry->UniqueID.c_str(),DriverName,DeviceUniqueID);

        if(!ConInfo->drv->API.Convert_Options_To_URI(DeviceUniqueID.c_str(),
                PIS_ConvertKVList2PIKVList(Options),URIBuffer,
                MAX_URI_LENGTH))
        {
            throw(0);
        }

        URI=URIBuffer;

        Success=true;
    }
    catch(...)
    {
        Success=false;
    }

    return Success;
}

/*******************************************************************************
 * NAME:
 *    IOS_EscUniqueID
 *
 * SYNOPSIS:
 *    static void IOS_EscUniqueID(string &UniqueID,string &EscUniqueID);
 *    static void IOS_EscUniqueID(const char *UniqueID,string &EscUniqueID);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The Unique ID to esc
 *    EscUniqueID [O] -- The new string with the esc'ed version in it.
 *
 * FUNCTION:
 *    This function escapes a uniqueID.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    Will throw out of memory (from the strings).
 *
 * SEE ALSO:
 *    IOS_UnEscUniqueID
 ******************************************************************************/
static void IOS_EscUniqueID(string &UniqueID,string &EscUniqueID)
{
    string::iterator pos;
    char c;
    char buff[100];

    /* Ok, escape the string */
    EscUniqueID="";
    EscUniqueID.reserve(UniqueID.length()*2);
    for(pos=UniqueID.begin();pos!=UniqueID.end();pos++)
    {
        c=*pos;
        if((c<'0' || c>'9') && (c<'A' || c>'Z') && (c<'a' || c>'z'))
        {
            /* Ok, not a valid char, esc it */
            sprintf(buff,"_%d_",(unsigned char)c);
            EscUniqueID+=buff;
        }
        else
        {
            EscUniqueID+=c;
        }
    }
}
static void IOS_EscUniqueID(const char *UniqueID,string &EscUniqueID)
{
    string TmpStr;
    TmpStr=UniqueID;
    IOS_EscUniqueID(TmpStr,EscUniqueID);
}

/*******************************************************************************
 * NAME:
 *    IOS_UnEscUniqueID
 *
 * SYNOPSIS:
 *    static void IOS_UnEscUniqueID(string &EscUniqueID,string &UniqueID);
 *
 * PARAMETERS:
 *    EscUniqueID [I] -- The string to unescape
 *    UniqueID [O] -- The string with the escaped chars replaced.
 *
 * FUNCTION:
 *    This function undoes what IOS_EscUniqueID() does.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    Will throw out of memory (from the strings).
 *
 * SEE ALSO:
 *    IOS_EscUniqueID()
 ******************************************************************************/
static void IOS_UnEscUniqueID(string &EscUniqueID,string &UniqueID)
{
    string::iterator pos;
    char c;
    char buff[100];
    char *InsertPos;

    /* Ok, escape the string */
    UniqueID="";
    UniqueID.reserve(EscUniqueID.length());
    for(pos=EscUniqueID.begin();pos!=EscUniqueID.end();pos++)
    {
        c=*pos;
        if(c=='_')
        {
            pos++;  // Move past _
            InsertPos=buff;
            while(*pos>='0' && *pos<='9' && InsertPos<buff+sizeof(buff)-1)
                *InsertPos++=*pos++;
            *InsertPos++=0;
            c=atoi(buff);
        }
        UniqueID+=c;
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_CombineID
 *
 * SYNOPSIS:
 *    static void IOS_CombineID(const char *DriverName,
 *          const char *DeviceUniqueID,string &UniqueID);
 *
 * PARAMETERS:
 *    DriverName [I] -- This is the name of the driver in the system.
 *    DeviceUniqueID [I] -- The device ID part of 'ID'.  This is the ID that is
 *                    sent to the device driver using it's API.
 *    UniqueID [O] -- The new unique ID built from the 'DriverName' and
 *                    'DeviceUniqueID'
 *
 * FUNCTION:
 *    This function builds a UniqueID for use in the system.  It takes the
 *    driver's name (ID) and combines it with the DeviceUniqueID from the
 *    driver it's self to make a system UniqueID.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_SplitID()
 ******************************************************************************/
static void IOS_CombineID(const char *DriverName,const char *DeviceUniqueID,
        string &UniqueID)
{
    string EscString;

    try
    {
        IOS_EscUniqueID(DeviceUniqueID,EscString);

        UniqueID=DriverName;
        UniqueID+="-";
        UniqueID+=EscString;
    }
    catch(...)
    {
        UniqueID="";
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_SplitID
 *
 * SYNOPSIS:
 *    static void IOS_SplitID(const char *ID,string &DriverName,
 *          string &DeviceUniqueID);
 *
 * PARAMETERS:
 *    ID [I] -- The string with the ID in it.  This the version that includes
 *              the driver name and other stuff used by the IO system.
 *    DriverName [O] -- This is the name of the driver in the system.
 *    DeviceUniqueID [O] -- The device ID part of 'ID'.  This is the ID that is
 *                    sent to the device driver using it's API.
 *
 * FUNCTION:
 *    This function takes a UniqueID and splits it in to it's parts.
 *
 *    The IO system includes a bunch of extra data in the ID's that the
 *    system uses.  The device however uses a cut down version.  This
 *    function converts a system ID to a device ID.
 *
 * RETURNS:
 *    NONE
 *
 * THROWS:
 *    This function can throw errors so you should try-catch it.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_SplitID(const char *ID,string &DriverName,string &DeviceUniqueID)
{
    const char *EndOfDrvName;
    string TmpStr;

    EndOfDrvName=strchr(ID,'-');
    if(EndOfDrvName==NULL)
        throw(0);

    DriverName.assign(ID,EndOfDrvName-ID);
    TmpStr=EndOfDrvName+1;
    IOS_UnEscUniqueID(TmpStr,DeviceUniqueID);
}

/*******************************************************************************
 * NAME:
 *    IOS_FindConnectionFromDetectedID
 *
 * SYNOPSIS:
 *    struct ConnectionInfoList *IOS_FindConnectionFromDetectedID(
 *              struct ConnectionInfoList *CInfo,const char *DetectedID);
 *
 * PARAMETERS:
 *    CInfo [I] -- The list of connections to find the unique id in.
 *    DetectedID [I] -- The detected ID string to search for
 *
 * FUNCTION:
 *    This function searchs for a unique ID from a list of connections.
 *
 * RETURNS:
 *    A pointer to the entry in the 'CInfo' list.
 *    NULL if it could not be found.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct ConnectionInfoList *IOS_FindConnectionFromDetectedID(
        struct ConnectionInfoList *CInfo,const char *DetectedID)
{
    struct ConnectionInfoInternal *ListOfCon=(struct ConnectionInfoInternal *)CInfo;
    string SearchDriverName;
    string DeviceDetectedID;
    i_IODriverListType drv;
    struct ConnectionInfoInternal *Entry;

    try
    {
        /* First find the driver */
        IOS_SplitID(DetectedID,SearchDriverName,DeviceDetectedID);

        /* Ok, search our drivers to find this one */
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
        {
            if(drv->DriverName==SearchDriverName)
            {
                /* Found it */
                break;
            }
        }
        if(drv==m_IODriverList.end())
        {
            /* Driver not found */
            return NULL;
        }

        /* Ok, now we try to find this in the connection list */
        for(Entry=ListOfCon;Entry!=NULL;
                Entry=(struct ConnectionInfoInternal *)Entry->CInfo.Next)
        {
            if(&*Entry->drv==&*drv)
            {
                if(strcmp(Entry->con->DeviceUniqueID.c_str(),
                        DeviceDetectedID.c_str())==0)
                {
                    break;
                }
            }
        }
    }
    catch(...)
    {
        return NULL;
    }
    return (struct ConnectionInfoList *)Entry;
}

/*******************************************************************************
 * NAME:
 *    IOS_AllocConnectionOptions
 *
 * SYNOPSIS:
 *    t_ConnectionOptionsDataType *IOS_AllocConnectionOptions(
 *          struct ConnectionInfoList *CInfoEntry,
 *          t_UIContainerCtrl *ContainerWidget,t_KVList &OptionsKeyValues,
 *          void (*UIChanged)(void *UserData),void *UserData);
 *
 * PARAMETERS:
 *    CInfoEntry [I] -- The connection entry to add the controls for
 *    ContainerWidget [I] -- The widget to add the controls to
 *    OptionsKeyValues [I] -- The options to set the new widgets to
 *    UIChanged [I] -- A pointer to a function to call if one of the options
 *                     changed.  This my be used to refresh other UI elements
 *                     that are based on the options.
 *    UserData [I] -- User data that is just sent to UIChanged()
 *
 * FUNCTION:
 *    This function adds controls for adjusting the options for a connection
 *    to the UI.
 *
 * RETURNS:
 *    The handle to the allocated options data.
 *
 * SEE ALSO:
 *    IOS_AllocConnectionOptionsFromUniqueID(), IOS_FreeConnectionOptions()
 ******************************************************************************/
t_ConnectionOptionsDataType *IOS_AllocConnectionOptions(
        struct ConnectionInfoList *CInfoEntry,
        t_UIContainerCtrl *ContainerWidget,t_KVList &OptionsKeyValues,
        void (*UIChanged)(void *UserData),void *UserData)
{
    struct ConnectionInfoInternal *ConInfo=(struct ConnectionInfoInternal *)CInfoEntry;
    t_ConnectionOptionsWidgetsType *ConOptions;
    struct ConnectionOptionsData *ConOptionData;
    string DriverName;
    string DeviceUniqueID;
    

    ConOptions=NULL;
    try
    {
        IOS_SplitID(CInfoEntry->UniqueID.c_str(),DriverName,DeviceUniqueID);

        ConOptionData=new struct ConnectionOptionsData;
        ConOptionData->ContainerWidget=ContainerWidget;
        ConOptionData->WidgetExtraData=NULL;
        ConOptionData->UIChangedCB=UIChanged;
        ConOptionData->UserData=UserData;

        if(ConInfo->drv->API.ConnectionOptionsWidgets_AllocWidgets!=NULL)
        {
            ConOptions=ConInfo->drv->API.ConnectionOptionsWidgets_AllocWidgets(
                    (t_WidgetSysHandle *)ConOptionData);
            if(ConOptions==NULL)
                throw(false);

            if(ConInfo->drv->API.ConnectionOptionsWidgets_UpdateUI!=NULL)
            {
                ConInfo->drv->API.ConnectionOptionsWidgets_UpdateUI(ConOptions,
                        (t_WidgetSysHandle *)ConOptionData,
                        DeviceUniqueID.c_str(),
                        PIS_ConvertKVList2PIKVList(OptionsKeyValues));
            }
        }
        ConOptionData->ConOptions=ConOptions;
        ConOptionData->drv=ConInfo->drv;
        ConOptionData->DeviceUniqueID=DeviceUniqueID;
    }
    catch(...)
    {
        if(ConOptions!=NULL)
        {
            if(ConInfo->drv->API.ConnectionOptionsWidgets_FreeWidgets!=NULL)
            {
                ConInfo->drv->API.ConnectionOptionsWidgets_FreeWidgets(
                        ConOptions,(t_WidgetSysHandle *)ConOptionData);
            }
            ConOptions=NULL;
        }
        UIAsk("Error","Failed to add options to UI.",e_AskBox_Error,
                e_AskBttns_Ok);
        return NULL;
    }

    return (t_ConnectionOptionsDataType *)ConOptionData;
}

/*******************************************************************************
 * NAME:
 *    IOS_AllocConnectionOptionsFromUniqueID
 *
 * SYNOPSIS:
 *    t_ConnectionOptionsDataType *IOS_AllocConnectionOptionsFromUniqueID(
 *              const char *UniqueID,t_UIContainerCtrl *ContainerWidget,
 *              t_KVList &OptionsKeyValues,,void (*UIChanged)(void *UserData),
 *              void *UserData);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The string with the DeviceUniqueID in it.  This the
 *                    version that include that driver name and other stuff
 *                    used by the IO system.
 *    ContainerWidget [I] -- The widget to add the controls to
 *    OptionsKeyValues [I] -- The options to set the new widgets to
 *    UIChanged [I] -- A pointer to a function to call if one of the options
 *                     changed.  This my be used to refresh other UI elements
 *                     that are based on the options.
 *    UserData [I] -- User data that is just sent to UIChanged()
 *
 * FUNCTION:
 *    This function adds controls for adjusting the options for a connection
 *    to the UI.
 *
 *    This version does it from a connections Unique ID.
 *
 * RETURNS:
 *    The handle to the allocated options data or NULL if there was an error.
 *
 * SEE ALSO:
 *    IOS_AllocConnectionOptions() IOS_FreeConnectionOptions()
 ******************************************************************************/
t_ConnectionOptionsDataType *IOS_AllocConnectionOptionsFromUniqueID(
        const char *UniqueID,t_UIContainerCtrl *ContainerWidget,
        t_KVList &OptionsKeyValues,void (*UIChanged)(void *UserData),
        void *UserData)
{
    t_ConnectionOptionsWidgetsType *ConOptions;
    struct ConnectionOptionsData *ConOptionData;
    string SearchDriverName;
    string DeviceUniqueID;
    i_IODriverListType drv;

    ConOptions=NULL;
    try
    {
        ConOptionData=new struct ConnectionOptionsData;
        ConOptionData->ContainerWidget=ContainerWidget;
        ConOptionData->WidgetExtraData=NULL;
        ConOptionData->UIChangedCB=UIChanged;
        ConOptionData->UserData=UserData;

        /* First find the driver */
        IOS_SplitID(UniqueID,SearchDriverName,DeviceUniqueID);

        /* Ok, search our drivers to find this one */
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
        {
            if(drv->DriverName==SearchDriverName)
            {
                /* Found it */
                break;
            }
        }
        if(drv==m_IODriverList.end())
        {
            /* Driver not found */
            delete ConOptionData;
            return NULL;
        }

        if(drv->API.ConnectionOptionsWidgets_AllocWidgets!=NULL)
        {
            ConOptions=drv->API.ConnectionOptionsWidgets_AllocWidgets(
                    (t_WidgetSysHandle *)ConOptionData);
            if(ConOptions==NULL)
                throw(false);

            if(drv->API.ConnectionOptionsWidgets_UpdateUI!=NULL)
            {
                drv->API.ConnectionOptionsWidgets_UpdateUI(ConOptions,
                        (t_WidgetSysHandle *)ConOptionData,
                        DeviceUniqueID.c_str(),
                        PIS_ConvertKVList2PIKVList(OptionsKeyValues));
            }
        }
        ConOptionData->ConOptions=ConOptions;
        ConOptionData->drv=drv;
        ConOptionData->DeviceUniqueID=DeviceUniqueID;
    }
    catch(...)
    {
        if(ConOptions!=NULL)
        {
            if(drv->API.ConnectionOptionsWidgets_FreeWidgets!=NULL)
            {
                drv->API.ConnectionOptionsWidgets_FreeWidgets(ConOptions,
                        (t_WidgetSysHandle *)ConOptionData);
            }
            ConOptions=NULL;
        }
        delete ConOptionData;
        UIAsk("Error","Failed to add options to UI.",e_AskBox_Error,
                e_AskBttns_Ok);
        return NULL;
    }

    return (t_ConnectionOptionsDataType *)ConOptionData;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeConnectionOptions
 *
 * SYNOPSIS:
 *    void IOS_FreeConnectionOptions(
 *          t_ConnectionOptionsDataType *ConOptionsHandle);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function removes the options that were added with
 *    IOS_AllocConnectionOptions().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_AllocConnectionOptions()
 ******************************************************************************/
void IOS_FreeConnectionOptions(t_ConnectionOptionsDataType *ConOptionsHandle)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)ConOptionsHandle;

    /* Ok we need to find this connection */
    try
    {
        if(ConOptionData->drv->API.ConnectionOptionsWidgets_FreeWidgets!=NULL)
        {
            ConOptionData->drv->API.ConnectionOptionsWidgets_FreeWidgets(
                    ConOptionData->ConOptions,
                    (t_WidgetSysHandle *)ConOptionData);
        }

        delete ConOptionData;
    }
    catch(...)
    {
        string ErrorMsg;
        ErrorMsg="Failed to free options from UI.";
        UIAsk("Error",ErrorMsg,e_AskBox_Error,e_AskBttns_Ok);
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_StoreConnectionOptions
 *
 * SYNOPSIS:
 *    void IOS_StoreConnectionOptions(
 *          t_ConnectionOptionsDataType *ConOptionsHandle,
 *          t_KVList &OptionsKeyValues);
 *
 * PARAMETERS:
 *    ConOptionsHandle [I] -- The handle that was allocated with
 *                              IOS_AllocConnectionOptions()
 *    OptionsKeyValues [O] -- The options to set to the UI values.
 *
 * FUNCTION:
 *    This function stores the current connection options from the UI to
 *    a KVList of options.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_SetUI2ConnectionOptions(), IOS_AllocConnectionOptions()
 ******************************************************************************/
void IOS_StoreConnectionOptions(t_ConnectionOptionsDataType *ConOptionsHandle,
        t_KVList &OptionsKeyValues)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)ConOptionsHandle;

    if(ConOptionData->drv->API.ConnectionOptionsWidgets_StoreUI!=NULL)
    {
        ConOptionData->drv->API.ConnectionOptionsWidgets_StoreUI(
                ConOptionData->ConOptions,
                (t_WidgetSysHandle *)ConOptionData,
                ConOptionData->DeviceUniqueID.c_str(),
                PIS_ConvertKVList2PIKVList(OptionsKeyValues));
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_SetUI2ConnectionOptions
 *
 * SYNOPSIS:
 *    void IOS_SetUI2ConnectionOptions(
 *          t_ConnectionOptionsDataType *ConOptionsHandle,
 *          t_KVList &OptionsKeyValues);
 *
 * PARAMETERS:
 *    ConOptionsHandle [I] -- The handle that was allocated with
 *                              IOS_AllocConnectionOptions()
 *    OptionsKeyValues [I] -- The options to set to the UI values.
 *
 * FUNCTION:
 *    This function updates the UI with values from the 'OptionsKeyValues'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_StoreConnectionOptions()
 ******************************************************************************/
void IOS_SetUI2ConnectionOptions(t_ConnectionOptionsDataType *ConOptionsHandle,
        t_KVList &OptionsKeyValues)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)ConOptionsHandle;

    if(ConOptionData->drv->API.ConnectionOptionsWidgets_UpdateUI!=NULL)
    {
        ConOptionData->drv->API.ConnectionOptionsWidgets_UpdateUI(
                ConOptionData->ConOptions,
                (t_WidgetSysHandle *)ConOptionData,
                ConOptionData->DeviceUniqueID.c_str(),
                PIS_ConvertKVList2PIKVList(OptionsKeyValues));
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_AllocIOSystemHandle
 *
 * SYNOPSIS:
 *    t_IOSystemHandle *IOS_AllocIOSystemHandle(const char *UniqueID,
 *              uintptr_t ID);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The unique ID string to search for
 *    ID [I] -- An ID/ptr to user data.  This will be sent to any higher level
 *              functions.
 *
 * FUNCTION:
 *    This function allocates a handle to the driver for sending bytes in/out.
 *
 * RETURNS:
 *    A handle to the driver or NULL if there was an error.
 *
 * SEE ALSO:
 *    IOS_FreeIOSystemHandle(), IOS_AllocIOSystemHandleFromURI()
 ******************************************************************************/
t_IOSystemHandle *IOS_AllocIOSystemHandle(const char *UniqueID,uintptr_t ID)
{
    string SearchDriverName;
    string DeviceUniqueID;
    i_IODriverListType drv;
    struct IOSystemDrvHandle *DrvHandle;

    DrvHandle=NULL;
    try
    {
        DrvHandle=new struct IOSystemDrvHandle;

        DrvHandle->DriverData=NULL;
        DrvHandle->DataEventMutex=NULL;
        DrvHandle->DataEventQueue=NULL;
        DrvHandle->DeviceUniqueID=UniqueID;

        DrvHandle->ID=ID;

        /* First find the driver */
        IOS_SplitID(UniqueID,SearchDriverName,DeviceUniqueID);

        /* Ok, search our drivers to find this one */
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
        {
            if(drv->DriverName==SearchDriverName)
            {
                /* Found it */
                break;
            }
        }
        if(drv==m_IODriverList.end())
        {
            /* Driver not found */
            throw(0);
        }

        DrvHandle->IOdrv=&(*drv);

        /* Now allocate the drivers data for this connection */
        if(drv->API.AllocateHandle!=NULL)
        {
            DrvHandle->DriverData=drv->API.AllocateHandle(DeviceUniqueID.c_str(),
                    (t_IOSystemHandle *)DrvHandle);
            if(DrvHandle->DriverData==NULL)
                throw(0);
        }

        DrvHandle->DataEventMutex=AllocMutex();
        if(DrvHandle->DataEventMutex==NULL)
            throw(0);

        DrvHandle->DataEventQueue=(e_DataEventCodeType *)
                malloc(DATAEVENTQUEUE_SIZE*sizeof(e_DataEventCodeType));
        if(DrvHandle->DataEventQueue==NULL)
            throw(0);

        /* Throw this handle on the list of active handles */
        m_ActiveHandlesList.push_back((t_IOSystemHandle *)DrvHandle);

        DrvHandle->DataEventHead=0;
        DrvHandle->DataEventTail=0;

        DrvHandle->DrvOpen=false;
        DrvHandle->DataAvailable=false;
    }
    catch(...)
    {
        if(DrvHandle!=NULL)
        {
            if(DrvHandle->DataEventQueue!=NULL)
                free(DrvHandle->DataEventQueue);
            if(DrvHandle->DataEventMutex!=NULL)
                FreeMutex(DrvHandle->DataEventMutex);
            if(DrvHandle->DriverData!=NULL)
                drv->API.FreeHandle(DrvHandle->DriverData);
            delete DrvHandle;
        }
        return NULL;
    }

    return (t_IOSystemHandle *)DrvHandle;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeIOSystemHandle
 *
 * SYNOPSIS:
 *    void IOS_FreeIOSystemHandle(t_IOSystemHandle *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- The driver handle to free.
 *
 * FUNCTION:
 *    This function frees a driver handle that was allocated with
 *    IOS_AllocIOSystemHandle()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_AllocIOSystemHandle()
 ******************************************************************************/
void IOS_FreeIOSystemHandle(t_IOSystemHandle *Handle)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;

    if(DrvHandle->DrvOpen)
        IOS_Close(Handle);

    /* Remove this handle from the list of available handles */
    m_ActiveHandlesList.remove(Handle);

    free(DrvHandle->DataEventQueue);
    DrvHandle->DataEventQueue=NULL;

    FreeMutex(DrvHandle->DataEventMutex);
    DrvHandle->DataEventMutex=NULL;

    if(DrvHandle->IOdrv->API.FreeHandle!=NULL)
        DrvHandle->IOdrv->API.FreeHandle(DrvHandle->DriverData);

    delete DrvHandle;
}

/*******************************************************************************
 * NAME:
 *    IOS_Open
 *
 * SYNOPSIS:
 *    bool IOS_Open(t_IOSystemHandle *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- The IO system handle to work on
 *
 * FUNCTION:
 *    This function opens the device (well tells the drive to open it)
 *
 * RETURNS:
 *    true -- Things worked out.  Device is open
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    IOS_WriteData(), IOS_ReadData(), IOS_Close()
 ******************************************************************************/
bool IOS_Open(t_IOSystemHandle *Handle)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;

    if(DrvHandle->IOdrv->API.Open!=NULL)
    {
        if(!DrvHandle->IOdrv->API.Open(DrvHandle->DriverData,
                PIS_ConvertKVList2PIKVList(DrvHandle->Options)))
        {
            return false;
        }
    }

    DrvHandle->DrvOpen=true;

    return true;
}

/*******************************************************************************
 * NAME:
 *    IOS_WriteData
 *
 * SYNOPSIS:
 *    e_IOSysIOErrorType IOS_WriteData(t_IOSystemHandle *Handle,
 *          const uint8_t *Data,int Bytes);
 *
 * PARAMETERS:
 *    Handle [I] -- The IO system handle to work on
 *    Data [I] -- The data to send to the device
 *    Bytes [I] -- The number of bytes to send.
 *
 * FUNCTION:
 *    This function writes data to a device (well the driver)
 *
 * RETURNS:
 *    e_IOSysIOError_Success -- Data was written.
 *    e_IOSysIOError_GenericIO -- There was a IO error of some sort.
 *    e_IOSysIOError_Disconnect -- The connection or file handle has become
 *                                 disconnected.
 *    e_IOSysIOError_Busy -- The data could not be written at this time.
 *                           Wait a bit and try again.
 *
 * SEE ALSO:
 *    IOS_Open()
 ******************************************************************************/
e_IOSysIOErrorType IOS_WriteData(t_IOSystemHandle *Handle,const uint8_t *Data,
        int Bytes)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;
    e_IOSysIOErrorType RetValue;
    int RetCode;

    if(!DrvHandle->DrvOpen)
        return e_IOSysIOError_Disconnect;

    RetCode=DrvHandle->IOdrv->API.Write(DrvHandle->DriverData,Data,Bytes);
    if(RetCode<0)
    {
        /* Error */
        RetValue=e_IOSysIOError_GenericIO;
        switch(RetCode)
        {
            case RETERROR_DISCONNECT:
                RetValue=e_IOSysIOError_Disconnect;
            break;
            default:
            case RETERROR_IOERROR:
                RetValue=e_IOSysIOError_GenericIO;
            break;
            case RETERROR_BUSY:
                RetValue=e_IOSysIOError_Busy;
            break;
        }
    }
    else
    {
        RetValue=e_IOSysIOError_Success;
    }

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    IOS_ReadData
 *
 * SYNOPSIS:
 *    int IOS_ReadData(t_IOSystemHandle *Handle,uint8_t *Data,int MaxBytes)
 *
 * PARAMETERS:
 *    Handle [I] -- The IO system handle to work on
 *    Data [O] -- Where to place the data we are reading
 *    MaxBytes [I] -- The size of 'Data'.  This is the max number of bytes to
 *                    read before returning.
 *
 * FUNCTION:
 *    This funciton reads data from the driver.  It is no blocking.
 *
 * RETURNS:
 *    The number of bytes read, 0 for no bytes available, and <0 for an error.
 *
 * SEE ALSO:
 *    IOS_WriteData(), IOS_Open()
 ******************************************************************************/
int IOS_ReadData(t_IOSystemHandle *Handle,uint8_t *Data,int MaxBytes)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;

    if(!DrvHandle->DrvOpen)
        return 0;

    return DrvHandle->IOdrv->API.Read(DrvHandle->DriverData,Data,MaxBytes);
}

/*******************************************************************************
 * NAME:
 *    IOS_Close
 *
 * SYNOPSIS:
 *    void IOS_Close(t_IOSystemHandle *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- The IO handle to close
 *
 * FUNCTION:
 *    This function closes an open connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_Open()
 ******************************************************************************/
void IOS_Close(t_IOSystemHandle *Handle)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;

    if(!DrvHandle->DrvOpen)
        return;

    if(DrvHandle->IOdrv->API.Close!=NULL)
        DrvHandle->IOdrv->API.Close(DrvHandle->DriverData);

    DrvHandle->DrvOpen=false;
}

/*******************************************************************************
 * NAME:
 *    IOS_TransmitQueuedData
 *
 * SYNOPSIS:
 *    e_IOSysIOErrorType IOS_TransmitQueuedData(t_IOSystemHandle *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- The IO handle to work on
 *
 * FUNCTION:
 *    This function sends any queued data.  This is really only usefull for
 *    block send devices (as you need to call this to have it send the packet)
 *
 * RETURNS:
 *    The same as IOS_WriteData()
 *
 * SEE ALSO:
 *    IOS_WriteData()
 ******************************************************************************/
e_IOSysIOErrorType IOS_TransmitQueuedData(t_IOSystemHandle *Handle)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;
    int RetCode;
    e_IOSysIOErrorType RetValue;

    if(!DrvHandle->DrvOpen)
        return e_IOSysIOError_Disconnect;

    RetValue=e_IOSysIOError_Success;
    if(DrvHandle->IOdrv->API.Transmit!=NULL)
    {
        RetCode=DrvHandle->IOdrv->API.Transmit(DrvHandle->DriverData);
        if(RetCode<0)
        {
            /* Error */
            RetValue=e_IOSysIOError_GenericIO;
            switch(RetCode)
            {
                case RETERROR_DISCONNECT:
                    RetValue=e_IOSysIOError_Disconnect;
                break;
                default:
                case RETERROR_IOERROR:
                    RetValue=e_IOSysIOError_GenericIO;
                break;
                case RETERROR_BUSY:
                    RetValue=e_IOSysIOError_Busy;
                break;
            }
        }
    }
    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    IOS_SetConnectionOptions
 *
 * SYNOPSIS:
 *    bool IOS_SetConnectionOptions(t_IOSystemHandle *Handle,
 *          const t_KVList &Options);
 *
 * PARAMETERS:
 *    Handle [I] -- The IO handle to work on
 *    Options [I] -- The new options to apply
 *
 * FUNCTION:
 *    This function changes the current options that a connection is using.
 *    If the connection is open then the connection maybe closed and reopened.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    IOS_GetConnectionOptions()
 ******************************************************************************/
bool IOS_SetConnectionOptions(t_IOSystemHandle *Handle,const t_KVList &Options)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;

    DrvHandle->Options=Options;

    if(DrvHandle->DrvOpen && DrvHandle->IOdrv->API.ChangeOptions!=NULL)
    {
        return DrvHandle->IOdrv->API.ChangeOptions(DrvHandle->DriverData,
                PIS_ConvertKVList2PIKVList(DrvHandle->Options));
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    IOS_GetConnectionOptions
 *
 * SYNOPSIS:
 *    void IOS_GetConnectionOptions(t_IOSystemHandle *Handle,
 *          t_KVList &Options);
 *
 * PARAMETERS:
 *    Handle [I] -- The IO handle to work on
 *    Options [O] -- The a copy of the options this IO handle is using.
 *
 * FUNCTION:
 *    This function gets the options that has been applied to a IO handle.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_SetConnectionOptions()
 ******************************************************************************/
void IOS_GetConnectionOptions(t_IOSystemHandle *Handle,t_KVList &Options)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;

    Options=DrvHandle->Options;
}

/*******************************************************************************
 * NAME:
 *    IOS_GetUniqueID
 *
 * SYNOPSIS:
 *    void IOS_GetUniqueID(t_IOSystemHandle *Handle,std::string &UniqueID);
 *
 * PARAMETERS:
 *    Handle [I] -- The IO handle to get the ID for
 *    UniqueID [O] -- The Unique ID for this IO handle.
 *
 * FUNCTION:
 *    This function gets the Unique ID for a IO handle.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void IOS_GetUniqueID(t_IOSystemHandle *Handle,std::string &UniqueID)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;

    UniqueID=DrvHandle->DeviceUniqueID;
}

/*******************************************************************************
 * NAME:
 *    IOS_DrvDataEvent
 *
 * SYNOPSIS:
 *    void IOS_DrvDataEvent(t_IOSystemHandle *IOHandle,int Code);
 *
 * PARAMETERS:
 *    IOHandle [I] -- The IOHandle for this connection
 *    Code [I] -- The event type:
 *                  e_DataEventCode_BytesAvailable -- There is new data
 *                          available to be read.
 *                  e_DataEventCode_Disconnected -- The connection has been
 *                          disconnected (force closed)
 *                  e_DataEventCode_Connected -- The connection has been
 *                          connected and is ready to read/write bytes.
 *                  e_DataEventCode_WriteReady -- The connection is ready
 *                          to accept more bytes to send.
 *
 * FUNCTION:
 *    This function is called from the IO system plugin API to tell the main
 *    system something has happened.
 *
 *    This can be called from a thread as it will queue the event and sent it
 *    to the main thread to be processed.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *      * 1 queue per drive alloc (IOS_AllocIOSystemHandle)
 *      * 1 mutex per drive alloc (IOS_AllocIOSystemHandle)
 *      * When IOS_DrvDataEvent() is called
 *          * Lock mutex
 *          * Add code to queue
 *          * Unlock mutex
 *          * If something added call UI RPC to main thread (with IOHandle)
 *      * When message from thread comes in call fn to handle queue:
 *          * Lock mutex
 *          * Remove code to process
 *          * Unlock mutex
 *          * Process code
 *          * Loop until queue empty
 *      * DATA AVAILABLE get special handing, only one on queue at a time,
 *          if already exists ignore
 *
 * SEE ALSO:
 *    IOS_InformOfNewDataEvent()
 ******************************************************************************/
void IOS_DrvDataEvent(t_IOSystemHandle *IOHandle,int Code)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)IOHandle;
    int NewHead;
    bool SavedDataAvailable;

    /* If we have freed the queue and mutex then don't use whatever this is */
    if(DrvHandle->DataEventMutex==NULL)
        return;

    /* Ok, this maybe called from the a thread so we need to do some magic
       to get it into the main thread (use the UI for that) */

    /* Lock the mutex (to make sure the other does look at anything
       as we change the world) */
    LockMutex(DrvHandle->DataEventMutex);

    /* Bytes Available is special in that we don't queue it as we don't
       want more than 1 of these going at a time */
    if(Code==e_DataEventCode_BytesAvailable)
    {
        SavedDataAvailable=DrvHandle->DataAvailable;
        DrvHandle->DataAvailable=true;
        UnLockMutex(DrvHandle->DataEventMutex);

        if(SavedDataAvailable==false)
        {
            /* Need to inform the other thread about the change */
            FlagDrvDataEvent(IOHandle);
        }

        return;
    }

    NewHead=DrvHandle->DataEventHead+1;
    if(NewHead>=DATAEVENTQUEUE_SIZE)
        NewHead=0;

    if(NewHead==DrvHandle->DataEventTail)
    {
        /* Out of space, fail */
        UnLockMutex(DrvHandle->DataEventMutex);
        return;
    }

    DrvHandle->DataEventQueue[DrvHandle->DataEventHead]=
            (e_DataEventCodeType)Code;
    DrvHandle->DataEventHead=NewHead;

    UnLockMutex(DrvHandle->DataEventMutex);

    FlagDrvDataEvent(IOHandle);
}

/*******************************************************************************
 * NAME:
 *    IOS_InformOfNewDataEvent
 *
 * SYNOPSIS:
 *    void IOS_InformOfNewDataEvent(t_IOSystemHandle *IOHandle);
 *
 * PARAMETERS:
 *    IOHandle [I] -- The connection IO system handle that this event goes with
 *
 * FUNCTION:
 *    This function is called in the main thread to process an event that was
 *    added to the connection event queue.
 *
 *    It will dispatch this event to the rest of the system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    IOS_DrvDataEvent()
 ******************************************************************************/
void IOS_InformOfNewDataEvent(t_IOSystemHandle *IOHandle)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)IOHandle;
    bool done;
    bool DataAvailable;
    e_DataEventCodeType Code;
    i_ActiveHandlesListType ahl;
    bool ReenterNeeded;

    /* Ok, first we need to make sure this handle wasn't free'ed.  We do this
       by looking at a list of active handles.

       This needs to be done because we queue the pointer to the IOHandle.
       This means that if you free the IOHandle, and then we process the
       queue we can get a pointer to free'ed memory.  Normally we would
       just remove the entries from the queue, but we don't control this
       queue so all we can really do is keep a list of valid handles and
       verify that the handle we just took off the queue is still valid */
    for(ahl=m_ActiveHandlesList.begin();ahl!=m_ActiveHandlesList.end();ahl++)
        if(*ahl==IOHandle)
            break;
    if(ahl==m_ActiveHandlesList.end())
    {
        /* We didn't find this handle, it must have been free'ed.  Ignore */
        return;
    }

    /* We are in the main thread here so we process all the queued events
       for this handle */

    ReenterNeeded=false;

    done=false;
    while(!done)
    {
        Code=e_DataEventCodeMAX;

        /* Lock the mutex (to make sure the other does look at anything
           as we change the world) */
        LockMutex(DrvHandle->DataEventMutex);

        DataAvailable=DrvHandle->DataAvailable;
        DrvHandle->DataAvailable=false;

        if(DrvHandle->DataEventTail!=DrvHandle->DataEventHead)
        {
            /* Pull the next event off the queue */
            Code=DrvHandle->DataEventQueue[DrvHandle->DataEventTail];
            DrvHandle->DataEventTail++;
            if(DrvHandle->DataEventTail>=DATAEVENTQUEUE_SIZE)
                DrvHandle->DataEventTail=0;
        }
        UnLockMutex(DrvHandle->DataEventMutex);

        /* Ok, now process what we just pulled off */
        if(DataAvailable)
        {
            ReenterNeeded=Con_InformOfDataAvaiable(DrvHandle->ID);
        }
        switch(Code)
        {
            case e_DataEventCode_BytesAvailable:
                /* Handled with the 'DataAvailable' flag */
            break;
            case e_DataEventCode_Disconnected:
                Con_InformOfDisconnected(DrvHandle->ID);
            break;
            case e_DataEventCode_Connected:
                Con_InformOfConnected(DrvHandle->ID);
            break;
            case e_DataEventCodeMAX:
            default:
                /* Done */
                done=true;
            break;
        }
    }
    if(ReenterNeeded)
    {
        /* Queue another bytes available so we reenter (after processing the
           main event queue) */
        IOS_DrvDataEvent(IOHandle,e_DataEventCode_BytesAvailable);
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_GetDeviceURI
 *
 * SYNOPSIS:
 *    bool IOS_GetDeviceURI(t_IOSystemHandle *Handle,std::string &URI);
 *
 * PARAMETERS:
 *    Handle [I] -- The IO system handle to work on
 *    URI [O] -- The string to place the built URI
 *
 * FUNCTION:
 *    This function asks the drive to build a URI string for this io system
 *    handle based on the device and connection options.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool IOS_GetDeviceURI(t_IOSystemHandle *Handle,std::string &URI)
{
    struct IOSystemDrvHandle *DrvHandle=(struct IOSystemDrvHandle *)Handle;
    char URIBuffer[MAX_URI_LENGTH];
    bool Success;
    string DriverName;
    string DeviceUniqueID;

    try
    {
        IOS_SplitID(DrvHandle->DeviceUniqueID.c_str(),DriverName,
                DeviceUniqueID);

        Success=DrvHandle->IOdrv->API.Convert_Options_To_URI(DeviceUniqueID.c_str(),
                PIS_ConvertKVList2PIKVList(DrvHandle->Options),URIBuffer,
                MAX_URI_LENGTH);

        URI="";
        if(Success)
            URI=URIBuffer;
    }
    catch(...)
    {
        return false;
    }

    return Success;
}

/*******************************************************************************
 * NAME:
 *    IOS_AllocIOSystemHandleFromURI
 *
 * SYNOPSIS:
 *    t_IOSystemHandle *IOS_AllocIOSystemHandleFromURI(const char *URI,
 *          uintptr_t ID);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to use to allocate this IO system handle.
 *    ID [I] -- An ID/ptr to user data.  This will be sent to any higher level
 *              functions.
 *
 * FUNCTION:
 *    This function allocates a handle to the driver for sending bytes in/out.
 *
 * RETURNS:
 *    A handle to the driver or NULL if there was an error.
 *
 * SEE ALSO:
 *    IOS_AllocIOSystemHandle()
 ******************************************************************************/
t_IOSystemHandle *IOS_AllocIOSystemHandleFromURI(const char *URI,uintptr_t ID)
{
    i_IODriverListType drv;
    string UniqueID;
    string BaseURI;
    const char *p;
    t_KVList Options;
    char DeviceUniqueID[MAX_UNIQUE_ID_LEN];
    t_IOSystemHandle *IOHandle;
    string EscString;
    string DeviceUniqueIDStr;

    IOHandle=NULL;
    try
    {
        /* Break the URI down into it's parts */
        p=URI;
        while((*p>='a' && *p<='z') || (*p>='A' && *p<='Z'))
            p++;
        BaseURI.assign(URI,p-URI);
        transform(BaseURI.begin(),BaseURI.end(),BaseURI.begin(),::toupper);

        /* Find the driver for this base URI */
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
            if(drv->BaseURI==BaseURI)
                break;
        /* See if it was found */
        if(drv==m_IODriverList.end())
            throw(0);

        if(!drv->API.Convert_URI_To_Options(URI,
                PIS_ConvertKVList2PIKVList(Options),
                DeviceUniqueID,MAX_UNIQUE_ID_LEN,false))
        {
            throw(0);
        }

        DeviceUniqueIDStr=DeviceUniqueID;
        UniqueID=drv->DriverName;
        UniqueID+="-";
        IOS_EscUniqueID(DeviceUniqueIDStr,EscString);
        UniqueID+=EscString;

        IOHandle=IOS_AllocIOSystemHandle(UniqueID.c_str(),ID);
        if(IOHandle==NULL)
            throw(0);

        /* Set the options */
        if(!IOS_SetConnectionOptions(IOHandle,Options))
            throw(0);
    }
    catch(...)
    {
        if(IOHandle!=NULL)
            IOS_FreeIOSystemHandle(IOHandle);
        IOHandle=NULL;
    }
    return IOHandle;
}

/*******************************************************************************
 * NAME:
 *    IOS_GetUniqueIDFromURI()
 *
 * SYNOPSIS:
 *    bool IOS_GetUniqueIDFromURI(const char *URI,std::string &UniqueID);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to get a UniqueID for
 *    UniqueID [O] -- The Unique ID that goes with this URI
 *
 * FUNCTION:
 *    This function takes a URI and will make a Unique connection ID from this
 *    URI.
 *
 * RETURNS:
 *    true -- We where able to make the Unique ID.
 *    false -- There was something wrong with the URI and we could not make
 *             the Unique ID (maybe the driver is no longer installed?).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool IOS_GetUniqueIDFromURI(const char *URI,std::string &UniqueID)
{
    i_IODriverListType drv;
    string BaseURI;
    const char *p;
    t_KVList Options;
    char DeviceUniqueID[MAX_UNIQUE_ID_LEN];
    string EscString;
    string DeviceUniqueIDStr;

    try
    {
        /* Break the URI down into it's parts */
        p=URI;
        while((*p>='a' && *p<='z') || (*p>='A' && *p<='Z'))
            p++;
        BaseURI.assign(URI,p-URI);
        transform(BaseURI.begin(),BaseURI.end(),BaseURI.begin(),::toupper);

        /* Find the driver for this base URI */
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
            if(drv->BaseURI==BaseURI)
                break;
        /* See if it was found */
        if(drv==m_IODriverList.end())
            throw(0);

        if(!drv->API.Convert_URI_To_Options(URI,
                PIS_ConvertKVList2PIKVList(Options),
                DeviceUniqueID,MAX_UNIQUE_ID_LEN,false))
        {
            throw(0);
        }

        DeviceUniqueIDStr=DeviceUniqueID;
        UniqueID=drv->DriverName;
        UniqueID+="-";
        IOS_EscUniqueID(DeviceUniqueIDStr,EscString);
        UniqueID+=EscString;
    }
    catch(...)
    {
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    IOS_GetURIFromUniqueID()
 *
 * SYNOPSIS:
 *    bool IOS_GetURIFromUniqueID(const char *UniqueID,const t_KVList &Options,
 *          std::string &URI);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The Unique connection ID to build the URI from
 *    Options [I] -- The options that will apply to this URI
 *    URI [O] -- The URI built from 'UniqueID' and 'Options'.
 *
 * FUNCTION:
 *    This function builds a URI string from a unique ID and some options.
 *
 * RETURNS:
 *    true -- The URI was made
 *    false -- There was an error making the URI (maybe missing a driver?)
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool IOS_GetURIFromUniqueID(const char *UniqueID,const t_KVList &Options,
        std::string &URI)
{
    char URIBuffer[MAX_URI_LENGTH];
    bool Success;
    string SearchDriverName;
    string DeviceUniqueID;
    i_IODriverListType drv;

    try
    {
        IOS_SplitID(UniqueID,SearchDriverName,DeviceUniqueID);

        /* Ok, search our drivers to find this one */
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
        {
            if(drv->DriverName==SearchDriverName)
            {
                /* Found it */
                break;
            }
        }
        if(drv==m_IODriverList.end())
        {
            /* Driver not found */
            throw(0);
        }

        Success=drv->API.Convert_Options_To_URI(DeviceUniqueID.c_str(),
                PIS_ConvertKVList2PIKVList(Options),URIBuffer,
                MAX_URI_LENGTH);

        URI="";
        if(Success)
            URI=URIBuffer;
    }
    catch(...)
    {
        Success=false;
    }

    return Success;
}

/*******************************************************************************
 * NAME:
 *    IOS_UpdateOptionsFromURI()
 *
 * SYNOPSIS:
 *    bool IOS_UpdateOptionsFromURI(const char *URI,t_KVList &Options);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to get a UniqueID for
 *    Options [I/O] -- The options for this connection to update
 *
 * FUNCTION:
 *    This function takes a URI and updates the any Options that are also
 *    in the URI.
 *
 * RETURNS:
 *    true -- We where able to update these options.
 *    false -- There was a problem updating the Options (bad URI?).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool IOS_UpdateOptionsFromURI(const char *URI,t_KVList &Options)
{
    i_IODriverListType drv;
    string BaseURI;
    const char *p;
    char DeviceUniqueID[MAX_UNIQUE_ID_LEN];

    try
    {
        /* Break the URI down into it's parts */
        p=URI;
        while((*p>='a' && *p<='z') || (*p>='A' && *p<='Z'))
            p++;
        BaseURI.assign(URI,p-URI);
        transform(BaseURI.begin(),BaseURI.end(),BaseURI.begin(),::toupper);

        /* Find the driver for this base URI */
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
            if(drv->BaseURI==BaseURI)
                break;
        /* See if it was found */
        if(drv==m_IODriverList.end())
            throw(0);

        if(!drv->API.Convert_URI_To_Options(URI,
                PIS_ConvertKVList2PIKVList(Options),
                DeviceUniqueID,MAX_UNIQUE_ID_LEN,true))
        {
            throw(0);
        }
    }
    catch(...)
    {
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    IOS_GetConnectionInfo
 *
 * SYNOPSIS:
 *    bool IOS_GetConnectionInfo(const char *UniqueID,const t_KVList &Options,
 *          struct ConnectionInfoList *Info);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The Unique connection ID to build the URI from
 *    Options [I] -- The options for this connection.
 *    Info [O] -- The structure to fill in with info about this connection.
 *                   Name -- A long version of this connections name.
 *                   Title -- A short version of this connections name.
 *                   InUse -- Is this device open.  This may not work depending
 *                            on the device.
 *                   BlockDevice -- If this is true then that device uses
 *                          blocks of data instead of a stream of data.
 *                          Think UDP vs TCP.
 *
 * FUNCTION:
 *    This function gets info about a connection.
 *
 * RETURNS:
 *    true -- Things worked out and 'Info' has been filled in
 *    false -- There was an error and 'Info' is not valid.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool IOS_GetConnectionInfo(const char *UniqueID,const t_KVList &Options,
        struct ConnectionInfoList *Info)
{
    bool Success;
    string SearchDriverName;
    string DeviceUniqueID;
    i_IODriverListType drv;
    struct IODriverDetectedInfo DriverInfo;

    try
    {
        IOS_SplitID(UniqueID,SearchDriverName,DeviceUniqueID);

        /* Ok, search our drivers to find this one */
        for(drv=m_IODriverList.begin();drv!=m_IODriverList.end();drv++)
        {
            if(drv->DriverName==SearchDriverName)
            {
                /* Found it */
                break;
            }
        }
        if(drv==m_IODriverList.end())
        {
            /* Driver not found */
            throw(0);
        }

        DriverInfo.Next=NULL;
        DriverInfo.StructureSize=sizeof(struct IODriverDetectedInfo);
        snprintf(DriverInfo.DeviceUniqueID,
                sizeof(DriverInfo.DeviceUniqueID),"%s",
                DeviceUniqueID.c_str());

        Success=drv->API.GetConnectionInfo(DeviceUniqueID.c_str(),
                PIS_ConvertKVList2PIKVList(Options),&DriverInfo);
        Info->UniqueID=UniqueID;
        Info->Name=DriverInfo.Name;
        Info->Title=DriverInfo.Title;
        Info->InUse=DriverInfo.Flags&IODRV_DETECTFLAG_INUSE;
        Info->BlockDevice=drv->Info.Flags&IODRVINFOFLAG_BLOCKDEV;
        Info->Next=NULL;
    }
    catch(...)
    {
        Success=false;
    }

    return Success;
}

/*******************************************************************************
 * NAME:
 *    IOS_AddComboBox
 *
 * SYNOPSIS:
 *    struct PI_ComboBox *IOS_AddComboBox(
 *              t_WidgetSysHandle *WidgetHandle,PG_BOOL UserEditable,
 *              const char *Label,void (*EventCB)(const struct PICBEvent *Event,
 *              void *UserData),void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UserEditable [I] -- Can the user type new strings into this box
 *    Label [I] -- The label to apply to this box
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is a version of add combox for the IO system.  It connects into
 *    the event system so we know when a combox is changed.
 *
 * RETURNS:
 *    The combox box handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static struct PI_ComboBox *IOS_AddComboBox(t_WidgetSysHandle *WidgetHandle,
        PG_BOOL UserEditable,const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct PI_ComboBox *NewComboBox;

    NewComboBox=NULL;
    ExtraData=NULL;
    try
    {
        ExtraData=(struct COD_WidgetData *)
                malloc(sizeof(struct COD_WidgetData));
        memset(ExtraData,0x00,sizeof(struct COD_WidgetData));
        ExtraData->EventCB=EventCB;
        ExtraData->UserData=UserData;
        ExtraData->ConOptionData=ConOptionData;

        NewComboBox=UIPI_AddComboBox(ConOptionData->ContainerWidget,
                UserEditable,Label,IOS_ComboBoxEventHandler,ExtraData);
        if(NewComboBox==NULL)
            throw(0);
        ExtraData->ComboBox=NewComboBox;

        /* Link in */
        ExtraData->Next=ConOptionData->WidgetExtraData;
        ConOptionData->WidgetExtraData=ExtraData;
    }
    catch(...)
    {
        if(NewComboBox!=NULL)
            UIPI_FreeComboBox(NewComboBox);
        if(ExtraData==NULL)
            free(ExtraData);
        NewComboBox=NULL;
    }
    return NewComboBox;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeComboBox
 *
 * SYNOPSIS:
 *    static void IOS_FreeComboBox(t_WidgetSysHandle *WidgetHandle,
 *          struct PI_ComboBox *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UICtrl [I] -- The combox box to free.
 *
 * FUNCTION:
 *    This function frees a combox box allocated with the IO system version
 *    of add combox box.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_FreeComboBox(t_WidgetSysHandle *WidgetHandle,
        struct PI_ComboBox *UICtrl)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct COD_WidgetData *PrevExtraData;

    /* Find and unlink (and free) this combox box */
    PrevExtraData=NULL;
    for(ExtraData=ConOptionData->WidgetExtraData;ExtraData!=NULL;
            ExtraData=ExtraData->Next)
    {
        if(ExtraData->ComboBox==UICtrl)
        {
            /* Found it, unlink and free */
            if(PrevExtraData==NULL)
                ConOptionData->WidgetExtraData=ExtraData->Next;
            else
                PrevExtraData->Next=ExtraData->Next;

            free(ExtraData);
            break;
        }
        PrevExtraData=ExtraData;
    }

    UIPI_FreeComboBox(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    IOS_ComboBoxEventHandler
 *
 * SYNOPSIS:
 *    static void IOS_ComboBoxEventHandler(const struct PICBEvent *Event,
 *              void *UserData);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here
 *    UserData [I] -- A handle to the 'struct COD_WidgetData'
 *                      for this combox box.
 *
 * FUNCTION:
 *    This function is called when there is an event from the UI.  We use
 *    it to flag the upper level that something has changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_ComboBoxEventHandler(const struct PICBEvent *Event,
        void *UserData)
{
    struct COD_WidgetData *ExtraData=(struct COD_WidgetData *)UserData;

    /* Handle the user provided event handler */
    if(ExtraData->EventCB!=NULL)
        ExtraData->EventCB(Event,ExtraData->UserData);

    if(ExtraData->ConOptionData->UIChangedCB!=NULL)
    {
        ExtraData->ConOptionData->UIChangedCB(ExtraData->ConOptionData->
                UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_AddTextInput
 *
 * SYNOPSIS:
 *    struct PI_TextInput *IOS_AddTextInput(t_WidgetSysHandle *WidgetHandle,
 *          const char *Label,void (*EventCB)(const struct PICBEvent *Event,
 *          void *UserData),void *UserData)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    Label [I] -- The label to apply to this input
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is a version of add text input for the IO system.  It connects into
 *    the event system so we know when a text input is changed.
 *
 * RETURNS:
 *    The text input handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct PI_TextInput *IOS_AddTextInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct PI_TextInput *NewTextInput;

    NewTextInput=NULL;
    ExtraData=NULL;
    try
    {
        ExtraData=(struct COD_WidgetData *)
                malloc(sizeof(struct COD_WidgetData));
        memset(ExtraData,0x00,sizeof(struct COD_WidgetData));
        ExtraData->EventCB=EventCB;
        ExtraData->UserData=UserData;
        ExtraData->ConOptionData=ConOptionData;

        NewTextInput=UIPI_AddTextInput(ConOptionData->ContainerWidget,
                Label,IOS_TextInputEventHandler,ExtraData);
        if(NewTextInput==NULL)
            throw(0);
        ExtraData->TextInput=NewTextInput;

        /* Link in */
        ExtraData->Next=ConOptionData->WidgetExtraData;
        ConOptionData->WidgetExtraData=ExtraData;
    }
    catch(...)
    {
        if(NewTextInput!=NULL)
            UIPI_FreeTextInput(NewTextInput);
        if(ExtraData==NULL)
            free(ExtraData);
        NewTextInput=NULL;
    }
    return NewTextInput;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeTextInput
 *
 * SYNOPSIS:
 *    static void IOS_FreeTextInput(t_WidgetSysHandle *WidgetHandle,
 *          struct PI_TextInput *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UICtrl [I] -- The text input to free.
 *
 * FUNCTION:
 *    This function frees a text input allocated with the IO system version
 *    of add text input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void IOS_FreeTextInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_TextInput *UICtrl)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct COD_WidgetData *PrevExtraData;

    /* Find and unlink (and free) this combox box */
    PrevExtraData=NULL;
    for(ExtraData=ConOptionData->WidgetExtraData;ExtraData!=NULL;
            ExtraData=ExtraData->Next)
    {
        if(ExtraData->TextInput==UICtrl)
        {
            /* Found it, unlink and free */
            if(PrevExtraData==NULL)
                ConOptionData->WidgetExtraData=ExtraData->Next;
            else
                PrevExtraData->Next=ExtraData->Next;

            free(ExtraData);

            break;
        }
        PrevExtraData=ExtraData;
    }

    UIPI_FreeTextInput(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    IOS_TextInputEventHandler
 *
 * SYNOPSIS:
 *    static void IOS_TextInputEventHandler(const struct PICBEvent *Event,
 *              void *UserData);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here
 *    UserData [I] -- A handle to the 'struct COD_WidgetData'
 *                      for this combox box.
 *
 * FUNCTION:
 *    This function is called when there is an event from the UI.  We use
 *    it to flag the upper level that something has changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_TextInputEventHandler(const struct PICBEvent *Event,
        void *UserData)
{
    struct COD_WidgetData *ExtraData=(struct COD_WidgetData *)UserData;

    /* Handle the user provided event handler */
    if(ExtraData->EventCB!=NULL)
        ExtraData->EventCB(Event,ExtraData->UserData);

    if(ExtraData->ConOptionData->UIChangedCB!=NULL)
    {
        ExtraData->ConOptionData->UIChangedCB(ExtraData->ConOptionData->
                UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_AddNumberInput
 *
 * SYNOPSIS:
 *    struct PI_NumberInput *IOS_AddNumberInput(t_WidgetSysHandle *WidgetHandle,
 *          const char *Label,void (*EventCB)(const struct PICBEvent *Event,
 *          void *UserData),void *UserData)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    Label [I] -- The label to apply to this input
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is a version of add a number input for the IO system.  It connects
 *    into the event system so we know when a number input has changed.
 *
 * RETURNS:
 *    The number input handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct PI_NumberInput *IOS_AddNumberInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct PI_NumberInput *NewNumberInput;

    NewNumberInput=NULL;
    ExtraData=NULL;
    try
    {
        ExtraData=(struct COD_WidgetData *)
                malloc(sizeof(struct COD_WidgetData));
        memset(ExtraData,0x00,sizeof(struct COD_WidgetData));
        ExtraData->EventCB=EventCB;
        ExtraData->UserData=UserData;
        ExtraData->ConOptionData=ConOptionData;

        NewNumberInput=UIPI_AddNumberInput(ConOptionData->ContainerWidget,
                Label,IOS_NumberInputEventHandler,ExtraData);
        if(NewNumberInput==NULL)
            throw(0);
        ExtraData->NumberInput=NewNumberInput;

        /* Link in */
        ExtraData->Next=ConOptionData->WidgetExtraData;
        ConOptionData->WidgetExtraData=ExtraData;
    }
    catch(...)
    {
        if(NewNumberInput!=NULL)
            UIPI_FreeNumberInput(NewNumberInput);
        if(ExtraData==NULL)
            free(ExtraData);
        NewNumberInput=NULL;
    }
    return NewNumberInput;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeNumberInput
 *
 * SYNOPSIS:
 *    static void IOS_FreeNumberInput(t_WidgetSysHandle *WidgetHandle,
 *          struct PI_NumberInput *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UICtrl [I] -- The text input to free.
 *
 * FUNCTION:
 *    This function frees a text input allocated with the IO system version
 *    of add text input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void IOS_FreeNumberInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_NumberInput *UICtrl)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct COD_WidgetData *PrevExtraData;

    /* Find and unlink (and free) this combox box */
    PrevExtraData=NULL;
    for(ExtraData=ConOptionData->WidgetExtraData;ExtraData!=NULL;
            ExtraData=ExtraData->Next)
    {
        if(ExtraData->NumberInput==UICtrl)
        {
            /* Found it, unlink and free */
            if(PrevExtraData==NULL)
                ConOptionData->WidgetExtraData=ExtraData->Next;
            else
                PrevExtraData->Next=ExtraData->Next;

            free(ExtraData);

            break;
        }
        PrevExtraData=ExtraData;
    }

    UIPI_FreeNumberInput(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    IOS_NumberInputEventHandler
 *
 * SYNOPSIS:
 *    static void IOS_NumberInputEventHandler(const struct PICBEvent *Event,
 *              void *UserData);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here
 *    UserData [I] -- A handle to the 'struct COD_WidgetData'
 *                      for this combox box.
 *
 * FUNCTION:
 *    This function is called when there is an event from the UI.  We use
 *    it to flag the upper level that something has changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_NumberInputEventHandler(const struct PICBEvent *Event,
        void *UserData)
{
    struct COD_WidgetData *ExtraData=(struct COD_WidgetData *)UserData;

    /* Handle the user provided event handler */
    if(ExtraData->EventCB!=NULL)
        ExtraData->EventCB(Event,ExtraData->UserData);

    if(ExtraData->ConOptionData->UIChangedCB!=NULL)
    {
        ExtraData->ConOptionData->UIChangedCB(ExtraData->ConOptionData->
                UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_AddDoubleInput
 *
 * SYNOPSIS:
 *    struct PI_DoubleInput *IOS_AddDoubleInput(t_WidgetSysHandle *WidgetHandle,
 *          const char *Label,void (*EventCB)(const struct PICBEvent *Event,
 *          void *UserData),void *UserData)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    Label [I] -- The label to apply to this input
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is a version of add a number input for the IO system.  It connects
 *    into the event system so we know when a number input has changed.
 *
 * RETURNS:
 *    The number input handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct PI_DoubleInput *IOS_AddDoubleInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct PI_DoubleInput *NewDoubleInput;

    NewDoubleInput=NULL;
    ExtraData=NULL;
    try
    {
        ExtraData=(struct COD_WidgetData *)
                malloc(sizeof(struct COD_WidgetData));
        memset(ExtraData,0x00,sizeof(struct COD_WidgetData));
        ExtraData->EventCB=EventCB;
        ExtraData->UserData=UserData;
        ExtraData->ConOptionData=ConOptionData;

        NewDoubleInput=UIPI_AddDoubleInput(ConOptionData->ContainerWidget,
                Label,IOS_DoubleInputEventHandler,ExtraData);
        if(NewDoubleInput==NULL)
            throw(0);
        ExtraData->DoubleInput=NewDoubleInput;

        /* Link in */
        ExtraData->Next=ConOptionData->WidgetExtraData;
        ConOptionData->WidgetExtraData=ExtraData;
    }
    catch(...)
    {
        if(NewDoubleInput!=NULL)
            UIPI_FreeDoubleInput(NewDoubleInput);
        if(ExtraData==NULL)
            free(ExtraData);
        NewDoubleInput=NULL;
    }
    return NewDoubleInput;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeDoubleInput
 *
 * SYNOPSIS:
 *    static void IOS_FreeDoubleInput(t_WidgetSysHandle *WidgetHandle,
 *          struct PI_DoubleInput *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UICtrl [I] -- The text input to free.
 *
 * FUNCTION:
 *    This function frees a text input allocated with the IO system version
 *    of add text input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void IOS_FreeDoubleInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_DoubleInput *UICtrl)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct COD_WidgetData *PrevExtraData;

    /* Find and unlink (and free) this combox box */
    PrevExtraData=NULL;
    for(ExtraData=ConOptionData->WidgetExtraData;ExtraData!=NULL;
            ExtraData=ExtraData->Next)
    {
        if(ExtraData->DoubleInput==UICtrl)
        {
            /* Found it, unlink and free */
            if(PrevExtraData==NULL)
                ConOptionData->WidgetExtraData=ExtraData->Next;
            else
                PrevExtraData->Next=ExtraData->Next;

            free(ExtraData);

            break;
        }
        PrevExtraData=ExtraData;
    }

    UIPI_FreeDoubleInput(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    IOS_DoubleInputEventHandler
 *
 * SYNOPSIS:
 *    static void IOS_DoubleInputEventHandler(const struct PICBEvent *Event,
 *              void *UserData);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here
 *    UserData [I] -- A handle to the 'struct COD_WidgetData'
 *                      for this combox box.
 *
 * FUNCTION:
 *    This function is called when there is an event from the UI.  We use
 *    it to flag the upper level that something has changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_DoubleInputEventHandler(const struct PICBEvent *Event,
        void *UserData)
{
    struct COD_WidgetData *ExtraData=(struct COD_WidgetData *)UserData;

    /* Handle the user provided event handler */
    if(ExtraData->EventCB!=NULL)
        ExtraData->EventCB(Event,ExtraData->UserData);

    if(ExtraData->ConOptionData->UIChangedCB!=NULL)
    {
        ExtraData->ConOptionData->UIChangedCB(ExtraData->ConOptionData->
                UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_AddCheckboxInput
 *
 * SYNOPSIS:
 *    struct PI_Checkbox *IOS_AddCheckboxInput(t_WidgetSysHandle *WidgetHandle,
 *          const char *Label,
 *          void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),
 *          void *UserData)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    Label [I] -- The label to apply to this input
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is a version of add a checkbox input for the IO system.  It connects
 *    into the event system so we know when a number input has changed.
 *
 * RETURNS:
 *    The checkbox input handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static struct PI_Checkbox *IOS_AddCheckboxInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),
        void *UserData)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct PI_Checkbox *NewCheckboxInput;

    NewCheckboxInput=NULL;
    ExtraData=NULL;
    try
    {
        ExtraData=(struct COD_WidgetData *)
                malloc(sizeof(struct COD_WidgetData));
        memset(ExtraData,0x00,sizeof(struct COD_WidgetData));
        ExtraData->CheckboxEventCB=EventCB;
        ExtraData->UserData=UserData;
        ExtraData->ConOptionData=ConOptionData;

        NewCheckboxInput=UIPI_AddCheckbox(ConOptionData->ContainerWidget,
                Label,IOS_CheckboxInputEventHandler,ExtraData);
        if(NewCheckboxInput==NULL)
            throw(0);
        ExtraData->CheckboxInput=NewCheckboxInput;

        /* Link in */
        ExtraData->Next=ConOptionData->WidgetExtraData;
        ConOptionData->WidgetExtraData=ExtraData;
    }
    catch(...)
    {
        if(NewCheckboxInput!=NULL)
            UIPI_FreeCheckbox(NewCheckboxInput);
        if(ExtraData==NULL)
            free(ExtraData);
        NewCheckboxInput=NULL;
    }
    return NewCheckboxInput;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeCheckboxInput
 *
 * SYNOPSIS:
 *    static void IOS_FreeCheckboxInput(t_WidgetSysHandle *WidgetHandle,
 *          struct PI_Checkbox *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UICtrl [I] -- The text input to free.
 *
 * FUNCTION:
 *    This function frees a text input allocated with the IO system version
 *    of add text input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_FreeCheckboxInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_Checkbox *UICtrl)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct COD_WidgetData *PrevExtraData;

    /* Find and unlink (and free) this combox box */
    PrevExtraData=NULL;
    for(ExtraData=ConOptionData->WidgetExtraData;ExtraData!=NULL;
            ExtraData=ExtraData->Next)
    {
        if(ExtraData->CheckboxInput==UICtrl)
        {
            /* Found it, unlink and free */
            if(PrevExtraData==NULL)
                ConOptionData->WidgetExtraData=ExtraData->Next;
            else
                PrevExtraData->Next=ExtraData->Next;

            free(ExtraData);

            break;
        }
        PrevExtraData=ExtraData;
    }

    UIPI_FreeCheckbox(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    IOS_CheckboxInputEventHandler
 *
 * SYNOPSIS:
 *    static void IOS_CheckboxInputEventHandler(const struct PICheckboxEvent *Event,
 *              void *UserData);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here
 *    UserData [I] -- A handle to the 'struct COD_WidgetData'
 *                      for this combox box.
 *
 * FUNCTION:
 *    This function is called when there is an event from the UI.  We use
 *    it to flag the upper level that something has changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_CheckboxInputEventHandler(const struct PICheckboxEvent *Event,
        void *UserData)
{
    struct COD_WidgetData *ExtraData=(struct COD_WidgetData *)UserData;

    /* Handle the user provided event handler */
    if(ExtraData->CheckboxEventCB!=NULL)
        ExtraData->CheckboxEventCB(Event,ExtraData->UserData);

    if(ExtraData->ConOptionData->UIChangedCB!=NULL)
    {
        ExtraData->ConOptionData->UIChangedCB(ExtraData->ConOptionData->
                UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    IOS_AllocRadioBttnGroup
 *
 * SYNOPSIS:
 *    static t_PI_RadioBttnGroup *IOS_AllocRadioBttnGroup(
 *          t_WidgetSysHandle *WidgetHandle,const char *Label);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    Label [I] -- The lable for this group
 *
 * FUNCTION:
 *    This function allocates a group box for radio buttons.  Radio buttons
 *    can only live in a group box.
 *
 * RETURNS:
 *    A handle to the group that you can put radio buttons into.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static t_PI_RadioBttnGroup *IOS_AllocRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
        const char *Label)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    t_PI_RadioBttnGroup *NewRadioBttnGroup;

    NewRadioBttnGroup=NULL;
    try
    {
        NewRadioBttnGroup=
                UIPI_AllocRadioBttnGroup(ConOptionData->ContainerWidget,Label);
        if(NewRadioBttnGroup==NULL)
            throw(0);
    }
    catch(...)
    {
        NewRadioBttnGroup=NULL;
    }
    return NewRadioBttnGroup;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeRadioBttnGroup
 *
 * SYNOPSIS:
 *    static void IOS_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
 *              t_PI_RadioBttnGroup *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UICtrl [I] -- The widget to free.
 *
 * FUNCTION:
 *    This function frees a group box.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
        t_PI_RadioBttnGroup *UICtrl)
{
    UIPI_FreeRadioBttnGroup(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    IOS_AddRadioBttnInput
 *
 * SYNOPSIS:
 *    struct PI_RadioBttn *IOS_AddRadioBttnInput(t_WidgetSysHandle *WidgetHandle,
 *          t_PI_RadioBttnGroup *RBGroup,const char *Label,
 *          void (*EventCB)(const struct PIRBEvent *Event,void *UserData),
 *          void *UserData)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    RBGroup [I] -- The group that this radio button goes with
 *    Label [I] -- The label to apply to this input
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is a version of add a checkbox input for the IO system.  It connects
 *    into the event system so we know when a number input has changed.
 *
 * RETURNS:
 *    The checkbox input handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static struct PI_RadioBttn *IOS_AddRadioBttnInput(t_WidgetSysHandle *WidgetHandle,
        t_PI_RadioBttnGroup *RBGroup,const char *Label,
        void (*EventCB)(const struct PIRBEvent *Event,void *UserData),
        void *UserData)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct PI_RadioBttn *NewRadioBttnInput;

    NewRadioBttnInput=NULL;
    ExtraData=NULL;
    try
    {
        ExtraData=(struct COD_WidgetData *)
                malloc(sizeof(struct COD_WidgetData));
        memset(ExtraData,0x00,sizeof(struct COD_WidgetData));
        ExtraData->RadioBttnEventCB=EventCB;
        ExtraData->UserData=UserData;
        ExtraData->ConOptionData=ConOptionData;

        NewRadioBttnInput=UIPI_AddRadioBttn(RBGroup,Label,
                IOS_RadioBttnInputEventHandler,ExtraData);
        if(NewRadioBttnInput==NULL)
            throw(0);
        ExtraData->RadioBttnInput=NewRadioBttnInput;

        /* Link in */
        ExtraData->Next=ConOptionData->WidgetExtraData;
        ConOptionData->WidgetExtraData=ExtraData;
    }
    catch(...)
    {
        if(NewRadioBttnInput!=NULL)
            UIPI_FreeRadioBttn(NewRadioBttnInput);
        if(ExtraData==NULL)
            free(ExtraData);
        NewRadioBttnInput=NULL;
    }
    return NewRadioBttnInput;
}

/*******************************************************************************
 * NAME:
 *    IOS_FreeRadioBttnInput
 *
 * SYNOPSIS:
 *    static void IOS_FreeRadioBttnInput(t_WidgetSysHandle *WidgetHandle,
 *          struct PI_RadioBttn *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UICtrl [I] -- The text input to free.
 *
 * FUNCTION:
 *    This function frees a text input allocated with the IO system version
 *    of add text input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_FreeRadioBttnInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_RadioBttn *UICtrl)
{
    struct ConnectionOptionsData *ConOptionData=(struct ConnectionOptionsData *)WidgetHandle;
    struct COD_WidgetData *ExtraData;
    struct COD_WidgetData *PrevExtraData;

    /* Find and unlink (and free) this combox box */
    PrevExtraData=NULL;
    for(ExtraData=ConOptionData->WidgetExtraData;ExtraData!=NULL;
            ExtraData=ExtraData->Next)
    {
        if(ExtraData->RadioBttnInput==UICtrl)
        {
            /* Found it, unlink and free */
            if(PrevExtraData==NULL)
                ConOptionData->WidgetExtraData=ExtraData->Next;
            else
                PrevExtraData->Next=ExtraData->Next;

            free(ExtraData);

            break;
        }
        PrevExtraData=ExtraData;
    }

    UIPI_FreeRadioBttn(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    IOS_RadioBttnInputEventHandler
 *
 * SYNOPSIS:
 *    static void IOS_RadioBttnInputEventHandler(const struct PIRBEvent *Event,
 *              void *UserData);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here
 *    UserData [I] -- A handle to the 'struct COD_WidgetData'
 *                      for this combox box.
 *
 * FUNCTION:
 *    This function is called when there is an event from the UI.  We use
 *    it to flag the upper level that something has changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void IOS_RadioBttnInputEventHandler(const struct PIRBEvent *Event,
        void *UserData)
{
    struct COD_WidgetData *ExtraData=(struct COD_WidgetData *)UserData;

    /* Handle the user provided event handler */
    if(ExtraData->RadioBttnEventCB!=NULL)
        ExtraData->RadioBttnEventCB(Event,ExtraData->UserData);

    if(ExtraData->ConOptionData->UIChangedCB!=NULL)
    {
        ExtraData->ConOptionData->UIChangedCB(ExtraData->ConOptionData->
                UserData);
    }
}

