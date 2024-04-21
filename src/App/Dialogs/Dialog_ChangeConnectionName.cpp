/*******************************************************************************
 * FILENAME: Dialog_ChangeConnectionName.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the change connection name dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (19 Sep 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/MaxSizes.h"
#include "Dialog_ChangeConnectionName.h"
#include "UI/UIChangeConnectionName.h"
#include "UI/UIControl.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunChangeConnectionNameDialog
 *
 * SYNOPSIS:
 *    void RunChangeConnectionNameDialog(std::string &Name);
 *
 * PARAMETERS:
 *    Name [I/O] -- The name of this connection to change.  This will be set
 *                  to the value the user inputs.
 *
 * FUNCTION:
 *    This function shows the change connection dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunChangeConnectionNameDialog(std::string &Name)
{
    t_UITextInputCtrl *NameInput;
    char NewName[MAX_CONNECTION_NAME_LEN+1];

    if(!UIAlloc_ChangeConnectionName())
        return;

    NameInput=UICCN_GetTxtInputHandle(e_UICCN_TxtInput_Name);
    UISetTextCtrlText(NameInput,Name.c_str());

    UIShow_ChangeConnectionName();

    UIGetTextCtrlText(NameInput,NewName,MAX_CONNECTION_NAME_LEN);
    Name=NewName;

    UIFree_ChangeConnectionName();
}
