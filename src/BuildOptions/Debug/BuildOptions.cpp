/*******************************************************************************
 * FILENAME: BuildOptions.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has function for handling build options for the debug build.
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
 * CREATED BY:
 *    Paul Hutchinson (15 Sep 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "BuildOptions/BuildOptions.h"

/*** DEFINES                  ***/
#define PLUGIN_DIR_DEBUG            "PluginsDebug"
#define PLUGIN_LIST_FILENAME_DEBUG  "PluginsDebug.cfg"

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    BuildOption_GetPluginPath
 *
 * SYNOPSIS:
 *    const char *BuildOption_GetPluginPath(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns a static string with the directory that the plugins
 *    are stored in.
 *
 * RETURNS:
 *    A static string with the directory name for the plugins in it.
 *
 * SEE ALSO:
 *    BuildOption_GetPluginListFilename()
 ******************************************************************************/
const char *BuildOption_GetPluginPath(void)
{
    return PLUGIN_DIR_DEBUG;
}

/*******************************************************************************
 * NAME:
 *    BuildOption_GetPluginListFilename
 *
 * SYNOPSIS:
 *    const char *BuildOption_GetPluginListFilename(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the filename (without path) for the file we store
 *    the list of installed plugins in.
 *
 * RETURNS:
 *    A static string with the filename of the plugin list in it.
 *
 * SEE ALSO:
 *    BuildOption_GetPluginPath()
 ******************************************************************************/
const char *BuildOption_GetPluginListFilename(void)
{
    return PLUGIN_LIST_FILENAME_DEBUG;
}

