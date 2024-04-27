/*******************************************************************************
 * FILENAME: Commands.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file is part of the Commands pair.
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
 * HISTORY:
 *    Paul Hutchinson (11 Jul 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __COMMANDS_H_
#define __COMMANDS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIMainWindow.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_Cmd_NewTab,
    e_Cmd_ResetTerm,
    e_Cmd_InsertHorizontalRule,
    e_Cmd_ClearScreen,
    e_Cmd_ClearScrollBackBuffer,
    e_Cmd_CloseTab,
    e_Cmd_Quit,
    e_Cmd_About,
    e_Cmd_Settings,
    e_Cmd_ImportSettings,
    e_Cmd_ExportSettings,
    e_Cmd_Connect,
    e_Cmd_Disconnect,
    e_Cmd_Copy,
    e_Cmd_Paste,
    e_Cmd_ConnectToggle,
    e_Cmd_ApplyConnectionOptions,
    e_Cmd_ChangeConnectionName,
    e_Cmd_ConnectionOptions,
    e_Cmd_ConnectionSettings,
    e_Cmd_URIGo,
    e_Cmd_AddBookmark,
    e_Cmd_ManageBookmarks,
    e_Cmd_StopWatch_StartStopToggle,
    e_Cmd_StopWatch_Start,
    e_Cmd_StopWatch_Stop,
    e_Cmd_StopWatch_Reset,
    e_Cmd_StopWatch_Lap,
    e_Cmd_StopWatch_Clear,
    e_Cmd_StopWatch_AutoLap,
    e_Cmd_StopWatch_AutoStart,
    e_Cmd_Capture_Capture2File,
    e_Cmd_Capture_Start,
    e_Cmd_Capture_SelectFilename,
    e_Cmd_Capture_TimestampToggle,
    e_Cmd_Capture_AppendToggle,
    e_Cmd_Capture_StripCtrlCharsToggle,
    e_Cmd_Capture_StripEscSeqToggle,
    e_Cmd_Capture_HexDumpToggle,
    e_Cmd_Capture_Stop,
    e_Cmd_Upload_Start,
    e_Cmd_Upload_SelectFilename,
    e_Cmd_Upload_Abort,
    e_Cmd_Download_Start,
    e_Cmd_Download_Abort,
    e_Cmd_HexDisplay_PauseToggle,
    e_Cmd_HexDisplay_Clear,
    e_Cmd_HexDisplay_Copy,
    e_Cmd_HexDisplay_CopyAs,
    e_Cmd_SendBuffer_Edit,
    e_Cmd_SendBuffer_Send1,
    e_Cmd_SendBuffer_Send2,
    e_Cmd_SendBuffer_Send3,
    e_Cmd_SendBuffer_Send4,
    e_Cmd_SendBuffer_Send5,
    e_Cmd_SendBuffer_Send6,
    e_Cmd_SendBuffer_Send7,
    e_Cmd_SendBuffer_Send8,
    e_Cmd_SendBuffer_Send9,
    e_Cmd_SendBuffer_Send10,
    e_Cmd_SendBuffer_Send11,
    e_Cmd_SendBuffer_Send12,
    e_Cmd_Tools_ComTest,
    e_Cmd_TransmitDelay,
    e_Cmd_GetPlugins,
    e_Cmd_InstallPlugin,
    e_Cmd_ManagePlugin,
    e_Cmd_BridgeConnections,
    e_Cmd_ReleaseBridge,
    e_Cmd_BridgeLockConnection1,
    e_Cmd_BridgeLockConnection2,
    e_Cmd_BridgeCurrentConnection,
    e_Cmd_ReleaseBridgedConnections,
    e_Cmd_RestoreConnectionSettings,
    e_Cmd_ShowNonPrintable,
    e_Cmd_ShowEndOfLines,
    e_CmdMAX
} e_CmdType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
e_UIMWMenuType Cmd2MainMenu(e_CmdType Cmd);
e_CmdType MainMenu2Cmd(e_UIMWMenuType Menu);
e_CmdType MainToolBar2Cmd(e_UIMWToolbarType Tool);
e_CmdType MainWindowBttn2Cmd(e_UIMWBttnType Bttn);
e_CmdType MainCheckboxes2Cmd(e_UIMWCheckboxType CheckboxID);
const char *GetCmdName(e_CmdType Cmd);
void DefaultCmdKeyMapping(struct CommandKeySeq *KeyMapping);
e_CmdType KeySeq2CmdID(uint8_t Mod,e_UIKeys Key,char Letter);

#endif
