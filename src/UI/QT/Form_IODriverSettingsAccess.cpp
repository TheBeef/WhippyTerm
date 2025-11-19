/*******************************************************************************
 * FILENAME: Form_IODriverSettingsAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 14 Jun 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (14 Jun 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_IODriverSettings.h"
#include "main.h"
#include "ui_Form_IODriverSettings.h"
#include "UI/UIIODriverSettings.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_IODriverSettings *g_IODriverSettingsDialog;

/*******************************************************************************
 * NAME:
 *    UIAlloc_IODriverSettings
 *
 * SYNOPSIS:
 *    bool UIAlloc_IODriverSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the data pro plugin settings dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_IODriverSettings(void)
{
    try
    {
        g_IODriverSettingsDialog=new Form_IODriverSettings(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_IODriverSettingsDialog=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIFree_IODriverSettings
 *
 * SYNOPSIS:
 *    void UIFree_IODriverSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the current data pro plugin settings dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_IODriverSettings(void)
{
    delete g_IODriverSettingsDialog;

    g_IODriverSettingsDialog=NULL;
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
 *    This function shows the data pro plugin settings dialog that was
 *    allocated.  You can only have 1 data pro plugin settings dialog active
 *    at a time.
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
bool UIShow_IODriverSettings(void)
{
    return g_IODriverSettingsDialog->exec();
}

t_UITab *UIIODS_AddNewTab(const char *Name)
{
    QWidget *NewTab;
    QVBoxLayout *verticalLayout;
    QFormLayout *NewFormLayout;

    NewTab = new QWidget();

    verticalLayout = new QVBoxLayout(NewTab);
    NewFormLayout = new QFormLayout();

    verticalLayout->addLayout(NewFormLayout);

    g_IODriverSettingsDialog->ui->tabWidget->addTab(NewTab,Name);

    return (t_UITab *)NewTab;
}

t_UITabCtrl *UIIODS_GetTabControl(void)
{
    return (t_UITabCtrl *)g_IODriverSettingsDialog->ui->tabWidget;
}

t_UILayoutContainerCtrl *UIIODS_GetTabContainerFrame(t_UITab *UITab)
{
    QWidget *tab=(QWidget *)UITab;
    QVBoxLayout *verticalLayout;
    QLayoutItem *Item;
    QLayout *FormLayout;

    verticalLayout=(QVBoxLayout *)tab->children().first();
    Item=verticalLayout->itemAt(0);
    FormLayout=Item->layout();
    return (t_UILayoutContainerCtrl *)FormLayout;
}
