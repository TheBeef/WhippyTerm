/*******************************************************************************
 * FILENAME: MainApp.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the main entry for the main application
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "MainApp.h"
#include "App/Bookmarks.h"
#include "App/Commands.h"
#include "App/MainWindow.h"
#include "App/Session.h"
#include "App/Settings.h"
#include "App/IOSystem.h"
#include "App/DataProcessorsSystem.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/StdPlugins/RegisterStdPlugins.h"
#include "App/SendBuffer.h"
#include "App/Util/CRCSystem.h"
#include "App/Dialogs/Dialog_EditSendBuffer.h"
#include "OS/System.h"
#include "UI/UIAsk.h"
#include "PluginSupport/ExternPluginsSystem.h"
#include "Version.h"

/* Temp external plugins */
//#include "App/PluginSupport/SystemSupport.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    AppMain
 *
 * SYNOPSIS:
 *    void AppMain(int argc,char argv[]);
 *
 * PARAMETERS:
 *    argc [I] -- The argument count
 *    argv [I] -- The arguments
 *
 * FUNCTION:
 *    This is the main entry point for the main app.  This will called from
 *    real main from the UI (whatever that is).  This is called before
 *    the main window is open.
 *
 * RETURNS:
 *    true -- All ok, finish startup
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool AppMain(int argc,char *argv[])
{
    /* Debug checks */
    CmdCheck();

    InitOS();

#if OFFICIAL_RELEASE!=1
    UIAsk("This is a developer build.  The version number can not be trusted.\n\n"
            "Look at build date in ABOUT to differentiate between builds.\n\n"
            "Goto WhippyTerm.com for an official version.");
#endif

    /* Startup code */
    InitSessionSystem();
    InitBookmarks();

    LoadSettings();
    LoadSession();
    LoadBookmarks();
    g_SendBuffers.LoadBuffers();

    IOS_Init();
    DPS_Init();
    FTPS_Init();

//    RegisterBuildInInputProcessors();
//    RegisterBuildInKeyPressProcessors();

    RegisterStdPlugins();
    RegisterExternPlugins();

    IOS_InitPlugins();

    MW_AllocNewMainWindow();

    ApplySettings();

    return true;
}

/*******************************************************************************
 * NAME:
 *    AppShutdown
 *
 * SYNOPSIS:
 *    void AppShutdown(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the app is starting to shutdown.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
void AppShutdown(void)
{
    SaveSessionIfNeeded();

    CRC_ShutDown();
    IOS_Shutdown();

    FreeLoadedExternPlugins();
}

/*******************************************************************************
 * NAME:
 *    App1SecTick
 *
 * SYNOPSIS:
 *    void App1SecTick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called about every second.  It is used to run background
 *    items.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
void App1SecTick(void)
{
    AutoSaveSessionTick();
}

/*******************************************************************************
 * NAME:
 *    AppInformOf_FileTransTimerTick
 *
 * SYNOPSIS:
 *    void AppInformOf_FileTransTimerTick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called from the file trans timer.  It sends a timer
 *    tick out to all the connections to run their download / uploads.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void AppInformOf_FileTransTimerTick(void)
{
    Con_FileTransTick();
}
