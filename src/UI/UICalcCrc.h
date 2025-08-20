/*******************************************************************************
 * FILENAME: UICalcCrc.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access functions for the edit send buffer dialog in it.
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
 *    Paul Hutchinson (16 Aug 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICALCCRC_H_
#define __UICALCCRC_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_CCRC_Button
{
    e_CCRC_Button_CalcCRC,
    e_CCRC_Button_ShowSource,
    e_CCRC_ButtonMAX
};

enum e_CCRC_TextInput
{
    e_CCRC_TextInput_CRC,
    e_CCRC_TextInputMAX
};

enum e_CCRC_ComboxType
{
    e_CCRC_Combox_CRCType,
    e_CCRC_ComboxMAX
};

typedef enum
{
    e_CCRCEvent_BttnTriggered,
    e_CCRCEvent_ComboxChange,
    e_CCRCEventMAX
} e_CCRCEventType;

struct CCRCEventDataBttn
{
    e_CCRC_Button BttnID;
};
struct CCRCEventDataText
{
    e_CCRC_TextInput TxtID;
};
struct CCRCEventDataCombox
{
    e_CCRC_ComboxType BoxID;
};

union CCRCEventData
{
    struct CCRCEventDataBttn Bttn;
    struct CCRCEventDataText Txt;
    struct CCRCEventDataCombox Combox;
};

struct CCRCEvent
{
    e_CCRCEventType EventType;
    uintptr_t ID;
    union CCRCEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_CalcCrc(void);
void UIFree_CalcCrc(void);
bool UIShow_CalcCrc(void);
t_UIButtonCtrl *UICCRC_GetButton(e_CCRC_Button bttn);
t_UIContainerFrameCtrl *UICCRC_GetHexContainerFrame(void);
t_UITextInputCtrl *UICCRC_GetTextInput(e_CCRC_TextInput UIObj);
t_UIComboBoxCtrl *UICCRC_GetComboBoxHandle(e_CCRC_ComboxType UIObj);

bool CCRC_Event(const struct CCRCEvent *Event);

#endif
