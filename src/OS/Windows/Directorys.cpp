/*******************************************************************************
 * FILENAME: Directorys.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has directory managment in it for Linux
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
#include "OS/Directorys.h"
#include <limits.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    ConvertPath2Native
 *
 * SYNOPSIS:
 *    const char *ConvertPath2Native(const char *Path);
 *
 * PARAMETERS:
 *    Path [I] -- The path to convert.
 *
 * FUNCTION:
 *    This function takes a unix path and converts it to a windows path.
 *
 * RETURNS:
 *    A pointer to a static buffer with the path in it.  If the path is too
 *    long this function returns NULL.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const char *ConvertPath2Native(const char *Path)
{
    static char RetPath[256];
    const char *s;
    char *d;
    char c;

    d=RetPath;
    s=Path;
    do
    {
        c=*s++;
        if(c=='/')
            *d++='\\';
        else
            *d++=c;
        if(d>RetPath+sizeof(RetPath))
            return NULL;
    } while(c!=0);

    return RetPath;
}

/*******************************************************************************
 * NAME:
 *    MakePathDir
 *
 * SYNOPSIS:
 *    bool MakePathDir(const char *Path);
 *
 * PARAMETERS:
 *    Path [I] -- The full path to make
 *
 * FUNCTION:
 *    This function makes all the directorys for a path.  For example given the
 *    path "/a/b/c/d" it will make a directory a then b under a and so on
 *    until the path "/a/b/c/d" exists.
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
 *       Based on http://stackoverflow.com/questions/2336242/recursive-mkdir-system-call-on-unix
 ******************************************************************************/
bool MakePathDir(const char *Path)
{
    char tmp[MAX_PATH];
    char *p;
    size_t len;
    int Status;

    /* We can only make path's up to 248 char's in len (see CreateDirectory())*/
    if(strlen(Path)>248)
        return false;

    snprintf(tmp,sizeof(tmp),"%s",Path);
    len=strlen(tmp);
    if(tmp[len-1]=='\\')
        tmp[len-1]=0;

    for(p=tmp+1;*p!=0;p++)
    {
        if(*p=='\\')
        {
            *p=0;
            Status=CreateDirectory(tmp,NULL);
            if(Status!=0 && GetLastError()!=ERROR_ALREADY_EXISTS)
                return false;
            *p='\\';
        }
    }
    Status=CreateDirectory(tmp,NULL);
    if(Status!=0 && GetLastError()!=ERROR_ALREADY_EXISTS)
        return false;

    return true;
}

/*******************************************************************************
 * NAME:
 *    PathExists
 *
 * SYNOPSIS:
 *    bool PathExists(const char *Path);
 *
 * PARAMETERS:
 *    Path [I] -- The path to check
 *
 * FUNCTION:
 *    This function checks to see if a path exists or not.
 *
 * RETURNS:
 *    true -- Path exists
 *    false -- Path does not exist
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool PathExists(const char *Path)
{
    DWORD dwAttrib;

    dwAttrib=GetFileAttributes(Path);

    return dwAttrib!=INVALID_FILE_ATTRIBUTES;
}

/*******************************************************************************
 * NAME:
 *    Basename
 *
 * SYNOPSIS:
 *    const char *Basename(const char *Filename);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename with a path.
 *
 * FUNCTION:
 *    This function finds the filename in 'Filename' and returns a pointer
 *    to the start of the filename.
 *
 * RETURNS:
 *    A pointer into 'Filename' where the filename starts.
 *
 * EXAMPLE:
 *    C:\test\path\to\ignore\file.txt     => file.txt
 *    file.txt                          => file.txt
 *    \                                 => ""
 *    \file.txt                         => file.txt
 *    ..\file.txt                       => file.txt
 *    \etc\file                         => file
 *    \etc                              => etc
 *    \etc\                             => ""
 *    C:/test/path/to/ignore/file.txt   => file.txt
 *    C:file.txt                        => file.txt
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const char *Basename(const char *Filename)
{
    const char *pos;

    pos=Filename+strlen(Filename);
    while(pos>=Filename)
    {
        if(*pos=='/' || *pos=='\\' || *pos==':')
            break;
        pos--;
    }
    pos++;  // We should be sitting 1 char before the start of the filename

    return pos;
}

///*******************************************************************************
// * NAME:
// *    GetFileList
// *
// * SYNOPSIS:
// *    bool GetFileList(const char *Path,t_FileListType &Files);
// *
// * PARAMETERS:
// *    Path [I] -- The path to get the file list for
// *    Files [O] -- The list of files we found.  This list is not sorted.
// *                 This is of type struct FileListInfo:
// *                      Filename -- The filename (no path)
// *                      Directory -- true = a directory, false = a file
// *
// * FUNCTION:
// *    This function gets a list of files from a directory.
// *
// * RETURNS:
// *    true -- Things worked out
// *    false -- There was an error.
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
//bool GetFileList(const char *Path,t_FileListType &Files)
//{
////https://docs.microsoft.com/en-ca/windows/win32/api/fileapi/nf-fileapi-findfirstfilea?redirectedfrom=MSDN
//}

/*******************************************************************************
 * NAME:
 *    MakeFileExe
 *
 * SYNOPSIS:
 *    bool MakeFileExe(const char *Filename);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to make exe
 *
 * FUNCTION:
 *    This function marks a file as runable.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool MakeFileExe(const char *Filename)
{
    /* Does nothing on Windows */
    return true;
}

/*******************************************************************************
 * NAME:
 *    GetOSPathSeparator
 *
 * SYNOPSIS:
 *    const char *GetOSPathSeparator(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the char that OS used as a path separator char.
 *
 * RETURNS:
 *    A static string with the path separator in it.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const char *GetOSPathSeparator(void)
{
    return "\\";
}

