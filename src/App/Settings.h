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
#include "App/Util/KeyValue.h"
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

typedef enum
{
    e_Beep_None,
    e_Beep_System,
    e_Beep_BuiltIn,
    e_Beep_AudioOnly,
    e_Beep_VisualOnly,
    e_BeepMAX
} e_BeepType;

typedef std::list<std::string> t_StringListType;
typedef t_StringListType::iterator i_StringListType;

typedef enum
{
    e_BackspaceKey_BS,
    e_BackspaceKey_DEL,
    e_BackspaceKeyMAX
} e_BackspaceKeyType;

typedef enum
{
    e_EnterKey_LF,
    e_EnterKey_CR,
    e_EnterKey_CRLF,
    e_EnterKeyMAX
} e_EnterKeyType;

typedef enum
{
    e_ClipboardMode_None,
    e_ClipboardMode_Normal,
    e_ClipboardMode_ShiftCtrl,
    e_ClipboardMode_Alt,
    e_ClipboardMode_Smart,
    e_ClipboardModeMAX
} e_ClipboardModeType;

/***  CLASS DEFINITIONS                ***/
struct PluginSettings
{
    public:
        std::string IDStr;
        t_KVList Settings;

        void RegisterAllMembers(class TinyCFG &cfg);
};
typedef std::list<struct PluginSettings> t_PluginSettings;
typedef t_PluginSettings::iterator i_PluginSettings;

/* These are settings that can be changed per connection */
class ConSettings
{
    public:
        uint32_t CursorColor;
        std::string FontName;
        int FontSize;
        bool FontBold;
        bool FontItalic;

        /***** Connections *****/
        bool AutoReopen;
        uint32_t AutoReopenWaitTime;

        /* Keyboard */
        e_BackspaceKeyType BackspaceKeyMode;
        e_EnterKeyType EnterKeyMode;
        e_ClipboardModeType ClipboardMode;

        /* Colors */
        uint32_t SysColors[e_SysColShadeMAX][e_SysColMAX];
        uint32_t DefaultColors[e_DefaultColorsMAX];
        uint32_t SelectionColors[e_ColorMAX];
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
        t_StringListType EnabledTermEmuDataProcessors;
        t_StringListType EnabledBinaryDataProcessors;

        /* Attribs on/off */
        bool BoldEnabled;
        bool ItalicEnabled;
        bool UnderlineEnabled;
        bool OverlineEnabled;
        bool ReverseEnabled;
        bool LineThroughEnabled;
        bool ColorsEnabled;

        /* Sounds */
        e_BeepType BeepMode;
        bool UseCustomSound;
        std::string BeepFilename;

        /* Plugins */
        t_PluginSettings PluginsSettings;

        void RegisterAllMembers(class TinyCFG &cfg);
        void DefaultSettings(void);
        bool RegisterBackspaceKey(class TinyCFG &cfg,const char *XmlName,e_BackspaceKeyType &Data);
        bool RegisterEnterKey(class TinyCFG &cfg,const char *XmlName,e_EnterKeyType &Data);
        bool RegisterClipboardMode(class TinyCFG &cfg,const char *XmlName,e_ClipboardModeType &Data);
        bool RegisterBeep(class TinyCFG &cfg,const char *XmlName,e_BeepType &Data);
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
        bool MouseCursorIBeam;

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

        /* Keyboard */

        /* Key bindings */
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
