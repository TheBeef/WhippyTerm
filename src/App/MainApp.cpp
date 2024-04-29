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
#include "PluginSupport/ExternPluginsSystem.h"

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

    /* Startup code */
    InitSessionSystem();
    InitBookmarks();

    LoadSettings();
    LoadSession();
    LoadBookmarks();
    g_SendBuffers.LoadBuffers();

///* DEBUG PAUL: Tmp hack to get data in there that we can work with */
//g_SendBuffers.SetBuffer(0,(uint8_t *)"Hello world!\n",13);
//g_SendBuffers.SetBuffer(1,(uint8_t *)"Paul was here and left this message.  How lame.\r\n",49);
//g_SendBuffers.SetBuffer(2,(uint8_t *)"\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20",16);
//g_SendBuffers.SetBuffer(3,(uint8_t *)"ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/ABCDEFGHIJKLMNOPabcdefghijklmnop0123456789:;<=>? !\"#$%&'()*+,-/",630);
//g_SendBuffers.SetBuffer(4,(uint8_t *)"ABCDEFGHIJKLMNOPQRSTUVWXYZ",26);

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
