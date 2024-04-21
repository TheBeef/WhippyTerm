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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
