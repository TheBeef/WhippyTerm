/*******************************************************************************
 * FILENAME: Form_ConnectionOptionsAccess.cpp
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
 *    Paul Hutchinson (10 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_ConnectionOptions.h"
#include "ui_Form_ConnectionOptions.h"
#include "UI/UIConnectionOptions.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_ConnectionOptions *g_ConnectionOptions;

/*******************************************************************************
 * NAME:
 *    UIAlloc_ConnectionOptions
 *
 * SYNOPSIS:
 *    bool UIAlloc_ConnectionOptions(void);
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
bool UIAlloc_ConnectionOptions(void)
{
    try
    {
        g_ConnectionOptions=new Form_ConnectionOptions(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ConnectionOptions=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_ConnectionOptions
 *
 * SYNOPSIS:
 *    bool UIShow_ConnectionOptions(void);
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
bool UIShow_ConnectionOptions(void)
{
    return g_ConnectionOptions->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_ConnectionOptions
 *
 * SYNOPSIS:
 *    void UIFree_ConnectionOptions(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_ConnectionOptions()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_ConnectionOptions(void)
{
    delete g_ConnectionOptions;

    g_ConnectionOptions=NULL;
}

/*******************************************************************************
 * NAME:
 *    UI_CO_GetOptionsFrameContainer
 *
 * SYNOPSIS:
 *    t_UIContainerCtrl *UI_CO_GetOptionsFrameContainer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the frame for the connection options (where
 *    you place the widgets for connection options).
 *
 * RETURNS:
 *    A handle to the connection options frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerCtrl *UI_CO_GetOptionsFrameContainer(void)
{
    return (t_UIContainerCtrl *)g_ConnectionOptions->ui->formLayout_ConOptions;
}
