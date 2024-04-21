/*******************************************************************************
 * FILENAME: UINewConnection.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the new connection dialog interface in it.
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UINEWCONNECTION_H_
#define __UINEWCONNECTION_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <string>
#include <stdint.h>
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_UINC_Button
{
    e_UINC_Button_Rescan,
    e_UINC_ButtonMAX
};

enum e_UINC_ComboBox
{
    e_UINC_ComboBox_Connection,
    e_UINC_ComboBoxMAX
};

enum e_UINC_TxtInput
{
    e_UINC_TxtInput_Name,
    e_UINC_TxtInputMAX
};

typedef enum
{
    e_DNCEvent_BttnTriggered,
    e_DNCEvent_ConnectionListChange,
    e_DNCEvent_NameChange,
    e_DNCEventMAX
} e_DNCEventType;

struct DNCEventDataBttn
{
    e_UINC_Button BttnID;
};

union DNCEventData
{
    struct DNCEventDataBttn Bttn;
};

struct DNCEvent
{
    e_DNCEventType EventType;
    uintptr_t ID;
    union DNCEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_NewConnection(void);
bool UIShow_NewConnection(void);
void UIFree_NewConnection(void);

t_UIContainerCtrl *UINC_GetOptionsFrameContainer(void);
t_UIComboBoxCtrl *UINC_GetComboBoxHandle(e_UINC_ComboBox CBox);
t_UITextInputCtrl *UINC_GetTxtInputHandle(e_UINC_TxtInput TInput);

bool DNC_Event(const struct DNCEvent *Event);

#endif
