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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
