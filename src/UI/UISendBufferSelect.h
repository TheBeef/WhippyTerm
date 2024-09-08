/*******************************************************************************
 * FILENAME: UISendBufferSelect.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 07 Sep 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (07 Sep 2024)
 *       Created
 *
 *******************************************************************************/
#ifndef __UISENDBUFFERSELECT_H_
#define __UISENDBUFFERSELECT_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_SBS_Button
{
    e_SBS_Button_Cancel,
    e_SBS_Button_GoButton,
    e_SBS_ButtonMAX
};

typedef enum
{
    e_SBSColumnView_Buffers_List,
    e_SBSColumnViewMAX
} e_SBSColumnViewType;

typedef enum
{
    e_SBSEvent_BttnTriggered,
    e_SBSEvent_ColumnViewSelectionChanged,
    e_SBSEventMAX
} e_SBSEventType;

struct SBSEventDataBttn
{
    e_SBS_Button BttnID;
};

struct SBSEventDataColumnView
{
    e_SBSColumnViewType InputID;
    int NewRow;
};

union SBSEventData
{
    struct SBSEventDataBttn Bttn;
    struct SBSEventDataColumnView ColumnView;
};

struct SBSEvent
{
    e_SBSEventType EventType;
    uintptr_t ID;
    union SBSEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_SendBufferSelect(void);
void UIFree_SendBufferSelect(void);
bool UIShow_SendBufferSelect(void);
t_UIButtonCtrl *UISBS_GetButton(e_SBS_Button bttn);
t_UIColumnView *UISBS_GetColumnViewHandle(e_SBSColumnViewType UIObj);
void UISBS_SetDialogTitle(const char *Title);
void UISBS_CloseDialog(void);

bool SBS_Event(const struct SBSEvent *Event);

#endif   /* end of "#ifndef __UISENDBUFFERSELECT_H_" */
