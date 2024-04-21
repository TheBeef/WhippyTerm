/*******************************************************************************
 * FILENAME: DebugUtils.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2019 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (01 Oct 2019)
 *       Created
 *
 *******************************************************************************/
#ifndef __DEBUGUTILS_H_
#define __DEBUGUTILS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>
#include "UI/UIDebug.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern uint64_t g_DBTimers[e_DBTMAX];
extern uint32_t g_DBTimes[e_DBTMAX];
extern uint32_t g_DBMaxTimes[e_DBTMAX];
extern uint32_t g_DBAvgTimes[e_DBTMAX];
extern uint32_t g_DBAvgCounts[e_DBTMAX];

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
