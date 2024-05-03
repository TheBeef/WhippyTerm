/*******************************************************************************
 * FILENAME: UISendByte.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI access functions for the send byte dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 02 May 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (02 May 2024)
 *       Created
 *
 *******************************************************************************/
#ifndef __UISENDBYTE_H_
#define __UISENDBYTE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_SBD_TextInput
{
    e_SBD_TextInput_Dec,
    e_SBD_TextInput_Hex,
    e_SBD_TextInput_Oct,
    e_SBD_TextInputMAX
};

struct SBDEventDataText
{
    e_SBD_TextInput TxtID;
};

struct SBDEventDataAscII
{
    uint8_t ByteValue;
};

union SBDEventData
{
    struct SBDEventDataText Txt;
    struct SBDEventDataAscII AscII;
};

typedef enum
{
    e_SBDEvent_AscIISelected,
    e_SBDEvent_TextEditDone,
    e_SBDEventMAX
} e_SBDEventType;

struct SBDEvent
{
    e_SBDEventType EventType;
    uintptr_t ID;
    union SBDEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_SendByte(void);
bool UIShow_SendByte(void);
void UIFree_SendByte(void);
t_UITextInputCtrl *UISBD_GetTextInput(e_SBD_TextInput UIObj);

bool SBD_Event(const struct SBDEvent *Event);

#endif   /* end of "#ifndef __UISENDBYTE_H_" */
