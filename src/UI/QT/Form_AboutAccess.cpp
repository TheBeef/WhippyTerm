/*******************************************************************************
 * FILENAME: Form_AboutAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Handles access to the about dialog.
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
#include "Form_About.h"
#include "main.h"
#include "ui_Form_About.h"
#include "UI/UIAbout.h"
#include <QtGlobal>
#include <stdio.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_About *g_AboutDialog;

/*******************************************************************************
 * NAME:
 *    UIAlloc_About
 *
 * SYNOPSIS:
 *    bool UIAlloc_About(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the about dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_About(void)
{
    try
    {
        g_AboutDialog=new Form_About(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_AboutDialog=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_About
 *
 * SYNOPSIS:
 *    bool UIShow_About(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the new connection dialog that was allocated.
 *    You can only have 1 new connection dialog active at a time.
 *
 *    This is a blocking call.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIShow_About(void)
{
    g_AboutDialog->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_About
 *
 * SYNOPSIS:
 *    void UIFree_About(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the current new connection dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_About(void)
{
    delete g_AboutDialog;

    g_AboutDialog=NULL;
}

/*******************************************************************************
 * NAME:
 *    UISetAboutName
 *
 * SYNOPSIS:
 *    void UISetAboutName(const char *Name);
 *
 * PARAMETERS:
 *    Name [I] -- The name to set
 *
 * FUNCTION:
 *    This function sets the name line.  This is used for the name of the
 *    program and the version number.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UISetAboutName(const char *Name)
{
    g_AboutDialog->ui->ProgramName_label->setText(Name);
}

/*******************************************************************************
 * NAME:
 *    UISetAboutText
 *
 * SYNOPSIS:
 *    void UISetAboutText(const char *Text,e_UIAboutTabType Tab);
 *
 * PARAMETERS:
 *    Text [I] -- The text to set.  This is HTML
 *    Tab [I] -- The tab to set the text for
 *
 * FUNCTION:
 *    This function sets the text to display in the about box.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UISetAboutText(const char *Text,e_UIAboutTabType Tab)
{
    if(Tab>=e_UIAboutTabMAX)
        return;

    switch(Tab)
    {
        case e_UIAboutTab_About:
            g_AboutDialog->ui->About_textEdit->setHtml(Text);
        break;
        case e_UIAboutTab_Author:
            g_AboutDialog->ui->Author_textEdit->setHtml(Text);
        break;
        case e_UIAboutTab_License:
            g_AboutDialog->ui->License_textEdit->setHtml(Text);
        break;
        case e_UIAboutTab_Libraries:
            g_AboutDialog->ui->Libraries_textEdit->setHtml(Text);
        break;
        case e_UIAboutTabMAX:
        default:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    GetGUIEngineName
 *
 * SYNOPSIS:
 *    const char *GetGUIEngineName(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the name of the GUI framework used.  If there is no
 *    framework in use then this returns "".
 *
 * RETURNS:
 *    The name of the framework
 *
 * SEE ALSO:
 *    GetGUIEngineVersion()
 ******************************************************************************/
const char *GetGUIEngineName(void)
{
    return "QT";
}

/*******************************************************************************
 * NAME:
 *    GetGUIEngineVersion
 *
 * SYNOPSIS:
 *    const char *GetGUIEngineVersion(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function return the version of the GUI framework in use.  This
 *    will be "" if there is no framework in use.
 *
 * RETURNS:
 *    A string with the frameworks version in it.  The format depends on the
 *    framework.
 *
 * SEE ALSO:
 *    GetGUIEngineName()
 ******************************************************************************/
const char *GetGUIEngineVersion(void)
{
    static char RetStr[100];
    int A,B,C,D;

    A=(QT_VERSION>>24)&0xFF;
    B=(QT_VERSION>>16)&0xFF;
    C=(QT_VERSION>>8)&0xFF;
    D=(QT_VERSION>>0)&0xFF;

    if(A==0 && B==0 && C==0)
        snprintf(RetStr,sizeof(RetStr)-1,"%d",D);
    else if(A==0 && B==0)
        snprintf(RetStr,sizeof(RetStr)-1,"%d.%d",C,D);
    else if(A==0)
        snprintf(RetStr,sizeof(RetStr)-1,"%d.%d.%d",B,C,D);
    else
        snprintf(RetStr,sizeof(RetStr)-1,"%d.%d.%d.%d",A,B,C,D);

    return RetStr;
}

