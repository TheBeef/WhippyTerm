/*******************************************************************************
 * FILENAME: Dialog_PasteData.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the dialog for paste from clipboard type in it.
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
 * CREATED BY:
 *    Paul Hutchinson (15 Oct 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_PasteData.h"
#include "UI/UIPasteData.h"
#include "UI/UIAsk.h"
#include <string>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunPasteDataDialog
 *
 * SYNOPSIS:
 *    e_PasteDataType RunPasteDataDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the hex display "copy as" dialog.
 *
 * RETURNS:
 *    The type of data to grab from the clipboard or 'e_PasteData_Cancel' to
 *    cancel.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_PasteDataType RunPasteDataDialog(void)
{
    e_PasteDataType RetValue;

    try
    {
        if(!UIAlloc_PasteData())
            return e_PasteData_Cancel;

        RetValue=UIShow_PasteData();
    }
    catch(...)
    {
        RetValue=e_PasteData_Cancel;
    }

    UIFree_PasteData();

    return RetValue;
}
