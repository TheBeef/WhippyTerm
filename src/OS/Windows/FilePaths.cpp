/*******************************************************************************
 * FILENAME: FilePaths.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file gets system file paths for a Linux system.
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
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/FilePaths.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <windows.h>
#include <Shlobj.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    GetOSAppDataPath
 *
 * SYNOPSIS:
 *    bool GetOSAppDataPath(std::string &AppPath);
 *
 * PARAMETERS:
 *    AppPath [O] -- The path (with trailing \) for the path to store app data.
 *
 * FUNCTION:
 *    This function gets a path of the OS where the program should store
 *    it's data.  This is data like the settings files.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool GetOSAppDataPath(std::string &AppPath)
{
    char szPath[MAX_PATH];
    if(SHGetFolderPath(NULL,CSIDL_APPDATA,NULL,0,szPath)!=S_OK)
        return false;

    AppPath=szPath;

    if(AppPath[AppPath.length()-1]!='\\')
        AppPath+="\\";
    AppPath+="WhippyTerm\\";

    return true;
}

/*******************************************************************************
 * NAME:
 *    GetOSAppExePath
 *
 * SYNOPSIS:
 *    bool GetOSAppExePath(std::string &AppPath);
 *
 * PARAMETERS:
 *    AppPath [O] -- The path (with trailing /) for the path to exe.
 *
 * FUNCTION:
 *    This function gets a path of the system where the program exe is.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    GetOSAppDataPath()
 ******************************************************************************/
bool GetOSAppExePath(std::string &AppPath)
{
    TCHAR szFileName[MAX_PATH];

    if(GetModuleFileName(NULL,szFileName,MAX_PATH)==0)
        return false;

    /* Remove the exe name */
    p=&dest[strlen(dest)];
    while(p>dest)
    {
        if(*p=='\\')
        {
            *(p+1)=0;
            break;
        }
        p--;
    }

    AppPath=szFileName;

    return true;
}
