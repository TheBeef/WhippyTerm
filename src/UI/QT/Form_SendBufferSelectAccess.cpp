/*******************************************************************************
 * FILENAME: Form_SendBufferSelectAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 07 Sep 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (07 Sep 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_SendBufferSelect.h"
#include "ui_Form_SendBufferSelect.h"
#include "UI/UISendBufferSelect.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_SendBufferSelect *g_SendBufferSelect;

/*******************************************************************************
 * NAME:
 *    UIAlloc_SendBufferSelect
 *
 * SYNOPSIS:
 *    bool UIAlloc_SendBufferSelect(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the select send buffer as dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_SendBufferSelect(void)
{
    try
    {
        g_SendBufferSelect=new Form_SendBufferSelect(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_SendBufferSelect=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_SendBufferSelect
 *
 * SYNOPSIS:
 *    bool UIShow_SendBufferSelect(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the select send buffer as dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_SendBufferSelect(void)
{
    return g_SendBufferSelect->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_SendBufferSelect
 *
 * SYNOPSIS:
 *    void UIFree_SendBufferSelect(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_SendBufferSelect()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_SendBufferSelect(void)
{
    delete g_SendBufferSelect;

    g_SendBufferSelect=NULL;
}

/*******************************************************************************
 * NAME:
 *    UISBS_GetButton
 *
 * SYNOPSIS:
 *    t_UIButtonCtrl *UISBS_GetButton(e_SBS_Button bttn);
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
t_UIButtonCtrl *UISBS_GetButton(e_SBS_Button bttn)
{
    switch(bttn)
    {
        case e_SBS_Button_Cancel:
            return (t_UIButtonCtrl *)g_SendBufferSelect->ui->pushButton_Cancel;
        case e_SBS_Button_GoButton:
            return (t_UIButtonCtrl *)g_SendBufferSelect->ui->pushButton_Go;
        case e_SBS_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

t_UIColumnView *UISBS_GetColumnViewHandle(e_SBSColumnViewType UIObj)
{
    switch(UIObj)
    {
        case e_UIMWColumnView_Buffers_List:
            return (t_UIColumnView *)g_SendBufferSelect->ui->treeWidget_BufferList;

        case e_UIMWColumnViewMAX:
        default:
        break;
    }
    return NULL;
}

void UISBS_SetDialogTitle(const char *Title)
{
    g_SendBufferSelect->setWindowTitle(Title);
}

void UISBS_CloseDialog(void)
{
    g_SendBufferSelect->close();
}
