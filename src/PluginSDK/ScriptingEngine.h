/*******************************************************************************
 * FILENAME: ScriptingEngine.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the plug in support for scripting languages in it.
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
#ifndef __SCRIPTINGENGINE_H_
#define __SCRIPTINGENGINE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginTypes.h"
#include "PluginUI.h"
#include <stdint.h>

/***  DEFINES                          ***/
/* Versions of struct ScriptingEngineAPI */
#define SCRIPTING_HANDLER_API_VERSION_1                 1

/* Versions of struct ScriptingSystem_API */
#define SCRIPTING_API_VERSION_1                         1

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef struct ScriptingEngineContext {int PrivateDataHere;} t_ScriptingEngineContextType;    // Fake type holder
typedef struct ScriptingEngineInst {int PrivateDataHere;} t_ScriptingEngineInstType;    // Fake type holder

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct ScriptingEngineAPI
{
    /********* Start of SCRIPTING_HANDLER_API_VERSION_1 *********/
    PG_BOOL (*Init)(void);
    t_ScriptingEngineContextType *(*AllocateContext)(t_ScriptingEngineInstType *Inst);
    void (*FreeContext)(t_ScriptingEngineContextType *Context);
    const char *(*GetLastError)(t_ScriptingEngineContextType *Context);
    PG_BOOL (*LoadScriptFromString)(t_ScriptingEngineContextType *Context,const char *Str);
    PG_BOOL (*RunLoadedScript)(t_ScriptingEngineContextType *Context);
    void (*AbortScript)(t_ScriptingEngineContextType *Context);
    void (*NewKeyPressDetected)(t_ScriptingEngineContextType *Context);
    /********* End of SCRIPTING_HANDLER_API_VERSION_1 *********/
};

struct ScriptingEngineInfo
{
    const char *IDStr;                              // What is the string we key things off.  This is used for the she-bang at the top of the script file (which will override the 'FileExt').  A-Z, a-z, and 0-9 only (no special chars)
    const char *DisplayName;                        // What to show in menu's etc.
    const char *LanguageGroup;                      // What is the name of the language being used (this would be things like "Basic", "Python", or "Perl")
    const char *LanguageVariant;                    // If this is a modified version of a language than what is the name of the base language this is a fork of
    const char *FileExt;                            // The file extension for the script on the disk (this will be used to detect the file type / when prompting for a filename)
    unsigned int ScriptingEngineAPISize;           /* The size of the ScriptingEngineAPI struct (sizeof(struct ScriptingEngineAPI)) */
    const struct ScriptingEngineAPI *API;          // The functions WhippyTerm can call in our plugin
};

/* !!!! You can only add to this.  Changing it will break the plugins !!!! */
struct ScriptingSystem_API
{
    /********* Start of SCRIPTING_API_VERSION_1 *********/
    PG_BOOL (*RegisterScriptingLanguage)(const struct ScriptingEngineInfo *Info,unsigned int SizeOfScriptingEngineInfo);
    const struct PI_UIAPI *(*GetAPI_UI)(void);
    uint32_t (*GetSysColor)(t_ScriptingEngineInstType *Inst,uint32_t SysColShade,uint32_t SysColor);
    uint32_t (*GetSysDefaultColor)(t_ScriptingEngineInstType *Inst,uint32_t DefaultColor);
    void (*DefaultStyleDataStruct)(t_ScriptingEngineInstType *Inst,struct StyleData *Struct2Default);
    void (*SetStyle)(t_ScriptingEngineInstType *Inst,struct StyleData *NewStyle);
    void (*GetStyle)(t_ScriptingEngineInstType *Inst,struct StyleData *CurStyle);
    unsigned int (*GetTitle)(t_ScriptingEngineInstType *Inst,char *Title,unsigned int MaxLen);
    void (*SetTitle)(t_ScriptingEngineInstType *Inst,const char *Title);
    void (*DoNewLine)(t_ScriptingEngineInstType *Inst);
    void (*DoReturn)(t_ScriptingEngineInstType *Inst);
    void (*DoBackspace)(t_ScriptingEngineInstType *Inst);
    void (*DoClearScreen)(t_ScriptingEngineInstType *Inst);
    void (*DoClearArea)(t_ScriptingEngineInstType *Inst,uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
    void (*DoTab)(t_ScriptingEngineInstType *Inst);
    void (*DoPrevTab)(t_ScriptingEngineInstType *Inst);
    void (*DoSystemBell)(t_ScriptingEngineInstType *Inst,int VisualOnly);
    void (*DoScrollArea)(t_ScriptingEngineInstType *Inst,uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t DeltaX,int32_t DeltaY);
    void (*DoClearScreenAndBackBuffer)(t_ScriptingEngineInstType *Inst);
    void (*GetCursorXY)(t_ScriptingEngineInstType *Inst,int32_t *RetCursorX,int32_t *RetCursorY);
    void (*SetCursorXY)(t_ScriptingEngineInstType *Inst,uint32_t X,uint32_t Y);
    void (*GetScreenSize)(t_ScriptingEngineInstType *Inst,int32_t *RetRows,int32_t *RetColumns);
    void (*SendBackspace)(t_ScriptingEngineInstType *Inst);
    void (*SendEnter)(t_ScriptingEngineInstType *Inst);
    void (*WriteScreen)(t_ScriptingEngineInstType *Inst,const uint8_t *Str,uint32_t Len);
    unsigned int (*ReadKeyboard)(t_ScriptingEngineInstType *Inst,struct PluginKeyPress *KeyPresses,uint32_t MaxCount);
    void (*WriteCom)(t_ScriptingEngineInstType *Inst,const uint8_t *Str,uint32_t Len);
    unsigned int (*ReadCom)(t_ScriptingEngineInstType *Inst,uint8_t *Buffer,uint32_t BufferSize);
    void (*DisableScreenDisplay)(t_ScriptingEngineInstType *Inst,PG_BOOL Enabled);
    void (*DisableKeyboardSend)(t_ScriptingEngineInstType *Inst,PG_BOOL Enabled);
    /********* End of SCRIPTING_API_VERSION_1 *********/
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif   /* end of "#ifndef __SCRIPTING_H_" */
