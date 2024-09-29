/*******************************************************************************
 * FILENAME: MainWindow.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    The main window handler.
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
#include "App/Dialogs/Dialog_About.h"
#include "App/Dialogs/Dialog_Bridge.h"
#include "App/Dialogs/Dialog_ChangeConnectionName.h"
#include "App/Dialogs/Dialog_ConnectionOptions.h"
#include "App/Dialogs/Dialog_ComTest.h"
#include "App/Dialogs/Dialog_ManagePlugins.h"
#include "App/Dialogs/Dialog_NewConnection.h"
#include "App/Dialogs/Dialog_SendByte.h"
#include "App/Dialogs/Dialog_Settings.h"
#include "App/Dialogs/Dialog_TransmitDelay.h"
#include "App/Dialogs/Dialog_SendBufferSelect.h"
#include "App/Bookmarks.h"
#include "App/Connections.h"
#include "App/MainApp.h"
#include "App/MainWindow.h"
#include "App/PluginSupport/ExternPluginsSystem.h"
#include "App/SendBuffer.h"
#include "App/Session.h"
#include "App/Settings.h"
#include "UI/UIAsk.h"
#include "UI/UIMainWindow.h"
#include "UI/UIFileReq.h"
#include "UI/UIDebug.h"
#include "UI/UISystem.h"
#include "UI/UITextInputBox.h"
#include "Version.h"
#include <list>
#include <map>
#include <string.h>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef list<class TheMainWindow *> t_MainWindowsListType;
typedef t_MainWindowsListType::iterator i_MainWindowsListType;

typedef map<string,e_UISubMenuCtrl *> t_BookmarkMenuFolderLookup;
typedef t_BookmarkMenuFolderLookup::iterator i_BookmarkMenuFolderLookup;

/* Must match UI */
typedef enum
{
    e_RightPanelTabIndexes_StopWatch=0,
    e_RightPanelTabIndexesMAX
} e_RightPanelTabIndexesType;

/* Must match UI */
typedef enum
{
    e_LeftPanelTabIndexes_Capture=0,
    e_LeftPanelTabIndexes_Upload,
    e_LeftPanelTabIndexes_Download,
    e_LeftPanelTabIndexes_ConnectionOptions=0,
    e_LeftPanelTabIndexes_AuxControls,
    e_LeftPanelTabIndexes_Bridge,
    e_LeftPanelTabIndexesMAX
} e_LeftPanelTabIndexesType;

/* Must match UI */
typedef enum
{
    e_BottomPanelTabIndexes_Hex=0,
    e_BottomPanelTabIndexes_Injection,
    e_BottomPanelTabIndexes_Buffers,
    e_BottomPanelTabIndexesMAX
} e_BottomPanelTabIndexesType;

/*** FUNCTION PROTOTYPES      ***/
static bool MW_DoMainWindowClose(class TheMainWindow *win);

/*** VARIABLE DEFINITIONS     ***/
t_MainWindowsListType m_MainWindowsList;

void MWDebug1(uintptr_t ID)
{
DB_StartTimer(e_DBT_DriverAvailData);

    class Connection *Con=(class Connection *)ID;
    if(Con!=NULL) return Con->Debug1();
}
void MWDebug2(uintptr_t ID)
{

DB_StopTimer(e_DBT_DriverAvailData);

    class Connection *Con=(class Connection *)ID;
    if(Con!=NULL) return Con->Debug2();
}
void MWDebug3(uintptr_t ID)
{
    class Connection *Con=(class Connection *)ID;
    if(Con!=NULL) return Con->Debug3();
}
void MWDebug4(uintptr_t ID)
{
    class Connection *Con=(class Connection *)ID;
    if(Con!=NULL) return Con->Debug4();
}
void MWDebug5(uintptr_t ID)
{
    class Connection *Con=(class Connection *)ID;
    if(Con!=NULL) return Con->Debug5();
}
void MWDebug6(uintptr_t ID)
{
    class Connection *Con=(class Connection *)ID;
    if(Con!=NULL) return Con->Debug6();
}

/*******************************************************************************
 * NAME:
 *    MW_AllocNewMainWindow
 *
 * SYNOPSIS:
 *    bool MW_AllocNewMainWindow(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates and shows a main window.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error and the user has been told
 *
 * SEE ALSO:
 *
 ******************************************************************************/
bool MW_AllocNewMainWindow(void)
{
    class TheMainWindow *MainWin;

    try
    {
        MainWin=new TheMainWindow();
    }
    catch(...)
    {
        UIAsk("Error","Failed to allocate the main window",e_AskBox_Error,
                e_AskBttns_Ok);
        return false;
    }

    m_MainWindowsList.push_back(MainWin);

    /* Show the first main window */
    MainWin->ShowWindow();
    MainWin->RethinkActiveConnectionUI();
    MainWin->RebuildBookmarkMenu();

    return true;
}

/*******************************************************************************
 * NAME:
 *    MW_ApplySettings
 *
 * SYNOPSIS:
 *    void MW_ApplySettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called from the settings system to apply settings to
 *    the main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    RestoreFromSettings()
 ******************************************************************************/
void MW_ApplySettings(void)
{
    i_MainWindowsListType MW;

    for(MW=m_MainWindowsList.begin();MW!=m_MainWindowsList.end();MW++)
        (*MW)->ApplySettings();
}

/*******************************************************************************
 * NAME:
 *    MW_InformOfNewPluginInstalled
 *
 * SYNOPSIS:
 *    void MW_InformOfNewPluginInstalled(const struct ExternPluginInfo *Info);
 *
 * PARAMETERS:
 *    Info [I] -- The info about this plugin
 *
 * FUNCTION:
 *    This function is called when a new plugin is installed.  It tells all
 *    the mains windows that the plugin has been installed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MW_InformOfNewPluginInstalled(const struct ExternPluginInfo *Info)
{
    i_MainWindowsListType MW;

    for(MW=m_MainWindowsList.begin();MW!=m_MainWindowsList.end();MW++)
        (*MW)->InformOfNewPluginInstalled(Info);
}

/*******************************************************************************
 * NAME:
 *    MW_RebuildAllBookmarkMenus
 *
 * SYNOPSIS:
 *    void MW_RebuildAllBookmarkMenus(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rebuilds all the bookmark menus on all the main windows.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void MW_RebuildAllBookmarkMenus(void)
{
    i_MainWindowsListType MW;

    /* Update all the main windows */
    for(MW=m_MainWindowsList.begin();MW!=m_MainWindowsList.end();MW++)
        (*MW)->RebuildBookmarkMenu();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::TheMainWindow
 *
 * SYNOPSIS:
 *    TheMainWindow::TheMainWindow()
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates and sets up the main window class.  It will
 *    allocate the GUI window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
TheMainWindow::TheMainWindow()
{
    t_UITabCtrl *MainTabs;
    t_UIContainerFrameCtrl *ContainerFrame;

    UIWin=NULL;
    NoTabsConnection=NULL;
    TextCanvasWidth=0;
    TextCanvasHeight=0;
    ActiveCon=NULL;
    BridgeActive=false;
    BridgedCon1=NULL;
    BridgedCon2=NULL;

    UIWin=UIMW_AllocMainWindow(this,0);
    if(UIWin==NULL)
        throw(0);

    ContainerFrame=UIMW_GetContainerFrameCtrlHandle(UIWin);
    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);

    UIMW_SetContainerFrameVisible(ContainerFrame,false);
    UIShowHideTabCtrl(MainTabs,false);

    UIMW_SetWindowTitle(UIWin,WHIPPYTERM_NAME);
    UIMW_SwitchTabControlCloseBttnPos(UIWin,g_Settings.CloseButtonOnTabs);

    ConnectionOptionsPanel.Setup(this,UIWin);
    StopWatchPanel.Setup(this,UIWin);
    CapturePanel.Setup(this,UIWin);
    UploadPanel.Setup(this,UIWin);
    DownloadPanel.Setup(this,UIWin);
    HexDisplayPanel.Setup(this,UIWin);
    SendBuffersPanel.Setup(this,UIWin);
    BridgePanel.Setup(this,UIWin);
    AuxControlsPanel.Setup(this,UIWin);

    EarlyRestoreWindowSize();   // Must be at the end because it (may) call Init()
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::~TheMainWindow
 *
 * SYNOPSIS:
 *    TheMainWindow::~TheMainWindow()
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Deconstructor
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
TheMainWindow::~TheMainWindow()
{
    RemoveAllTabPanelControls();

    /* UIWin will delete it's self */
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ShowWindow
 *
 * SYNOPSIS:
 *    void TheMainWindow::ShowWindow(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Shows the main window
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ShowWindow(void)
{
    UIMW_ShowMainWindow(UIWin);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::Init
 *
 * SYNOPSIS:
 *    void TheMainWindow::Init(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's a new main window.  This needs to be called after
 *    the window has been shown.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::Init(void)
{
    t_UITabCtrl *PanelCtrl;

    /* Restore the session data */
    RestoreFromSession();
    RestoreFromSettings();

///* DEBUG PAUL: Move this (currently test code) */
//{
//int r;
//t_UIContainerFrameCtrl *Hex;
//t_UIDisplayFrameCtrl *DisplayFrame;
//struct UITextCanvas *TextCanvas; // The text canvas we are connected to
//struct ScreenLine AddLine;
//struct CharStyling style[10];
//
//Hex=UIMW_GetHexDisplayContainerFrameCtrlHandle(UIWin);
//DisplayFrame=UITC_AllocDisplayFrame(Hex,xxx,(uintptr_t)0);
//TextCanvas=UITC_GetTextCanvasCtrlHandle(DisplayFrame);
//UITC_SetCanvasSize(TextCanvas,100,100);
//UITC_SetShowCursor(TextCanvas,true);
//UITC_SetFont(TextCanvas,g_Settings.FontName.c_str(),g_Settings.FontSize,
//            g_Settings.FontBold,g_Settings.FontItalic);
//UITC_SetCursorColor(TextCanvas,g_Settings.CursorColor);
//AddLine.BGFillColor=0x111111;
//AddLine.Line=(uint8_t *)"1234567890";
//AddLine.LineStyles=style;
//AddLine.Next=NULL;
//for(r=0;r<10;r++)
//{
//    style[r].FGColor=0xFFFFFF;
//    style[r].BGColor=0xFF0000;
//    style[r].ULineColor=0x00FF00;
//    style[r].Attribs=0;
//}
//UITC_ClearTextCanvasLines(TextCanvas);
//UITC_AppendTextCanvasLine_Screen(TextCanvas,&AddLine,0,10,1);
///* DEBUG PAUL: Have to rework how the events from this work */
//
//}

    /* There are no tabs open so we deactivate the panels */
    ConnectionOptionsPanel.ActivateCtrls(false);
    StopWatchPanel.ActivateCtrls(false);
    CapturePanel.ActivateCtrls(false);
    UploadPanel.ActivateCtrls(false);
    DownloadPanel.ActivateCtrls(false);
    HexDisplayPanel.ActivateCtrls(false);
    SendBuffersPanel.ActivateCtrls(false);
    BridgePanel.ActivateCtrls(false);
    AuxControlsPanel.ActivateCtrls(false);

    /* Force us to the default selected tabs */
    PanelCtrl=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_LeftPanel);
    UITabCtrlMakeTabActiveUsingIndex(PanelCtrl,e_LeftPanelTabIndexes_Capture);

    PanelCtrl=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_BottomPanel);
    UITabCtrlMakeTabActiveUsingIndex(PanelCtrl,e_BottomPanelTabIndexes_Hex);

    PanelCtrl=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_RightPanel);
    UITabCtrlMakeTabActiveUsingIndex(PanelCtrl,e_RightPanelTabIndexes_StopWatch);

    /* Add all the terminal emulations to the menu */
    RebuildTerminalEmulationMenu();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::EarlyRestoreWindowSize
 *
 * SYNOPSIS:
 *    void TheMainWindow::EarlyRestoreWindowSize(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function trys to restore the window size and position from the
 *    settings and session.  It is for early startup before the window has
 *    been show so we can try to prevent flickering.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::EarlyRestoreWindowSize(void)
{
    if(g_Settings.RestoreWindowPos==e_WindowStartupPos_RestoreFromSession)
    {
        UIMW_SetWindowPos(UIWin,g_Session.WindowPosX,g_Session.WindowPosY);
        UIMW_SetWindowSize(UIWin,g_Session.WindowWidth,g_Session.WindowHeight);
    }
    else if(g_Settings.RestoreWindowPos==e_WindowStartupPos_RestoreFromSettings)
    {
        UIMW_SetWindowPos(UIWin,g_Settings.WindowPosX,
                g_Settings.WindowPosY);
        UIMW_SetWindowSize(UIWin,g_Settings.WindowWidth,
                g_Settings.WindowHeight);
    }

    if(g_Settings.AppMaximized)
        UIMW_Maximize(UIWin);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RestoreFromSession
 *
 * SYNOPSIS:
 *    void TheMainWindow::RestoreFromSession(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function restore the main window to the settings that where stored
 *    in the session system.
 *
 *    It will only restore things that are set to be used from the session
 *    data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::RestoreFromSession(void)
{
    if(g_Settings.RestoreWindowPos==e_WindowStartupPos_RestoreFromSession)
    {
        UIMW_SetWindowPos(UIWin,g_Session.WindowPosX,g_Session.WindowPosY);
        UIMW_SetWindowSize(UIWin,g_Session.WindowWidth,g_Session.WindowHeight);

        if(g_Session.AppMaximized)
            UIMW_Maximize(UIWin);
    }

    if(!g_Settings.LeftPanelFromSettings)
    {
        UIMW_SetLeftPanel(UIWin,g_Session.LeftPanelSize,
                g_Session.LeftPanelOpen);
    }

    if(!g_Settings.BottomPanelFromSettings)
    {
        UIMW_SetBottomPanel(UIWin,g_Session.BottomPanelSize,
                g_Session.BottomPanelOpen);
    }

    if(!g_Settings.RightPanelFromSettings)
    {
        UIMW_SetRightPanel(UIWin,g_Session.RightPanelSize,
                g_Session.RightPanelOpen);
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RestoreFromSettings
 *
 * SYNOPSIS:
 *    void TheMainWindow::RestoreFromSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function restore the main window to the settings that where stored
 *    in the settings system.
 *
 *    This only applies settings that are used for startup.  Other settings
 *    are applied with TheMainWindow::ApplySettings().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TheMainWindow::ApplySettings()
 ******************************************************************************/
void TheMainWindow::RestoreFromSettings(void)
{
    if(g_Settings.RestoreWindowPos==e_WindowStartupPos_RestoreFromSettings)
    {
        UIMW_SetWindowPos(UIWin,g_Settings.WindowPosX,
                g_Settings.WindowPosY);
        UIMW_SetWindowSize(UIWin,g_Settings.WindowWidth,
                g_Settings.WindowHeight);

        if(g_Settings.AppMaximized)
            UIMW_Maximize(UIWin);
    }

    if(g_Settings.LeftPanelFromSettings)
    {
        UIMW_SetLeftPanel(UIWin,g_Settings.LeftPanelSize,
                g_Settings.LeftPanelOpenOnStartup);
    }

    if(g_Settings.RightPanelFromSettings)
    {
        UIMW_SetRightPanel(UIWin,g_Settings.RightPanelSize,
                g_Settings.RightPanelOpenOnStartup);
    }

    if(g_Settings.BottomPanelFromSettings)
    {
        UIMW_SetBottomPanel(UIWin,g_Settings.BottomPanelSize,
                g_Settings.BottomPanelOpenOnStartup);
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ApplySettings
 *
 * SYNOPSIS:
 *    void TheMainWindow::ApplySettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function applies the current settings to things in the main
 *    window.
 *    This includes things like:
 *          * Menu shortcuts
 *          * Telling each connection in each tab to apply the settings
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    Window position and sizing is only applied at start up using
 *    RestoreFromSettings()
 *
 * SEE ALSO:
 *    TheMainWindow::RestoreFromSettings()
 ******************************************************************************/
void TheMainWindow::ApplySettings(void)
{
    int MenuID;
    e_UIMenuCtrl *MenuHandle;
    e_CmdType Cmd;
    uint8_t Mod;
    e_UIKeys Key;
    char Letter;

    /* Setup all the menu shortcuts */

    /* First clear all the current shortcuts */
    for(MenuID=0;MenuID<e_UIMWMenuMAX;MenuID++)
    {
        MenuHandle=UIMW_GetMenuHandle(UIWin,(e_UIMWMenuType)MenuID);
        if(MenuHandle==NULL)
            continue;

        Mod=0;
        Key=e_UIKeysMAX;
        Letter=0;

        /* See if there is short key for this cmd */
        Cmd=MainMenu2Cmd((e_UIMWMenuType)MenuID);
        if(Cmd<e_CmdMAX)
        {
            Mod=g_Settings.KeyMapping[Cmd].Mod;
            Key=g_Settings.KeyMapping[Cmd].Key;
            Letter=g_Settings.KeyMapping[Cmd].Letter;
        }

        UISetMenuKeySeq(MenuHandle,Mod,Key,Letter);
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::GetSizeAndPos
 *
 * SYNOPSIS:
 *    void TheMainWindow::GetSizeAndPos(int &x,int &y,int &Width,int &Height);
 *
 * PARAMETERS:
 *    x [O] -- The x pos of the window
 *    y [O] -- The y pos of the window
 *    Width [O] -- The width of the window
 *    Height [O] -- The height of the window
 *
 * FUNCTION:
 *    This function gets the current size and pos of the main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::GetSizeAndPos(int &x,int &y,int &Width,int &Height)
{
    UIMW_GetWindowPos(UIWin,x,y);
    UIMW_GetWindowSize(UIWin,Width,Height);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::NewConnection
 *
 * SYNOPSIS:
 *    void TheMainWindow::NewConnection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function opens a new tab (connection)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::NewConnection(void)
{
    RunNewConnectionDialog(this);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::AllocNewTab
 *
 * SYNOPSIS:
 *    class Connection *TheMainWindow::AllocNewTab(const char *TabLabel,
 *              class ConSettings *UseSettings,const char *URI,
 *              t_KVList &Options);
 *
 * PARAMETERS:
 *    TabLabel [I] -- The display label for this new tab.
 *    UseSettings [I] -- The settings to apply to this connection (or NULL
 *                       for global settings).
 *    URI [I] -- The URI to open the connection with
 *
 * FUNCTION:
 *    This function allocates a new tab and adds it to the main window.
 *    The new tab will also allocate a connection for this new tab.
 *
 * RETURNS:
 *    The newly allocated connection for this tab or NULL if there was an error.
 *
 * SEE ALSO:
 *    TheMainWindow::ReloadTabFromURI()
 ******************************************************************************/
class Connection *TheMainWindow::AllocNewTab(const char *TabLabel,
        class ConSettings *UseSettings,const char *URI,t_KVList &Options)
{
    t_UITabCtrl *MainTabs;
    t_UITab *NewTab;
    int TabCount;
    bool UseContainer;
    t_UIContainerFrameCtrl *ContainerFrame;
    void *ParentWidget;
    t_UITab *Tab0;
    class Connection *NewConnection;

    MainTabs=NULL;
    ContainerFrame=NULL;
    NewConnection=nullptr;
    NewTab=nullptr;
    try
    {
        MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
        ContainerFrame=UIMW_GetContainerFrameCtrlHandle(UIWin);

        /* Add the tab */
        NewTab=UITabCtrlAddTab(MainTabs,TabLabel,0);
        if(NewTab==NULL)
            throw("Failed to allocate a new tab");

        TabCount=UITabCtrlGetTabCount(MainTabs);

        if(g_Settings.AlwaysShowTabs)
        {
            /* Ok, we are always showing the tabs */
            UseContainer=false;
            ParentWidget=NewTab;
        }
        else
        {
            if(TabCount>=2)
            {
                UseContainer=false;
                ParentWidget=NewTab;
                if(NoTabsConnection!=NULL)
                {
                    /* Ok, we need to move the existing display frame to the tabs */
                    Tab0=UITabCtrlGetTabFromIndex(MainTabs,0);
                    NoTabsConnection->ReParentWidget(Tab0);
                    NoTabsConnection=NULL;
                }
            }
            else
            {
                UseContainer=true;
                ParentWidget=ContainerFrame;
            }
        }

        /* Allocate a new connection for this tab */
        NewConnection=Con_AllocateConnection(URI);
        if(NewConnection==NULL)
            throw("Failed to allocate a new connection");

        if(UseContainer)
        {
            UIShowHideTabCtrl(MainTabs,false);
            UIMW_SetContainerFrameVisible(ContainerFrame,true);
            NoTabsConnection=NewConnection;
        }
        else
        {
            UIShowHideTabCtrl(MainTabs,true);
            UIMW_SetContainerFrameVisible(ContainerFrame,false);
            NoTabsConnection=NULL;
        }

        UITabCtrlSetTabID(MainTabs,NewTab,(uintptr_t)NewConnection);

        if(!NewConnection->Init(this,ParentWidget,UseSettings))
            throw("Failed to setup the new connection");

        NewConnection->SetDisplayName(TabLabel);

        SetActiveTab(NewConnection);

        NewConnection->SetConnectionOptions(Options);

        AuxControlsPanel.NewConnection(NewConnection);

        NewConnection->FinalizeNewConnection();
    }
    catch(const char *Msg)
    {
        uintptr_t ConnectionID;

        ConnectionID=(uintptr_t)NewConnection;

        if(NewConnection!=nullptr)
            delete NewConnection;
        NewConnection=nullptr;

        if(NewTab!=nullptr && MainTabs!=nullptr)
            UITabCtrlRemoveTab(MainTabs,ConnectionID);

        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
    }
    return NewConnection;
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ReloadTabFromURI
 *
 * SYNOPSIS:
 *    class Connection *TheMainWindow::ReloadTabFromURI(const char *TabLabel,
 *              class ConSettings *UseSettings,const char *URI);
 *
 * PARAMETERS:
 *    TabLabel [I] -- The display label for this new tab.  If NULL then the
 *                    short name from the connection will be used.
 *    UseSettings [I] -- The settings to apply to this connection (or NULL
 *                       for global settings).
 *    URI [I] -- The URI to open the connection with
 *
 * FUNCTION:
 *    This function is called when you want to reload a tab from a URI
 *    It will close the current connection (if needed) and tell
 *    the connection to reload everything.
 *
 * RETURNS:
 *    A pointer to the connection that was opened or NULL if there was an error
 *
 * SEE ALSO:
 *
 ******************************************************************************/
class Connection *TheMainWindow::ReloadTabFromURI(const char *TabLabel,
        class ConSettings *UseSettings,const char *URI)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;
    string Name;
    char URIBuffer[MAX_URI_LENGTH];
    char *URIStart;
    char *URIEnd;
    void *ParentWidget;
    class Connection *NewConnection;
    t_UITab *ActiveTab;
    t_UIContainerFrameCtrl *ContainerFrame;
    t_KVList EmptyOptions;

    try
    {
        MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
        TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
        ActiveTab=UITabCtrlGetActiveTabHandle(MainTabs);
        ContainerFrame=UIMW_GetContainerFrameCtrlHandle(UIWin);

        if(strlen(URI)>MAX_URI_LENGTH)
            throw("URI string too long");

        /* Make a copy */
        strcpy(URIBuffer,URI);

        /* Trim the string */
        URIStart=URIBuffer;
        if(*URIStart!=0)
        {
            while(*URIStart==' ' || *URIStart=='\t')
                URIStart++;

            URIEnd=&URIBuffer[strlen(URIBuffer)-1];
            while(*URIEnd==' ' || *URIEnd=='\t')
                URIEnd--;
            *(URIEnd+1)=0;
        }

        if(ActiveTab!=nullptr && TabCon!=nullptr && *URIStart!=0)
        {
            /* Close the existing connection and then reopen it, we don't
               free the tab so we can reuse it */

            /* Allocate a new connection for this tab */
            NewConnection=Con_AllocateConnection(URI);
            if(NewConnection==NULL)
                throw(0);

            SetActiveConnection(NULL);
            Con_FreeConnection(TabCon);

            if(NoTabsConnection!=nullptr)
            {
                NoTabsConnection=NewConnection;
                ParentWidget=ContainerFrame;
            }
            else
            {
                ParentWidget=ActiveTab;
            }

            if(!NewConnection->Init(this,ParentWidget,UseSettings))
            {
                /* We also need to free the tab because it's now an orphan */
                UITabCtrlRemoveTab(MainTabs,(uintptr_t)TabCon);
                RethinkTabCountAfterFree();
                BridgePanel.ConnectionAddedRemoved();
                RethinkBridgeMenu();

                throw("Failed to setup the new connection");
            }

            UITabCtrlSetTabID(MainTabs,ActiveTab,(uintptr_t)NewConnection);
            NewConnection->SetDisplayName(TabLabel);

            SetActiveTab(NewConnection);

            NewConnection->FinalizeNewConnection();
        }
        else
        {
            /* Allocate a new tab and connection */
            NewConnection=AllocNewTab(TabLabel,UseSettings,URI,EmptyOptions);
        }
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error);
        return nullptr;
    }
    catch(int junk)
    {
        /* We have already prompted */
        return nullptr;
    }

    return NewConnection;
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::FreeTab
 *
 * SYNOPSIS:
 *    void TheMainWindow::FreeTab(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The connection connected to the tab to delete
 *    DisplayFrame [I] -- The display frame that goes with this tab.
 *
 * FUNCTION:
 *    This function removes (and frees) a tab from a main window.  It will
 *    free the tab and the display frame but that is all.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::FreeTab(class Connection *Con)
{
    t_UITabCtrl *MainTabs;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);

    /* Kill of the active connection (because we are about to close it),
       when we remove the tab the next tab over will become the active
       tab (because we get a tab changed event when we free the tab) */
    SetActiveConnection(NULL);

    AuxControlsPanel.RemoveConnection(Con);

    Con_FreeConnection(Con);
    UITabCtrlRemoveTab(MainTabs,(uintptr_t)Con);

    RethinkTabCountAfterFree();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RethinkTabCountAfterFree
 *
 * SYNOPSIS:
 *    void TheMainWindow::RethinkTabCountAfterFree(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function will rethink the tabs (hiding the tab bar, showing the
 *    container frame) after a tab as been freed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::RethinkTabCountAfterFree(void)
{
    t_UITabCtrl *MainTabs;
    t_UITab *Tab0;
    int TabCount;
    t_UIContainerFrameCtrl *ContainerFrame;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    ContainerFrame=UIMW_GetContainerFrameCtrlHandle(UIWin);

    TabCount=UITabCtrlGetTabCount(MainTabs);
    if(TabCount==0)
    {
        /* No tabs */
        AllTabsClosed();
        NoTabsConnection=NULL;
    }
    else if(TabCount==1)
    {
        /* We have only 1 tab left */
        if(g_Settings.AlwaysShowTabs)
        {
            /* Nothing to do */
            NoTabsConnection=NULL;
        }
        else
        {
            /* Switch to the container */
            Tab0=UITabCtrlGetTabFromIndex(MainTabs,0);
            if(Tab0!=NULL)
            {
                NoTabsConnection=(class Connection *)UITabCtrlGetID(MainTabs,
                        Tab0);
                if(NoTabsConnection!=NULL)
                {
                    NoTabsConnection->ReParentWidget(ContainerFrame);
                    UIShowHideTabCtrl(MainTabs,false);
                    UIMW_SetContainerFrameVisible(ContainerFrame,true);
                }
            }
        }
    }
    else
    {
        /* Lots of tabs left, don't touch anything */
        NoTabsConnection=NULL;
    }
}


/*******************************************************************************
 * NAME:
 *    TheMainWindow::ConnectionStatusChange
 *
 * SYNOPSIS:
 *    void TheMainWindow::ConnectionStatusChange(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The connection that had the change.
 *
 * FUNCTION:
 *    This function makes the GUI rethink it's self after a connection
 *    status change.  It will only rethink things if 'Con' is the active
 *    connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::ConnectionStatusChange(class Connection *Con)
{
    t_UITabCtrl *MainTabs;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);

    if(UITabCtrlGetActiveTabID(MainTabs)==(uintptr_t)Con)
    {
        /* Ok, this is for the active tab.  Rethink display */
        RethinkActiveConnectionUI();
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::AllTabsClosed
 *
 * SYNOPSIS:
 *    void TheMainWindow::AllTabsClosed(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when all the tabs are closed.  It handles cleaning
 *    up the GUI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::AllTabsClosed(void)
{
    SetURIText("");
    RemoveAllTabPanelControls();
    UIMW_SetWindowTitle(UIWin,WHIPPYTERM_NAME);

    ActiveCon=NULL;
    RethinkActiveConnectionUI();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::SetActiveTab
 *
 * SYNOPSIS:
 *    void TheMainWindow::SetActiveTab(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The connection to make active
 *
 * FUNCTION:
 *    This function makes the tab with 'Con' connected to it active.
 *    The active tab is the one that is visable and has with keyboard focus.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::SetActiveTab(class Connection *Con)
{
    t_UITabCtrl *MainTabs;
    int TabCount;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);

    TabCount=UITabCtrlGetTabCount(MainTabs);

    if(TabCount<2)
    {
        /* Nothing to do */
    }
    else
    {
        UITabCtrlMakeTabActive(MainTabs,(uintptr_t)Con);
    }

    /* Give the connection focus */
    Con->GiveFocus();

    SetActiveConnection(Con);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RethinkActiveConnectionUI
 *
 * SYNOPSIS:
 *    void TheMainWindow::RethinkActiveConnectionUI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the UI (enable / disable, etc) to reflect the
 *    state of the currently active connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::RethinkActiveConnectionUI(void)
{
    bool Connected;
    bool ActivatePanels;
    t_UITabCtrl *MainTabs;
    class Connection *Con;
    bool RestoreConnectionSettingsActive;
    i_BookmarkList bm;
    t_UIToolbarCtrl *ConnectToggle;
    t_UIToolbarCtrl *CopyTool;
    t_UIToolbarCtrl *PasteTool;
    e_UIMenuCtrl *CloseTabMenu;
    e_UIMenuCtrl *CloseAllMenu;
    e_UIMenuCtrl *ConnectMenu;
    e_UIMenuCtrl *DisconnectMenu;
    e_UIMenuCtrl *ChangeConnectionNameMenu;
    e_UIMenuCtrl *ConnectionOptionsMenu;
    e_UIMenuCtrl *ConnectionSettingsMenu;
    e_UIMenuCtrl *TransmitDelayMenu;
    e_UIMenuCtrl *AddBookmark;
    e_UIMenuCtrl *RestoreConnectionSettings;
    e_UIMenuCtrl *ShowNonPrintable;
    e_UIMenuCtrl *ShowEndOfLines;
    e_UIMenuCtrl *InsertHorizontalRule;
    e_UIMenuCtrl *ResetTerm;
    e_UIMenuCtrl *ClearScreen;
    e_UIMenuCtrl *ClearScrollBackBuffer;
    e_UIMenuCtrl *GotoColumn;
    e_UIMenuCtrl *GotoRow;
    e_UIMenuCtrl *Copy;
    e_UIMenuCtrl *Paste;
    e_UIMenuCtrl *SelectAll;
    e_UIMenuCtrl *ZoomIn;
    e_UIMenuCtrl *ZoomOut;
    e_UIMenuCtrl *ResetZoom;
    e_UIMenuCtrl *Send_NULL;
    e_UIMenuCtrl *Send_Backspace;
    e_UIMenuCtrl *Send_Tab;
    e_UIMenuCtrl *Send_Line_Feed;
    e_UIMenuCtrl *Send_Form_Feed;
    e_UIMenuCtrl *Send_Carriage_Return;
    e_UIMenuCtrl *Send_Escape;
    e_UIMenuCtrl *Send_Delete;
    e_UIMenuCtrl *Send_Other;
    e_UIMenuCtrl *SendBuffer1;
    e_UIMenuCtrl *SendBuffer2;
    e_UIMenuCtrl *SendBuffer3;
    e_UIMenuCtrl *SendBuffer4;
    e_UIMenuCtrl *SendBuffer5;
    e_UIMenuCtrl *SendBuffer6;
    e_UIMenuCtrl *SendBuffer7;
    e_UIMenuCtrl *SendBuffer8;
    e_UIMenuCtrl *SendBuffer9;
    e_UIMenuCtrl *SendBuffer10;
    e_UIMenuCtrl *SendBuffer11;
    e_UIMenuCtrl *SendBuffer12;
    e_UIMenuCtrl *SendBufferSendGeneric;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    ConnectToggle=UIMW_GetToolbarHandle(UIWin,e_UIMWToolbar_ConnectToggle);
    CopyTool=UIMW_GetToolbarHandle(UIWin,e_UIMWToolbar_Copy);
    PasteTool=UIMW_GetToolbarHandle(UIWin,e_UIMWToolbar_Paste);
    CloseTabMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_CloseTab);
    CloseAllMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_CloseAll);
    ConnectMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Connect);
    DisconnectMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Disconnect);
    ChangeConnectionNameMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ChangeConnectionName);
    ConnectionOptionsMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ConnectionOptions);
    ConnectionSettingsMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ConnectionSettings);
    TransmitDelayMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_TransmitDelay);
    AddBookmark=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_AddBookmark);
    RestoreConnectionSettings=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_RestoreConnectionSettings);
    ShowNonPrintable=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ShowNonPrintable);
    ShowEndOfLines=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ShowEndOfLines);
    InsertHorizontalRule=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_InsertHorizontalRule);
    ResetTerm=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ResetTerm);
    ClearScreen=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ClearScreen);
    ClearScrollBackBuffer=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ClearScrollBackBuffer);
    GotoColumn=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_GotoColumn);
    GotoRow=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_GotoRow);
    Copy=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Copy);
    Paste=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Paste);
    SelectAll=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_SelectAll);
    ZoomIn=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ZoomIn);
    ZoomOut=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ZoomOut);
    ResetZoom=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ResetZoom);
    Send_NULL=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_NULL);
    Send_Backspace=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_Backspace);
    Send_Tab=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_Tab);
    Send_Line_Feed=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_Line_Feed);
    Send_Form_Feed=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_Form_Feed);
    Send_Carriage_Return=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_Carriage_Return);
    Send_Escape=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_Escape);
    Send_Delete=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_Delete);
    Send_Other=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Send_Other);

    SendBuffer1=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer1);
    SendBuffer2=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer2);
    SendBuffer3=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer3);
    SendBuffer4=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer4);
    SendBuffer5=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer5);
    SendBuffer6=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer6);
    SendBuffer7=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer7);
    SendBuffer8=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer8);
    SendBuffer9=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer9);
    SendBuffer10=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer10);
    SendBuffer11=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer11);
    SendBuffer12=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBuffer12);
    SendBufferSendGeneric=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Buffers_SendBufferSendGeneric);

    RestoreConnectionSettingsActive=false;

    if(UITabCtrlGetTabCount(MainTabs)==0)
    {
        UIEnableToolbar(ConnectToggle,false);
        UIEnableMenu(CloseTabMenu,false);
        UIEnableMenu(CloseAllMenu,false);
        UICheckToolbar(ConnectToggle,false);
        UIEnableMenu(ConnectMenu,false);
        UIEnableMenu(DisconnectMenu,false);
        UIEnableMenu(ChangeConnectionNameMenu,false);
        UIEnableMenu(ConnectionOptionsMenu,false);
        UIEnableMenu(ConnectionSettingsMenu,false);
        UIEnableMenu(TransmitDelayMenu,false);
        UIEnableMenu(AddBookmark,false);
        UIEnableMenu(ShowNonPrintable,false);
        UIEnableMenu(ShowEndOfLines,false);
        UIEnableMenu(InsertHorizontalRule,false);
        UIEnableMenu(ResetTerm,false);
        UIEnableMenu(ClearScreen,false);
        UIEnableMenu(ClearScrollBackBuffer,false);
        UIEnableMenu(GotoColumn,false);
        UIEnableMenu(GotoRow,false);
        UIEnableMenu(Copy,false);
        UIEnableMenu(Paste,false);
        UIEnableToolbar(CopyTool,false);
        UIEnableToolbar(PasteTool,false);
        UIEnableMenu(SelectAll,false);
        UIEnableMenu(ZoomIn,false);
        UIEnableMenu(ZoomOut,false);
        UIEnableMenu(ResetZoom,false);
        UIEnableMenu(Send_NULL,false);
        UIEnableMenu(Send_Backspace,false);
        UIEnableMenu(Send_Tab,false);
        UIEnableMenu(Send_Line_Feed,false);
        UIEnableMenu(Send_Form_Feed,false);
        UIEnableMenu(Send_Carriage_Return,false);
        UIEnableMenu(Send_Escape,false);
        UIEnableMenu(Send_Delete,false);
        UIEnableMenu(Send_Other,false);
        UIEnableMenu(SendBuffer1,false);
        UIEnableMenu(SendBuffer2,false);
        UIEnableMenu(SendBuffer3,false);
        UIEnableMenu(SendBuffer4,false);
        UIEnableMenu(SendBuffer5,false);
        UIEnableMenu(SendBuffer6,false);
        UIEnableMenu(SendBuffer7,false);
        UIEnableMenu(SendBuffer8,false);
        UIEnableMenu(SendBuffer9,false);
        UIEnableMenu(SendBuffer10,false);
        UIEnableMenu(SendBuffer11,false);
        UIEnableMenu(SendBuffer12,false);
        UIEnableMenu(SendBufferSendGeneric,false);

        UIMW_EnableApplyTerminalEmulationMenu(UIWin,false);

        ActivatePanels=false;
    }
    else
    {
        UIEnableToolbar(ConnectToggle,true);
        UIEnableMenu(CloseTabMenu,true);
        UIEnableMenu(CloseAllMenu,true);
        UIEnableMenu(ChangeConnectionNameMenu,true);
        UIEnableMenu(ConnectionOptionsMenu,true);
        UIEnableMenu(ConnectionSettingsMenu,true);
        UIEnableMenu(TransmitDelayMenu,true);
        UIEnableMenu(AddBookmark,true);
        UIEnableMenu(ShowNonPrintable,true);
        UIEnableMenu(ShowEndOfLines,true);
        UIEnableMenu(ResetTerm,true);
        UIEnableMenu(ClearScreen,true);
        UIEnableMenu(ClearScrollBackBuffer,true);
        UIEnableMenu(GotoColumn,true);
        UIEnableMenu(GotoRow,true);
        UIEnableMenu(Copy,true);
        UIEnableMenu(Paste,true);
        UIEnableToolbar(CopyTool,true);
        UIEnableToolbar(PasteTool,true);
        UIEnableMenu(SelectAll,true);
        UIEnableMenu(ZoomIn,true);
        UIEnableMenu(ZoomOut,true);
        UIEnableMenu(ResetZoom,true);
        UIEnableMenu(Send_NULL,true);
        UIEnableMenu(Send_Backspace,true);
        UIEnableMenu(Send_Tab,true);
        UIEnableMenu(Send_Line_Feed,true);
        UIEnableMenu(Send_Form_Feed,true);
        UIEnableMenu(Send_Carriage_Return,true);
        UIEnableMenu(Send_Escape,true);
        UIEnableMenu(Send_Delete,true);
        UIEnableMenu(Send_Other,true);

        UIMW_EnableApplyTerminalEmulationMenu(UIWin,true);

        Con=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
        if(Con==NULL)
            return;

        Connected=Con->GetConnectedStatus();
        UICheckToolbar(ConnectToggle,Connected);
        UIEnableMenu(ConnectMenu,!Connected);
        UIEnableMenu(DisconnectMenu,Connected);

        UIEnableMenu(SendBuffer1,Connected);
        UIEnableMenu(SendBuffer2,Connected);
        UIEnableMenu(SendBuffer3,Connected);
        UIEnableMenu(SendBuffer4,Connected);
        UIEnableMenu(SendBuffer5,Connected);
        UIEnableMenu(SendBuffer6,Connected);
        UIEnableMenu(SendBuffer7,Connected);
        UIEnableMenu(SendBuffer8,Connected);
        UIEnableMenu(SendBuffer9,Connected);
        UIEnableMenu(SendBuffer10,Connected);
        UIEnableMenu(SendBuffer11,Connected);
        UIEnableMenu(SendBuffer12,Connected);
        UIEnableMenu(SendBufferSendGeneric,Connected);

        if(Con->UsingCustomSettings)
            RestoreConnectionSettingsActive=true;

        UICheckMenu(ShowNonPrintable,Con->GetShowNonPrintable());
        UICheckMenu(ShowEndOfLines,Con->GetShowEndOfLines());

        if(Con->IsThereASelection())
        {
            UIEnableMenu(Copy,true);
            UIEnableToolbar(CopyTool,true);
        }
        else
        {
            UIEnableMenu(Copy,false);
            UIEnableToolbar(CopyTool,false);
        }

        if(Con->IsConnectionBinary())
            UIEnableMenu(InsertHorizontalRule,false);
        else
            UIEnableMenu(InsertHorizontalRule,true);

        ActivatePanels=true;
    }

    UIEnableMenu(RestoreConnectionSettings,RestoreConnectionSettingsActive);

    HandleGoURIToolBttnEnabled();

    RethinkBridgeMenu();

    ConnectionOptionsPanel.ActivateCtrls(ActivatePanels);
    StopWatchPanel.ActivateCtrls(ActivatePanels);
    CapturePanel.ActivateCtrls(ActivatePanels);
    UploadPanel.ActivateCtrls(ActivatePanels);
    DownloadPanel.ActivateCtrls(ActivatePanels);
    HexDisplayPanel.ActivateCtrls(ActivatePanels);
    SendBuffersPanel.ActivateCtrls(ActivatePanels);
    BridgePanel.ActivateCtrls(ActivatePanels);
    AuxControlsPanel.ActivateCtrls(ActivatePanels);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::HandleGoURIToolBttnEnabled
 *
 * SYNOPSIS:
 *    void TheMainWindow::HandleGoURIToolBttnEnabled(void)
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function enables / disables the Go URI button.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::HandleGoURIToolBttnEnabled(void)
{
    char buff[100];
    char *pos;
    t_UIToolbarCtrl *GoURL;
    t_UITextInputCtrl *URIInput;

    GoURL=UIMW_GetToolbarHandle(UIWin,e_UIMWToolbar_URIGo);
    URIInput=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_URI);

    UIGetTextCtrlText(URIInput,buff,sizeof(buff));
    pos=buff;
    while(*pos==' ')
        pos++;

    /* Check for empty URI */
    if(*pos==0)
        UIEnableToolbar(GoURL,false);
    else
        UIEnableToolbar(GoURL,true);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::CloseActiveConnection
 *
 * SYNOPSIS:
 *    void TheMainWindow::CloseActiveConnection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function closes the active (selected) tab by calling the connection
 *    close tab function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::CloseActiveConnection(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        CloseConnection(TabCon);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::CloseConnection
 *
 * SYNOPSIS:
 *    void TheMainWindow::CloseConnection(class Connection *TabCon);
 *
 * PARAMETERS:
 *    TabCon [I] -- The connection to close
 *
 * FUNCTION:
 *    This function closes a connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::CloseConnection(class Connection *TabCon)
{
    FreeTab(TabCon);
    BridgePanel.ConnectionAddedRemoved();
    RethinkBridgeMenu();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::CloseAllConnections
 *
 * SYNOPSIS:
 *    void TheMainWindow::CloseAllConnections(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function closes all the tabs in this window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::CloseAllConnections(void)
{
    t_UITabCtrl *MainTabs;
    t_UITab *Tab;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    if(MainTabs==NULL)
        return;
    while((Tab=UITabCtrlGetTabFromIndex(MainTabs,0))!=NULL)
    {
        TabCon=(class Connection *)UITabCtrlGetID(MainTabs,Tab);
        if(TabCon!=NULL)
            CloseConnection(TabCon);
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ChangeConnectStatus
 *
 * SYNOPSIS:
 *    void TheMainWindow::ChangeConnectStatus(bool Connected);
 *
 * PARAMETERS:
 *    Connected [I] -- Set the connection to connected (true) or not (false)
 *
 * FUNCTION:
 *    This function requests that a connection change it's connected status.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ChangeConnectStatus(bool Connected)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->SetConnectedState(Connected);
}

/*******************************************************************************
 * NAME:
 *    TheMainMenu::ToggleConnectStatus
 *
 * SYNOPSIS:
 *    void TheMainMenu::ToggleConnectStatus(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function requests that a connection toggle it's connected status.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ToggleConnectStatus(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->ToggleConnectedState();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ApplyConnectionOptions
 *
 * SYNOPSIS:
 *    void TheMainWindow::ApplyConnectionOptions(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function applies the currently selected connection options from
 *    the side panel to the current connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ApplyConnectionOptions(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
    {
        ConnectionOptionsPanel.ApplyConnectionOptions(TabCon);
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::CopyActiveTabSelectionToClipboard
 *
 * SYNOPSIS:
 *    void TheMainWindow::CopyActiveTabSelectionToClipboard(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function looks up the current tab and sends a copy selection to
 *    clip board to it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::CopyActiveTabSelectionToClipboard(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->CopySelectionToClipboard();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::PasteFromClipboard
 *
 * SYNOPSIS:
 *    void TheMainWindow::PasteFromClipboard(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function looks up the current tab and sends a copy selection to
 *    clip board to it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::PasteFromClipboard(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->PasteFromClipboard();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::GotoColumn
 *
 * SYNOPSIS:
 *    void TheMainWindow::GotoColumn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user for what column to goto and then goes
 *    to that column.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::GotoColumn(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;
    string ColumnStr;
    int ColumnNumber;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
    {
        if(UITextInputBox("Goto Column",
                "Select the column you want to move the cursor to",ColumnStr))
        {
            ColumnNumber=atoi(ColumnStr.c_str());
            if(ColumnNumber>0)
                TabCon->GotoColumn(ColumnNumber-1);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::GotoRow
 *
 * SYNOPSIS:
 *    void TheMainWindow::GotoRow(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user for what row to goto and then goes
 *    to that row.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::GotoRow(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;
    string RowStr;
    int RowNumber;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
    {
        if(UITextInputBox("Goto Row",
                "Select the row you want to move the cursor to",RowStr))
        {
            RowNumber=atoi(RowStr.c_str());
            if(RowNumber>0)
                TabCon->GotoRow(RowNumber-1);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RemoveAllTabPanelControls
 *
 * SYNOPSIS:
 *    void TheMainWindow::RemoveAllTabPanelControls(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function removes all the controls that where added for a tab
 *    in the panels.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
/* DEBUG PAUL:Rename this? */
void TheMainWindow::RemoveAllTabPanelControls(void)
{
    ConnectionOptionsPanel.ActivateCtrls(false);
    StopWatchPanel.ActivateCtrls(false);
    CapturePanel.ActivateCtrls(false);
    UploadPanel.ActivateCtrls(false);
    DownloadPanel.ActivateCtrls(false);
    HexDisplayPanel.ActivateCtrls(false);
    SendBuffersPanel.ActivateCtrls(false);
    BridgePanel.ActivateCtrls(false);
    AuxControlsPanel.ActivateCtrls(false);
}

/*******************************************************************************
 * NAME:
 *    TheMainMenu::ChangeCurrentConnectionName
 *
 * SYNOPSIS:
 *    void TheMainMenu::ChangeCurrentConnectionName(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user to input a new connection name for the
 *    active tab.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ChangeCurrentConnectionName(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;
    string Name;
    t_UITab *ActiveTab;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    ActiveTab=UITabCtrlGetActiveTabHandle(MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL && ActiveTab!=NULL)
    {
        TabCon->GetDisplayName(Name);
        RunChangeConnectionNameDialog(Name);
        TabCon->SetDisplayName(Name.c_str());
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainMenu::ShowConnectionOptions
 *
 * SYNOPSIS:
 *    void TheMainMenu::ShowConnectionOptions(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user to change the current connections options.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ShowConnectionOptions(void)
{
    if(ActiveCon==NULL)
        return;

    RunConnectionOptionsDialog(ActiveCon);
}

/*******************************************************************************
 * NAME:
 *    TheMainMenu::ShowTransmitDelayDialog
 *
 * SYNOPSIS:
 *    void TheMainMenu::ShowTransmitDelayDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user to change the transmit delay options.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ShowTransmitDelayDialog(void)
{
    if(ActiveCon==NULL)
        return;

    RunTransmitDelayDialog(ActiveCon);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ChangeTabLabel
 *
 * SYNOPSIS:
 *    void TheMainWindow::ChangeTabLabel(class Connection *Con,
 *          const char *NewLabel);
 *
 * PARAMETERS:
 *    Con [I] -- The connection that is connected to the tab (which tab)
 *    NewLabel [I] -- The new label for this connection.
 *
 * FUNCTION:
 *    This function changes the name of a tab in this window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ChangeTabLabel(class Connection *Con,const char *NewLabel)
{
    t_UITabCtrl *MainTabs;
    string Name;
    t_UITab *ActiveTab;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    ActiveTab=UITabCtrlGetTabFromID(MainTabs,(uintptr_t)Con);
    if(ActiveTab!=NULL)
        UITabCtrlSetTabLabel(MainTabs,ActiveTab,NewLabel);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::SetURIText
 *
 * SYNOPSIS:
 *    void TheMainWindow::SetURIText(const char *NewURI);
 *
 * PARAMETERS:
 *    NewURI [I] -- The string to set the URI text input to
 *
 * FUNCTION:
 *    This function changes the text in the URI input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::SetURIText(const char *NewURI)
{
    t_UITextInputCtrl *URIInput;

    URIInput=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_URI);
    UISetTextCtrlText(URIInput,NewURI);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ToggleShowNonPrintables
 *
 * SYNOPSIS:
 *    void TheMainWindow::ToggleShowNonPrintables(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the show non-printables display for the active
 *    connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ToggleShowNonPrintables(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->SetShowNonPrintable(!TabCon->GetShowNonPrintable());

    RethinkActiveConnectionUI();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ToggleShowEndOfLines
 *
 * SYNOPSIS:
 *    void TheMainWindow::ToggleShowEndOfLines(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the show non-printables display for the active
 *    connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ToggleShowEndOfLines(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->SetShowEndOfLines(!TabCon->GetShowEndOfLines());

    RethinkActiveConnectionUI();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::SelectAll
 *
 * SYNOPSIS:
 *    void TheMainWindow::SelectAll(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function select everything in the display of the active connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::SelectAll(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->SelectAll();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ZoomIn
 *
 * SYNOPSIS:
 *    void TheMainWindow::ZoomIn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function zooms in on the active connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ZoomIn(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->ZoomIn();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ZoomOut
 *
 * SYNOPSIS:
 *    void TheMainWindow::ZoomOut(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function zooms out on the active connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ZoomOut(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->ZoomOut();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ResetZoom
 *
 * SYNOPSIS:
 *    void TheMainWindow::ResetZoom(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function zooms out on the active connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ResetZoom(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->ResetZoom();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::DoSendByte
 *
 * SYNOPSIS:
 *    void TheMainWindow::DoSendByte(uint8_t Byte);
 *
 * PARAMETERS:
 *    Byte [I] -- The byte to send
 *
 * FUNCTION:
 *    This function sends a raw byte.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::DoSendByte(uint8_t Byte)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->WriteData(&Byte,1,e_ConWriteSource_Keyboard);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::KeyPress
 *
 * SYNOPSIS:
 *    bool TheMainWindow::KeyPress(uint8_t Mods,e_UIKeys Key,
 *              const uint8_t *TextPtr,unsigned int TextLen);
 *
 * PARAMETERS:
 *    Mods [I] -- What keys where held when this key event happended
 *    Key [I] -- What "special" key was pressed (things like arrow keys)
 *    TextPtr [I] -- A string with the key in it (UTF8).  If the key can be
 *                   converted to a char this will have it.
 *    TextLen [I] -- The number of bytes in 'TextPtr'
 *
 * FUNCTION:
 *    This function is called when there is a key press in a main window.
 *
 * RETURNS:
 *    true -- The key press was consumed
 *    false -- The key press was ignored.
 *
 * SEE ALSO:
 *
 ******************************************************************************/
bool TheMainWindow::KeyPress(uint8_t Mods,e_UIKeys Key,const uint8_t *TextPtr,
        unsigned int TextLen)
{
    e_CmdType KeyShortCutCmd;
    char Letter;
//    t_UITabCtrl *MainTabs;
//    class Connection *TabCon;

    /* This is not the correct spot for this */
    Letter=0;
    if(TextLen>0)
        Letter=TextPtr[0];
    KeyShortCutCmd=KeySeq2CmdID(Mods,Key,Letter);
    if(KeyShortCutCmd!=e_CmdMAX)
    {
        /* This is a shortcut key */
        ExeCmd(KeyShortCutCmd);
        return true;
    }

    return false;
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::BookmarkCurrentTab
 *
 * SYNOPSIS:
 *    void TheMainWindow::BookmarkCurrentTab(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Adds a new bookmark of the current tab.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::BookmarkCurrentTab(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon==NULL)
        return;

    if(BookmarkConnection(TabCon))
    {
        /* Update GUI */
        MW_RebuildAllBookmarkMenus();
    }
    else
    {
        UIAsk("Error","Failed to add the bookmark",e_AskBox_Error,
                e_AskBttns_Ok);
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RebuildBookmarkMenu
 *
 * SYNOPSIS:
 *    void TheMainWindow::RebuildBookmarkMenu(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rebuilds the bookmarks menu for this main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::RebuildBookmarkMenu(void)
{
    e_UISubMenuCtrl *AddedMenu;
    e_UISubMenuCtrl *MenuParent;
    i_BookmarkList bm;
    unsigned int Index;
    t_BookmarkMenuFolderLookup FolderLookup;
    i_BookmarkMenuFolderLookup ExistingMenu;

    try
    {
        UIMW_AddBookmarkClearAllMenus(UIWin);

        Index=0;
        for(bm=g_BookmarkList.begin();bm!=g_BookmarkList.end();bm++,Index++)
        {
            /* See if we have added this folder already */
            if(bm->MenuName!="")
            {
                ExistingMenu=FolderLookup.find(bm->MenuName);
                if(ExistingMenu!=FolderLookup.end())
                {
                    /* All ready have this menu, add under here */
                    MenuParent=ExistingMenu->second;
                }
                else
                {
                    /* We need to add this one */
                    AddedMenu=UIMW_AddBookmarkSubMenu(UIWin,
                            bm->MenuName.c_str());
                    FolderLookup.insert(make_pair(bm->MenuName,AddedMenu));
                    MenuParent=AddedMenu;
                }
            }
            else
            {
                /* We are adding to the root */
                MenuParent=NULL;
            }

            UIMW_AddBookmarkMenuItem(UIWin,MenuParent,bm->Name.c_str(),Index);
        }
    }
    catch(...)
    {
        /* Not much we can do... */
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::GotoBookmark
 *
 * SYNOPSIS:
 *    void TheMainWindow::GotoBookmark(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The id of the bookmark to goto.  This is it index into
 *              the bookmarks list.
 *
 * FUNCTION:
 *    This function changes the current tab to a bookmark from the bookmark
 *    list (g_BookmarkList).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::GotoBookmark(uintptr_t ID)
{
    i_BookmarkList bm;
    unsigned int Index;
    class Connection *NewCon;
    class ConSettings *UseSettings;

    /* Find this bookmark */
    for(Index=0,bm=g_BookmarkList.begin();bm!=g_BookmarkList.end() && Index<ID;
            bm++,Index++)
    {
    }
    if(bm==g_BookmarkList.end())
    {
        /* Hu?  Shouldn't happen */
        return;
    }

    /* Figure out if we are using global settings or the bookmarks */
    if(bm->UseCustomSettings)
        UseSettings=&bm->CustomSettings;
    else
        UseSettings=&g_Settings.DefaultConSettings;

    if(g_Settings.BookmarksOpenNewTabs)
    {
        /* Ok, open a new tab */
        NewCon=AllocNewTab(bm->Name.c_str(),UseSettings,bm->URI.c_str(),
                bm->Options);
        if(NewCon==nullptr)
            return;    // We have already prompted
    }
    else
    {
        NewCon=ReloadTabFromURI(bm->Name.c_str(),UseSettings,bm->URI.c_str());
        if(NewCon==NULL)
        {
            UIAsk("Error","Failed to load tab from the URI string",
                    e_AskBox_Error,e_AskBttns_Ok);
            return;
        }

        /* Apply custom settings */
        if(bm->UseCustomSettings)
            NewCon->SetCustomSettings(bm->CustomSettings);
    }

    /* Apply options */
    if(!NewCon->SetConnectionOptions(bm->Options))
    {
        UIAsk("Error","There was an error applying connection options",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    NewCon->Connect2Bookmark(bm->BookmarkUID);

    /* Change the tab name to the same as the bookmark */
    NewCon->SetDisplayName(bm->Name.c_str());

    /* Make this new connection the active one */
    SetActiveTab(NewCon);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::IsThisYourUIWindow
 *
 * SYNOPSIS:
 *    bool TheMainWindow::IsThisYourUIWindow(t_UIMainWindow *GUIWin);
 *
 * PARAMETERS:
 *    GUIWin [I] -- The GUI window to look for
 *
 * FUNCTION:
 *    This function checks to see if a GUI window is the one attached to this
 *    main window.
 *
 * RETURNS:
 *    true -- This is the GUI window for this main window
 *    false -- Isn't connected to this main window.
 *
 * SEE ALSO:
 *
 ******************************************************************************/
bool TheMainWindow::IsThisYourUIWindow(t_UIMainWindow *GUIWin)
{
    if(GUIWin==UIWin)
        return true;
    return false;
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ShowPanel
 *
 * SYNOPSIS:
 *    void TheMainWindow::ShowPanel(e_MWPanelsType PanelID);
 *
 * PARAMETERS:
 *    PanelID [I] - The panel to make visible
 *
 * FUNCTION:
 *    This function opens the side / bottom panel with the requested panel in
 *    it and selects it's tab.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::ShowPanel(e_MWPanelsType PanelID)
{
    t_UITabCtrl *PanelCtrl;
    t_UITab *Tab;
    e_RightPanelTabIndexesType RightPanelTab;
    e_LeftPanelTabIndexesType LeftPanelTab;
    e_BottomPanelTabIndexesType BottomPanelTab;

    RightPanelTab=e_RightPanelTabIndexesMAX;
    LeftPanelTab=e_LeftPanelTabIndexesMAX;
    BottomPanelTab=e_BottomPanelTabIndexesMAX;

    switch(PanelID)
    {
        case e_MWPanels_ConnectionOptions:
            LeftPanelTab=e_LeftPanelTabIndexes_ConnectionOptions;
        break;
        case e_MWPanels_Capture:
            LeftPanelTab=e_LeftPanelTabIndexes_Capture;
        break;
        case e_MWPanels_Upload:
            LeftPanelTab=e_LeftPanelTabIndexes_Upload;
        break;
        case e_MWPanels_Downloadload:
            LeftPanelTab=e_LeftPanelTabIndexes_Download;
        break;
        case e_MWPanels_Bridge:
            LeftPanelTab=e_LeftPanelTabIndexes_Bridge;
        break;
        case e_MWPanels_AuxControls:
            LeftPanelTab=e_LeftPanelTabIndexes_AuxControls;
        break;

        case e_MWPanels_StopWatch:
            RightPanelTab=e_RightPanelTabIndexes_StopWatch;
        break;

        case e_MWPanels_Hex:
            BottomPanelTab=e_BottomPanelTabIndexes_Hex;
        break;
        case e_MWPanels_Injection:
            BottomPanelTab=e_BottomPanelTabIndexes_Injection;
        break;
        case e_MWPanels_Buffers:
            BottomPanelTab=e_BottomPanelTabIndexes_Buffers;
        break;
        case e_MWPanelsMAX:
        default:
        break;
    }

    if(RightPanelTab!=e_RightPanelTabIndexesMAX)
    {
        PanelCtrl=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_RightPanel);
        Tab=UITabCtrlGetTabFromIndex(PanelCtrl,RightPanelTab);
        UITabCtrlMakeTabActive(PanelCtrl,Tab);
        UIMW_SetRightPanel(UIWin,g_Session.RightPanelSize,true);
    }

    if(LeftPanelTab!=e_LeftPanelTabIndexesMAX)
    {
        PanelCtrl=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_LeftPanel);
        Tab=UITabCtrlGetTabFromIndex(PanelCtrl,LeftPanelTab);
        UITabCtrlMakeTabActive(PanelCtrl,Tab);

        UIMW_SetLeftPanel(UIWin,g_Session.LeftPanelSize,true);
    }

    if(BottomPanelTab!=e_BottomPanelTabIndexesMAX)
    {
        PanelCtrl=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_BottomPanel);
        Tab=UITabCtrlGetTabFromIndex(PanelCtrl,BottomPanelTab);
        UITabCtrlMakeTabActive(PanelCtrl,Tab);

        UIMW_SetBottomPanel(UIWin,g_Session.BottomPanelSize,true);
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RethinkActiveTabControls
 *
 * SYNOPSIS:
 *    void TheMainWindow::RethinkActiveTabControls(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to resetup things after the active tab changes.
 *    It will do things like rebuild the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::RethinkActiveTabControls(void)
{
    string UniqueID;
    t_KVList Options;
    string WinTitle;
    string URI;
    t_UITabCtrl *PanelTab;
    e_BottomPanelTabType BottomPanelTab;
    e_RightPanelTabType RightPanelTab;
    e_LeftPanelTabType LeftPanelTab;

    if(ActiveCon==NULL)
        return;

    ActiveCon->GetConnectionUniqueID(UniqueID);
    if(!ActiveCon->GetConnectionOptions(Options))
        return;

    /* Change the windows title */
    ActiveCon->GetDisplayName(WinTitle);
    WinTitle+=" - ";
    WinTitle+=WHIPPYTERM_NAME;
    UIMW_SetWindowTitle(UIWin,WinTitle.c_str());

    /* Tell the panels the connection has changed */
    ConnectionOptionsPanel.SetupConnectionOptionsPanel(UniqueID,Options);

    /* Change the URI */
    if(ActiveCon->GetURI(URI))
        SetURIText(URI.c_str());
    else
        SetURIText("");

    /* Reselect the tab in the panels that was selected before */
    PanelTab=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_LeftPanel);
    LeftPanelTab=ActiveCon->GetLeftPanelInfo();
    if(LeftPanelTab==e_LeftPanelTabMAX)
    {
        /* Grab from the UI and set it in the connection */
        LeftPanelTab=(e_LeftPanelTabType)UITabCtrlGetActiveTabIndex(PanelTab);
        if(LeftPanelTab<0 || LeftPanelTab>=e_LeftPanelTabMAX)
            LeftPanelTab=(e_LeftPanelTabType)0;
        ActiveCon->SetLeftPanelInfo(LeftPanelTab);
    }
    else
    {
        UITabCtrlMakeTabActiveUsingIndex(PanelTab,LeftPanelTab);
    }

    PanelTab=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_RightPanel);
    RightPanelTab=ActiveCon->GetRightPanelInfo();
    if(RightPanelTab==e_RightPanelTabMAX)
    {
        /* Grab from the UI and set it in the connection */
        RightPanelTab=(e_RightPanelTabType)UITabCtrlGetActiveTabIndex(PanelTab);
        if(RightPanelTab<0 || RightPanelTab>=e_RightPanelTabMAX)
            RightPanelTab=(e_RightPanelTabType)0;
        ActiveCon->SetRightPanelInfo(RightPanelTab);
    }
    else
    {
        UITabCtrlMakeTabActiveUsingIndex(PanelTab,RightPanelTab);
    }

    PanelTab=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_BottomPanel);
    BottomPanelTab=ActiveCon->GetBottomPanelInfo();
    if(BottomPanelTab==e_BottomPanelTabMAX)
    {
        /* Grab from the UI and set it in the connection */
        BottomPanelTab=(e_BottomPanelTabType)
                UITabCtrlGetActiveTabIndex(PanelTab);
        if(BottomPanelTab<0 || BottomPanelTab>=e_BottomPanelTabMAX)
            BottomPanelTab=(e_BottomPanelTabType)0;
        ActiveCon->SetBottomPanelInfo(BottomPanelTab);
    }
    else
    {
        UITabCtrlMakeTabActiveUsingIndex(PanelTab,BottomPanelTab);
    }
    RethinkActiveConnectionUI();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::SetActiveConnection
 *
 * SYNOPSIS:
 *    void TheMainWindow::SetActiveConnection(class Connection *NewCon);
 *
 * PARAMETERS:
 *    NewCon [I] -- The connection to make the new active connection.
 *
 * FUNCTION:
 *    This function changes the active connection.  It does not update
 *    the UI current tab only the internal view.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    SetActiveTab();
 ******************************************************************************/
void TheMainWindow::SetActiveConnection(class Connection *NewCon)
{
    /* Tell the panels the connection is about to changed */
    CapturePanel.ConnectionAbout2Changed();
    StopWatchPanel.ConnectionAbout2Changed();
    UploadPanel.ConnectionAbout2Changed();
    DownloadPanel.ConnectionAbout2Changed();
    HexDisplayPanel.ConnectionAbout2Changed();
    SendBuffersPanel.ConnectionAbout2Changed();
    BridgePanel.ConnectionAbout2Changed();
    ConnectionOptionsPanel.ConnectionAbout2Changed();
    AuxControlsPanel.ConnectionAbout2Changed();

    ActiveCon=NewCon;

    /* Tell the panels the connection has changed */
    CapturePanel.ConnectionChanged();
    StopWatchPanel.ConnectionChanged();
    UploadPanel.ConnectionChanged();
    DownloadPanel.ConnectionChanged();
    HexDisplayPanel.ConnectionChanged();
    SendBuffersPanel.ConnectionChanged();
    BridgePanel.ConnectionChanged();
    ConnectionOptionsPanel.ConnectionChanged();
    AuxControlsPanel.ConnectionChanged();

    RethinkActiveTabControls();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ConnectionEvent
 *
 * SYNOPSIS:
 *    void TheMainWindow::ConnectionEvent(const struct ConMWEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event
 *
 * FUNCTION:
 *    This function is called from the connection to tell the main window that
 *    something has happend.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::ConnectionEvent(const struct ConMWEvent *Event)
{
    switch(Event->EventType)
    {
        case ConMWEvent_ConOptionsChange:
            RethinkActiveTabControls();
        break;
        case ConMWEvent_NameChange:
            ChangeTabLabel(Event->Con,Event->Info->NameChange.NewName);
            RethinkActiveTabControls();
            BridgePanel.ConnectionAttribChanged();
        break;
        case ConMWEvent_StatusChange:
            ConnectionStatusChange(Event->Con);
        break;
        case ConMWEvent_StopWatchAddLap:
            StopWatchPanel.AddLapLine(Event->Info->SW.LapTime,
                    Event->Info->SW.LapDelta);
        break;
        case ConMWEvent_StopWatchAutoStarted:
            StopWatchPanel.TimerAutoStarted();
        break;
        case ConMWEvent_UploadStatUpdate:
            UploadPanel.UploadStatChanged();
        break;
        case ConMWEvent_UploadAborted:
            UploadPanel.InformOfUploadAborted();
        break;
        case ConMWEvent_UploadDone:
            UploadPanel.InformOfUploadDone();
        break;
        case ConMWEvent_NewConnection:
            UploadPanel.NewConnectionAllocated(Event->Con);
            BridgePanel.ConnectionAddedRemoved();
        break;
        case ConMWEvent_DownloadStatUpdate:
            DownloadPanel.DownloadStatChanged();
        break;
        case ConMWEvent_DownloadAborted:
            DownloadPanel.InformOfDownloadAborted();
        break;
        case ConMWEvent_DownloadDone:
            DownloadPanel.InformOfDownloadDone();
        break;
        case ConMWEvent_HexDisplayUpdate:
            HexDisplayPanel.InformOfUpdate(Event->Con,&Event->Info->HexDis);
        break;
        case ConMWEvent_HexDisplayBufferChange:
            HexDisplayPanel.InformOfBufferChange(Event->Con,
                    &Event->Info->HexDis);
        break;
        case ConMWEvent_BridgeStateChange:
            if(Event->Info->Bridged.BridgedTo==NULL)
            {
                BridgeActive=false;
                BridgedCon1=NULL;
                BridgedCon2=NULL;
            }
            else
            {
                BridgeActive=true;
                BridgedCon1=Event->Con;
                BridgedCon2=Event->Info->Bridged.BridgedTo;
            }

            BridgePanel.ConnectionBridgedChanged(BridgedCon1,BridgedCon2);

            RethinkBridgeMenu();
        break;
        case ConMWEvent_SelectionChanged:
            RethinkActiveConnectionUI();
        break;
        case ConMWEventMAX:
        default:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::CloseTab
 *
 * SYNOPSIS:
 *    void TheMainWindow::CloseTab(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- This connection to close
 *
 * FUNCTION:
 *    This function closes a tab that is linked to a connection.  It also
 *    frees the connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::CloseTab(class Connection *Con)
{
    if(Con!=NULL)
    {
        FreeTab(Con);
        BridgePanel.ConnectionAddedRemoved();
        RethinkBridgeMenu();

        /* Rethink the whole UI as we have just deleted a tab */
        RethinkActiveConnectionUI();
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::GetListOfConnections
 *
 * SYNOPSIS:
 *    void TheMainWindow::GetListOfConnections(t_MainWindowConnectionList &List);
 *
 * PARAMETERS:
 *    List [O] -- The list of connections.
 *
 * FUNCTION:
 *    This function gets a list of connections that are connected to this
 *    main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::GetListOfConnections(t_MainWindowConnectionList &List)
{
    t_UITabCtrl *MainTabs;
    int Tabs;
    t_UITab *Tab;
    int r;
    class Connection *Con;

    List.clear();

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);

    Tabs=UITabCtrlGetTabCount(MainTabs);
    for(r=0;r<Tabs;r++)
    {
        Tab=UITabCtrlGetTabFromIndex(MainTabs,r);
        if(Tab!=NULL)
        {
            Con=(class Connection *)UITabCtrlGetID(MainTabs,Tab);
            if(Con!=NULL)
            {
                List.push_back(Con);
            }
        }
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RethinkBridgeMenu
 *
 * SYNOPSIS:
 *    void TheMainWindow::RethinkBridgeMenu(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the name of the bridge connections menu.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::RethinkBridgeMenu(void)
{
    t_UITabCtrl *MainTabs;
    e_UIMenuCtrl *BridgeMenu;
    e_UIMenuCtrl *ReleaseBridgeMenu;
    bool EnableBridge;
    bool EnableRelease;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    BridgeMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_BridgeConnections);
    ReleaseBridgeMenu=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_ReleaseBridgedConnections);

    EnableBridge=false;
    EnableRelease=false;
    if(UITabCtrlGetTabCount(MainTabs)>=2)
    {
        if(BridgeActive)
            EnableRelease=true;
        else
            EnableBridge=true;
    }

    UIEnableMenu(BridgeMenu,EnableBridge);
    UIEnableMenu(ReleaseBridgeMenu,EnableRelease);
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::GetBridgedStateInfo
 *
 * SYNOPSIS:
 *    bool TheMainWindow::GetBridgedStateInfo(class Connection **Con1,
 *          class Connection **Con2);
 *
 * PARAMETERS:
 *    Con1 [I] -- A pointer to fill in with the first connection that is
 *                bridged in this main window.  Pass NULL to ignore.
 *    Con2 [I] -- A pointer to fill in with the second connection that is
 *                bridged in this main window.  Pass NULL to ignore.
 *
 * FUNCTION:
 *    This function gets info about any connection bridging that is in place.
 *
 * RETURNS:
 *    true -- There is a bridge in place
 *    false -- Connection in the main window are not bridged.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool TheMainWindow::GetBridgedStateInfo(class Connection **Con1,
        class Connection **Con2)
{
    if(Con1!=NULL)
        *Con1=BridgedCon1;
    if(Con2!=NULL)
        *Con2=BridgedCon2;

    return BridgeActive;
}

/*******************************************************************************
 * NAME:
 *    TheMainMenu::ShowConnectionSettings
 *
 * SYNOPSIS:
 *    void TheMainMenu::ShowConnectionSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user to change the current connections
 *    custom settings.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ShowConnectionSettings(void)
{
    i_BookmarkList bm;

    if(ActiveCon==NULL)
        return;

    if(RunSettingsDialog(this,&ActiveCon->CustomSettings,
            e_SettingsJump2_Default))
    {
        /* Mark that we are using custom settings */
        ActiveCon->UsingCustomSettings=true;

        /* Apply these custom settings */
        ActiveCon->ApplyCustomSettings();

        /* Find this bookmark */
        bm=FindBookmarkByUID(ActiveCon->GetConnectionBookmark());
        if(bm!=g_BookmarkList.end())
        {
            if(UIAsk("Apply Custom Settings","Do you also want to "
                    "apply these settings to the bookmark as well?",
                    e_AskBox_Question,e_AskBttns_YesNo)==e_AskRet_Yes)
            {
                bm->UseCustomSettings=true;
                bm->CustomSettings=ActiveCon->CustomSettings;
                SaveBookmarks();
            }
        }
        RethinkActiveConnectionUI();
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ResetConnectionCustomSettings
 *
 * SYNOPSIS:
 *    void TheMainWindow::ResetConnectionCustomSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function resets the active connections custom settings to the
 *    defaults.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ResetConnectionCustomSettings(void)
{
    i_BookmarkList bm;

    if(ActiveCon==NULL)
        return;

    if(UIAsk("Apply Default Settings","Are you sure you want to "
            "apply default settings to this connection?",
            e_AskBox_Question,e_AskBttns_YesNo)!=e_AskRet_Yes)
    {
        return;
    }

    ActiveCon->CustomSettings.DefaultSettings();
    ActiveCon->UsingCustomSettings=false;

    ActiveCon->ApplyCustomSettings();

    /* Find this bookmark */
    bm=FindBookmarkByUID(ActiveCon->GetConnectionBookmark());
    if(bm!=g_BookmarkList.end())
    {
        if(UIAsk("Apply Default Settings","Do you also want to "
                "apply default settings to the bookmark as well?",
                e_AskBox_Question,e_AskBttns_YesNo)==e_AskRet_Yes)
        {
            bm->CustomSettings.DefaultSettings();
            bm->UseCustomSettings=false;
            SaveBookmarks();
        }
    }
    RethinkActiveConnectionUI();
}

/*******************************************************************************
 * NAME:
 *    MW_DoMainWindowClose
 *
 * SYNOPSIS:
 *    static bool MW_DoMainWindowClose(class TheMainWindow *win);
 *
 * PARAMETERS:
 *    ID [I] -- The ID of the window (a pointer to the main window class)
 *
 * FUNCTION:
 *    This function is called when the main window closes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
static bool MW_DoMainWindowClose(class TheMainWindow *win)
{
    i_MainWindowsListType Search;

    /* Remove this window from the main list of windows */
    for(Search=m_MainWindowsList.begin();Search!=m_MainWindowsList.end();
            Search++)
    {
        if(*Search==win)
            break;
    }
    if(Search!=m_MainWindowsList.end())
    {
        /* Remove this entry */
        m_MainWindowsList.erase(Search);
    }

    /* Ok, the main window is about to go away, delete the main window class */
    delete win;

    if(m_MainWindowsList.empty())
    {
        /* Ok, we are quiting */
        AppShutdown();
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ClearScreen
 *
 * SYNOPSIS:
 *    void TheMainWindow::ClearScreen(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function looks up the current tab and sends a clear screen to it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ClearScreen(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->ClearScreen();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ClearScrollBackBuffer
 *
 * SYNOPSIS:
 *    void TheMainWindow::ClearScrollBackBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function looks up the current tab and sends a clear scroll back
 *    buffer to it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ClearScrollBackBuffer(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->ClearScrollBackBuffer();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::InsertHorizontalRule
 *
 * SYNOPSIS:
 *    void TheMainWindow::InsertHorizontalRule(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function looks up the current tab and sends an insert horizontal
 *    rule to it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::InsertHorizontalRule(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->InsertHorizontalRule();
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ResetTerm
 *
 * SYNOPSIS:
 *    void TheMainWindow::ResetTerm(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function looks up the current tab and sends an reset term command to
 *    it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ResetTerm(void)
{
    t_UITabCtrl *MainTabs;
    class Connection *TabCon;

    MainTabs=UIMW_GetTabCtrlHandle(UIWin,e_UIMWTabCtrl_MainTabs);
    TabCon=(class Connection *)UITabCtrlGetActiveTabID(MainTabs);
    if(TabCon!=NULL)
        TabCon->ResetTerm();
}

struct TermEmuSortCB
{
    inline bool operator() (const struct DPS_ProInfo &ent1,
            const struct DPS_ProInfo &ent2)
    {
        return strcasecmp(ent1.DisplayName,ent2.DisplayName)<0;
    }
};

/*******************************************************************************
 * NAME:
 *    TheMainWindow::RebuildTerminalEmulationMenu
 *
 * SYNOPSIS:
 *    void TheMainWindow::RebuildTerminalEmulationMenu(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears and fills in the quick change terminal emulation
 *    menu.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::RebuildTerminalEmulationMenu(void)
{
    t_DPS_ProInfoType TxtTerms;
    t_DPS_ProInfoType BinTerms;
    unsigned int r;

    UIMW_ApplyTerminalEmulationMenuClearAll(UIWin);

    DPS_GetListOfTextProcessors(e_TextDataProcessorClass_TermEmulation,
            TxtTerms);
    DPS_GetListOfBinaryProcessors(BinTerms);

    /* Sort them */
    std::sort(TxtTerms.begin(),TxtTerms.end(),
            TermEmuSortCB());
    std::sort(BinTerms.begin(),BinTerms.end(),TermEmuSortCB());

    for(r=0;r<TxtTerms.size();r++)
    {
        UIMW_Add2ApplyTerminalEmulationMenu(UIWin,TxtTerms[r].DisplayName,false,
                (uintptr_t)TxtTerms[r].IDStr);
    }

    for(r=0;r<BinTerms.size();r++)
    {
        UIMW_Add2ApplyTerminalEmulationMenu(UIWin,BinTerms[r].DisplayName,true,
                (uintptr_t)BinTerms[r].IDStr);
    }
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ApplyTerminalEmulationMenuTriggered
 *
 * SYNOPSIS:
 *    void TheMainWindow::ApplyTerminalEmulationMenuTriggered(uint64_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The ID of the menu that was triggered (this was added with
 *              UIMW_AddFTPUploadMenuItem()
 *
 * FUNCTION:
 *    This function is called when the user selects an item in the apply
 *    terminal emulation menu.  It changes the terminal emulation.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::ApplyTerminalEmulationMenuTriggered(uint64_t ID)
{
    const char *TermIdStr=(const char *)ID;
    const struct DataProcessor *DPro;

    if(TermIdStr==NULL)
        return;

    /* See if this new processor is binary or text */
    DPro=DPS_GetProcessorsInfo(TermIdStr);
    if(DPro==NULL)
        return;
    /* See if it's binary */
    if(DPro->Info.ProType==e_DataProcessorType_Binary)
        ActiveCon->CustomSettings.DataProcessorType=e_DataProcessorType_Binary;
    else
        ActiveCon->CustomSettings.DataProcessorType=e_DataProcessorType_Text;

    /* Replace the term emulator with the one selected */
    ActiveCon->CustomSettings.EnabledTermEmuDataProcessors.clear();
    ActiveCon->CustomSettings.EnabledTermEmuDataProcessors.push_back(TermIdStr);

    /* Now apply this change */
    ActiveCon->SetCustomSettings(ActiveCon->CustomSettings);

    SendBuffersPanel.ConnectionChanged();   // The connection may have changed (kinda, the type may have)
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::InformOfNewPluginInstalled
 *
 * SYNOPSIS:
 *    void TheMainWindow::InformOfNewPluginInstalled(const struct ExternPluginInfo *Info)
 *
 * PARAMETERS:
 *    Info [I] -- The info about this plugin
 *
 * FUNCTION:
 *    This function rebuilds the apply terminal emulation menu after a new
 *    plugin is installed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TheMainWindow::InformOfNewPluginInstalled(const struct ExternPluginInfo *Info)
{
    RebuildTerminalEmulationMenu();
}

/*******************************************************************************
 * NAME:
 *    MW_Event
 *
 * SYNOPSIS:
 *    bool MW_Event(const struct MWEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event we should process.
 *
 * FUNCTION:
 *    This function is called from the UI to tell us there was a UI event.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *
 ******************************************************************************/
bool MW_Event(const struct MWEvent *Event)
{
    i_MainWindowsListType MW;
    bool AcceptEvent;

    AcceptEvent=true;
    switch(Event->EventType)
    {
        case e_MWEvent_MainWindowClose:
            AcceptEvent=MW_DoMainWindowClose(Event->MW);
        break;
        case e_MWEvent_MenuTriggered:
            Event->MW->ExeCmd(MainMenu2Cmd(Event->Info.Menu.InputID));
        break;
        case e_MWEvent_BttnTriggered:
            switch(Event->Info.Bttn.InputID)
            {
                case e_UIMWBttn_SendBuffers_Send:
                    Event->MW->SendBuffersPanel.SendBttn2Cmd();
                break;
                case e_UIMWBttn_ConnectionOptionApply:
                case e_UIMWBttn_StopWatch_Start:
                case e_UIMWBttn_StopWatch_Reset:
                case e_UIMWBttn_StopWatch_Lap:
                case e_UIMWBttn_StopWatch_Clear:
                case e_UIMWBttn_Capture_Start:
                case e_UIMWBttn_Capture_SelectFilename:
                case e_UIMWBttn_Upload_Start:
                case e_UIMWBttn_Upload_SelectFilename:
                case e_UIMWBttn_Upload_Abort:
                case e_UIMWBttn_Download_Start:
                case e_UIMWBttn_Download_Abort:
                case e_UIMWBttn_HexDisplay_Clear:
                case e_UIMWBttn_HexDisplay_Copy:
                case e_UIMWBttn_HexDisplay_CopyAs:
                case e_UIMWBttn_SendBuffers_Edit:
                case e_UIMWBttn_Bridge_Bridge:
                case e_UIMWBttn_Bridge_Release:
                case e_UIMWBttnMAX:
                default:
                    Event->MW->ExeCmd(MainWindowBttn2Cmd(Event->
                            Info.Bttn.InputID));
                break;
            }
        break;
        case e_MWEvent_ToolbarTriggered:
            Event->MW->ExeCmd(MainToolBar2Cmd(Event->Info.Toolbar.InputID));
        break;
        case e_MWEvent_CheckboxTriggered:
            Event->MW->ExeCmd(MainCheckboxes2Cmd(Event->Info.Checkbox.InputID));
        break;
        case e_MWEvent_TxtInputEditFinished:
            switch(Event->Info.Txt.InputID)
            {
                case e_UIMWTxtInput_URI:
                    Event->MW->HandleGoURIToolBttnEnabled();
                break;
                case e_UIMWTxtInput_Capture_Filename:
                    Event->MW->CapturePanel.NoteFilenameChanged();
                break;
                case e_UIMWTxtInput_Upload_Filename:
                    Event->MW->UploadPanel.NoteFilenameChanged();
                break;
                case e_UIMWTxtInputMAX:
                default:
                break;
            }
        break;
        case e_MWEvent_ComboBoxChanged:
            switch(Event->Info.ComboBox.InputID)
            {
                case e_UIMWComboBox_Upload_Protocol:
                    Event->MW->UploadPanel.UploadProtocolChange(Event->ID);
                break;
                case e_UIMWComboBox_Download_Protocol:
                    Event->MW->DownloadPanel.DownloadProtocolChange(Event->ID);
                break;
                case e_UIMWComboBox_Bridge_Connection1:
                case e_UIMWComboBox_Bridge_Connection2:
                    Event->MW->BridgePanel.SelectedConnectionChanged();
                break;
                case e_UIMWComboBoxMAX:
                default:
                break;
            }
        break;
        case e_MWEvent_FirstShow:
            Event->MW->Init();
        break;
        case e_MWEvent_WindowResize:
            g_Session.WindowWidth=Event->Info.NewSize.Width;
            g_Session.WindowHeight=Event->Info.NewSize.Height;
            NoteSessionChanged();
        break;
        case e_MWEvent_LeftPanelSizeChange:
            g_Session.LeftPanelSize=Event->Info.PanelInfo.NewSize;
            g_Session.LeftPanelOpen=Event->Info.PanelInfo.PanelOpen;
            NoteSessionChanged();
        break;
        case e_MWEvent_RightPanelSizeChange:
            g_Session.RightPanelSize=Event->Info.PanelInfo.NewSize;
            g_Session.RightPanelOpen=Event->Info.PanelInfo.PanelOpen;
            NoteSessionChanged();
        break;
        case e_MWEvent_BottomPanelSizeChange:
            g_Session.BottomPanelSize=Event->Info.PanelInfo.NewSize;
            g_Session.BottomPanelOpen=Event->Info.PanelInfo.PanelOpen;
            NoteSessionChanged();
        break;
        case e_MWEvent_WindowSet2Maximized:
            g_Session.AppMaximized=Event->Info.MaximizedInfo.Max;
            NoteSessionChanged();
        break;
        case e_MWEvent_WindowMoved:
            g_Session.WindowPosX=Event->Info.Moved.x;
            g_Session.WindowPosY=Event->Info.Moved.y;
            NoteSessionChanged();
        break;
        case e_MWEvent_MainWindowKeyPress:
            AcceptEvent=Event->MW->KeyPress(Event->Info.Key.Mods,
                    Event->Info.Key.Key,Event->Info.Key.Text,
                    Event->Info.Key.TextLen);
        break;
        case e_MWEvent_BookmarkMenuSelected:
            /* We need to find the window that this bookmark was selected in */
            for(MW=m_MainWindowsList.begin();MW!=m_MainWindowsList.end();MW++)
            {
                if((*MW)->IsThisYourUIWindow(Event->UIWindow))
                {
                    (*MW)->GotoBookmark(Event->ID);
                    break;
                }
            }
        break;
        case e_MWEvent_TabClose:
            Event->MW->CloseTab((class Connection *)Event->ID);
        break;
        case e_MWEvent_StopWatch_Timer:
            Event->MW->StopWatchPanel.UpdateDisplayedTime();
        break;
        case e_MWEvent_UploadMenuTriggered:
            Event->MW->UploadPanel.UploadMenuTriggered(Event->ID);
        break;
        case e_MWEvent_DownloadMenuTriggered:
            Event->MW->DownloadPanel.DownloadMenuTriggered(Event->ID);
        break;
        case e_MWEvent_PanelTabChange:
            switch(Event->Info.PanelTab.TabID)
            {
                case e_UIMWTabCtrl_MainTabs:
                    Event->MW->SetActiveConnection((class Connection *)
                            Event->ID);
                break;
                case e_UIMWTabCtrl_LeftPanel:
                    if(Event->MW->ActiveCon!=NULL)
                    {
                        Event->MW->ActiveCon->SetLeftPanelInfo(
                                (e_LeftPanelTabType)Event->Info.PanelTab.
                                NewIndex);
                    }
                break;
                case e_UIMWTabCtrl_RightPanel:
                    if(Event->MW->ActiveCon!=NULL)
                    {
                        Event->MW->ActiveCon->SetRightPanelInfo(
                                (e_RightPanelTabType)Event->Info.PanelTab.
                                NewIndex);
                    }
                break;
                case e_UIMWTabCtrl_BottomPanel:
                    if(Event->MW->ActiveCon!=NULL)
                    {
                        Event->MW->ActiveCon->SetBottomPanelInfo(
                                (e_BottomPanelTabType)Event->Info.PanelTab.
                                NewIndex);
                        Event->MW->HexDisplayPanel.InformOfPanelTabChange(
                                (e_BottomPanelTabType)Event->Info.PanelTab.
                                NewIndex);
                    }
                break;
                case e_UIMWTabMAX:
                default:
                break;
            }
        break;
        case e_MWEvent_ListViewChange:
            switch(Event->Info.ListView.InputID)
            {
                case e_UIMWListView_StopWatch_Laps:
                break;
                case e_UIMWListViewMAX:
                default:
                break;
            }
        break;
        case e_MWEvent_ColumnViewChange:
            switch(Event->Info.ColumnView.InputID)
            {
                case e_UIMWColumnView_Buffers_List:
                    Event->MW->SendBuffersPanel.
                            SelectedBufferChanged(Event->Info.ColumnView.NewRow);
                break;
                case e_UIMWColumnViewMAX:
                default:
                break;
            }
        break;
        case e_MWEvent_ApplyTerminalEmulationMenuTriggered:
            Event->MW->ApplyTerminalEmulationMenuTriggered(Event->ID);
        break;
        case e_MWEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}

/*******************************************************************************
 * NAME:
 *    TheMainWindow::ExeCmd
 *
 * SYNOPSIS:
 *    void TheMainWindow::ExeCmd(e_CmdType Cmd);
 *
 * PARAMETERS:
 *    Cmd [I] -- The command to exe.
 *                  e_Cmd_NewTab -- Open a new connection.
 *                  e_Cmd_ResetTerm -- Resets the term
 *                  e_Cmd_InsertHorizontalRule -- A horizontal rule replaces
 *                          the current line.  Cursor is moved to the next line.
 *                  e_Cmd_ClearScreen -- Clear the screen.
 *                  e_Cmd_ClearScrollBackBuffer -- Clear the scroll back buffer
 *                  e_Cmd_CloseTab -- Close the active connection
 *                  e_Cmd_CloseAll -- Close all the open tabs in this window
 *                  e_Cmd_Quit -- Quit the program
 *                  e_Cmd_About -- Show about dialog
 *                  e_Cmd_Settings -- Show the settings dialog
 *                  e_Cmd_ImportSettings -- Ask for a filename and import
 *                      settings
 *                  e_Cmd_ExportSettings -- Ask for a filename and export
 *                      settings
 *                  e_Cmd_Connect -- Open the active connection
 *                  e_Cmd_Disconnect -- Close the active connection
 *                  e_Cmd_Copy -- Copy selected text to the clipboard
 *                  e_Cmd_Paste -- Paste the text from the clipboard
 *                  e_Cmd_GotoColumn -- Let the user change the cursor column
 *                  e_Cmd_GotoRow -- Let the user change the cursor row
 *                  e_Cmd_ConnectToggle -- Toggle the connect / disconnect
 *                      status of active connection.
 *                  e_Cmd_BridgeCurrentConnection -- Show the bridge current
 *                      connection dialog.
 *                  e_Cmd_ReleaseBridgedConnections -- Release the bridged
 *                      connections
 *                  e_Cmd_SelectAll -- Select all in the connection
 *                  e_Cmd_ZoomIn -- Zoom in (make font bigger)
 *                  e_Cmd_ZoomOut -- Zoom out (make font smaller)
 *                  e_Cmd_ResetZoom -- Reset the zoom level to settings.
 *                  e_Cmd_Send_NULL -- Send a 0x00
 *                  e_Cmd_Send_Backspace -- Send a 0x08
 *                  e_Cmd_Send_Tab -- Send a 0x09
 *                  e_Cmd_Send_Line_Feed -- Send a 0x0A
 *                  e_Cmd_Send_Form_Feed -- Send a 0x0C
 *                  e_Cmd_Send_Carriage_Return -- Send a 0x0D
 *                  e_Cmd_Send_Escape -- Send a 0x1B
 *                  e_Cmd_Send_Delete -- Send a 0x7F
 *                  e_Cmd_Send_Other -- Send a byte
 *                  e_Cmd_SettingsQuickJump_TermSize -- Goto settings term size
 *                  e_Cmd_SettingsQuickJump_TermEmu -- Goto settings term emu
 *                  e_Cmd_SettingsQuickJump_Font -- Goto settings font
 *                  e_Cmd_SettingsQuickJump_Colors -- Goto settings colors
 *                  e_Cmd_SaveSendBufferSet -- Prompt and save a send buffer set to disk
 *                  e_Cmd_LoadSendBufferSet -- Prompt and load a send buffer set from disk
 *                  e_Cmd_SendBufferSendGeneric -- Prompt user for what buffer to send
 *                  e_Cmd_SendBuffer_EditPrompted -- Prompt and edit a send buffer
 *
 * FUNCTION:
 *    This function executes a command.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
void TheMainWindow::ExeCmd(e_CmdType Cmd)
{
    t_UITextInputCtrl *URIInput;
    char URIBuffer[MAX_URI_LENGTH];
    string path;
    string file;
    string FullFilename;

    switch(Cmd)
    {
        case e_Cmd_NOP:
            /* Does nothing */
        break;
        case e_Cmd_NewTab:
            NewConnection();
        break;
        case e_Cmd_ResetTerm:
            ResetTerm();
        break;
        case e_Cmd_InsertHorizontalRule:
            InsertHorizontalRule();
        break;
        case e_Cmd_ClearScreen:
            ClearScreen();
        break;
        case e_Cmd_ClearScrollBackBuffer:
            ClearScrollBackBuffer();
        break;
        case e_Cmd_CloseTab:
            CloseActiveConnection();
        break;
        case e_Cmd_CloseAll:
            CloseAllConnections();
        break;
        case e_Cmd_Quit:
            if(m_MainWindowsList.size()>1)
            {
                if(UIAsk("Exit program","Are you sure you want to exit?",
                        e_AskBox_Warning,e_AskBttns_YesNo)==e_AskRet_No)
                {
                    return;
                }
            }

            AppShutdown();
            UIExit(0);
        break;
        case e_Cmd_About:
            RunAboutDialog();
        break;
        case e_Cmd_Settings:
            RunSettingsDialog(this,NULL,e_SettingsJump2_Default);
        break;
        case e_Cmd_ImportSettings:
            g_Settings.GetDefaultSettingsPathAndName(path,file);

            if(UI_LoadFileReq("Import Settings",path,file,
                    "All Files|*.*\nSettings|*.cfg",1))
            {
                FullFilename=UI_ConcatFile2Path(path,file);
                LoadSettings(FullFilename.c_str());
            }
        break;
        case e_Cmd_ExportSettings:
            g_Settings.GetDefaultSettingsPathAndName(path,file);

            if(UI_SaveFileReq("Export Settings",path,file,
                    "All Files|*.*\nSettings|*.cfg",1))
            {
                FullFilename=UI_ConcatFile2Path(path,file);
                SaveSettings(FullFilename.c_str());
            }
        break;
        case e_Cmd_Connect:
            ChangeConnectStatus(true);
        break;
        case e_Cmd_Disconnect:
            ChangeConnectStatus(false);
        break;
        case e_Cmd_Copy:
            CopyActiveTabSelectionToClipboard();
        break;
        case e_Cmd_Paste:
            PasteFromClipboard();
        break;
        case e_Cmd_GotoColumn:
            GotoColumn();
        break;
        case e_Cmd_GotoRow:
            GotoRow();
        break;
        case e_Cmd_ConnectToggle:
            ToggleConnectStatus();
        break;
        case e_Cmd_ApplyConnectionOptions:
            ApplyConnectionOptions();
        break;
        case e_Cmd_ChangeConnectionName:
            ChangeCurrentConnectionName();
        break;
        case e_Cmd_ConnectionOptions:
            ShowConnectionOptions();
        break;
        case e_Cmd_ConnectionSettings:
            ShowConnectionSettings();
        break;
        case e_Cmd_URIGo:
            URIInput=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_URI);
            UIGetTextCtrlText(URIInput,URIBuffer,MAX_URI_LENGTH);
            ReloadTabFromURI(NULL,NULL,URIBuffer);
        break;
        case e_Cmd_AddBookmark:
            BookmarkCurrentTab();
        break;
        case e_Cmd_ManageBookmarks:
            ManageBookmarks();
            MW_RebuildAllBookmarkMenus();
        break;
        case e_Cmd_StopWatch_StartStopToggle:
            StopWatchPanel.ToggleStartStop();
        break;
        case e_Cmd_StopWatch_Start:
            StopWatchPanel.StartTimer();
        break;
        case e_Cmd_StopWatch_Stop:
            StopWatchPanel.StopTimer();
        break;
        case e_Cmd_StopWatch_Reset:
            StopWatchPanel.ResetTimer();
        break;
        case e_Cmd_StopWatch_Lap:
            StopWatchPanel.AddLap();
        break;
        case e_Cmd_StopWatch_Clear:
            StopWatchPanel.ClearLaps();
        break;
        case e_Cmd_StopWatch_AutoLap:
            StopWatchPanel.ToggleAutoLap();
        break;
        case e_Cmd_StopWatch_AutoStart:
            StopWatchPanel.ToggleAutoStartOnTx();
        break;
        case e_Cmd_Capture_Capture2File:
            CapturePanel.Prompt4FileAndStart();
        break;
        case e_Cmd_Capture_Start:
            CapturePanel.PressStartBttn();
        break;
        case e_Cmd_Capture_SelectFilename:
            CapturePanel.SelectFilename();
        break;
        case e_Cmd_Upload_Start:
            UploadPanel.Start();
        break;
        case e_Cmd_Upload_Abort:
            UploadPanel.Abort();
        break;
        case e_Cmd_Upload_SelectFilename:
            UploadPanel.SelectFilename();
        break;
        case e_Cmd_Capture_TimestampToggle:
            CapturePanel.ToggleTimestamp();
        break;
        case e_Cmd_Capture_AppendToggle:
            CapturePanel.ToggleAppend();
        break;
        case e_Cmd_Capture_StripCtrlCharsToggle:
            CapturePanel.ToggleStripCtrlChars();
        break;
        case e_Cmd_Capture_StripEscSeqToggle:
            CapturePanel.ToggleStripEscSeq();
        break;
        case e_Cmd_Capture_HexDumpToggle:
            CapturePanel.ToggleHexDump();
        break;
        case e_Cmd_Capture_Stop:
            CapturePanel.Stop();
        break;
        case e_Cmd_Download_Start:
            DownloadPanel.Start();
        break;
        case e_Cmd_Download_Abort:
            DownloadPanel.Abort();
        break;
        case e_Cmd_HexDisplay_PauseToggle:
            HexDisplayPanel.TogglePause();
        break;
        case e_Cmd_HexDisplay_Clear:
            HexDisplayPanel.Clear();
        break;
        case e_Cmd_HexDisplay_Copy:
            HexDisplayPanel.Copy2Clip();
        break;
        case e_Cmd_HexDisplay_CopyAs:
            HexDisplayPanel.CopyAs();
        break;
        case e_Cmd_SendBuffer_Edit:
            SendBuffersPanel.EditCurrentBuffer();
        break;
        case e_Cmd_SendBuffer_Send1:
            SendBuffersPanel.SendBuffer(0);
        break;
        case e_Cmd_SendBuffer_Send2:
            SendBuffersPanel.SendBuffer(1);
        break;
        case e_Cmd_SendBuffer_Send3:
            SendBuffersPanel.SendBuffer(2);
        break;
        case e_Cmd_SendBuffer_Send4:
            SendBuffersPanel.SendBuffer(3);
        break;
        case e_Cmd_SendBuffer_Send5:
            SendBuffersPanel.SendBuffer(4);
        break;
        case e_Cmd_SendBuffer_Send6:
            SendBuffersPanel.SendBuffer(5);
        break;
        case e_Cmd_SendBuffer_Send7:
            SendBuffersPanel.SendBuffer(6);
        break;
        case e_Cmd_SendBuffer_Send8:
            SendBuffersPanel.SendBuffer(7);
        break;
        case e_Cmd_SendBuffer_Send9:
            SendBuffersPanel.SendBuffer(8);
        break;
        case e_Cmd_SendBuffer_Send10:
            SendBuffersPanel.SendBuffer(9);
        break;
        case e_Cmd_SendBuffer_Send11:
            SendBuffersPanel.SendBuffer(10);
        break;
        case e_Cmd_SendBuffer_Send12:
            SendBuffersPanel.SendBuffer(11);
        break;
        case e_Cmd_SendBuffer_SendSelectedBuffer:
            SendBuffersPanel.SendCurrentBuffer();
        break;
        case e_Cmd_SendBuffer_ClearAllBuffers:
            if(UIAsk("Clear all send buffers",
                    "Are you sure you want to clear (erase) all send buffers?",
                    e_AskBox_Question,e_AskBttns_YesNo)==e_AskRet_Yes)
            {
                g_SendBuffers.ClearAllBuffers();
                SendBuffersPanel.NewSendBufferSetLoaded();
                g_SendBuffers.SaveBuffers();
            }
        break;
        case e_Cmd_SaveSendBufferSet:
            path="";
            file="SendBufferSet.wtb";
            if(UI_SaveFileReq("Save Send Buffer Set",path,file,
                    "All Files|*.*\nWhippy Term Send Buffer Set|*.wtb",1))
            {
                FullFilename=UI_ConcatFile2Path(path,file);
                g_SendBuffers.SaveBuffers(FullFilename.c_str());
            }
        break;
        case e_Cmd_LoadSendBufferSet:
            path="";
            file="SendBufferSet.wtb";
            if(UI_LoadFileReq("Load Send Buffer Set",path,file,
                    "All Files|*.*\nWhippy Term Send Buffer Set|*.wtb",1))
            {
                FullFilename=UI_ConcatFile2Path(path,file);
                g_SendBuffers.LoadBuffers(FullFilename.c_str());
                SendBuffersPanel.NewSendBufferSetLoaded();

                /* Save out the newly loaded set as the default set */
                g_SendBuffers.SaveBuffers();
            }
        break;
        case e_Cmd_SendBufferSendGeneric:
            RunSendBufferSelectDialog(this,e_SBSD_Send);
        break;
        case e_Cmd_SendBuffer_EditPrompted:
            RunSendBufferSelectDialog(this,e_SBSD_Edit);
        break;
        case e_Cmd_Tools_ComTest:
            RunComTestDialog();
        break;
        case e_Cmd_TransmitDelay:
            ShowTransmitDelayDialog();
        break;
        case e_Cmd_GetPlugins:
            UI_GotoWebPage("http://whippyterm.com/plugins.php");
        break;
        case e_Cmd_InstallPlugin:
            PromptAndInstallPlugin();
        break;
        case e_Cmd_ManagePlugin:
            RunManagePluginsDialog();
        break;
        case e_Cmd_BridgeConnections:
            BridgePanel.BridgeConnections();
            RethinkBridgeMenu();
        break;
        case e_Cmd_ReleaseBridge:
            BridgePanel.ReleaseConnections();
            RethinkBridgeMenu();
        break;
        case e_Cmd_BridgeLockConnection1:
            BridgePanel.LockConnectionChange(0);
        break;
        case e_Cmd_BridgeLockConnection2:
            BridgePanel.LockConnectionChange(1);
        break;
        case e_Cmd_BridgeCurrentConnection:
            RunBridgeConDialog(this);
        break;
        case e_Cmd_ReleaseBridgedConnections:
            if(BridgedCon1!=NULL && BridgedCon2!=NULL)
            {
                if(UIAsk("Bridged connections",
                        "Undo the bridging of the two connections?",
                        e_AskBox_Question,e_AskBttns_YesNo)==e_AskRet_Yes)
                {
                    class Connection *Saved1;
                    class Connection *Saved2;

                    /* We need to save them because we can get an event that
                       will clear 'BridgedCon1' and 'BridgedCon2' */
                    Saved1=BridgedCon1;
                    Saved2=BridgedCon2;

                    Saved1->BridgeConnection(NULL);
                    Saved2->BridgeConnection(NULL);
                }
            }
        break;
        case e_Cmd_RestoreConnectionSettings:
            ResetConnectionCustomSettings();
        break;
        case e_Cmd_ShowNonPrintable:
            ToggleShowNonPrintables();
        break;
        case e_Cmd_ShowEndOfLines:
            ToggleShowEndOfLines();
        break;
        case e_Cmd_SelectAll:
            SelectAll();
        break;
        case e_Cmd_ZoomIn:
            ZoomIn();
        break;
        case e_Cmd_ZoomOut:
            ZoomOut();
        break;
        case e_Cmd_ResetZoom:
            ResetZoom();
        break;
        case e_Cmd_Send_NULL:
            DoSendByte(0x00);
        break;
        case e_Cmd_Send_Backspace:
            DoSendByte(0x08);
        break;
        case e_Cmd_Send_Tab:
            DoSendByte(0x09);
        break;
        case e_Cmd_Send_Line_Feed:
            DoSendByte(0x0A);
        break;
        case e_Cmd_Send_Form_Feed:
            DoSendByte(0x0C);
        break;
        case e_Cmd_Send_Carriage_Return:
            DoSendByte(0x0D);
        break;
        case e_Cmd_Send_Escape:
            DoSendByte(0x1B);
        break;
        case e_Cmd_Send_Delete:
            DoSendByte(0x7F);
        break;
        case e_Cmd_Send_Other:
            RunSendByteDialog(this);
        break;
        case e_Cmd_SettingsQuickJump_TermSize:
            RunSettingsDialog(this,NULL,e_SettingsJump2_TermSize);
        break;
        case e_Cmd_SettingsQuickJump_TermEmu:
            RunSettingsDialog(this,NULL,e_SettingsJump2_TermEmu);
        break;
        case e_Cmd_SettingsQuickJump_Font:
            RunSettingsDialog(this,NULL,e_SettingsJump2_Font);
        break;
        case e_Cmd_SettingsQuickJump_Colors:
            RunSettingsDialog(this,NULL,e_SettingsJump2_Colors);
        break;

        case e_CmdMAX:
        default:
        break;
    }
}

