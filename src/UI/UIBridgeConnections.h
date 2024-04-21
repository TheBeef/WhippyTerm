/*******************************************************************************
 * FILENAME: UIBridgeConnections.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access to the bridge connection dialog UI.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Aug 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIBRIDGECONNECTIONS_H_
#define __UIBRIDGECONNECTIONS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_BC_Checkbox_Locked,
    e_BC_CheckboxMAX
} e_BC_CheckboxType;

typedef enum
{
    e_BC_Combox_Connection2,
    e_BC_ComboxMAX
} e_BC_ComboxType;

typedef enum
{
    e_BC_Label_Connection1,
    e_BC_LabelMAX
} e_BC_LabelType;

typedef enum
{
    e_BCEvent_ComboxChange,
    e_BCEvent_CheckboxChange,
    e_BCEventMAX
} e_BCEventType;

struct BCEventDataCombox
{
    e_BC_ComboxType BoxID;
};

struct BCEventDataCheckbox
{
    e_BC_CheckboxType BoxID;
};

union BCEventData
{
    struct BCEventDataCombox Combox;
    struct BCEventDataCheckbox Checkbox;
};

struct BCEvent
{
    e_BCEventType EventType;
    uintptr_t ID;
    union BCEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_BridgeCon(void);
void UIFree_BridgeCon(void);
bool UIShow_BridgeCon(void);
t_UIComboBoxCtrl *UIBC_GetComboBoxHandle(e_BC_ComboxType UIObj);
t_UICheckboxCtrl *UIBC_GetCheckboxHandle(e_BC_CheckboxType UIObj);
t_UILabelCtrl *UIBC_GetLabelHandle(e_BC_LabelType UIObj);

bool BC_Event(const struct BCEvent *Event);

#endif
