/*******************************************************************************
 * FILENAME: Dialog_About.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
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
 *    Paul Hutchinson (06 Jul 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_About.h"
#include "App/MainApp.h"
#include "UI/UIAbout.h"
#include "Version.h"
#include <stdio.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunAboutDialog
 *
 * SYNOPSIS:
 *    void RunAboutDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the about dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunAboutDialog(void)
{
    char buff[1000];

    if(!UIAlloc_About())
        return;

    UISetAboutName(WHIPPYTERM_TITLE " " VER_STR(WHIPPYTERM_VERSION_MAJOR) "." VER_STR(WHIPPYTERM_VERSION_MINOR));

    snprintf(buff,sizeof(buff)-1,
            "A GUI based modern serial terminal<br/>"
            "<br/>"
            "Full Version:%s<br/>"
            "Build date  :%s<br/>"
            "Build time  :%s<br/>",
            WHIPPYTERM_VERSION_STR,
            __DATE__,
            __TIME__);
    UISetAboutText(buff,e_UIAboutTab_About);

    snprintf(buff,sizeof(buff)-1,"Paul Hutchinson<br/>paul@whippyterm.com");
    UISetAboutText(buff,e_UIAboutTab_Author);

    snprintf(buff,sizeof(buff)-1,
            "Copyright 2025 Paul Hutchinson.<br/>"
            ""
            "<p>This program is free software: you can redistribute it and/or modify it "
            "under the terms of the GNU General Public License as published by the "
            "Free Software Foundation, either version 3 of the License, or (at your "
            "option) any later version.</p>"
            ""
            "<p>This program is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU "
            "General Public License for more details.</p>"
            ""
            "<p>You should have received a copy of the GNU General Public License along "
            "with this program. If not, see https://www.gnu.org/licenses/.</p>");
    UISetAboutText(buff,e_UIAboutTab_License);

    snprintf(buff,sizeof(buff)-1,
            "%s was build using the following libraries:<br/>"
            "<table>"
            "<tr><td>%s</td><td>%s</td><td><a href='https://www.qt.io/'>https://www.qt.io/</a></td>"
            "<tr><td>UTF8-CPP</td><td>2.3</td><td><a href='https://github.com/nemtrif/utfcpp'>https://github.com/nemtrif/utfcpp</a></td>"
            "<tr><td>strnatcmp</td><td></td><td><a href='https://github.com/sourcefrog/natsort'>https://github.com/sourcefrog/natsort</a></td>"
            "<tr><td>Silk icons</td><td>1.3</td><td><a href='http://www.famfamfam.com/lab/icons/silk/'>http://www.famfamfam.com/lab/icons/silk/</a></td>"
            "<tr><td>Computer Error Alert</td><td>1.0</td><td><a href='https://soundbible.com/1540-Computer-Error-Alert.html'>https://soundbible.com/1540-Computer-Error-Alert.html</a></td>"
            "<tr><td>Bell Icon designed by Pixel perfect from Flaticon</td><td>1.0</td><td><a href='https://www.flaticon.com/free-icon/bell_1827272'>https://www.flaticon.com/free-icon/bell_1827272</a></td>"
            "</table>",
            WHIPPYTERM_NAME,
            GetGUIEngineName(),
            GetGUIEngineVersion());
    UISetAboutText(buff,e_UIAboutTab_Libraries);

    UIShow_About();
    UIFree_About();
}
