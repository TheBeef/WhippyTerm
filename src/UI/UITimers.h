/*******************************************************************************
 * FILENAME: UITimers.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has timers from the UI.  These times may not be all that
 *    responsive as they come from the main event loop, but you know they're
 *    timers.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (22 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UITIMERS_H_
#define __UITIMERS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct UITimer;         // Not a real type

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
struct UITimer *AllocUITimer(void);
void FreeUITimer(struct UITimer *Timer);
void SetupUITimer(struct UITimer *Timer,void (*Timeout)(uintptr_t UserData),uintptr_t UserData,bool Repeats);
void UITimerSetTimeout(struct UITimer *Timer,uint32_t ms);
void UITimerStart(struct UITimer *Timer);
void UITimerStop(struct UITimer *Timer);
bool UITimerRunning(struct UITimer *Timer);

#endif
