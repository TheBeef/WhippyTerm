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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
#include <dirent.h>

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
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Based on http://stackoverflow.com/questions/2336242/recursive-mkdir-system-call-on-unix
 ******************************************************************************/
bool MakePathDir(const char *Path)
{
    char tmp[PATH_MAX];
    char *p;
    size_t len;
    int Status;

    snprintf(tmp,sizeof(tmp),"%s",Path);
    len=strlen(tmp);
    if(tmp[len-1]=='/')
        tmp[len-1]=0;

    for(p=tmp+1;*p!=0;p++)
    {
        if(*p=='/')
        {
            *p=0;
            Status=mkdir(tmp,S_IRWXU);
            if(Status!=0 && errno!=EEXIST)
                return false;
            *p='/';
        }
    }
    Status=mkdir(tmp,S_IRWXU);
    if(Status!=0 && errno!=EEXIST)
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
    struct stat sbuf;

    if(stat(Path,&sbuf)!=0)
        return false;

    return S_ISDIR(sbuf.st_mode);
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
// *                 The . and .. directories are NOT included in the list.
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
//    DIR *dir;
//    struct dirent *ent;
//    struct FileListInfo NewListEntry;
//
//    dir=opendir(Path);
//    if(!dir)
//        return false;
//
//    while((ent=readdir(dir))!=NULL)
//    {
//        /* Do not include . and .. */
//        if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
//            continue;
//
//        NewListEntry.Filename=ent->d_name;
//        NewListEntry.Directory=false;
//        if(ent->d_type==DT_DIR)
//            NewListEntry.Directory=true;
//        Files.push_back(NewListEntry);
//    }
//    closedir(dir);
//
//    return true;
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
    struct stat OldMode;

    if(stat(Filename,&OldMode)<0)
        return false;
    if(chmod(Filename,OldMode.st_mode|S_IXUSR|S_IXGRP|S_IXOTH)<0)
        return false;
    return true;
}

