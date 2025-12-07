/*******************************************************************************
 * FILENAME: UIStylePicker.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the UI definitions for the style picker dialog.
 *
 * COPYRIGHT:
 *    Copyright 05 Dec 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (05 Dec 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __UISTYLEPICKER_H_
#define __UISTYLEPICKER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include "PluginSDK/PluginUI.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_StylePicker(void);
void UIFree_StylePicker(void);
bool UIShow_StylePicker(void);

void UISP_SetStyleData(struct StyleData *SD);
void UISP_GetStyleData(struct StyleData *SD);

#endif   /* end of "#ifndef __UISTYLEPICKER_H_" */
