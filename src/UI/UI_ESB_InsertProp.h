/*******************************************************************************
 * FILENAME: UI_ESB_InsertProp.h
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
#ifndef __UI_ESB_INSERTPROP_H_
#define __UI_ESB_INSERTPROP_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/
enum e_ESBIP_RadioButton
{
    e_ESBIP_RadioButton_Endian_Big,
    e_ESBIP_RadioButton_Endian_Little,
    e_ESBIP_RadioButton_Bits_8,
    e_ESBIP_RadioButton_Bits_16,
    e_ESBIP_RadioButton_Bits_32,
    e_ESBIP_RadioButton_Bits_64,
    e_ESBIP_RadioButtonMAX
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_InsertProp(void);
bool UIShow_ESB_InsertProp(void);
void UIFree_ESB_InsertProp(void);
t_UIRadioBttnCtrl *UIESBIP_GetRadioButton(e_ESBIP_RadioButton bttn);

#endif
