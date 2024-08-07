/*******************************************************************************
 * FILENAME: ExternPluginsSystem.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the API for the extern plugin system in it.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (01 Mar 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __EXTERNPLUGINSSYSTEM_H_
#define __EXTERNPLUGINSSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "ThirdParty/TinyCFG/TinyCFG.h"
#include <string>
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_ExtPluginClass_Unknown,
    e_ExtPluginClass_DataProcessor,
    e_ExtPluginClass_IODriver,
    e_ExtPluginClass_FileTransfer,
    e_ExtPluginClass_Tool,
    e_ExtPluginClassMAX
} e_ExtPluginClassType;

typedef enum
{
    e_ExtPluginSubClass_DataProTxt_CharEncoding,
    e_ExtPluginSubClass_DataProTxt_TermEmulation,
    e_ExtPluginSubClass_DataProTxt_Highlighter,
    e_ExtPluginSubClass_DataProTxt_Logger,
    e_ExtPluginSubClass_DataProTxt_Other,
    e_ExtPluginSubClass_DataProTxtMAX
} e_ExtPluginSubClass_DataProType;

struct ExternPluginInfo
{
    /* System info */
    std::string Filename;
    bool Enabled;

    /* User info */
    std::string PluginName;
    std::string Description;
    std::string Contributors;
    std::string Copyright;
    std::string ReleaseDate;
    uint32_t Version;
    uint32_t APIVersion;
    int PluginClass;
    int PluginSubClass;
    bool DLLFound;          // Not stored to disk

    public:
        void Register(class TinyCFG *cfg)
        {
            cfg->Register("Filename",Filename);
            cfg->Register("Enabled",Enabled);
            cfg->Register("PluginName",PluginName);
            cfg->Register("Description",Description);
            cfg->Register("Contributors",Contributors);
            cfg->Register("Copyright",Copyright);
            cfg->Register("ReleaseDate",ReleaseDate);
            cfg->Register("Version",Version,true);
            cfg->Register("APIVersion",APIVersion,true);
            cfg->RegisterEnum("PluginClass",PluginClass,
                    e_ExtPluginClass_Unknown,
                    e_ExtPluginClassMAX-1,
                    e_ExtPluginClass_Unknown,"Unknown",
                    e_ExtPluginClass_DataProcessor,"DataProcessor",
                    e_ExtPluginClass_IODriver,"IODriver",
                    e_ExtPluginClass_FileTransfer,"FileTransfer",
                    e_ExtPluginClass_Tool,"Tool");
            cfg->Register("PluginSubClass",PluginSubClass);
        }
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void RegisterExternPlugins(void);
void FreeLoadedExternPlugins(void);
bool GetExternPluginInfo(int Index,struct ExternPluginInfo &RetInfo);
void SetExternPluginEnabled(int Index,bool Enabled);
void UninstallExternPlugin(int Index);
void PromptAndInstallPlugin(void);
bool InstallNewExternPlugin(const char *Filename);
bool GetNewExternPluginInfo(const char *Filename,struct ExternPluginInfo &Info);

#endif
