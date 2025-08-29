/*******************************************************************************
 * FILENAME: PluginSystem.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 21 Aug 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (21 Aug 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __PLUGINSYSTEM_H_
#define __PLUGINSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_PlugInType_FileTransfer,
    e_PlugInType_IODriver,
    e_PlugInType_DataProcessor,
    e_PlugInTypeMAX
} e_PlugInTypeType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void InitPluginSystem(void);
void RegisterPluginWithSystem(const char *IDStr);
void UnRegisterPluginWithSystem(const char *IDStr);
void NotePluginInUse(const char *IDStr);
void UnNotePluginInUse(const char *IDStr);
bool IsPluginInUse(struct ExternPluginInfo *ExPlugin);
void InformOfNewPluginInstalled(struct ExternPluginInfo *Info);
void InformOfPluginUninstalled(struct ExternPluginInfo *Info);

#endif   /* end of "#ifndef __PLUGINSYSTEM_H_" */
