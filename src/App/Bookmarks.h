/*******************************************************************************
 * FILENAME: Bookmarks.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (12 Nov 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __BOOKMARKS_H_
#define __BOOKMARKS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Settings.h"
#include "Connections.h"
#include <string>
#include <list>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct Bookmark
{
    int BookmarkUID;        // The unique bookmark ID (RAM only)
    std::string MenuName;   // All menus with the same name make a sub menu ("" is root)
    std::string Name;
    std::string URI;
    t_KVList Options;
    bool UseCustomSettings;
    class ConSettings CustomSettings;
};

typedef std::list<struct Bookmark> t_BookmarkList;
typedef t_BookmarkList::iterator i_BookmarkList;

//typedef std::list<std::string> t_BookmarkMenuList;
//typedef t_BookmarkMenuList::iterator i_BookmarkMenuList;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
extern t_BookmarkList g_BookmarkList;

void InitBookmarks(void);
bool BookmarkConnection(class Connection *Con);
void ManageBookmarks(void);
bool LoadBookmarks(void);
bool SaveBookmarks(void);
i_BookmarkList FindBookmarkByUID(int UID);

#endif
