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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
