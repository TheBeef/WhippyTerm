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

void WTB_Mkdir(const char *DirName)
{
    _mkdir(DirName);
}

void WTB_Rmdir(const char *DirName)
{
    _rmdir(DirName);
}

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

void WTB_CloseDir(void *DirHandle)
{
    struct OpenDirData *dir=(struct OpenDirData *)DirHandle;

    FindClose(dir->hFind);
    free(dir);
}

bool WTB_IsDir(const char *FileName)
{
    DWORD Status = ERROR_SUCCESS;
    FILE_STAT_INFORMATION FileStatInfo;

    if (!GetFileInformationByName(FileName,
                                  FileStatByNameInfo,
                                  &FileStatInfo,
                                  sizeof(FileStatInfo)))
    {
        return false;
    }

    return FileStatInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}
