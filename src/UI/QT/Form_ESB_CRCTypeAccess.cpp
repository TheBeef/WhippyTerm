/*******************************************************************************
 * FILENAME: Form_ESB_CRCTypeAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Access functions for the Form_ESB_CRCType form.
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
 *    Paul Hutchinson (30 Aug 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_ESB_CRCType.h"
#include "ui_Form_ESB_CRCType.h"
#include "UI/UI_ESB_CRCType.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

class Form_ESB_CRCType *g_ESB_CRCType;

/*******************************************************************************
 * NAME:
 *    UIAlloc_ESB_CRCType
 *
 * SYNOPSIS:
 *    bool UIAlloc_ESB_CRCType(void);
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
bool UIAlloc_ESB_CRCType(void)
{
    try
    {
        g_ESB_CRCType=new Form_ESB_CRCType(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ESB_CRCType=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_ESB_CRCType
 *
 * SYNOPSIS:
 *    bool UIShow_ESB_CRCType(void);
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
bool UIShow_ESB_CRCType(void)
{
    return g_ESB_CRCType->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_ESB_CRCType
 *
 * SYNOPSIS:
 *    void UIFree_ESB_CRCType(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_ESB_CRCType()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_ESB_CRCType(void)
{
    delete g_ESB_CRCType;

    g_ESB_CRCType=NULL;
}

/*******************************************************************************
 * NAME:
 *    UIESBCT_GetButton
 *
 * SYNOPSIS:
 *    t_UIButtonCtrl *UIESBCT_GetButton(e_ESBCT_Button bttn);
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
t_UIButtonCtrl *UIESBCT_GetButton(e_ESBCT_Button bttn)
{
    switch(bttn)
    {
        case e_ESBCT_Button_ShowSource:
            return (t_UIButtonCtrl *)g_ESB_CRCType->ui->ShowSource_pushButton;
        case e_ESBCT_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UIESBCR_GetListView
 *
 * SYNOPSIS:
 *    t_UIListViewCtrl *UIESBCR_GetListView(e_ESBCR_ListView lv);
 *
 * PARAMETERS:
 *    lv [I] -- The list view to get the handle for
 *
 * FUNCTION:
 *    This function gets a handle to a listview in the dialog.
 *
 * RETURNS:
 *    A handle to the listview or NULL.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIListViewCtrl *UIESBCR_GetListView(e_ESBCR_ListView lv)
{
    switch(lv)
    {
        case e_ESBCR_ListView_CRCList:
            return (t_UIListViewCtrl *)g_ESB_CRCType->ui->CRCList_listWidget;
        case e_ESBCR_ListViewMAX:
        default:
            return NULL;
    }
    return NULL;
}

