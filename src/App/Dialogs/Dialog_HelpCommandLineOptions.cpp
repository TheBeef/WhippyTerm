/*******************************************************************************
 * FILENAME: Dialog_HelpCommandLineOptions.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the dialog that shows the command line options help screen.
 *
 * COPYRIGHT:
 *    Copyright 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (13 Sep 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Dialog_HelpCommandLineOptions.h"
#include "UI/UIGenericTextDisplay.h"
#include "UI/UIAsk.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunHCLO_HelpCommandLineOptionsDialog
 *
 * SYNOPSIS:
 *    bool RunHCLO_HelpCommandLineOptionsDialog(void);
 *
 * PARAMETERS:
 *    NONE
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
bool RunHCLO_HelpCommandLineOptionsDialog(void)
{
    bool RetValue;

    try
    {
        if(!UIAlloc_GTD_GenericTextDisplay("Command Line Options"))
            return false;

//USAGE:
//    whippyterm [Options] [URI]...
//
//WHERE:
//    Options -- Supported options:
//        --bookmark -- Open a bookmark.  The name of the bookmark follows the
//                      bookmark option.  You can open more than one bookmark
//                      by using more than one --bookmark options.
//                      Example --bookmark "First Bookmark" --bookmark "Two".
//    URI -- This is a list of URI's to open.  You can open more than one.
//           Example "TCP://192.168.1.99:1234"


        UIGTD_AppendText("USAGE",e_GTD_Style_Heading);
        UIGTD_AppendText("whippyterm [Options] [URI]...",e_GTD_Style_TextLine);
        UIGTD_AppendText("WHERE",e_GTD_Style_Heading);
        UIGTD_AppendText("Options -- Supported options:",e_GTD_Style_Def);
        UIGTD_AppendText("--bookmark -- Open a bookmark.  The name of the "
                "bookmark follows the bookmark option.  You can open more than "
                "one bookmark by using more than one --bookmark options.  "
                "Example --bookmark \"First Bookmark\" --bookmark \"Two\"",
                e_GTD_Style_DefLevel2);
        UIGTD_AppendText("URI -- This is a list of URI's to open.  You can "
                "open more than one. Example \"TCP://192.168.1.99:1234\"",
                e_GTD_Style_Def);

        UIGTD_AppendText("EXAMPLE",e_GTD_Style_Heading);
        UIGTD_AppendText("whippyterm --bookmark \"My bookmark\"  \"LB1:1\" \"LB2:1\"",
                e_GTD_Style_Def);

        UIShow_GTD_GenericTextDisplay();

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

    UIFree_GTD_GenericTextDisplay();

    return RetValue;
}
