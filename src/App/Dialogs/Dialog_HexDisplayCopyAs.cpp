/*******************************************************************************
 * FILENAME: Dialog_HexDisplayCopyAs.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the dialog for the hex display panel "copy as" popup in it.
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
 * CREATED BY:
 *    Paul Hutchinson (14 Jun 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_HexDisplayCopyAs.h"
#include "UI/UIHexDisplayCopyAs.h"
#include "UI/UIAsk.h"
#include <string>
#include <map>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunHexDisplayCopyAsDialog
 *
 * SYNOPSIS:
 *    bool RunHexDisplayCopyAsDialog(e_HDBCFormatType &SelectedFormat);
 *
 * PARAMETERS:
 *    SelectedFormat [I/O] -- What format to select and was selected
 *
 * FUNCTION:
 *    This function shows the hex display "copy as" dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunHexDisplayCopyAsDialog(e_HDBCFormatType &SelectedFormat)
{
    bool RetValue;

    try
    {
        if(!UIAlloc_HexDisplayCopyAs())
            return false;

        SetFormat_HexDisplayCopyAs(SelectedFormat);

        RetValue=UIShow_HexDisplayCopyAs();

        if(RetValue)
            SelectedFormat=GetFormat_HexDisplayCopyAs();
    }
    catch(...)
    {
        RetValue=false;
    }

    UIFree_HexDisplayCopyAs();

    return RetValue;
}
