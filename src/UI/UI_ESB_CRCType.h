/*******************************************************************************
 * FILENAME: UI_ESB_CRCType.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (30 Aug 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UI_ESB_CRCTYPE_H_
#define __UI_ESB_CRCTYPE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_ESBCT_Button
{
    e_ESBCT_Button_ShowSource,
    e_ESBCT_ButtonMAX
};

enum e_ESBCR_ListView
{
    e_ESBCR_ListView_CRCList,
    e_ESBCR_ListViewMAX
};

typedef enum
{
    e_ESBCTEvent_BttnTriggered,
    e_ESBCTEventMAX
} e_ESBCTEventType;

struct ESBCTEventDataBttn
{
    e_ESBCT_Button BttnID;
};

union ESBCTEventData
{
    struct ESBCTEventDataBttn Bttn;
};

struct ESBCTEvent
{
    e_ESBCTEventType EventType;
    uintptr_t ID;
    union ESBCTEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_CRCType(void);
bool UIShow_ESB_CRCType(void);
void UIFree_ESB_CRCType(void);
t_UIButtonCtrl *UIESBCT_GetButton(e_ESBCT_Button bttn);
t_UIListViewCtrl *UIESBCR_GetListView(e_ESBCR_ListView lv);

bool ESBCT_Event(const struct ESBCTEvent *Event);

#endif
