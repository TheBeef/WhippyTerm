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
    std::string MenuName;   // Submenu name.  All menus with the same name make a sub menu (a MenuName of "" is in the root)
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
