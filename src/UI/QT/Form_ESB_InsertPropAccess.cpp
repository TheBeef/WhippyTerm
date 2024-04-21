/*******************************************************************************
 * FILENAME: Form_ESB_InsertPropAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (03 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_ESB_InsertProp.h"
#include "ui_Form_ESB_InsertProp.h"
#include "UI/UI_ESB_InsertProp.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_ESB_InsertProp *g_ESB_InsertProp;

/*******************************************************************************
 * NAME:
 *    UIAlloc_ESB_InsertProp
 *
 * SYNOPSIS:
 *    bool UIAlloc_ESB_InsertProp(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the edit send buffer crc type dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_ESB_InsertProp(void)
{
    try
    {
        g_ESB_InsertProp=new Form_ESB_InsertProp(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ESB_InsertProp=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_ESB_InsertProp
 *
 * SYNOPSIS:
 *    bool UIShow_ESB_InsertProp(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the hex display copy as dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_ESB_InsertProp(void)
{
    return g_ESB_InsertProp->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_ESB_InsertProp
 *
 * SYNOPSIS:
 *    void UIFree_ESB_InsertProp(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_ESB_InsertProp()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_ESB_InsertProp(void)
{
    delete g_ESB_InsertProp;

    g_ESB_InsertProp=NULL;
}

/*******************************************************************************
 * NAME:
 *    UIESBIP_GetRadioButton
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *UIESBIP_GetRadioButton(e_ESBIP_RadioButton bttn);
 *
 * PARAMETERS:
 *    bttn [I] -- What radio button to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a radio button in the dialog.
 *
 * RETURNS:
 *    A handle to the buffer or NULL if it's unknown
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIRadioBttnCtrl *UIESBIP_GetRadioButton(e_ESBIP_RadioButton bttn)
{
    switch(bttn)
    {
        case e_ESBIP_RadioButton_Endian_Big:
            return (t_UIRadioBttnCtrl *)g_ESB_InsertProp->ui->Big_radioButton;
        case e_ESBIP_RadioButton_Endian_Little:
            return (t_UIRadioBttnCtrl *)g_ESB_InsertProp->ui->Little_radioButton;
        case e_ESBIP_RadioButton_Bits_8:
            return (t_UIRadioBttnCtrl *)g_ESB_InsertProp->ui->radioButton_8;
        case e_ESBIP_RadioButton_Bits_16:
            return (t_UIRadioBttnCtrl *)g_ESB_InsertProp->ui->radioButton_16;
        case e_ESBIP_RadioButton_Bits_32:
            return (t_UIRadioBttnCtrl *)g_ESB_InsertProp->ui->radioButton_32;
        case e_ESBIP_RadioButton_Bits_64:
            return (t_UIRadioBttnCtrl *)g_ESB_InsertProp->ui->radioButton_64;
        case e_ESBIP_RadioButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}
