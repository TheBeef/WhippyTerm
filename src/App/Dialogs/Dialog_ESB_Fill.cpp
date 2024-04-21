/*******************************************************************************
 * FILENAME: Dialog_ESB_Fill.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (02 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_ESB_Fill.h"
#include "UI/UI_ESB_Fill.h"
#include "UI/UIAsk.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunESB_FillDialog
 *
 * SYNOPSIS:
 *    bool RunESB_FillDialog(uint8_t &Value);
 *
 * PARAMETERS:
 *    Value [I/O] -- The value to place in the dialog and the selected value.
 *
 * FUNCTION:
 *    This function shows the edit send buffer fill dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunESB_FillDialog(uint8_t &Value)
{
    bool RetValue;

    try
    {
        if(!UIAlloc_ESB_Fill())
            return false;

        ESBF_SetFillValue(Value);

        RetValue=UIShow_ESB_Fill();
        if(RetValue)
        {
            Value=ESBF_GetFillValue();
            RetValue=true;
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

    UIFree_ESB_Fill();

    return RetValue;
}
