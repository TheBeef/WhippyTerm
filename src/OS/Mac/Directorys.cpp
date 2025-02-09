/*******************************************************************************
 * FILENAME: Directorys.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (24 Feb 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/Directorys.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

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
    return Path;
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
 ******************************************************************************/
bool MakePathDir(const char *Path)
{
#warning TBD
    return false;
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
 ******************************************************************************/
bool PathExists(const char *Path)
{
#warning TBD
    return false;
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
 *    /test/path/to/ignore/file.txt     => file.txt
 *    file.txt                          => file.txt
 *    /                                 => ""
 *    /file.txt                         => file.txt
 *    ../file.txt                       => file.txt
 *    /etc/file                         => file
 *    /etc                              => etc
 *    /etc/                             => ""
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
        if(*pos=='/')
            break;
        pos--;
    }
    pos++;  // We should be sitting 1 char before the start of the filename

    return pos;
}

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
    return false;
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
    return "/";
}
