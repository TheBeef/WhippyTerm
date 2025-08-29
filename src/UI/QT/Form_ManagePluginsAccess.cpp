/*******************************************************************************
 * FILENAME: Form_ManagePluginsAccess.cpp
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
 *    Paul Hutchinson (04 Mar 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_ManagePlugins.h"
#include "ui_Form_ManagePlugins.h"
#include "Form_MainWindow.h"
#include "UI/UIManagePlugins.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_ManagePlugins *g_ManagePluginsDialog;

t_UIListViewCtrl *UIMP_GetListViewHandle(e_UIMP_ListView UIObj)
{
    switch(UIObj)
    {
        case e_UIMP_ListView_PluginList:
            return (t_UIListViewCtrl *)g_ManagePluginsDialog->ui->PluginList_listWidget;

        case e_UIMP_ListViewMAX:
        default:
        break;
    }
    return NULL;
}

t_UIButtonCtrl *UIMP_GetButtonHandle(e_UIMP_Button UIObj)
{
    switch(UIObj)
    {
        case e_UIMP_Button_Enable:
            return (t_UIButtonCtrl *)g_ManagePluginsDialog->ui->Enable_pushButton;
        case e_UIMP_Button_Install:
            return (t_UIButtonCtrl *)g_ManagePluginsDialog->ui->Install_pushButton;
        case e_UIMP_Button_Uninstall:
            return (t_UIButtonCtrl *)g_ManagePluginsDialog->ui->Uninstall_pushButton;

        case e_UIMP_ButtonMAX:
        default:
        break;
    }
    return NULL;
}

t_UITextInputCtrl *UIMP_GetTextInputHandle(e_UIMP_TextInput UIObj)
{
    switch(UIObj)
    {
        case e_UIMP_TextInput_Name:
            return (t_UITextInputCtrl *)g_ManagePluginsDialog->ui->Name_lineEdit;
        case e_UIMP_TextInput_Copyright:
            return (t_UITextInputCtrl *)g_ManagePluginsDialog->ui->Copyright_lineEdit;
        case e_UIMP_TextInput_ReleaseDate:
            return (t_UITextInputCtrl *)g_ManagePluginsDialog->ui->ReleaseDate_lineEdit;
        case e_UIMP_TextInput_Contrib:
            return (t_UITextInputCtrl *)g_ManagePluginsDialog->ui->Contrib_lineEdit;
        case e_UIMP_TextInput_PluginType:
            return (t_UITextInputCtrl *)g_ManagePluginsDialog->ui->PluginType_lineEdit;
        case e_UIMP_TextInput_Version:
            return (t_UITextInputCtrl *)g_ManagePluginsDialog->ui->Version_lineEdit;

        case e_UIMP_TextInputMAX:
        default:
        break;
    }
    return NULL;
}

t_UIMuliLineTextInputCtrl *UIMP_GetMultiLineTextInputHandle(e_UIMP_MultiLineTextInput UIObj)
{
    switch(UIObj)
    {
        case e_UIMP_MultiLineTextInput_Description:
            return (t_UIMuliLineTextInputCtrl *)g_ManagePluginsDialog->ui->Description_textEdit;

        case e_UIMP_MultiLineTextInputMAX:
        default:
        break;
    }
    return NULL;
}

t_UILabelCtrl *UIMP_GetLabelInputHandle(e_UIMP_LabelInput UIObj)
{
    switch(UIObj)
    {
        case e_UIMP_LabelInputMAX:
        default:
        break;
    }
    return NULL;
}

bool UIAlloc_ManagePlugins(void)
{
    try
    {
        g_ManagePluginsDialog=new Form_ManagePlugins(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ManagePluginsDialog=NULL;
        return false;
    }
    return true;
}

bool UIShow_ManagePlugins(void)
{
    if(g_ManagePluginsDialog->exec()==QDialog::Accepted)
    {
        return true;
    }

    return false;
}

void UIFree_ManagePlugins(void)
{
    delete g_ManagePluginsDialog;

    g_ManagePluginsDialog=NULL;
}
