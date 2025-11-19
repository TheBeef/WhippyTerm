/*******************************************************************************
 * FILENAME: UIIODriverSettings.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Has the UI access for the IO Driver settings dialog.
 *
 * COPYRIGHT:
 *    Copyright 12 Nov 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (12 Nov 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIIODRIVERSETTINGS_H_
#define __UIIODRIVERSETTINGS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_IODriverSettings(void);
void UIFree_IODriverSettings(void);
bool UIShow_IODriverSettings(void);
t_UITabCtrl *UIIODS_GetTabControl(void);
t_UITab *UIIODS_AddNewTab(const char *Name);
t_UILayoutContainerCtrl *UIIODS_GetTabContainerFrame(t_UITab *UITab);

#endif   /* end of "#ifndef __UIIODRIVERSETTINGS_H_" */
