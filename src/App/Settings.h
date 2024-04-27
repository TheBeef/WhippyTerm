/*******************************************************************************
 * FILENAME: Settings.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is an .h file.
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
#ifndef __SETTINGS_H_
#define __SETTINGS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/KeySeqs.h"
#include "App/Display/DisplayColors.h"
#include "App/Display/DisplayBase.h"
#include "App/MainWindow.h"
#include "App/Util/StorageHelpers.h"
#include "PluginSDK/DataProcessors.h"
#include "ThirdParty/TinyCFG/TinyCFG.h"
#include <string>
#include <list>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_WindowStartupPos_OSDefault=0,
    e_WindowStartupPos_RestoreFromSession,
    e_WindowStartupPos_RestoreFromSettings,
    e_WindowStartupPosMAX
} e_WindowStartupPosType;

typedef std::list<std::string> t_StringListType;
typedef t_StringListType::iterator i_StringListType;

/***  CLASS DEFINITIONS                ***/
/* These are settings that can be changed per connection */
class ConSettings
{
    public:
        uint32_t CursorColor;
        std::string FontName;
        int FontSize;
        bool FontBold;
        bool FontItalic;

        /* Colors */
        uint32_t SysColors[e_SysColShadeMAX][e_SysColMAX];
        uint32_t DefaultColors[e_DefaultColorsMAX];
        bool CursorBlink;

        /***** Terminal *****/
        bool TermSizeFixedWidth;
        bool TermSizeFixedHeight;
        int TermSizeWidth;
        int TermSizeHeight;
        unsigned int ScrollBufferLines;
        bool CenterTextInWindow;

        /* Input */
        e_DataProcessorTypeType DataProcessorType;
        t_StringListType EnabledTextDataProcessors;
        t_StringListType EnabledKeyPressProcessors;

        void RegisterAllMembers(class TinyCFG &cfg);
        void DefaultSettings(void);
};

/* Global settings + connection settings defaults */
class Settings
{
    public:
        /***** Panels *****/
        bool LeftPanelFromSettings;
        bool RightPanelFromSettings;
        bool BottomPanelFromSettings;
        bool LeftPanelAutoHide;
        bool RightPanelAutoHide;
        bool BottomPanelAutoHide;
        int LeftPanelSize;
        int RightPanelSize;
        int BottomPanelSize;
        bool LeftPanelOpenOnStartup;
        bool RightPanelOpenOnStartup;
        bool BottomPanelOpenOnStartup;

        /* Stop watch */
        bool StopWatchAutoLap;
        bool StopWatchAutoStart;
        bool StopWatchShowPanel;

        /* Capture */
        bool CaptureTimestamp;
        bool CaptureAppend;
        bool CaptureStripCtrl;
        bool CaptureStripEsc;
        bool CaptureHexDump;
        std::string CaptureDefaultFilename;
        bool CaptureShowPanel;

        /* Hex Display */
        bool HexDisplayEnabled;
        int HexDisplayBufferSize;

        /***** Display *****/
        bool AlwaysShowTabs;
        bool CloseButtonOnTabs;

        /***** Terminal *****/
        e_ScreenClearType ScreenClear;

        /* Hex Displays */
        uint32_t HexDisplaysFGColor;
        uint32_t HexDisplaysBGColor;
        uint32_t HexDisplaysSelBGColor;
        std::string HexDisplaysFontName;
        int HexDisplaysFontSize;
        bool HexDisplaysFontBold;
        bool HexDisplaysFontItalic;

        /***** Connections *****/
        bool AutoConnectOnNewConnection;
//        bool UseConnectionDefaults;
//        t_ConnectionsOptions DefaultConnectionsOptions;

        /* Keyboard */
        struct CommandKeySeq KeyMapping[e_CmdMAX];
        bool DotInputStartsAt0;

        /***** Behaviour *****/
        bool BookmarksOpenNewTabs;

        /***** Startup *****/
        e_WindowStartupPosType RestoreWindowPos;
        bool AppMaximized;
        int WindowPosX;
        int WindowPosY;
        int WindowWidth;
        int WindowHeight;

        /* Connection settings defaults */
        class ConSettings DefaultConSettings;

        Settings();
        void RegisterAllMembers(class TinyCFG &cfg);
        bool GetDefaultSettingsPathAndName(std::string &Path,std::string &Filename);
        void DefaultSettings(void);

    private:
        bool RegisterWindowStartupPos(class TinyCFG &cfg,const char *XmlName,
              e_WindowStartupPosType &Data);
        bool RegisterScreenClear(class TinyCFG &cfg,const char *XmlName,
                e_ScreenClearType &Data);
        bool RegisterKeyCommandType(class TinyCFG &cfg,const char *XmlName,
                struct CommandKeySeq (*Data)[e_CmdMAX]);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern class Settings g_Settings;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool LoadSettings(const char *Filename=NULL);
bool SaveSettings(const char *Filename=NULL);
void ApplySettings(void);
bool AreConSettingsEqual(class ConSettings &Con1,class ConSettings &Con2);

#endif   /* end of "#ifndef __SETTINGS_H_" */
