/*******************************************************************************
 * FILENAME: UIComTest.h
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
 *    Paul Hutchinson (20 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICOMTEST_H_
#define __UICOMTEST_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_CT_Button
{
    e_CT_Button_Rescan1,
    e_CT_Button_Rescan2,
    e_CT_Button_Start,
    e_CT_Button_Stop,
    e_CT_ButtonMAX
};

enum e_CT_TextInput
{
    e_CT_TextInput_Stat_PacketsSent,
    e_CT_TextInput_Stat_PacketsRecv,
    e_CT_TextInput_Stat_Bytes,
    e_CT_TextInput_Stat_SendErrors,
    e_CT_TextInput_Stat_SendBusyErrors,
//    e_CT_TextInput_Stat_BytesPerSec,
    e_CT_TextInput_Stat_RxErrors,
    e_CT_TextInputMAX
};

typedef enum
{
    e_CT_Frame_Connection1,
    e_CT_Frame_Connection2,
    e_CT_Frame_ConnectionMAX
} e_CT_Frame;

typedef enum
{
    e_CT_Number_PacketLength,
    e_CT_Number_Packets,
    e_CT_NumberMAX
} e_CT_NumberType;

typedef enum
{
    e_CT_DoubleNumber_Delay,
    e_CT_DoubleNumberMAX
} e_CT_DoubleNumberType;

typedef enum
{
    e_CT_Combox_ConnectionList1,
    e_CT_Combox_ConnectionList2,
    e_CT_Combox_Pattern,
    e_CT_ComboxMAX
} e_CT_ComboxType;

typedef enum
{
    e_CT_Checkbox_Loopback,
    e_CT_Checkbox_FullDuplex,
    e_CT_CheckboxMAX
} e_CT_CheckboxType;

typedef enum
{
    e_CT_Label_Status,
    e_CT_LabelMAX
} e_CT_LabelType;

typedef enum
{
    e_CTEvent_BttnTriggered,
    e_CTEvent_ComboxChange,
    e_CTEvent_CheckboxChange,
    e_CTEventMAX
} e_CTEventType;

struct CTEventDataBttn
{
    e_CT_Button BttnID;
};

struct CTEventDataCombox
{
    e_CT_ComboxType BoxID;
};

struct CTEventDataCheckbox
{
    e_CT_CheckboxType BoxID;
};

union CTEventData
{
    struct CTEventDataBttn Bttn;
    struct CTEventDataCombox Combox;
    struct CTEventDataCheckbox Checkbox;
};

struct CTEvent
{
    e_CTEventType EventType;
    uintptr_t ID;
    union CTEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ComTest(void);
void UIFree_ComTest(void);
bool UIShow_ComTest(void);
t_UIButtonCtrl *UICT_GetButton(e_CT_Button bttn);
t_UIContainerCtrl *UICT_GetOptionsFrameContainer(e_CT_Frame frame);
t_UITextInputCtrl *UICT_GetTextInput(e_CT_TextInput UIObj);
t_UIComboBoxCtrl *UICT_GetComboBoxHandle(e_CT_ComboxType UIObj);
t_UICheckboxCtrl *UICT_GetCheckboxHandle(e_CT_CheckboxType UIObj);
t_UINumberInput *UICT_GetNumberInputHandle(e_CT_NumberType UIObj);
t_UIDoubleInput *UICT_GetDoubleNumberInputHandle(e_CT_DoubleNumberType UIObj);
t_UILabelCtrl *UICT_GetLabelHandle(e_CT_LabelType UIObj);

bool CT_Event(const struct CTEvent *Event);

#endif
