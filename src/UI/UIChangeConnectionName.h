/*******************************************************************************
 * FILENAME: UIChangeConnectionName.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI access functions for the change connection name
 *    dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
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
 *    Paul Hutchinson (19 Sep 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICHANGECONNECTIONNAME_H_
#define __UICHANGECONNECTIONNAME_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_UICCN_TxtInput
{
    e_UICCN_TxtInput_Name,
    e_UICCN_TxtInputMAX
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ChangeConnectionName(void);
void UIShow_ChangeConnectionName(void);
void UIFree_ChangeConnectionName(void);
t_UITextInputCtrl *UICCN_GetTxtInputHandle(e_UICCN_TxtInput TInput);

#endif
