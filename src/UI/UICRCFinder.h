/*******************************************************************************
 * FILENAME: UICRCFinder.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 04 Jun 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (04 Jun 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICRCFINDER_H_
#define __UICRCFINDER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_CF_Button
{
    e_CF_Button_FindCRC,
    e_CF_Button_ShowSource,
    e_CF_ButtonMAX
};

enum e_CF_TextInput
{
    e_CF_TextInput_CRC,
    e_CF_TextInputMAX
};

typedef enum
{
    e_CF_Combox_CRCType,
    e_CF_ComboxMAX
} e_CF_ComboxType;

typedef enum
{
    e_CF_RadioBttn_Big,
    e_CF_RadioBttn_Little,
    e_CF_RadioBttnMAX
} e_CF_RadioBttnType;

typedef enum
{
    e_CFEvent_BttnTriggered,
    e_CFEvent_TextEditDone,
    e_CFEvent_TextEdited,
    e_CTEvent_ComboxChange,
    e_CFEventMAX
} e_CFEventType;

struct CFEventDataBttn
{
    e_CF_Button BttnID;
};
struct CFEventDataText
{
    e_CF_TextInput TxtID;
};

struct CFEventDataCombox
{
    e_CF_ComboxType BoxID;
};

union CFEventData
{
    struct CFEventDataBttn Bttn;
    struct CFEventDataText Txt;
    struct CFEventDataCombox Combox;
};

struct CFEvent
{
    e_CFEventType EventType;
    uintptr_t ID;
    union CFEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_CRCFinder(void);
void UIFree_CRCFinder(void);
bool UIShow_CRCFinder(void);
t_UIButtonCtrl *UICF_GetButton(e_CF_Button bttn);
t_UIFrameContainerCtrl *UICF_GetHexContainerFrame(void);
t_UITextInputCtrl *UICF_GetTextInput(e_CF_TextInput UIObj);
t_UIComboBoxCtrl *UICF_GetComboBoxHandle(e_CF_ComboxType UIObj);
t_UIRadioBttnCtrl *UICF_GetRadioBttnInput(e_CF_RadioBttnType UIObj);

bool CF_Event(const struct CFEvent *Event);

#endif   /* end of "#ifndef __UICRCFINDER_H_" */
