/*******************************************************************************
 * FILENAME: Form_ChangeConnectionNameAccess.cpp
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
 *    Paul Hutchinson (19 Sep 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIChangeConnectionName.h"
#include "Form_ChangeConnectionName.h"
#include "main.h"
#include "ui_Form_ChangeConnectionName.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
class Form_ChangeConnectionName *g_ChangeConnectionNameDialog;

/*** VARIABLE DEFINITIONS     ***/
bool UIAlloc_ChangeConnectionName(void)
{
    try
    {
        g_ChangeConnectionNameDialog=new Form_ChangeConnectionName(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ChangeConnectionNameDialog=NULL;
        return false;
    }
    return true;
}

void UIShow_ChangeConnectionName(void)
{
    g_ChangeConnectionNameDialog->exec();
}

void UIFree_ChangeConnectionName(void)
{
    delete g_ChangeConnectionNameDialog;

    g_ChangeConnectionNameDialog=NULL;
}

t_UITextInputCtrl *UICCN_GetTxtInputHandle(e_UICCN_TxtInput TInput)
{
    switch(TInput)
    {
        case e_UICCN_TxtInput_Name:
            return (t_UITextInputCtrl *)g_ChangeConnectionNameDialog->ui->
                    Name_lineEdit;
        break;
        case e_UICCN_TxtInputMAX:
        default:
        break;
    }
    return NULL;
}

