/*******************************************************************************
 * FILENAME: UISystem.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file functions to control the main UI.
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
 *
 * HISTORY:
 *    Paul Hutchinson (21 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UISYSTEM_H_
#define __UISYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/IOSystem.h"
#include "PluginSDK/KeyDefines.h"
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef struct UIMutex {int Private;} t_UIMutex;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void FlagDrvDataEvent(t_IOSystemHandle *IOHandle);
void UI_ProcessAllPendingUIEvents(void);
t_UIMutex *AllocMutex(void);
void FreeMutex(t_UIMutex *Mut);
void LockMutex(t_UIMutex *Mut);
void UnLockMutex(t_UIMutex *Mut);
uint64_t GetMSCounter(void);
void UI_GotoWebPage(const char *WebSite);
int caseinsensitivestrcmp(const char *a,const char *b);
void UI_CollectAllKeyPresses(void (*Callback)(uint8_t Mods,e_UIKeys UIKey,std::string &Text));

#endif
