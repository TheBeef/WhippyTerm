/*******************************************************************************
 * FILENAME: Dialog_ESB_ViewSource.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the view source dialog.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (31 Aug 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_ESB_ViewSource.h"
#include "UI/UI_ESB_ViewSource.h"
#include "UI/UIAsk.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunESB_ViewSourceDialog
 *
 * SYNOPSIS:
 *    bool RunESB_ViewSourceDialog(const char *Source);
 *
 * PARAMETERS:
 *    Source [I] -- The text to place in the dialog.
 *
 * FUNCTION:
 *    This function shows the edit send buffer show CRC source dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunESB_ViewSourceDialog(const char *Source)
{
    bool RetValue;

    try
    {
        if(!UIAlloc_ESB_ViewSource())
            return false;

        ESBVS_SetSourceText(Source);

        UIShow_ESB_ViewSource();

        RetValue=true;
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

    UIFree_ESB_ViewSource();

    return RetValue;
}
