/*******************************************************************************
 * FILENAME: Dialog_SettingsSelectEditType.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the dialog for when the user selects to edit the global
 *    settings but the current active connect has custom settings.  It should
 *    let them select if they want to edit the global settings (that may not
 *    affect the current connection) or edit the connection settings.
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
 * CREATED BY:
 *    Paul Hutchinson (31 May 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_SettingsSelectEditType.h"
#include "UI/UISettingsSelectEditType.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunSettingSelectEditTypeDialog
 *
 * SYNOPSIS:
 *    e_USSET_UserSelectedType RunSettingSelectEditTypeDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the settings select edit type dialog.
 *
 * RETURNS:
 *    e_USSET_UserSelected_Global -- User selected they want global settings
 *    e_USSET_UserSelected_Connection -- User selected they want connection
 *                                       settings
 *    e_USSET_UserSelected_Cancel -- User canceled the settings.
 *    e_USSET_UserSelectedMAX -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_USSET_UserSelectedType RunSettingSelectEditTypeDialog(void)
{
    e_USSET_UserSelectedType RetValue;

    if(!UIAlloc_SettingSelectEditType())
        return e_USSET_UserSelectedMAX;

    RetValue=UIShow_SettingSelectEditType();

    UIFree_SettingSelectEditType();

    return RetValue;
}
