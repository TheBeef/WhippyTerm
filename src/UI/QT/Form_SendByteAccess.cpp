/*******************************************************************************
 * FILENAME: Form_SendByteAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 02 May 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (02 May 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_SendByte.h"
#include "ui_Form_SendByte.h"
#include "UI/UISendByte.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_SendByte *g_SendByte;

/*******************************************************************************
 * NAME:
 *    UIAlloc_SendByte
 *
 * SYNOPSIS:
 *    bool UIAlloc_SendByte(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the hex dispaly copy as dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_SendByte(void)
{
    try
    {
        g_SendByte=new Form_SendByte(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_SendByte=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_SendByte
 *
 * SYNOPSIS:
 *    bool UIShow_SendByte(void);
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
bool UIShow_SendByte(void)
{
    return g_SendByte->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_SendByte
 *
 * SYNOPSIS:
 *    void UIFree_SendByte(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_SendByte()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_SendByte(void)
{
    delete g_SendByte;

    g_SendByte=NULL;
}

/*******************************************************************************
 * NAME:
 *    UISBD_GetTextInput
 *
 * SYNOPSIS:
 *    t_UITextInputCtrl *UISBD_GetTextInput(e_ESB_TextInput UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The text input to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a text input.
 *
 * RETURNS:
 *    The handle to the text input or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UITextInputCtrl *UISBD_GetTextInput(e_SBD_TextInput UIObj)
{
    switch(UIObj)
    {
        case e_SBD_TextInput_Dec:
            return (t_UITextInputCtrl *)g_SendByte->ui->Dec_lineEdit;
        case e_SBD_TextInput_Hex:
            return (t_UITextInputCtrl *)g_SendByte->ui->Hex_lineEdit;
        case e_SBD_TextInput_Oct:
            return (t_UITextInputCtrl *)g_SendByte->ui->Oct_lineEdit;
        case e_SBD_TextInputMAX:
        default:
            return NULL;
    }
    return NULL;
}
