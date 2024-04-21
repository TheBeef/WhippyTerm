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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
//bool GetFileList(const char *Path,t_FileListType &Files);
bool MakeFileExe(const char *Filename);

#endif   /* end of "#ifndef __DIRECTORYS_H_" */
