/*******************************************************************************
 * FILENAME: Form_CRCFinderAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 04 Jun 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (04 Jun 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_CRCFinder.h"
#include "ui_Form_CRCFinder.h"
#include "UI/UICRCFinder.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_CRCFinder *g_CRCFinder;

/*******************************************************************************
 * NAME:
 *    UIAlloc_CRCFinder
 *
 * SYNOPSIS:
 *    bool UIAlloc_CRCFinder(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the crc finder dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_CRCFinder(void)
{
    try
    {
        g_CRCFinder=new Form_CRCFinder(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_CRCFinder=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_CRCFinder
 *
 * SYNOPSIS:
 *    bool UIShow_CRCFinder(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the crc finder dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_CRCFinder(void)
{
    return g_CRCFinder->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_CRCFinder
 *
 * SYNOPSIS:
 *    void UIFree_CRCFinder(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_CRCFinder()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_CRCFinder(void)
{
    delete g_CRCFinder;

    g_CRCFinder=NULL;
}

/*******************************************************************************
 * NAME:
 *    UICF_GetButton
 *
 * SYNOPSIS:
 *    t_UIButtonCtrl *UICF_GetButton(e_CF_Button bttn);
 *
 * PARAMETERS:
 *    bttn [I] -- What button to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a button in the dialog.
 *
 * RETURNS:
 *    A handle to the buffer or NULL if it's unknown
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIButtonCtrl *UICF_GetButton(e_CF_Button bttn)
{
    switch(bttn)
    {
        case e_CF_Button_FindCRC:
            return (t_UIButtonCtrl *)g_CRCFinder->ui->FindCRC_pushButton;
        case e_CF_Button_ShowSource:
            return (t_UIButtonCtrl *)g_CRCFinder->ui->ShowSource_pushButton;
        case e_CF_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICF_GetTextInput
 *
 * SYNOPSIS:
 *    t_UITextInputCtrl *UICF_GetTextInput(e_CF_TextInput UIObj);
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
t_UITextInputCtrl *UICF_GetTextInput(e_CF_TextInput UIObj)
{
    switch(UIObj)
    {
        case e_CF_TextInput_CRC:
            return (t_UITextInputCtrl *)g_CRCFinder->ui->CRC_lineEdit;
        case e_CF_TextInputMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICF_GetComboBoxHandle
 *
 * SYNOPSIS:
 *    t_UIComboBoxCtrl *UICF_GetComboBoxHandle(e_CF_ComboxType UIObj);
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
t_UIComboBoxCtrl *UICF_GetComboBoxHandle(e_CF_ComboxType UIObj)
{
    switch(UIObj)
    {
        case e_CF_Combox_CRCType:
            return (t_UIComboBoxCtrl *)g_CRCFinder->ui->CRCType_comboBox;
        case e_CF_ComboxMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICF_GetRadioBttnInput
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *UICF_GetRadioBttnInput(e_CF_RadioBttnType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The radio button to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a radio button.
 *
 * RETURNS:
 *    The handle to the radio button or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIRadioBttnCtrl *UICF_GetRadioBttnInput(e_CF_RadioBttnType UIObj)
{
    switch(UIObj)
    {
        case e_CF_RadioBttn_Big:
            return (t_UIRadioBttnCtrl *)g_CRCFinder->ui->BigEndian_radioButton;
        case e_CF_RadioBttn_Little:
            return (t_UIRadioBttnCtrl *)g_CRCFinder->ui->LittleEndian_radioButton;
        case e_CF_RadioBttnMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICF_GetHexContainerFrame
 *
 * SYNOPSIS:
 *    t_UIFrameContainerCtrl *UICF_GetHexContainerFrame(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the frame handle for the hex display.
 *
 * RETURNS:
 *    A pointer to the hex display frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIFrameContainerCtrl *UICF_GetHexContainerFrame(void)
{
    return (t_UIFrameContainerCtrl *)g_CRCFinder->ui->Hex_frame;
}

