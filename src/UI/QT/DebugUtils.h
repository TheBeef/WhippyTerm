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
