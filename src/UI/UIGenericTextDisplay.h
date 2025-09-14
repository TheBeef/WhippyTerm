/*******************************************************************************
 * FILENAME: UIGenericTextDisplay.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the API to the generic text display form in it.  The
 *    deneric text display form is a simple form that just has a text input
 *    set to read only and an OK button.
 *
 * COPYRIGHT:
 *    Copyright 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (13 Sep 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIGENERICTEXTDISPLAY_H_
#define __UIGENERICTEXTDISPLAY_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdbool.h>
#include <stdint.h>
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_GTD_Style_Heading,
    e_GTD_Style_Def,
    e_GTD_Style_DefLevel2,
    e_GTD_Style_TextLine,
    e_GTD_StyleTypeMAX
} e_GTD_StyleType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_GTD_GenericTextDisplay(const char *Title);
bool UIShow_GTD_GenericTextDisplay(void);
void UIFree_GTD_GenericTextDisplay(void);

void UIGTD_ClearCtrlText(void);
void UIGTD_AppendText(const char *NewLine,e_GTD_StyleType Style);

#endif   /* end of "#ifndef __UIGENERICTEXTDISPLAY_H_" */
