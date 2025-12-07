/*******************************************************************************
 * FILENAME: Dialog_StylePicker.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has a style picker dialog in it.  Normally called from the
 *    UI to do a popup that lets the user select styles, but can be called
 *    from anywhere.
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
 * CREATED BY:
 *    Paul Hutchinson (05 Dec 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_StylePicker.h"
#include "App/IOSystem.h"
#include "App/Connections.h"
#include "UI/UIStylePicker.h"
#include "UI/UIAsk.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunStylePickerDialog
 *
 * SYNOPSIS:
 *    bool RunStylePickerDialog(struct StyleData *SD);
 *
 * PARAMETERS:
 *    SD [I/O] -- The styling to use, and where we store what the user selected.
 *
 * FUNCTION:
 *    This function shows the style picker dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunStylePickerDialog(struct StyleData *SD)
{
    bool RetValue;
    try
    {
        if(!UIAlloc_StylePicker())
            return false;

        UISP_SetStyleData(SD);

        RetValue=UIShow_StylePicker();
        if(RetValue)
        {
            /* Ok, apply any changes */
            UISP_GetStyleData(SD);
        }
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
        RetValue=false;
    }
    catch(...)
    {
        RetValue=false;
    }

    UIFree_StylePicker();

    return RetValue;
}
