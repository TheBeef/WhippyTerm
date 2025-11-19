/*******************************************************************************
 * FILENAME: Form_CalcCrcAccess.cpp
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
 *    Paul Hutchinson (16 Aug 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_CalcCrc.h"
#include "ui_Form_CalcCrc.h"
#include "UI/UICalcCrc.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_CalcCrc *g_CalcCrc;

/*******************************************************************************
 * NAME:
 *    UIAlloc_CalcCrc
 *
 * SYNOPSIS:
 *    bool UIAlloc_CalcCrc(void);
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
bool UIAlloc_CalcCrc(void)
{
    try
    {
        g_CalcCrc=new Form_CalcCrc(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_CalcCrc=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_CalcCrc
 *
 * SYNOPSIS:
 *    bool UIShow_CalcCrc(void);
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
bool UIShow_CalcCrc(void)
{
    return g_CalcCrc->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_CalcCrc
 *
 * SYNOPSIS:
 *    void UIFree_CalcCrc(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_CalcCrc()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_CalcCrc(void)
{
    delete g_CalcCrc;

    g_CalcCrc=NULL;
}

/*******************************************************************************
 * NAME:
 *    UICCRC_GetButton
 *
 * SYNOPSIS:
 *    t_UIButtonCtrl *UICCRC_GetButton(e_CCRC_Button bttn);
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
t_UIButtonCtrl *UICCRC_GetButton(e_CCRC_Button bttn)
{
    switch(bttn)
    {
        case e_CCRC_Button_CalcCRC:
            return (t_UIButtonCtrl *)g_CalcCrc->ui->CalcCRC_pushButton;
        case e_CCRC_Button_ShowSource:
            return (t_UIButtonCtrl *)g_CalcCrc->ui->ShowSource_pushButton;
        case e_CCRC_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICCRC_GetTextInput
 *
 * SYNOPSIS:
 *    t_UITextInputCtrl *UICCRC_GetTextInput(e_CCRC_TextInput UIObj);
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
t_UITextInputCtrl *UICCRC_GetTextInput(e_CCRC_TextInput UIObj)
{
    switch(UIObj)
    {
        case e_CCRC_TextInput_CRC:
            return (t_UITextInputCtrl *)g_CalcCrc->ui->CRC_lineEdit;
        case e_CCRC_TextInputMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICCRC_GetComboBoxHandle
 *
 * SYNOPSIS:
 *    t_UIComboBoxCtrl *UICCRC_GetComboBoxHandle(e_CCRC_ComboxType UIObj);
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
t_UIComboBoxCtrl *UICCRC_GetComboBoxHandle(e_CCRC_ComboxType UIObj)
{
    switch(UIObj)
    {
        case e_CCRC_Combox_CRCType:
            return (t_UIComboBoxCtrl *)g_CalcCrc->ui->CRCType_comboBox;
        case e_CCRC_ComboxMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICCRC_GetHexContainerFrame
 *
 * SYNOPSIS:
 *    t_UIFrameContainerCtrl *UICCRC_GetHexContainerFrame(void);
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
t_UIFrameContainerCtrl *UICCRC_GetHexContainerFrame(void)
{
    return (t_UIFrameContainerCtrl *)g_CalcCrc->ui->Hex_frame;
}

