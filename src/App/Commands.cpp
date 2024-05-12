/*******************************************************************************
 * FILENAME: Commands.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file controls all the commands that the program does.  Things
 *    like new tab start as a menu / keypress and are translated into a
 *    function ID which is then sent to the program to be executed.
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
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
 *    Paul Hutchinson (11 Jul 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Commands.h"
#include "App/KeySeqs.h"
#include "App/Settings.h"
#include "UI/UIMainWindow.h"
#include "UI/UIAsk.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void SetKeySeq(struct CommandKeySeq *KeyMapping,uint8_t Mod,
        e_UIKeys Key,char Letter);

/*** VARIABLE DEFINITIONS     ***/
const char *m_CmdNames[]=
{
    "NOP",                                  // e_Cmd_NOP
    "NewTab",                               // e_Cmd_NewTab
    "ResetTerm",                            // e_Cmd_ResetTerm
    "InsertHorizontalRule",                 // e_Cmd_InsertHorizontalRule
    "ClearScreen",                          // e_Cmd_ClearScreen
    "ClearScrollBackBuffer",                // e_Cmd_ClearScrollBackBuffer
    "CloseTab",                             // e_Cmd_CloseTab
    "CloseAll",                             // e_Cmd_CloseAll
    "Quit",                                 // e_Cmd_Quit
    "About",                                // e_Cmd_About
    "Settings",                             // e_Cmd_Settings
    "ImportSettings",                       // e_Cmd_ImportSettings
    "ExportSettings",                       // e_Cmd_ExportSettings
    "Connect",                              // e_Cmd_Connect
    "Disconnect",                           // e_Cmd_Disconnect
    "Copy",                                 // e_Cmd_Copy
    "Paste",                                // e_Cmd_Paste
    "GotoColumn",                           // e_UIMWMenu_GotoColumn
    "GotoRow",                              // e_UIMWMenu_GotoRow
    "ConnectToggle",                        // e_Cmd_ConnectToggle
    "ApplyConnectionOptions",               // e_Cmd_ApplyConnectionOptions
    "ChangeConnectionName",                 // e_Cmd_ChangeConnectionName
    "ConnectionOptions",                    // e_Cmd_ConnectionOptions
    "ConnectionSettings",                   // e_Cmd_ConnectionSettings
    "URIGo",                                // e_Cmd_URIGo
    "AddBookmark",                          // e_Cmd_AddBookmark
    "ManageBookmarks",                      // e_Cmd_ManageBookmarks
    "StopWatchStartStopToggle",             // e_Cmd_StopWatch_StartStopToggle
    "StopWatchStart",                       // e_Cmd_StopWatch_Start
    "StopWatchStop",                        // e_Cmd_StopWatch_Stop
    "StopWatchReset",                       // e_Cmd_StopWatch_Reset
    "StopWatchLap",                         // e_Cmd_StopWatch_Lap
    "StopWatchClear",                       // e_Cmd_StopWatch_Clear
    "StopWatchAutoLap",                     // e_Cmd_StopWatch_AutoLap
    "StopWatchAutoStart",                   // e_Cmd_StopWatch_AutoStart
    "CaptureToFile",                        // e_Cmd_Capture_Capture2File
    "CaptureStart",                         // e_Cmd_Capture_Start
    "CaptureSelectFilename",                // e_Cmd_Capture_SelectFilename
    "CaptureTimestampToggle",               // e_Cmd_Capture_TimestampToggle
    "CaptureAppendToggle",                  // e_Cmd_Capture_AppendToggle
    "CaptureStripCtrlCharsToggle",          // e_Cmd_Capture_StripCtrlCharsToggle
    "CaptureStripEscSeqToggle",             // e_Cmd_Capture_StripEscSeqToggle
    "CaptureHexDumpToggle",                 // e_Cmd_Capture_HexDumpToggle
    "CaptureStop",                          // e_Cmd_Capture_Stop
    "UploadStart",                          // e_Cmd_Upload_Start
    "UploadSelectFilename",                 // e_Cmd_Upload_SelectFilename
    "UploadAbort",                          // e_Cmd_Upload_Abort
    "DownloadStart",                        // e_Cmd_Download_Start
    "DownloadAbort",                        // e_Cmd_Download_Abort
    "HexDisplayPauseToggle",                // e_Cmd_HexDisplay_PauseToggle
    "HexDisplayClear",                      // e_Cmd_HexDisplay_Clear
    "HexDisplayCopy",                       // e_Cmd_HexDisplay_Copy
    "HexDisplayCopyAs",                     // e_Cmd_HexDisplay_CopyAs
    "SendBufferEdit",                       // e_Cmd_SendBuffer_Edit
    "SendBufferSend1",                      // e_Cmd_SendBuffer_Send1
    "SendBufferSend2",                      // e_Cmd_SendBuffer_Send2
    "SendBufferSend3",                      // e_Cmd_SendBuffer_Send3
    "SendBufferSend4",                      // e_Cmd_SendBuffer_Send4
    "SendBufferSend5",                      // e_Cmd_SendBuffer_Send5
    "SendBufferSend6",                      // e_Cmd_SendBuffer_Send6
    "SendBufferSend7",                      // e_Cmd_SendBuffer_Send7
    "SendBufferSend8",                      // e_Cmd_SendBuffer_Send8
    "SendBufferSend9",                      // e_Cmd_SendBuffer_Send9
    "SendBufferSend10",                     // e_Cmd_SendBuffer_Send10
    "SendBufferSend11",                     // e_Cmd_SendBuffer_Send11
    "SendBufferSend12",                     // e_Cmd_SendBuffer_Send12
    "ToolsComTest",                         // e_Cmd_Tools_ComTest
    "TransmitDelay",                        // e_Cmd_TransmitDelay
    "GetPlugins",                           // e_Cmd_GetPlugins
    "InstallPlugin",                        // e_Cmd_InstallPlugin
    "ManagePlugin",                         // e_Cmd_ManagePlugin
    "BridgeConnections",                    // e_Cmd_BridgeConnections,
    "ReleaseBridge",                        // e_Cmd_ReleaseBridge,
    "BridgeLockConnection1",                // e_Cmd_BridgeLockConnection1
    "BridgeLockConnection2",                // e_Cmd_BridgeLockConnection2
    "BridgeCurrentConnection",              // e_Cmd_BridgeCurrentConnection
    "ReleaseBridgedConnections",            // e_Cmd_ReleaseBridgedConnections
    "RestoreConnectionSettings",            // e_Cmd_RestoreConnectionSettings
    "ShowNonPrintable",                     // e_Cmd_ShowNonPrintable
    "ShowEndOfLines",                       // e_Cmd_ShowEndOfLines
    "SelectAll",                            // e_Cmd_SelectAll
    "ZoomIn",                               // e_Cmd_ZoomIn
    "ZoomOut",                              // e_Cmd_ZoomOut
    "ResetZoom",                            // e_Cmd_ResetZoom
    "SendNULL",                             // e_Cmd_Send_NULL
    "SendBackspace",                        // e_Cmd_Send_Backspace
    "SendTab",                              // e_Cmd_Send_Tab
    "SendLineFeed",                         // e_Cmd_Send_Line_Feed
    "SendFormFeed",                         // e_Cmd_Send_Form_Feed
    "SendCarriageReturn",                   // e_Cmd_Send_Carriage_Return
    "SendEscape",                           // e_Cmd_Send_Escape
    "SendDelete",                           // e_Cmd_Send_Delete
    "SendOther",                            // e_Cmd_Send_Other
    "SettingsQuickJumpTermSize",            // e_Cmd_SettingsQuickJump_TermSize
    "SettingsQuickJumpTermEmu",             // e_Cmd_SettingsQuickJump_TermEmu
    "SettingsQuickJumpFont",                // e_Cmd_SettingsQuickJump_Font
    "SettingsQuickJumpColors",              // e_Cmd_SettingsQuickJump_Colors
};

e_CmdType m_Cmd2MenuMapping[]=
{
    e_Cmd_NewTab,                       // e_UIMWMenu_NewTab
    e_Cmd_ResetTerm,                    // e_UIMWMenu_ResetTerm
    e_Cmd_InsertHorizontalRule,         // e_UIMWMenu_InsertHorizontalRule
    e_Cmd_ClearScreen,                  // e_UIMWMenu_ClearScreen
    e_Cmd_ClearScrollBackBuffer,        // e_UIMWMenu_ClearScrollBackBuffer
    e_Cmd_CloseTab,                     // e_UIMWMenu_CloseTab
    e_Cmd_CloseAll,                     // e_UIMWMenu_CloseAll
    e_Cmd_Quit,                         // e_UIMWMenu_Quit
    e_Cmd_About,                        // e_UIMWMenu_About
    e_Cmd_Settings,                     // e_UIMWMenu_Settings
    e_Cmd_ImportSettings,               // e_UIMWMenu_ImportSettings
    e_Cmd_ExportSettings,               // e_UIMWMenu_ExportSettings
    e_Cmd_Connect,                      // e_UIMWMenu_Connect
    e_Cmd_Disconnect,                   // e_UIMWMenu_Disconnect
    e_Cmd_Copy,                         // e_UIMWMenu_Copy
    e_Cmd_Paste,                        // e_UIMWMenu_Paste
    e_Cmd_GotoColumn,                   // e_UIMWMenu_GotoColumn
    e_Cmd_GotoRow,                      // e_UIMWMenu_GotoRow
    e_Cmd_ChangeConnectionName,         // e_UIMWMenu_ChangeConnectionName
    e_Cmd_ConnectionOptions,            // e_UIMWMenu_ConnectionOptions
    e_Cmd_ConnectionSettings,           // e_UIMWMenu_ConnectionSettings
    e_Cmd_TransmitDelay,                // e_UIMWMenu_TransmitDelay
    e_Cmd_AddBookmark,                  // e_UIMWMenu_AddBookmark
    e_Cmd_ManageBookmarks,              // e_UIMWMenu_ManageBookmarks
    e_Cmd_StopWatch_Start,              // e_UIMWMenu_StopWatch_Start
    e_Cmd_StopWatch_Stop,               // e_UIMWMenu_StopWatch_Stop
    e_Cmd_StopWatch_Reset,              // e_UIMWMenu_StopWatch_Reset
    e_Cmd_StopWatch_Lap,                // e_UIMWMenu_StopWatch_Lap
    e_Cmd_StopWatch_Clear,              // e_UIMWMenu_StopWatch_Clear
    e_Cmd_StopWatch_AutoStart,          // e_UIMWMenu_StopWatch_StartOnTx
    e_Cmd_StopWatch_AutoLap,            // e_UIMWMenu_StopWatch_AutoLap
    e_Cmd_Capture_Capture2File,         // e_UIMWMenu_Capture_CaptureToFile
    e_Cmd_Capture_TimestampToggle,      // e_UIMWMenu_Capture_TimestampToggle
    e_Cmd_Capture_AppendToggle,         // e_UIMWMenu_Capture_AppendToggle
    e_Cmd_Capture_StripCtrlCharsToggle, // e_UIMWMenu_Capture_StripCtrlCharsToggle
    e_Cmd_Capture_StripEscSeqToggle,    // e_UIMWMenu_Capture_StripEscSeqToggle
    e_Cmd_Capture_HexDumpToggle,        // e_UIMWMenu_Capture_HexDumpToggle
    e_Cmd_Capture_Stop,                 // e_UIMWMenu_Capture_Stop
    e_Cmd_Tools_ComTest,                // e_UIMWMenu_Tools_ComTest
    e_Cmd_GetPlugins,                   // e_UIMWMenu_GetPlugins
    e_Cmd_InstallPlugin,                // e_UIMWMenu_InstallPlugin
    e_Cmd_ManagePlugin,                 // e_UIMWMenu_ManagePlugin
    e_Cmd_BridgeCurrentConnection,      // e_UIMWMenu_BridgeConnections
    e_Cmd_ReleaseBridgedConnections,    // e_UIMWMenu_ReleaseBridgedConnections
    e_Cmd_RestoreConnectionSettings,    // e_UIMWMenu_RestoreConnectionSettings
    e_Cmd_ShowNonPrintable,             // e_UIMWMenu_ShowNonPrintable
    e_Cmd_ShowEndOfLines,               // e_UIMWMenu_ShowEndOfLines
    e_Cmd_SelectAll,                    // e_UIMWMenu_SelectAll
    e_Cmd_ZoomIn,                       // e_UIMWMenu_ZoomIn
    e_Cmd_ZoomOut,                      // e_UIMWMenu_ZoomOut
    e_Cmd_ResetZoom,                    // e_UIMWMenu_ResetZoom
    e_Cmd_Send_NULL,                    // e_UIMWMenu_Send_NULL
    e_Cmd_Send_Backspace,               // e_UIMWMenu_Send_Backspace
    e_Cmd_Send_Tab,                     // e_UIMWMenu_Send_Tab
    e_Cmd_Send_Line_Feed,               // e_UIMWMenu_Send_Line_Feed
    e_Cmd_Send_Form_Feed,               // e_UIMWMenu_Send_Form_Feed
    e_Cmd_Send_Carriage_Return,         // e_UIMWMenu_Send_Carriage_Return
    e_Cmd_Send_Escape,                  // e_UIMWMenu_Send_Escape
    e_Cmd_Send_Delete,                  // e_UIMWMenu_Send_Delete
    e_Cmd_Send_Other,                   // e_UIMWMenu_Send_Other
    e_Cmd_SettingsQuickJump_TermSize,   // e_UIMWMenu_SettingsQuickJump_TermSize
    e_Cmd_SettingsQuickJump_TermEmu,    // e_UIMWMenu_SettingsQuickJump_TermEmu
    e_Cmd_SettingsQuickJump_Font,       // e_UIMWMenu_SettingsQuickJump_Font
    e_Cmd_SettingsQuickJump_Colors,     // e_UIMWMenu_SettingsQuickJump_Colors
};

e_CmdType m_Cmd2ToolbarMapping[]=
{
    e_Cmd_NewTab,                       // e_UIMWToolbar_NewTab
    e_Cmd_ConnectToggle,                // e_UIMWToolbar_ConnectToggle
    e_Cmd_URIGo,                        // e_UIMWToolbar_URIGo
    e_Cmd_Copy,                         // e_UIMWToolbar_Copy
    e_Cmd_Paste,                        // e_UIMWToolbar_Paste
};

e_CmdType m_Cmd2ButtonMapping[]=
{
    e_Cmd_ApplyConnectionOptions,       // e_UIMWBttn_ConnectionOptionApply
    e_Cmd_StopWatch_StartStopToggle,    // e_UIMWBttn_StopWatch_Start
    e_Cmd_StopWatch_Reset,              // e_UIMWBttn_StopWatch_Reset
    e_Cmd_StopWatch_Lap,                // e_UIMWBttn_StopWatch_Lap
    e_Cmd_StopWatch_Clear,              // e_UIMWBttn_StopWatch_Clear
    e_Cmd_Capture_Start,                // e_UIMWBttn_Capture_Start
    e_Cmd_Capture_SelectFilename,       // e_UIMWBttn_Capture_SelectFilename
    e_Cmd_Upload_Start,                 // e_UIMWBttn_Upload_Start
    e_Cmd_Upload_SelectFilename,        // e_UIMWBttn_Upload_SelectFilename
    e_Cmd_Upload_Abort,                 // e_UIMWBttn_Upload_Abort
    e_Cmd_Download_Start,               // e_UIMWBttn_Download_Start
    e_Cmd_Download_Abort,               // e_UIMWBttn_Download_Abort
    e_Cmd_HexDisplay_Clear,             // e_UIMWBttn_HexDisplay_Clear
    e_Cmd_HexDisplay_Copy,              // e_UIMWBttn_HexDisplay_Copy
    e_Cmd_HexDisplay_CopyAs,            // e_UIMWBttn_HexDisplay_CopyAs
    e_Cmd_SendBuffer_Edit,              // e_UIMWBttn_SendBuffers_Edit
    e_CmdMAX,                           // e_UIMWBttn_SendBuffers_Send      (Not used)
    e_Cmd_BridgeConnections,            // e_UIMWBttn_Bridge_Bridge
    e_Cmd_ReleaseBridge,                // e_UIMWBttn_Bridge_Release
};

e_CmdType m_Cmd2CheckboxMapping[]=
{
    e_Cmd_StopWatch_AutoStart,          // e_UIMWCheckbox_StopWatch_StartOnTx
    e_Cmd_StopWatch_AutoLap,            // e_UIMWCheckbox_StopWatch_AutoLap
    e_Cmd_Capture_TimestampToggle,      // e_UIMWCheckbox_Capture_Timestamp
    e_Cmd_Capture_AppendToggle,         // e_UIMWCheckbox_Capture_Append
    e_Cmd_Capture_StripCtrlCharsToggle, // e_UIMWCheckbox_Capture_StripCtrlChars
    e_Cmd_Capture_StripEscSeqToggle,    // e_UIMWCheckbox_Capture_StripEscSeq
    e_Cmd_Capture_HexDumpToggle,        // e_UIMWCheckbox_Capture_HexDump
    e_Cmd_HexDisplay_PauseToggle,       // e_UIMWCheckbox_HexDisplay_Paused
    e_Cmd_BridgeLockConnection1,        // e_UIMWCheckbox_Bridge_Lock1
    e_Cmd_BridgeLockConnection2,        // e_UIMWCheckbox_Bridge_Lock2
};

/*******************************************************************************
 * NAME:
 *    CmdCheck
 *
 * SYNOPSIS:
 *    void CmdCheck(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is a helper function for the programmer.  It checks that
 *    all the command stuff looks good.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void CmdCheck(void)
{
    /* Check if there is a problem with the name of the commands */
    if(sizeof(m_CmdNames)/sizeof(const char *)!=e_CmdMAX)
    {
        UIAsk("To programmer:\nYou do not have the correct number of entries in 'm_CmdNames'");
        exit(0);
    }

    if(sizeof(m_Cmd2MenuMapping)/sizeof(e_CmdType)!=e_UIMWMenuMAX)
    {
        UIAsk("To programmer:\n'm_Cmd2MenuMapping' does not have the correct number of entries.");
        exit(0);
    }

    if(sizeof(m_Cmd2CheckboxMapping)/sizeof(e_CmdType)!=e_UIMWCheckboxMAX)
    {
        UIAsk("To programmer:\n'm_Cmd2CheckboxMapping' does not have the correct number of entries.");
        exit(0);
    }

    if(sizeof(m_Cmd2ToolbarMapping)/sizeof(e_CmdType)!=e_UIMWToolbarMAX)
    {
        UIAsk("To programmer:\n'm_Cmd2ToolbarMapping' does not have the correct number of entries.");
        exit(0);
    }

    if(sizeof(m_Cmd2ButtonMapping)/sizeof(e_CmdType)!=e_UIMWBttnMAX)
    {
        UIAsk("To programmer:\n'm_Cmd2ButtonMapping' does not have the correct number of entries.");
        exit(0);
    }
}

/*******************************************************************************
 * NAME:
 *    Cmd2MainMenu
 *
 * SYNOPSIS:
 *    e_UIMWMenuType Cmd2MainMenu(e_CmdType Cmd);
 *
 * PARAMETERS:
 *    Cmd [I] -- The command ID to get the menu ID for.
 *
 * FUNCTION:
 *    This function converts a command ID into the menu item that goes with
 *    that command (if there is one)
 *
 * RETURNS:
 *    The menu ID or 'e_UIMWMenuMAX' if there is no match.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_UIMWMenuType Cmd2MainMenu(e_CmdType Cmd)
{
    int r;

    for(r=0;r<e_UIMWMenuMAX;r++)
        if(m_Cmd2MenuMapping[r]==Cmd)
            return (e_UIMWMenuType)r;
    return e_UIMWMenuMAX;
}

/*******************************************************************************
 * NAME:
 *    MainMenu2Cmd
 *
 * SYNOPSIS:
 *    e_CmdType MainMenu2Cmd(e_UIMWMenuType Menu);
 *
 * PARAMETERS:
 *    Menu [I] -- The menu to convert
 *
 * FUNCTION:
 *    This function converts a menu ID to a command ID.
 *
 * RETURNS:
 *    The command ID for this menu or 'e_CmdMAX' if there isn't one
 *
 * SEE ALSO:
 *    MainWindowBttn2Cmd(), MainToolBar2Cmd()
 ******************************************************************************/
e_CmdType MainMenu2Cmd(e_UIMWMenuType Menu)
{
    if(Menu>=sizeof(m_Cmd2MenuMapping)/sizeof(e_CmdType))
        return e_CmdMAX;
    return m_Cmd2MenuMapping[Menu];
}

/*******************************************************************************
 * NAME:
 *    MainToolBar2Cmd
 *
 * SYNOPSIS:
 *    e_CmdType MainToolBar2Cmd(e_UIMWToolbarType Tool);
 *
 * PARAMETERS:
 *    Tool [I] -- The toolbar button to convert
 *
 * FUNCTION:
 *    This function converts a toolbar button to a command ID.
 *
 * RETURNS:
 *    The command ID for this tool or 'e_CmdMAX' if there is no match.
 *
 * SEE ALSO:
 *    MainMenu2Cmd()
 ******************************************************************************/
e_CmdType MainToolBar2Cmd(e_UIMWToolbarType Tool)
{
    if(Tool>=sizeof(m_Cmd2ToolbarMapping)/sizeof(e_CmdType))
        return e_CmdMAX;
    return m_Cmd2ToolbarMapping[Tool];
}

/*******************************************************************************
 * NAME:
 *    MainCheckboxes2Cmd
 *
 * SYNOPSIS:
 *    e_CmdType MainCheckboxes2Cmd(e_UIMWCheckboxType CheckboxID);
 *
 * PARAMETERS:
 *    CheckboxID [I] -- The checkbox button to convert
 *
 * FUNCTION:
 *    This function converts a checkbox input to a command ID.
 *
 * RETURNS:
 *    The command ID for this checkbox or 'e_CmdMAX' if there is no match.
 *
 * SEE ALSO:
 *    MainMenu2Cmd()
 ******************************************************************************/
e_CmdType MainCheckboxes2Cmd(e_UIMWCheckboxType CheckboxID)
{
    if(CheckboxID>=sizeof(m_Cmd2CheckboxMapping)/sizeof(e_CmdType))
        return e_CmdMAX;
    return m_Cmd2CheckboxMapping[CheckboxID];
}

/*******************************************************************************
 * NAME:
 *    MainWindowBttn2Cmd
 *
 * SYNOPSIS:
 *    e_CmdType MainWindowBttn2Cmd(e_UIMWBttnType Bttn);
 *
 * PARAMETERS:
 *    Bttn [I] -- The button to convert
 *
 * FUNCTION:
 *    This function converts a button from the main window to a command ID.
 *
 * RETURNS:
 *    The command ID for this tool or 'e_CmdMAX' if there is no match.
 *
 * SEE ALSO:
 *    MainMenu2Cmd()
 ******************************************************************************/
e_CmdType MainWindowBttn2Cmd(e_UIMWBttnType Bttn)
{
    if(Bttn>=sizeof(m_Cmd2ButtonMapping)/sizeof(e_CmdType))
        return e_CmdMAX;
    return m_Cmd2ButtonMapping[Bttn];
}

/*******************************************************************************
 * NAME:
 *    GetCmdName
 *
 * SYNOPSIS:
 *    const char *GetCmdName(e_CmdType Cmd);
 *
 * PARAMETERS:
 *    Cmd [I] -- The command to convert
 *
 * FUNCTION:
 *    This function converts a command to a string.
 *
 * RETURNS:
 *    The name of the command as a string.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const char *GetCmdName(e_CmdType Cmd)
{
    if(Cmd>=e_CmdMAX)
        return "";
    return m_CmdNames[Cmd];
}

/*******************************************************************************
 * NAME:
 *    SetKeySeq
 *
 * SYNOPSIS:
 *    static void SetKeySeq(struct CommandKeySeq *KeyMapping,uint8_t Mod,
 *              e_UIKeys Key,char Letter);
 *
 * PARAMETERS:
 *    KeyMapping [O] -- The key seq to set
 *    Mod [I] -- The mod keys that need to be held
 *    Key [I] -- The key for this key seq or 'e_UIKeysMAX' to use 'Letter'
 *    Letter [I] -- The letter to use if we are not using 'Key'
 *
 * FUNCTION:
 *    This is a helper function for the DefaultCmdKeyMapping() function.
 *    It just sets the key seq.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DefaultCmdKeyMapping()
 ******************************************************************************/
static void SetKeySeq(struct CommandKeySeq *KeyMapping,uint8_t Mod,
        e_UIKeys Key,char Letter)
{
    KeyMapping->Mod=Mod;
    KeyMapping->Key=Key;
    KeyMapping->Letter=Letter;
}

/*******************************************************************************
 * NAME:
 *    DefaultCmdKeyMapping
 *
 * SYNOPSIS:
 *    void DefaultCmdKeyMapping(struct CommandKeySeq *KeyMapping);
 *
 * PARAMETERS:
 *    KeyMapping [I] -- An array of 'struct CommandKeySeq' of 'e_CmdMAX' size
 *          to default.
 *
 * FUNCTION:
 *    This function defaults all the key seq's to what command they do.
 *
 *    This is here instead of in the Key Seq's because you normally think
 *    of this command uses this key seq.  It naturally flows from the command
 *    to the seq that does that command.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DefaultCmdKeyMapping(struct CommandKeySeq *KeyMapping)
{
    int r;

    for(r=0;r<e_CmdMAX;r++)
    {
        KeyMapping[r].Mod=0;
        KeyMapping[r].Key=e_UIKeysMAX;
        KeyMapping[r].Letter=0;
    }

    SetKeySeq(&KeyMapping[e_Cmd_NewTab],KEYMOD_SHIFT|KEYMOD_CONTROL,e_UIKeysMAX,'T');
    // e_Cmd_ResetTerm
    // e_Cmd_InsertHorizontalRule
    // e_Cmd_ClearScreen
    // e_Cmd_ClearScrollBackBuffer
    SetKeySeq(&KeyMapping[e_Cmd_CloseTab],KEYMOD_SHIFT|KEYMOD_CONTROL,e_UIKeysMAX,'W');
    // e_Cmd_CloseAll
    SetKeySeq(&KeyMapping[e_Cmd_Quit],KEYMOD_SHIFT|KEYMOD_CONTROL,e_UIKeysMAX,'Q');
    // e_Cmd_About
    // e_Cmd_Settings
    // e_Cmd_ImportSettings
    // e_Cmd_ExportSettings
    // e_Cmd_Connect
    // e_Cmd_Disconnect
//    SetKeySeq(&KeyMapping[e_Cmd_Copy],KEYMOD_SHIFT|KEYMOD_CONTROL,e_UIKeysMAX,'C');
//    SetKeySeq(&KeyMapping[e_Cmd_Paste],KEYMOD_SHIFT|KEYMOD_CONTROL,e_UIKeysMAX,'V');
//    SetKeySeq(&KeyMapping[e_Cmd_ConnectToggle],KEYMOD_SHIFT|KEYMOD_CONTROL,e_UIKeysMAX,'I');
    // e_Cmd_ApplyConnectionOptions
    // e_Cmd_ChangeConnectionName
    // e_Cmd_ConnectionOptions
    // e_Cmd_ConnectionSettings
    // e_Cmd_URIGo
    // e_Cmd_AddBookmark
    // e_Cmd_ManageBookmarks
    // e_Cmd_StopWatch_StartStopToggle
    // e_Cmd_StopWatch_Start
    // e_Cmd_StopWatch_Stop
    // e_Cmd_StopWatch_Reset
    // e_Cmd_StopWatch_Lap
    // e_Cmd_StopWatch_Clear
    // e_Cmd_StopWatch_AutoLap
    // e_Cmd_StopWatch_AutoStart
    // e_Cmd_Capture_Capture2File
    // e_Cmd_Capture_Start
    // e_Cmd_Capture_SelectFilename
    // e_Cmd_Capture_TimestampToggle
    // e_Cmd_Capture_AppendToggle
    // e_Cmd_Capture_StripCtrlCharsToggle
    // e_Cmd_Capture_StripEscSeqToggle
    // e_Cmd_Capture_HexDumpToggle
    // e_Cmd_Capture_Stop
    // e_Cmd_Upload_Start
    // e_Cmd_Upload_SelectFilename
    // e_Cmd_Upload_Abort
    // e_Cmd_Download_Start
    // e_Cmd_Download_Abort
    // e_Cmd_HexDisplay_PauseToggle
    // e_Cmd_HexDisplay_Clear
    // e_Cmd_HexDisplay_Copy
    // e_Cmd_HexDisplay_CopyAs
    // e_Cmd_SendBuffer_Edit
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send1],KEYMOD_NONE,e_UIKeys_F1,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send2],KEYMOD_NONE,e_UIKeys_F2,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send3],KEYMOD_NONE,e_UIKeys_F3,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send4],KEYMOD_NONE,e_UIKeys_F4,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send5],KEYMOD_NONE,e_UIKeys_F5,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send6],KEYMOD_NONE,e_UIKeys_F6,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send7],KEYMOD_NONE,e_UIKeys_F7,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send8],KEYMOD_NONE,e_UIKeys_F8,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send9],KEYMOD_NONE,e_UIKeys_F9,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send10],KEYMOD_NONE,e_UIKeys_F10,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send11],KEYMOD_NONE,e_UIKeys_F11,0);
    SetKeySeq(&KeyMapping[e_Cmd_SendBuffer_Send12],KEYMOD_NONE,e_UIKeys_F12,0);
    // e_Cmd_Tools_ComTest
    // e_Cmd_TransmitDelay
    // e_Cmd_GetPlugins
    // e_Cmd_InstallPlugin
    // e_Cmd_ManagePlugin
    // e_Cmd_BridgeConnections,
    // e_Cmd_ReleaseBridge,
    // e_Cmd_BridgeLockConnection1
    // e_Cmd_BridgeLockConnection2
    // e_Cmd_BridgeCurrentConnection
    // e_Cmd_ReleaseBridgedConnections
    // e_Cmd_ShowNonPrintable
    // e_Cmd_ShowEndOfLines
    SetKeySeq(&KeyMapping[e_Cmd_SelectAll],KEYMOD_SHIFT|KEYMOD_CONTROL,e_UIKeysMAX,'A');
    SetKeySeq(&KeyMapping[e_Cmd_ZoomIn],KEYMOD_CONTROL,e_UIKeysMAX,'+');
    SetKeySeq(&KeyMapping[e_Cmd_ZoomOut],KEYMOD_CONTROL,e_UIKeysMAX,'-');
    SetKeySeq(&KeyMapping[e_Cmd_ResetZoom],KEYMOD_CONTROL,e_UIKeysMAX,'0');

/* Other commands / key seq do to:
 * Select All???    Shift+Ctrl+A
 * New Window       Shift+Ctrl+N
 * Find             Shift+Ctrl+F
 * Find Next        Shift+Ctrl+H (F3?)
 * Find Prev        Shift+Ctrl+G
 * Next Tab         Ctrl+Page Down
 * Prev Tab         Ctrl+Page Up
 * Tab 1            Alt-1
 * Tab 2            Alt-2
 * Tab 3            Alt-3
 * Tab 4            Alt-4
 * Tab 5            Alt-5
 * Tab 6            Alt-6
 * Tab 7            Alt-7
 * Tab 8            Alt-8
 * Tab 9            Alt-9
 * Tab 10           Alt-0
*/
}

e_CmdType KeySeq2CmdID(uint8_t Mod,e_UIKeys Key,char Letter)
{
    int r;
    for(r=0;r<e_CmdMAX;r++)
    {
        if(Mod==g_Settings.KeyMapping[r].Mod &&
                Key==g_Settings.KeyMapping[r].Key &&
                Letter==g_Settings.KeyMapping[r].Letter)
        {
            return (e_CmdType)r;
        }
    }
    return e_CmdMAX;
}
