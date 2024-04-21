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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef struct UIMutex {int Private;} t_UIMutex;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void FlagDrvDataEvent(t_IOSystemHandle *IOHandle);
void UI_ProcessAllPendingUIEvents(void);
void UI_Beep(void);
t_UIMutex *AllocMutex(void);
void FreeMutex(t_UIMutex *Mut);
void LockMutex(t_UIMutex *Mut);
void UnLockMutex(t_UIMutex *Mut);
uint64_t GetMSCounter(void);
void UI_GotoWebPage(const char *WebSite);

#endif
