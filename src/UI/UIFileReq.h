/*******************************************************************************
 * FILENAME: UIFileReq.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI access to file requesters.
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIFILEREQ_H_
#define __UIFILEREQ_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UI_LoadFileReq(const char *Title,std::string &Path,
        std::string &Filename,const char *Filters,int SelectedFilter);
bool UI_SaveFileReq(const char *Title,std::string &Path,
        std::string &Filename,const char *Filters,int SelectedFilter);
std::string UI_ConcatFile2Path(const std::string &Path,const std::string &File);

#endif
