/*******************************************************************************
 * FILENAME: Form_NewConnectionAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_NewConnection.h"
#include "ui_Form_NewConnection.h"
#include "Form_MainWindow.h"
#include "UI/UINewConnection.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_NewConnection *g_NewConnectionDialog;

t_UIComboBoxCtrl *UINC_GetComboBoxHandle(e_UINC_ComboBox CBox)
{
    switch(CBox)
    {
        case e_UINC_ComboBox_Connection:
            return (t_UIComboBoxCtrl *)g_NewConnectionDialog->ui->
                    ConnectionList_comboBox;
        break;
        default:
        case e_UINC_ComboBoxMAX:
        break;
    }
    return NULL;
}

t_UITextInputCtrl *UINC_GetTxtInputHandle(e_UINC_TxtInput TInput)
{
    switch(TInput)
    {
        case e_UINC_TxtInput_Name:
            return (t_UITextInputCtrl *)g_NewConnectionDialog->ui->
                    Name_lineEdit;
        break;
        default:
        case e_UINC_TxtInputMAX:
        break;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UIAlloc_NewConnection
 *
 * SYNOPSIS:
 *    bool UIAlloc_NewConnection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates a new connection dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool UIAlloc_NewConnection(void)
{
    try
    {
        g_NewConnectionDialog=new Form_NewConnection(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_NewConnectionDialog=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_NewConnection
 *
 * SYNOPSIS:
 *    bool UIShow_NewConnection(void);
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
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool UIShow_NewConnection(void)
{
    if(g_NewConnectionDialog->exec()==QDialog::Accepted)
    {
        return true;
    }

    return false;
}

/*******************************************************************************
 * NAME:
 *    UIFree_NewConnection
 *
 * SYNOPSIS:
 *    void UIFree_NewConnection(void);
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
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
void UIFree_NewConnection(void)
{
    delete g_NewConnectionDialog;

    g_NewConnectionDialog=NULL;
}

t_UIContainerCtrl *UINC_GetOptionsFrameContainer(void)
{
    /* Always has to be a QFormLayout based widget */
    return (t_UIContainerCtrl *)g_NewConnectionDialog->ui->Options_Layout;
}
