/*******************************************************************************
 * FILENAME: MainWindow.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    The .h file
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
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Commands.h"
#include "App/Display/HexDisplayBuffers.h"
#include "App/IOSystem.h"
#include "App/Util/KeyValue.h"
#include "App/PluginSupport/ExternPluginsSystem.h"
#include "MWPanels/MWPanels.h"
#include "MWPanels/MW_ConnectionOptions.h"
#include "MWPanels/MW_Capture.h"
#include "MWPanels/MW_Upload.h"
#include "MWPanels/MW_Download.h"
#include "MWPanels/MW_StopWatch.h"
#include "MWPanels/MW_HexDisplay.h"
#include "MWPanels/MW_SendBuffers.h"
#include "MWPanels/MW_Bridge.h"
#include "MWPanels/MW_AuxControls.h"
#include "UI/UIMainWindow.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <list>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef std::list<class Connection *> t_MainWindowConnectionList;
typedef t_MainWindowConnectionList::iterator i_MainWindowConnectionList;

/***  CLASS DEFINITIONS                ***/
class TheMainWindow
{
    public:
void Debug1(void);
void Debug2(void);
void Debug3(void);
void Debug4(void);
void Debug5(void);
void Debug6(void);
        TheMainWindow();
        ~TheMainWindow();
        void Init(void);
        void Shutdown(void);
        void ShowWindow(void);
        void GetSizeAndPos(int &x,int &y,int &Width,int &Height);
        void NewConnection(bool MakeNewTab);
        class Connection *AllocNewTab(const char *TabLabel,class ConSettings *UseSettings,const char *URI,t_KVList *Options);
        class Connection *ReloadTabFromURI(const char *TabLabel,class ConSettings *UseSettings,const char *URI);
        void FreeTab(class Connection *Con);
        void RethinkActiveConnectionUI(void);
        void CloseActiveConnection(void);
        void CloseAllConnections(void);
        void ChangeConnectStatus(bool Connected);
        void Timer_CursorTimer(void);
        void ExeCmd(e_CmdType Cmd);
        bool KeyPress(uint8_t Mods,e_UIKeys Key,const uint8_t *TextPtr,unsigned int TextLen);
        void ApplySettings(void);
        void ChangeCurrentConnectionName(void);
        void ShowConnectionOptions(void);
        void ShowTransmitDelayDialog(void);
        void BookmarkCurrentTab(void);
        void GotoBookmark(uintptr_t ID);
        bool IsThisYourUIWindow(t_UIMainWindow *GUIWin);
        void RebuildBookmarkMenu(void);
        void ShowPanel(e_MWPanelsType PanelID);
        void SetActiveConnection(class Connection *NewCon);
        void SetActiveTab(class Connection *Con);
        void RethinkActiveTabControls(void);
        void ConnectionEvent(const struct ConMWEvent *Event);
        void CloseTab(class Connection *Con);
        void GetListOfConnections(t_MainWindowConnectionList &List);
        bool GetBridgedStateInfo(class Connection **Con1,
                class Connection **Con2);
        void DoSendByte(uint8_t Byte);
        void ApplyTerminalEmulationMenuTriggered(uint64_t ID);
        void InformOfNewPluginInstalled(const char *PluginIDStr);
        void InformOfPluginAboutToUninstall(const char *PluginIDStr);
        void InformOfPluginRemoved(const char *PluginIDStr);
        void HandleGoURIToolBttnEnabled(void);
        void InformOfSendBufferChange(int BufferIndex);
        void ToolbarMenuSelected(e_UIMWToolbarMenuType InputID);

        /* Panel handlers */
        class MWConnectionOptions ConnectionOptionsPanel;
        class MWStopWatch StopWatchPanel;
        class MWCapture CapturePanel;
        class MWUpload UploadPanel;
        class MWDownload DownloadPanel;
        class MWHexDisplay HexDisplayPanel;
        class MWSendBuffers SendBuffersPanel;
        class MWAuxControls AuxControlsPanel;

        class MWBridge BridgePanel;

        class Connection *ActiveCon;

    private:
        t_UIMainWindow *UIWin;
        void EarlyRestoreWindowSize(void);
        void RestoreFromSettings(void);
        void RestoreFromSession(void);
//        void SetupTabDisplay(void);
        int TextCanvasWidth;
        int TextCanvasHeight;
        bool BridgeActive;
        class Connection *BridgedCon1;
        class Connection *BridgedCon2;
        e_SysColType CurrentBGStyleColor;
        e_SysColShadeType CurrentBGStyleShade;

        class Connection *NoTabsConnection;

        void CopyActiveTabSelectionToClipboard(void);
        void PasteFromClipboard(void);
        void GotoColumn(void);
        void GotoRow(void);
        void ToggleConnectStatus(void);
        void ToggleAutoReconnect(void);
        void RemoveAllTabPanelControls(void);
        void ApplyConnectionOptions(void);
        void AllTabsClosed(void);
        void SetURIText(const char *NewURI);
        void ConnectionStatusChange(class Connection *Con);
        void ChangeTabLabel(class Connection *Con,const char *NewLabel);
        void RethinkBridgeMenu(void);
        void ShowConnectionSettings(void);
        void RethinkTabCountAfterFree(void);
        void ToggleShowNonPrintables(void);
        void ToggleShowEndOfLines(void);
        void SelectAll(void);
        void ClearScreen(void);
        void ClearScrollBackBuffer(void);
        void InsertHorizontalRule(void);
        void ResetTerm(void);
        void ZoomIn(void);
        void ZoomOut(void);
        void ResetZoom(void);
        void CloseConnection(class Connection *TabCon);
        void RebuildTerminalEmulationMenu(void);
        void MoveToNextTab(void);
        void MoveToPrevTab(void);
        void ApplyAttrib2Selection(uint32_t Attribs);
        void ApplyBGColor2Selection(void);
        void ApplyBGColor2Selection(e_SysColType Color,e_SysColShadeType Shade);
        void ChangeStyleBGColorSelectedColor(e_SysColType Color,e_SysColShadeType Shade);
        void DoFindCRCFromSelection(void);
        void DoCalcCRCFromSelection(void);
        void ToggleConnectionUseGlobalSettings(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool MW_AllocNewMainWindow(void);
void MW_ApplySettings(void);
void MW_RebuildAllBookmarkMenus(void);
void MW_InformOfNewPluginInstalled(const char *PluginIDStr);
void MW_InformOfPluginAboutToUninstall(const char *PluginIDStr);
void MW_InformOfPluginUninstalled(const char *PluginIDStr);
void MW_InformOfSendBufferChange(int BufferIndex);

#endif
