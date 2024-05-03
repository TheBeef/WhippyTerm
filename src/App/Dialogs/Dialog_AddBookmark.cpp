/*******************************************************************************
 * FILENAME: Dialog_AddBookmark.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the add bookmark dialog in it.
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
 * CREATED BY:
 *    Paul Hutchinson (16 Nov 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Bookmarks.h"
#include "App/Dialogs/Dialog_AddBookmark.h"
#include "UI/UIAddBookmark.h"
#include "UI/UITextInputBox.h"
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void ABD_HandleNewFolderBttn(void);

/*** VARIABLE DEFINITIONS     ***/
t_UITreeItem *m_ABD_NewFolderItem;

/*******************************************************************************
 * NAME:
 *    RunAddBookmarkDialog
 *
 * SYNOPSIS:
 *    bool RunAddBookmarkDialog(struct Bookmark &BookmarkInfo);
 *
 * PARAMETERS:
 *    BookmarkInfo [I/O] -- The bookmark to add
 *
 * FUNCTION:
 *    This function shows the add bookmark dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunAddBookmarkDialog(struct Bookmark &BookmarkInfo)
{
    bool RetValue;
    string LastMenuName;
    i_BookmarkList bm;
    t_UITreeItem *FolderItem;
    t_UITreeItem *NoneFolderItem;

    m_ABD_NewFolderItem=NULL;

    if(!UIAlloc_AddBookmark())
        return false;

    NoneFolderItem=UIAB_AddFolderName("None");
    LastMenuName="";
    for(bm=g_BookmarkList.begin();bm!=g_BookmarkList.end();bm++)
    {
        if(bm->MenuName!=LastMenuName)
        {
            /* Ok, we are switching menus */
            if(bm->MenuName!="")
                UIAB_AddFolderName(bm->MenuName.c_str());

            LastMenuName=bm->MenuName;
        }
    }

    UIAB_SetBookmarkName(BookmarkInfo.Name.c_str());

    RetValue=UIShow_AddBookmark();

    UIAB_GetBookmarkName(BookmarkInfo.Name);
    FolderItem=UIAB_GetSelectedFolderItem();
    if(FolderItem==NULL || FolderItem==NoneFolderItem)
        BookmarkInfo.MenuName="";
    else
        UIGetTreeItemText(FolderItem,BookmarkInfo.MenuName);

    UIFree_AddBookmark();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    ABD_HandleNewFolderBttn
 *
 * SYNOPSIS:
 *    static void ABD_HandleNewFolderBttn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles the new folder button being clicked.  It
 *    prompts for a new folders name and adds it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void ABD_HandleNewFolderBttn(void)
{
    string NewFolderName;

    if(UITextInputBox("New Folder","Input the name of the new folder.",
            NewFolderName))
    {
        if(m_ABD_NewFolderItem!=NULL)
            UIRemoveTreeItem(m_ABD_NewFolderItem);
        m_ABD_NewFolderItem=UIAB_AddFolderName(NewFolderName.c_str());
        if(m_ABD_NewFolderItem!=NULL)
            UISelectTreeItem(m_ABD_NewFolderItem);
    }
}

/*******************************************************************************
 * NAME:
 *    ABD_Event
 *
 * SYNOPSIS:
 *    bool ABD_Event(const struct ABDEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event we should process.
 *
 * FUNCTION:
 *    This function is called from the UI to tell us there was a UI event.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool ABD_Event(const struct ABDEvent *Event)
{
    bool AcceptEvent;

    AcceptEvent=true;
    switch(Event->EventType)
    {
        case e_ABDEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_UIABDBttn_NewFolder:
                    ABD_HandleNewFolderBttn();
                break;
                case e_UIABDBttnMAX:
                default:
                break;
            }
        break;
        case e_ABDEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}
