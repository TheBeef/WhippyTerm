/*******************************************************************************
 * FILENAME: ScriptingSystem.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    The .h file for the scripting system.
 *
 * COPYRIGHT:
 *    Copyright 25 Jan 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (25 Jan 2026)
 *       Created
 *
 *******************************************************************************/
#ifndef __SCRIPTINGSYSTEM_H_
#define __SCRIPTINGSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct ScriptHandle;

/* DEBUG PAUL: Move this out into it's own file??? */
typedef enum
{
    e_SysScript_ManualScript,
    e_SysScript_OnConenctScript,
    e_SysScript_OnDisconenctScript,
    e_SysScript_OnSendBufferScript,
    e_SysScript_OnOptionsChangeScript,
    e_SysScript_OnBridgeConnectsScript,
    e_SysScript_OnBreakBridgeConnectsScript,
    e_SysScript_OnCaptureStartScript,
    e_SysScript_OnCaptureStopScript,
    e_SysScript_OnUploadStartScript,
    e_SysScript_OnUploadDoneScript,
    e_SysScript_OnDownloadStartScript,
    e_SysScript_OnDownloadDoneScript,
    e_SysScript_OnStopWatchStartScript,
    e_SysScript_OnStopWatchStopScript,
    e_SysScript_OnClearScreenScript,
    e_SysScriptMAX
} e_SysScriptType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern struct ScriptingSystem_API g_ScriptingAPI;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void Scripting_Init(void);
void Scripting_InitPlugins(void);
void Scripting_AppendListOfFileTypeFilters(std::string &RetList);
void Scripting_InformOfNewPluginInstalled(const char *PluginIDStr);
void Scripting_InformOfPluginUninstalled(const char *PluginIDStr);
bool Scripting_CheckifPluginCanUninstall(const char *PluginIDStr);

/* Scripting control */
struct ScriptHandle *Scripting_LoadScript(const char *Filename);
void Scripting_RunScript(struct ScriptHandle *Handle);
void Scripting_AbortRunningScript(struct ScriptHandle *Handle);
void Scripting_FreeScriptHandle(struct ScriptHandle *Handle);

/* Scripting setup */
void Scripting_SetConnectedWindow(struct ScriptHandle *Handle,
        class TheMainWindow *MW,class Connection *Con);

void Scripting_KeyPress(struct ScriptHandle *Handle,class Connection *Con,
        uint8_t Mods,e_UIKeys Key,const uint8_t *TextPtr,unsigned int TextLen);
void Scripting_RecvBytes(struct ScriptHandle *Handle,class Connection *Con,
        const uint8_t *inbuff,unsigned int bytes);

#endif   /* end of "#ifndef __SCRIPTINGSYSTEM_H_" */
