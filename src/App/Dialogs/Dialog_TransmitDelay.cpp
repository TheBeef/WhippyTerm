/*******************************************************************************
 * FILENAME: Dialog_TransmitDelay.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the code to run the transmit delay dialog in it.
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
 *    Paul Hutchinson (12 Feb 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_TransmitDelay.h"
#include "App/IOSystem.h"
#include "App/Connections.h"
#include "UI/UITransmitDelay.h"
#include "UI/UIAsk.h"
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunTransmitDelayDialog
 *
 * SYNOPSIS:
 *    bool RunTransmitDelayDialog(const std::string &UniqueID,
 *              t_KVList &Options);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The unique ID that identifies the connection.
 *    Options [I] -- The options for this connection.
 *
 * FUNCTION:
 *    This function shows the transmit delay dialog.  It will edit the
 *    requested connection.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunTransmitDelayDialog(class Connection *Con)
{
    bool RetValue;
    string UniqueID;
    t_KVList Options;

    try
    {
        if(!UIAlloc_TransmitDelay())
            return false;

        UITD_SetByteDelay(Con->GetTransmitDelayPerByte());
        UITD_SetLineDelay(Con->GetTransmitDelayPerLine());

        RetValue=UIShow_TransmitDelay();
        if(RetValue)
        {
            /* Ok, apply any changes */
            Con->SetTransmitDelayPerByte(UITD_GetByteDelay());
            Con->SetTransmitDelayPerLine(UITD_GetLineDelay());
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

    UIFree_TransmitDelay();

    return RetValue;
}
