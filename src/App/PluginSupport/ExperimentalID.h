/*******************************************************************************
 * FILENAME: ExperimentalID.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the Experimental ID for the PISys_GetExperimentalID()
 *    Plugin API call.
 *    
 *    See PISys_GetExperimentalID() for details on what the Experimental ID
 *    is has to use it.
 *
 *    TLDR verison:
 *      Every time you change the plugin API add 1 to the
 *      EXPERIMENTAL_PLUGIN_API_ID in the unofficial release version.  When
 *      an official release is made reset this number to 1.
 *
 * COPYRIGHT:
 *    Copyright 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (14 Oct 2024)
 *       Created
 *
 *******************************************************************************/
#ifndef __EXPERIMENTALID_H_
#define __EXPERIMENTALID_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "Version.h"

/***  DEFINES                          ***/
#if OFFICIAL_RELEASE==1
 #define EXPERIMENTAL_PLUGIN_API_ID     0   // Always 0 on official releases
#else
 #define EXPERIMENTAL_PLUGIN_API_ID     1   // This starts at 1 and goes up every time you change only of the plugin API's
#endif

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif   /* end of "#ifndef __EXPERIMENTALID_H_" */
