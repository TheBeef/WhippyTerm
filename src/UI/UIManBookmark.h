/*******************************************************************************
 * FILENAME: UIManBookmark.h
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
 *    Paul Hutchinson (23 Nov 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIMANBOOKMARK_H_
#define __UIMANBOOKMARK_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_UIDMB_Button
{
    e_UIDMB_Button_Rename,
    e_UIDMB_Button_Delete,
    e_UIDMB_Button_Options,
    e_UIDMB_Button_Settings,
    e_UIDMB_ButtonMAX
};

enum e_UIDMB_TextInput
{
    e_UIDMB_TextInput_BookmarkName,
    e_UIDMB_TextInput_URI,
    e_UIDMB_TextInputMAX
};

enum e_UIDMB_TreeView
{
    e_UIDMB_TreeView_Folders,
    e_UIDMB_TreeViewMAX
};

typedef enum
{
    e_UIDMB_Checkbox_UseGlobalSettings,
    e_UIDMB_CheckboxMAX
} e_UIDMB_CheckboxType;

/* Events */
typedef enum
{
    e_DMBEvent_BttnTriggered,
    e_DMBEvent_BookmarkNameChanged,
    e_DMBEvent_URITextEditFinished,
    e_DMBEvent_SelectedFolderItemChanged,
    e_DMBEvent_FolderDragAndDropped,
    e_DMBEvent_CheckboxChange,
    e_DMBEventMAX
} e_DMBEventType;

struct DMBEventDataBttn
{
    e_UIDMB_Button BttnID;
};

struct DMBEventDataStringChange
{
    const char *NewText;
};

struct DMBEventDataItemChange
{
    t_UITreeItem *NewItem;
};

struct DMBEventDataDrag
{
    t_UITreeItem *ItemDragged;
    t_UITreeItem *DroppedOn;
    int DropPlacement;
};

struct DMBEventDataCheckbox
{
    e_UIDMB_CheckboxType BoxID;
};

union DMBEventData
{
    struct DMBEventDataBttn Bttn;
    struct DMBEventDataStringChange StringChange;
    struct DMBEventDataItemChange Item;
    struct DMBEventDataDrag Drag;
    struct DMBEventDataCheckbox Checkbox;
};

struct DMBEvent
{
    e_DMBEventType EventType;
    uintptr_t ID;
    union DMBEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ManBookmark(void);
bool UIShow_ManBookmark(void);
void UIFree_ManBookmark(void);
t_UITreeItem *UIDMB_AddFolderName(const char *Name);
t_UITreeItem *UIDMB_AddBookmarkEntry(t_UITreeItem *Parent,const char *Name);
t_UITreeItem *UIDMB_GetSelectedFolderItem(void);
void UIDMB_SetBookmarkName(const char *Name);
void UIDMB_GetBookmarkName(std::string &Name);
void UIDMB_SetURIInput(const char *URI);
void UIDMB_GetURIInput(std::string &URI);

t_UIButtonCtrl *UIDMB_GetButton(e_UIDMB_Button bttn);
t_UITextInputCtrl *UIDMB_GetTextInput(e_UIDMB_TextInput Input);
t_UITreeView *UIDMB_GetTreeView(e_UIDMB_TreeView TreeView);
t_UICheckboxCtrl *UIDMB_GetCheckbox(e_UIDMB_CheckboxType Checkbox);

bool DMB_Event(const struct DMBEvent *Event);

#endif
