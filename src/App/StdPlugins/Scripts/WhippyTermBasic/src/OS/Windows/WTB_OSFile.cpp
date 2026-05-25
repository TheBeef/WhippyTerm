/*******************************************************************************
 * FILENAME: WTB_OSFile.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 16 Feb 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (16 Feb 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "../WTB_OSFile.h"
#include <windows.h>
#include <direct.h>
#include <stdio.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct OpenDirData
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    bool DataReady;
};

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    WTB_Mkdir
 *
 * SYNOPSIS:
 *    void WTB_Mkdir(const char *DirName);
 *
 * PARAMETERS:
 *    DirName [I] -- The directory path.
 *
 * FUNCTION:
 *    Creates a directory at the given path.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void WTB_Mkdir(const char *DirName)
{
    CreateDirectoryA(DirName,NULL);
}

/*******************************************************************************
 * NAME:
 *    WTB_Rmdir
 *
 * SYNOPSIS:
 *    void WTB_Rmdir(const char *DirName);
 *
 * PARAMETERS:
 *    DirName [I] -- The directory path.
 *
 * FUNCTION:
 *    Removes the directory at the given path.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void WTB_Rmdir(const char *DirName)
{
    RemoveDirectoryA(DirName);
}

/*******************************************************************************
 * NAME:
 *    WTB_OpenDir
 *
 * SYNOPSIS:
 *    void *WTB_OpenDir(const char *Path);
 *
 * PARAMETERS:
 *    Path [I] -- Path of the directory whose contents are to be
 *                enumerated. May be absolute or relative to the current
 *                working directory.
 *
 * FUNCTION:
 *    Opens a directory for iteration with WTB_NextDirEntry(). Backs the
 *    BASIC keyword DIR (in concert with WTB_NextDirEntry() and
 *    WTB_CloseDir()).
 *
 * RETURNS:
 *    A handle on success.
 *
 *    NULL if the directory could not be opened.
 *
 * NOTES:
 *    The first entries returned by the matching WTB_NextDirEntry() will
 *    typically be "." and ".." -- the BASIC DIR keyword does not filter
 *    these out, so script authors who want only real children must skip
 *    them themselves.
 *
 * SEE ALSO:
 *    WTB_NextDirEntry(), WTB_CloseDir()
 ******************************************************************************/
void *WTB_OpenDir(const char *Path)
{
    char searchPath[MAX_PATH];
    struct OpenDirData *dir;

    // Windows requires a wildcard (e.g., "C:\Temp\*") to list contents
    snprintf(searchPath, MAX_PATH, "%s\\*", Path);

    dir=(struct OpenDirData *)malloc(sizeof(struct OpenDirData));
    if(dir==NULL)
        return NULL;

    dir->hFind=FindFirstFileA(Path,&dir->FindFileData);
    if(dir->hFind==INVALID_HANDLE_VALUE)
    {
        free(dir);
        return NULL;
    }
    dir->DataReady=true;

    return (void *)dir;
}

/*******************************************************************************
 * NAME:
 *    WTB_NextDirEntry
 *
 * SYNOPSIS:
 *    const char *WTB_NextDirEntry(void *DirHandle);
 *
 * PARAMETERS:
 *    DirHandle [I] -- A handle previously returned by WTB_OpenDir().
 *
 * FUNCTION:
 *    Returns the name of the next entry in a directory that was opened
 *    with WTB_OpenDir(). Called repeatedly to walk the whole directory.
 *
 * RETURNS:
 *    A pointer to a NUL-terminated entry name on success. The pointer is
 *    valid only until the next call on the same handle, or until
 *    WTB_CloseDir() is called -- the caller must copy the string if it
 *    needs to keep it.
 *
 *    NULL when the end of the directory has been reached, or on error. These
 *    two cases are not distinguished.
 *
 * SEE ALSO:
 *    WTB_OpenDir(), WTB_CloseDir()
 ******************************************************************************/
const char *WTB_NextDirEntry(void *DirHandle)
{
    struct OpenDirData *dir=(struct OpenDirData *)DirHandle;

    if(dir->DataReady)
    {
        dir->DataReady=false;
        return dir->FindFileData.cFileName;
    }
    else
    {
        if(FindNextFileA(dir->hFind,&dir->FindFileData))
            return dir->FindFileData.cFileName;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    WTB_CloseDir
 *
 * SYNOPSIS:
 *    void WTB_CloseDir(void *DirHandle);
 *
 * PARAMETERS:
 *    DirHandle [I] -- The directory handle returned by the matching open call.
 *
 * FUNCTION:
 *    Closes a directory that was opened with the matching open call.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void WTB_CloseDir(void *DirHandle)
{
    struct OpenDirData *dir=(struct OpenDirData *)DirHandle;

    FindClose(dir->hFind);
    free(dir);
}

/*******************************************************************************
 * NAME:
 *    WTB_IsDir
 *
 * SYNOPSIS:
 *    bool WTB_IsDir(const char *FileName);
 *
 * PARAMETERS:
 *    FileName [I] -- The path to test.
 *
 * FUNCTION:
 *    Tests whether the given path exists and refers to a directory.
 *
 * RETURNS:
 *    true -- The path exists and is a directory.
 *    false -- The path does not exist, is not accessible to the caller,
 *             or refers to something that is not a directory.
 *
 * SEE ALSO:
 *    WTB_OpenDir(), WTB_Mkdir(), WTB_Rmdir()
 ******************************************************************************/
bool WTB_IsDir(const char *FileName)
{
    DWORD attrs = GetFileAttributesA(FileName);
    if(attrs == INVALID_FILE_ATTRIBUTES)
        return false;
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}
