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
#include "App/DataProcessorsSystem.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/MainWindow.h"
#include "App/Portable.h"
#include "App/Session.h"
#include "App/Settings.h"
#include "App/IOSystem.h"
#include "App/SendBuffer.h"
#include "App/Util/CRCSystem.h"
#include "App/Dialogs/Dialog_EditSendBuffer.h"
#include "OS/System.h"
#include "UI/UIAsk.h"
#include "UI/UISystem.h"
#include "App/PluginSupport/PluginSystem.h"
#include "Version.h"
#include <stdint.h>
#include <string>
#include <list>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
bool g_CLI_FirstWindowOpen=false;
//bool g_CLI_URIOpened=false;
//bool g_CLI_BookmarksOpened=false;
bool g_AppShuttingDown=false;
t_CLIArgList g_CLI_URIList;
t_CLIArgList g_CLI_BookmarkList;

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
    int_fast32_t r;

    g_AppShuttingDown=false;

    /* Code checks (make sure enum's are correct) */
    CmdCheck();

    /* Handle command line args */
    if(argc>1)
    {
        /* Handle options first */
        for(r=1;r<argc;r++)
        {
            if(argv[r][0]=='-')
            {
                /* It's an option */
                if(caseinsensitivestrcmp(argv[r],"--bookmark")==0)
                {
                    /* Next arg is a bookmark name */
                    r++;
                    if(r==argc)
                    {
                        UIAsk("Error missing arg after bookmark option");
                        return false;
                    }
                    g_CLI_BookmarkList.push_back(argv[r]);
                }
            }
            else
            {
                /* Must be a URI then */
                g_CLI_URIList.push_back(argv[r]);
            }
        }
    }

    InitOS();

#if OFFICIAL_RELEASE!=1
    UIAsk("This is a developer build.  The version number can not be trusted.\n\n"
            "Look at build date in ABOUT to differentiate between builds.\n\n"
            "Goto WhippyTerm.com for an official version.");
#endif

    InitPortableSystem();

    /* Startup code */
    InitSessionSystem();
    InitBookmarks();
    g_SendBuffers.Init();

    LoadSettings();
    LoadSession();
    LoadBookmarks();
    g_SendBuffers.LoadBuffers();

    IOS_Init();
    DPS_Init();
    FTPS_Init();

    InitPluginSystem();

    MW_AllocNewMainWindow();

    ApplySettings();

//    /* Open any URI's that where requested */
//    for(a=URIList.begin();a!=URIList.begin();a++)
//    {
//        
//    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    StartAppShutDown
 *
 * SYNOPSIS:
 *    void StartAppShutDown(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to start the shut down of the app.  It mostly
 *    just sets a flag that the system uses to know that shutdown is pending.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void StartAppShutDown(void)
{
    SaveSession();  // Always save the session data

    g_AppShuttingDown=true;
}

/*******************************************************************************
 * NAME:
 *    FinishAppShutdown
 *
 * SYNOPSIS:
 *    void FinishAppShutdown(void);
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
void FinishAppShutdown(void)
{
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
