/*******************************************************************************
 * FILENAME: TestFile.cpp
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
#include "PluginSDK/Plugin.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      TestFile // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x000B0000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL TestFile_Init(void);
const struct IODriverInfo *TestFile_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *TestFile_DetectDevices(void);
void TestFile_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
t_DriverIOHandleType *TestFile_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle);
PG_BOOL TestFile_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update);
PG_BOOL TestFile_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen);
PG_BOOL TestFile_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
void TestFile_Close(t_DriverIOHandleType *DriverIO);
void TestFile_FreeHandle(t_DriverIOHandleType *DriverIO);
int TestFile_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
int TestFile_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes);
PG_BOOL TestFile_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);

/*** VARIABLE DEFINITIONS     ***/
const struct IOS_API *m_TestFile_IOSAPI;
const struct PI_UIAPI *m_TestFile_UIAPI;
const struct PI_SystemAPI *m_TestFile_SysAPI;
const struct IODriverAPI g_TestFilePluginAPI=
{
    TestFile_Init,
    TestFile_GetDriverInfo,
    NULL,                                               // InstallPlugin
    NULL,                                               // UnInstallPlugin
    TestFile_DetectDevices,
    TestFile_FreeDetectedDevices,
    TestFile_GetConnectionInfo,
    NULL,                                       // ConnectionOptionsWidgets_AllocWidgets
    NULL,                                       // ConnectionOptionsWidgets_FreeWidgets
    NULL,                                       // ConnectionOptionsWidgets_StoreUI
    NULL,                                       // ConnectionOptionsWidgets_UpdateUI
    TestFile_Convert_URI_To_Options,
    TestFile_Convert_Options_To_URI,
    TestFile_AllocateHandle,
    TestFile_FreeHandle,
    TestFile_Open,
    TestFile_Close,
    TestFile_Read,
    TestFile_Write,
    NULL,                                       // ChangeOptions
    NULL,                                       // Transmit
};

struct IODriverInfo m_TestFileInfo=
{
    0
};

static const struct IODriverDetectedInfo g_TestFile_DeviceInfo=
{
    NULL,
    sizeof(struct IODriverDetectedInfo),
    0,                      // Flags
    "1",                    // DeviceUniqueID
    "File Stream",          // Name
    "FileStream",           // Title
};

/*******************************************************************************
 * NAME:
 *    TestFile_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int TestFile_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        m_TestFile_SysAPI=SysAPI;
        m_TestFile_IOSAPI=SysAPI->GetAPI_IO();
        m_TestFile_UIAPI=m_TestFile_IOSAPI->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(m_TestFile_SysAPI->GetExperimentalID()>0 &&
                m_TestFile_SysAPI->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_TestFile_IOSAPI->RegisterDriver("TestFile","FILE",
                &g_TestFilePluginAPI,sizeof(g_TestFilePluginAPI));

        return 0;
    }
}

PG_BOOL TestFile_Init(void)
{
    return true;
}

/*******************************************************************************
 * NAME:
 *   TestFile_GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*TestFile_GetDriverInfo)(
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
const struct IODriverInfo *TestFile_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_TestFileInfo;
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
const struct IODriverDetectedInfo *TestFile_DetectDevices(void)
{
    return &g_TestFile_DeviceInfo;
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
void TestFile_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    /* Does nothing */
}

struct TestFileData
{
    FILE *in;
    t_IOSystemHandle *IOHandle;
};

t_DriverIOHandleType *TestFile_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct TestFileData *OurData;

    OurData=(struct TestFileData *)malloc(sizeof(struct TestFileData));
    if(OurData==NULL)
        return NULL;

    OurData->IOHandle=IOHandle;
    OurData->in=NULL;

    return (t_DriverIOHandleType *)OurData;
}

void TestFile_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct TestFileData *OurData=(struct TestFileData *)DriverIO;

    if(OurData->in!=NULL)
        fclose(OurData->in);

    free(OurData);
}

PG_BOOL TestFile_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct TestFileData *OurData=(struct TestFileData *)DriverIO;

    OurData->in=fopen("Testfile.bin","rb");
    if(OurData->in==NULL)
        return false;
    m_TestFile_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);
    m_TestFile_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_BytesAvailable);

    return true;
}

int TestFile_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
//    struct TestFileData *OurData=(struct TestFileData *)DriverIO;

    return 0;
}

int TestFile_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int MaxBytes)
{
    struct TestFileData *OurData=(struct TestFileData *)DriverIO;
    int BytesRead;

    if(OurData->in==NULL)
        return -1;
    BytesRead=fread(Data,1,MaxBytes,OurData->in);
    if(BytesRead<MaxBytes)
    {
        fclose(OurData->in);
        OurData->in=NULL;
        m_TestFile_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Disconnected);
    }
    else
    {
        /* We always have more data */
        m_TestFile_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_BytesAvailable);
    }
    return BytesRead;
}

void TestFile_Close(t_DriverIOHandleType *DriverIO)
{
    struct TestFileData *OurData=(struct TestFileData *)DriverIO;

    fclose(OurData->in);
    OurData->in=NULL;
}

PG_BOOL TestFile_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    return false;
}

PG_BOOL TestFile_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    return false;
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
PG_BOOL TestFile_GetConnectionInfo(const char *DeviceUniqueID,
        t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    *RetInfo=g_TestFile_DeviceInfo;

    return true;
}
