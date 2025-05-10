/*******************************************************************************
 * FILENAME: Form_BridgeConnectionAccess.cpp
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
 *    Paul Hutchinson (27 Aug 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_BridgeConnection.h"
#include "ui_Form_BridgeConnection.h"
#include "UI/UIBridgeConnections.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

class Form_BridgeConnection *g_BridgeCon;

/*******************************************************************************
 * NAME:
 *    UIAlloc_BridgeCon
 *
 * SYNOPSIS:
 *    bool UIAlloc_BridgeCon(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the bridge connections dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_BridgeCon(void)
{
    try
    {
        g_BridgeCon=new Form_BridgeConnection(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_BridgeCon=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_BridgeCon
 *
 * SYNOPSIS:
 *    bool UIShow_BridgeCon(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the bridge connections dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_BridgeCon(void)
{
    return g_BridgeCon->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_BridgeCon
 *
 * SYNOPSIS:
 *    void UIFree_BridgeCon(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_BridgeCon()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_BridgeCon(void)
{
    delete g_BridgeCon;

    g_BridgeCon=NULL;
}

/*******************************************************************************
 * NAME:
 *    UIBC_GetComboBoxHandle
 *
 * SYNOPSIS:
 *    t_UIComboBoxCtrl *UIBC_GetComboBoxHandle(e_BC_ComboxType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The combox box to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a combox box.
 *
 * RETURNS:
 *    The handle to the combox box or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIComboBoxCtrl *UIBC_GetComboBoxHandle(e_BC_ComboxType UIObj)
{
    switch(UIObj)
    {
        case e_BC_Combox_Connection2:
            return (t_UIComboBoxCtrl *)g_BridgeCon->ui->comboBox_Connection2;
        case e_BC_ComboxMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UIBC_GetCheckboxHandle
 *
 * SYNOPSIS:
 *    t_UICheckboxCtrl *UIBC_GetCheckboxHandle(e_BC_CheckboxType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The check box to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a check box.
 *
 * RETURNS:
 *    The handle to the check box or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UICheckboxCtrl *UIBC_GetCheckboxHandle(e_BC_CheckboxType UIObj)
{
    switch(UIObj)
    {
        case e_BC_Checkbox_Locked:
            return (t_UICheckboxCtrl *)g_BridgeCon->ui->checkBox_Lock;
        case e_BC_CheckboxMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UIBC_GetLabelHandle
 *
 * SYNOPSIS:
 *    t_UILabelCtrl *UIBC_GetLabelHandle(e_BC_LabelType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The label to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a label.
 *
 * RETURNS:
 *    The handle to the label or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UILabelCtrl *UIBC_GetLabelHandle(e_BC_LabelType UIObj)
{
    switch(UIObj)
    {
        case e_BC_Label_Connection1:
            return (t_UILabelCtrl *)g_BridgeCon->ui->label_Connection1;
        case e_BC_LabelMAX:
        default:
            return NULL;
    }
    return NULL;
}

