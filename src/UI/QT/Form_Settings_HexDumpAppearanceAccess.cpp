/*******************************************************************************
 * FILENAME: Form_Settings_HexDumpAppearanceAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (04 Jul 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_Settings_HexDumpAppearance.h"
#include "ui_Form_Settings_HexDumpAppearance.h"
#include "UI/UISettingsHexDumpAppearance.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_Settings_HexDumpAppearance *g_Settings_HexDumpAppearance;

/*******************************************************************************
 * NAME:
 *    UIAlloc_Settings_HexDumpAppearance
 *
 * SYNOPSIS:
 *    bool UIAlloc_Settings_HexDumpAppearance(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the hex dump appearance settings dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    UIFree_Settings_HexDumpAppearance(),
 *    UIShow_Settings_HexDumpAppearance()
 ******************************************************************************/
bool UIAlloc_Settings_HexDumpAppearance(void)
{
    try
    {
        g_Settings_HexDumpAppearance=new Form_Settings_HexDumpAppearance(
                g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_Settings_HexDumpAppearance=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_Settings_HexDumpAppearance
 *
 * SYNOPSIS:
 *    bool UIShow_Settings_HexDumpAppearance(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the hex dump appearance settings dialog (modal).
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    UIAlloc_Settings_HexDumpAppearance()
 ******************************************************************************/
bool UIShow_Settings_HexDumpAppearance(void)
{
    return g_Settings_HexDumpAppearance->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_Settings_HexDumpAppearance
 *
 * SYNOPSIS:
 *    void UIFree_Settings_HexDumpAppearance(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with
 *    UIAlloc_Settings_HexDumpAppearance().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UIAlloc_Settings_HexDumpAppearance()
 ******************************************************************************/
void UIFree_Settings_HexDumpAppearance(void)
{
    delete g_Settings_HexDumpAppearance;

    g_Settings_HexDumpAppearance=NULL;
}

t_UIButtonCtrl *UISHDA_GetButton(e_UISHDA_Button Bttn)
{
    switch(Bttn)
    {
        case e_UISHDA_Button_SelectDivLineColor:
            return (t_UIButtonCtrl *)g_Settings_HexDumpAppearance->
                    ui->HexDisplay_SelectDivLineColor_pushButton;
        case e_UISHDA_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

t_UINumberInput *UISHDA_GetNumberInput(e_UISHDA_NumberInput Input)
{
    switch(Input)
    {
        case e_UISHDA_NumberInput_DivLineWidth:
            return (t_UINumberInput *)g_Settings_HexDumpAppearance->
                    ui->HexDisplay_DivLineWidth_spinBox;
        case e_UISHDA_NumberInput_BytesPerLine:
            return (t_UINumberInput *)g_Settings_HexDumpAppearance->
                    ui->HexDisplay_BytesPerLine_spinBox;
        case e_UISHDA_NumberInput_DividerEvery:
            return (t_UINumberInput *)g_Settings_HexDumpAppearance->
                    ui->HexDisplay_DividerEvery_spinBox;
        case e_UISHDA_NumberInputMAX:
        default:
            return NULL;
    }
    return NULL;
}

t_UIColorPreviewCtrl *UISHDA_GetColorPreview(e_UISHDA_ColorPreview Preview)
{
    switch(Preview)
    {
        case e_UISHDA_ColorPreview_DivLineColor:
            return (t_UIColorPreviewCtrl *)g_Settings_HexDumpAppearance->
                    ui->HexDisplay_DivLineColor_frame;
        case e_UISHDA_ColorPreviewMAX:
        default:
            return NULL;
    }
    return NULL;
}
