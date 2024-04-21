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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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

