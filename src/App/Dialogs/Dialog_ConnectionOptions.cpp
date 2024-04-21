/*******************************************************************************
 * FILENAME: Dialog_ConnectionOptions.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the connection options dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (10 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_ConnectionOptions.h"
#include "App/IOSystem.h"
#include "App/Connections.h"
#include "UI/UIConnectionOptions.h"
#include "UI/UIAsk.h"
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
t_ConnectionOptionsDataType *m_CO_ConnectionOptionsWidgets=NULL;

/*******************************************************************************
 * NAME:
 *    RunConnectionOptionsDialog
 *
 * SYNOPSIS:
 *    bool RunConnectionOptionsDialog(const std::string &UniqueID,
 *              t_KVList &Options);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The unique ID that identifies the connection.
 *    Options [I] -- The options for this connection.
 *
 * FUNCTION:
 *    This function shows the set connection options dialog.  It will edit the
 *    requested connection.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunConnectionOptionsDialog(class Connection *Con)
{
    bool RetValue;
    string UniqueID;
    t_KVList Options;

    try
    {
        if(!UIAlloc_ConnectionOptions())
            return false;

        Con->GetConnectionUniqueID(UniqueID);
        if(!Con->GetConnectionOptions(Options))
            throw("Failed to get connection options");

        m_CO_ConnectionOptionsWidgets=IOS_AllocConnectionOptionsFromUniqueID(
                UniqueID.c_str(),UI_CO_GetOptionsFrameContainer(),Options,
                NULL,0);

        RetValue=UIShow_ConnectionOptions();
        if(RetValue)
        {
            /* Ok, apply any changes */

            /* Grap from UI */
            IOS_StoreConnectionOptions(m_CO_ConnectionOptionsWidgets,Options);

            /* Send to connection */
            if(!Con->SetConnectionOptions(Options))
                throw("There was an error applying connection options");
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

    if(m_CO_ConnectionOptionsWidgets!=NULL)
    {
        IOS_FreeConnectionOptions(m_CO_ConnectionOptionsWidgets);
        m_CO_ConnectionOptionsWidgets=NULL;
    }

    UIFree_ConnectionOptions();

    return RetValue;
}
