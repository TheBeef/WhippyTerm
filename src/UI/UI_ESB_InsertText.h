/*******************************************************************************
 * FILENAME: UI_ESB_InsertText.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (28 May 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __UI_ESB_INSERTTEXT_H_
#define __UI_ESB_INSERTTEXT_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_ESBIT_MuliLineTextInput
{
    e_ESBIT_MuliLineTextInput_InsertText,
    e_ESBIT_MuliLineTextInputMAX
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_InsertText(void);
bool UIShow_ESB_InsertText(void);
void UIFree_ESB_InsertText(void);
t_UIMuliLineTextInputCtrl *UIESBIT_GetMuliLineTextInput(e_ESBIT_MuliLineTextInput input);

#endif
