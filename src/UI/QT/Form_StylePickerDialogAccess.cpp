/*******************************************************************************
 * FILENAME: Form_StylePickerDialogAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 05 Dec 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (05 Dec 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_StylePickerDialog.h"
#include "ui_Form_StylePickerDialog.h"
#include "UI/UIStylePicker.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

class Form_StylePickerDialog *g_StylePicker;

/*******************************************************************************
 * NAME:
 *    UIAlloc_StylePicker
 *
 * SYNOPSIS:
 *    bool UIAlloc_StylePicker(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the style picker dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_StylePicker(void)
{
    try
    {
        g_StylePicker=new Form_StylePickerDialog(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_StylePicker=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_StylePicker
 *
 * SYNOPSIS:
 *    bool UIShow_StylePicker(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the style picker dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_StylePicker(void)
{
    return g_StylePicker->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_StylePicker
 *
 * SYNOPSIS:
 *    void UIFree_StylePicker(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_StylePicker()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_StylePicker(void)
{
    delete g_StylePicker;

    g_StylePicker=NULL;
}

/*******************************************************************************
 * NAME:
 *    UISP_SetStyleData
 *
 * SYNOPSIS:
 *    void UISP_SetStyleData(struct StyleData *SD);
 *
 * PARAMETERS:
 *    SD [I] -- The style to apply
 *
 * FUNCTION:
 *    This function sets the style data to fill into the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UISP_GetStyleData()
 ******************************************************************************/
void UISP_SetStyleData(struct StyleData *SD)
{
    g_StylePicker->CurStyle=*SD;
    g_StylePicker->UpdateDisplay();
}

/*******************************************************************************
 * NAME:
 *    UISP_GetStyleData
 *
 * SYNOPSIS:
 *    void UISP_GetStyleData(struct StyleData *SD);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the style that the UI has been set to.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UISP_SetStyleData()
 ******************************************************************************/
void UISP_GetStyleData(struct StyleData *SD)
{
    g_StylePicker->CollectStylesFromGUI();
    *SD=g_StylePicker->CurStyle;
}
