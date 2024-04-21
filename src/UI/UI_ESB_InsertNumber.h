/*******************************************************************************
 * FILENAME: UI_ESB_InsertNumber.h
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
 *    Paul Hutchinson (03 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UI_ESB_INSERTNUMBER_H_
#define __UI_ESB_INSERTNUMBER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/
enum e_ESBIN_RadioButton
{
    e_ESBIN_RadioButton_Endian_Big,
    e_ESBIN_RadioButton_Endian_Little,
    e_ESBIN_RadioButton_Bits_8,
    e_ESBIN_RadioButton_Bits_16,
    e_ESBIN_RadioButton_Bits_32,
    e_ESBIN_RadioButton_Bits_64,
    e_ESBIN_RadioButtonMAX
};

enum e_ESBIN_TextInput
{
    e_ESBIN_TextInput_Number,
    e_ESBIN_TextInputMAX
};

//typedef enum
//{
//    e_ESBINEvent_BttnTriggered,
//    e_ESBINEventMAX
//} e_ESBINEventType;
//
//struct ESBINEventDataBttn
//{
//    e_ESBIN_Button BttnID;
//};
//
//union ESBINEventData
//{
//    struct ESBINEventDataBttn Bttn;
//};

struct ESBINEvent
{
//    e_ESBINEventType EventType;
    uintptr_t ID;
//    union ESBINEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_InsertNumber(void);
bool UIShow_ESB_InsertNumber(void);
void UIFree_ESB_InsertNumber(void);
t_UIRadioBttnCtrl *UIESBIN_GetRadioButton(e_ESBIN_RadioButton bttn);
t_UITextInputCtrl *UIESBIN_GetTextInput(e_ESBIN_TextInput input);

bool ESBIN_Event(const struct ESBINEvent *Event);

#endif
