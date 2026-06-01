/*******************************************************************************
 * FILENAME: UISettingsSelectEditType.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    UI access file for the settects select edit type dialog.
 *
 * COPYRIGHT:
 *    Copyright 31 May 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (31 May 2026)
 *       Created
 *
 *******************************************************************************/
#ifndef __UISETTINGSSELECTEDITTYPE_H_
#define __UISETTINGSSELECTEDITTYPE_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_USSET_UserSelected_Global,
    e_USSET_UserSelected_Connection,
    e_USSET_UserSelected_Cancel,
    e_USSET_UserSelectedMAX
} e_USSET_UserSelectedType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_SettingSelectEditType(void);
e_USSET_UserSelectedType UIShow_SettingSelectEditType(void);
void UIFree_SettingSelectEditType(void);

#endif   /* end of "#ifndef __UISETTINGSSELECTEDITTYPE_H_" */
