/*******************************************************************************
 * FILENAME: UIAddBookmark.h
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
 *    Paul Hutchinson (16 Nov 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIADDBOOKMARK_H_
#define __UIADDBOOKMARK_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_UIABDBttn_NewFolder,
    e_UIABDBttnMAX
} e_UIABDBttnType;

typedef enum
{
    e_ABDEvent_BttnTriggered,
    e_ABDEventMAX
} e_ABDEventType;

struct ABDEventDataBttn
{
    e_UIABDBttnType BttnID;
};

union ABDEventData
{
    struct ABDEventDataBttn Bttn;
};

struct ABDEvent
{
    e_ABDEventType EventType;
    uintptr_t ID;
    union ABDEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_AddBookmark(void);
bool UIShow_AddBookmark(void);
void UIFree_AddBookmark(void);
t_UITreeItem *UIAB_AddFolderName(const char *Name);
void UIAB_SetBookmarkName(const char *Name);
void UIAB_GetBookmarkName(std::string &Name);
t_UITreeItem *UIAB_GetSelectedFolderItem(void);

bool ABD_Event(const struct ABDEvent *Event);

#endif
