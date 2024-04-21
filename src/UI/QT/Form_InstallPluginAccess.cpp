/*******************************************************************************
 * FILENAME: Form_InstallPluginAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (06 Mar 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_InstallPlugin.h"
#include "ui_Form_InstallPlugin.h"
#include "Form_MainWindow.h"
#include "UI/UIInstallPlugin.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_InstallPlugin *g_InstallPluginDialog;

t_UITextInputCtrl *UIIP_GetTextInputHandle(e_UIIP_TextInput UIObj)
{
    switch(UIObj)
    {
        case e_UIIP_TextInput_Name:
            return (t_UITextInputCtrl *)g_InstallPluginDialog->ui->Name_lineEdit;
        case e_UIIP_TextInput_Copyright:
            return (t_UITextInputCtrl *)g_InstallPluginDialog->ui->Copyright_lineEdit;
        case e_UIIP_TextInput_ReleaseDate:
            return (t_UITextInputCtrl *)g_InstallPluginDialog->ui->ReleaseDate_lineEdit;
        case e_UIIP_TextInput_Contrib:
            return (t_UITextInputCtrl *)g_InstallPluginDialog->ui->Contrib_lineEdit;
        case e_UIIP_TextInput_PluginType:
            return (t_UITextInputCtrl *)g_InstallPluginDialog->ui->PluginType_lineEdit;
        case e_UIIP_TextInput_Version:
            return (t_UITextInputCtrl *)g_InstallPluginDialog->ui->Version_lineEdit;

        case e_UIIP_TextInputMAX:
        default:
        break;
    }
    return NULL;
}

t_UIMuliLineTextInputCtrl *UIIP_GetMultiLineTextInputHandle(e_UIIP_MultiLineTextInput UIObj)
{
    switch(UIObj)
    {
        case e_UIIP_MultiLineTextInput_Description:
            return (t_UIMuliLineTextInputCtrl *)g_InstallPluginDialog->ui->Description_textEdit;

        case e_UIIP_MultiLineTextInputMAX:
        default:
        break;
    }
    return NULL;
}

bool UIAlloc_InstallPlugin(void)
{
    try
    {
        g_InstallPluginDialog=new Form_InstallPlugin(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_InstallPluginDialog=NULL;
        return false;
    }
    return true;
}

bool UIShow_InstallPlugin(void)
{
    if(g_InstallPluginDialog->exec()==QDialog::Accepted)
    {
        return true;
    }

    return false;
}

void UIFree_InstallPlugin(void)
{
    delete g_InstallPluginDialog;

    g_InstallPluginDialog=NULL;
}
