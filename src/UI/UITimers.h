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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
