/*******************************************************************************
 * FILENAME: Directorys.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file is for directory managment.
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
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __DIRECTORYS_H_
#define __DIRECTORYS_H_

/***  HEADER FILES TO INCLUDE          ***/
//#include <list>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
//struct FileListInfo
//{
//    std::string Filename;
//    bool Directory;
//};
//typedef std::list<struct FileListInfo> t_FileListType;
//typedef t_FileListType::iterator i_FileListType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool MakePathDir(const char *Path);
bool PathExists(const char *Path);
const char *ConvertPath2Native(const char *Path);
const char *Basename(const char *Filename);
const char *GetOSPathSeparator(void);
//bool GetFileList(const char *Path,t_FileListType &Files);
bool MakeFileExe(const char *Filename);

#endif   /* end of "#ifndef __DIRECTORYS_H_" */
