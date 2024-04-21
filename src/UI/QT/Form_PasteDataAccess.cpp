/*******************************************************************************
 * FILENAME: Form_PasteDataAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access functions for the Form_PasteData file in it.
 *    This provides access to the form without mixing the QT auto gen code
 *    with the hand written code.
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
#include "Form_PasteData.h"
#include "ui_Form_PasteData.h"
#include "UI/UIPasteData.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_PasteData *g_PasteData;

/*******************************************************************************
 * NAME:
 *    UIAlloc_PasteData
 *
 * SYNOPSIS:
 *    bool UIAlloc_PasteData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the paste data dialog.  This is used to prompt
 *    the user for what format they want to read the data out of the clipboard
 *    in.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_PasteData(void)
{
    try
    {
        g_PasteData=new Form_PasteData(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_PasteData=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_PasteData
 *
 * SYNOPSIS:
 *    e_PasteDataType UIShow_PasteData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the hex display copy as dialog.
 *
 * RETURNS:
 *    e_PasteData_Text -- The user selected text mode
 *    e_PasteData_HexDump -- The user selected Hex Dump mode
 *    e_PasteData_Cancel -- The user canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_PasteDataType UIShow_PasteData(void)
{
    if(!g_PasteData->exec())
        return e_PasteData_Cancel;

    return g_PasteData->SelectedDataType;
}

/*******************************************************************************
 * NAME:
 *    UIFree_PasteData
 *
 * SYNOPSIS:
 *    void UIFree_PasteData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_PasteData()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_PasteData(void)
{
    delete g_PasteData;

    g_PasteData=NULL;
}

