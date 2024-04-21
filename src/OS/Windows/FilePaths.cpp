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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
 *    GetAppDataPath
 *
 * SYNOPSIS:
 *    bool GetAppDataPath(std::string &AppPath);
 *
 * PARAMETERS:
 *    AppPath [O] -- The path (with trailing \) for the path to store app data.
 *
 * FUNCTION:
 *    This function gets a path of the system where the program should store
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
bool GetAppDataPath(std::string &AppPath)
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
