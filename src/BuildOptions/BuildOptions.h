/*******************************************************************************
 * FILENAME: BuildOptions.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the API to the build options.  Build options are a list
 *    of functions that are different between a debug build and a release build.
 *    Both versions will define the functions listed in this file but the
 *    build system will only include the debug version or the release version.
 *
 * COPYRIGHT:
 *    Copyright 15 Sep 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (15 Sep 2024)
 *       Created
 *
 *******************************************************************************/
#ifndef __BUILDOPTIONS_H_
#define __BUILDOPTIONS_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
const char *BuildOption_GetPluginPath(void);
const char *BuildOption_GetPluginListFilename(void);

#endif   /* end of "#ifndef __BUILDOPTIONS_H_" */
