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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (06 Jul 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_About.h"
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

    UISetAboutName(WHIPPYTERM_NAME " " VER_STR(WHIPPYTERM_VERSION_MAJOR) "." VER_STR(WHIPPYTERM_VERSION_MINOR));

/* DEBUG PAUL: At some point:
    * Fix up this program is text
    * Add copyright to below text
    * Add links the website and repos
*/

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

    snprintf(buff,sizeof(buff)-1,"Paul Hutchinson<br/>hutchinson.paul@gmail.com");
    UISetAboutText(buff,e_UIAboutTab_Author);

    snprintf(buff,sizeof(buff)-1,"Prerelease.  License not selected yet.  Source not released.  So for now:<br/><br/>"
        "Copyright (C) Paul Hutchinson - All Rights Reserved<br/>"
        "Unauthorized copying of this file, via any medium is strictly prohibited<br/>"
        "Proprietary and confidential<br/>"
        "Written by Paul Hutchinson &lt;hutchinson.paul@gmail.com&gt;, 2020<br/>");
    UISetAboutText(buff,e_UIAboutTab_License);

    snprintf(buff,sizeof(buff)-1,
            "%s was build using the following libraries:<br/>"
            "<table>"
            "<tr><td>%s</td><td>%s</td><td><a href='https://www.qt.io/'>https://www.qt.io/</a></td>"
            "<tr><td>UTF8-CPP</td><td>2.3</td><td><a href='https://github.com/nemtrif/utfcpp'>https://github.com/nemtrif/utfcpp</a></td>"
            "<tr><td>strnatcmp</td><td></td><td><a href='https://github.com/sourcefrog/natsort'>https://github.com/sourcefrog/natsort</a></td>"
            "<tr><td>Silk icons</td><td>1.3</td><td><a href='http://www.famfamfam.com/lab/icons/silk/'>http://www.famfamfam.com/lab/icons/silk/</a></td>"
            "</table>",
            WHIPPYTERM_NAME,
            GetGUIEngineName(),
            GetGUIEngineVersion());
    UISetAboutText(buff,e_UIAboutTab_Libraries);

    UIShow_About();
    UIFree_About();
}
