/*******************************************************************************
 * FILENAME: Portable.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the portable system in it.  It handles if we are in portable
 *    mode or normal mode.
 *
 * COPYRIGHT:
 *    Copyright 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (12 Aug 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/Directorys.h"
#include "OS/FilePaths.h"
#include "Portable.h"
#include <string>
#include <stdio.h>

using namespace std;

/*** DEFINES                  ***/
#define PORTABLE_CFG_FILENAME                       "Portable.cfg"
#define PORTABLE_SETTINGS_FILESNAME                 "Settings"

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
bool g_InPortableMode=false;

/*******************************************************************************
 * NAME:
 *    InitPortableSystem
 *
 * SYNOPSIS:
 *    void InitPortableSystem(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the portable system and figures out if we are running
 *    in portable mode.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void InitPortableSystem(void)
{
    string ExePath;
    string PortableExePath;
    string SettingsPath;
    FILE *in;
    char buff[100];

    /* Check if we are running in portable mode */
    g_InPortableMode=false;
    if(GetOSAppExePath(ExePath))
    {
        PortableExePath=ExePath;
        PortableExePath+=PORTABLE_CFG_FILENAME;
        in=fopen(PortableExePath.c_str(),"rb");
        if(in!=NULL)
        {
            buff[0]='0';
            fread(buff,1,1,in);
            if(buff[0]=='1')
            {
                /* We want portable mode */
                g_InPortableMode=true;

                /* Make sure we have the settings dir */
                if(GetAppDataPath(SettingsPath))
                    MakePathDir(SettingsPath.c_str());
            }
            fclose(in);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    GetAppDataPath
 *
 * SYNOPSIS:
 *    bool GetAppDataPath(std::string &AppPath);
 *
 * PARAMETERS:
 *    AppPath [O] -- The path (with trailing /) for the path to store app data.
 *
 * FUNCTION:
 *    This function gets a path where the program should store it's data.
 *    This has been corrected for if we are portable mode or not.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool GetAppDataPath(std::string &AppPath)
{
    if(g_InPortableMode)
    {
        if(!GetOSAppExePath(AppPath))
            return false;
        AppPath+=PORTABLE_SETTINGS_FILESNAME;
        AppPath+=GetOSPathSeparator();
    }
    else
    {
#if OFFICIAL_RELEASE!=1
        GetOSAppDataPath(AppPath);
        AppPath+="Debug/";
        MakePathDir(AppPath.c_str());
#else
        return GetOSAppDataPath(AppPath);
#endif
    }
    return true;
}

