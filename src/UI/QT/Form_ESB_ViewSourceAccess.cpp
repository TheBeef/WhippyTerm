/*******************************************************************************
 * FILENAME: Form_ESB_ViewSourceAccess.cpp
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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (31 Aug 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_ESB_ViewSource.h"
#include "ui_Form_ESB_ViewSource.h"
#include "UI/UI_ESB_ViewSource.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

class Form_ESB_ViewSource *g_ESB_ViewSource;

/*******************************************************************************
 * NAME:
 *    UIAlloc_ESB_ViewSource
 *
 * SYNOPSIS:
 *    bool UIAlloc_ESB_ViewSource(void);
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
bool UIAlloc_ESB_ViewSource(void)
{
    try
    {
        g_ESB_ViewSource=new Form_ESB_ViewSource(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ESB_ViewSource=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_ESB_ViewSource
 *
 * SYNOPSIS:
 *    bool UIShow_ESB_ViewSource(void);
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
bool UIShow_ESB_ViewSource(void)
{
    return g_ESB_ViewSource->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_ESB_ViewSource
 *
 * SYNOPSIS:
 *    void UIFree_ESB_ViewSource(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_ESB_ViewSource()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_ESB_ViewSource(void)
{
    delete g_ESB_ViewSource;

    g_ESB_ViewSource=NULL;
}

void ESBVS_SetSourceText(const char *Source)
{
    g_ESB_ViewSource->ui->Source_plainTextEdit->setPlainText(Source);
}

