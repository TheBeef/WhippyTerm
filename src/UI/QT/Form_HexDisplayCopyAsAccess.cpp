/*******************************************************************************
 * FILENAME: Form_HexDisplayCopyAsAccess.cpp
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
 *    Paul Hutchinson (15 Jun 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_HexDisplayCopyAs.h"
#include "ui_Form_HexDisplayCopyAs.h"
#include "UI/UIHexDisplayCopyAs.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_HexDisplayCopyAs *g_HexDisplayCopyAs;

/*******************************************************************************
 * NAME:
 *    UIAlloc_HexDisplayCopyAs
 *
 * SYNOPSIS:
 *    bool UIAlloc_HexDisplayCopyAs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the hex display copy as dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_HexDisplayCopyAs(void)
{
    try
    {
        g_HexDisplayCopyAs=new Form_HexDisplayCopyAs(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_HexDisplayCopyAs=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_HexDisplayCopyAs
 *
 * SYNOPSIS:
 *    bool UIShow_HexDisplayCopyAs(void);
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
bool UIShow_HexDisplayCopyAs(void)
{
    return g_HexDisplayCopyAs->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_HexDisplayCopyAs
 *
 * SYNOPSIS:
 *    void UIFree_HexDisplayCopyAs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_HexDisplayCopyAs()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_HexDisplayCopyAs(void)
{
    delete g_HexDisplayCopyAs;

    g_HexDisplayCopyAs=NULL;
}

/*******************************************************************************
 * NAME:
 *    SetFormat_HexDisplayCopyAs
 *
 * SYNOPSIS:
 *    void SetFormat_HexDisplayCopyAs(e_HDBCFormatType SelectedFormat);
 *
 * PARAMETERS:
 *    SelectedFormat [I] -- What format to select.  Not all are value.
 *                          Supported values: e_HDBCFormat_HexDump,
 *                          e_HDBCFormat_Hex, e_HDBCFormat_AscII,
 *                          e_HDBCFormat_RAW
 *
 * FUNCTION:
 *    This function sets what format is selected in the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void SetFormat_HexDisplayCopyAs(e_HDBCFormatType SelectedFormat)
{
    if(g_HexDisplayCopyAs==NULL)
        return;

    switch(SelectedFormat)
    {
        case e_HDBCFormat_HexDump:
            g_HexDisplayCopyAs->ui->radioButton_HexDump->setChecked(true);
        break;
        case e_HDBCFormat_Hex:
            g_HexDisplayCopyAs->ui->radioButton_Hex->setChecked(true);
        break;
        case e_HDBCFormat_AscII:
            g_HexDisplayCopyAs->ui->radioButton_AscII->setChecked(true);
        break;
        case e_HDBCFormat_RAW:
            g_HexDisplayCopyAs->ui->radioButton_RAW->setChecked(true);
        break;
        case e_HDBCFormat_Default:
        case e_HDBCFormatMAX:
        default:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    GetFormat_HexDisplayCopyAs
 *
 * SYNOPSIS:
 *    e_HDBCFormatType GetFormat_HexDisplayCopyAs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets what the user has selected in the UI.
 *
 * RETURNS:
 *    The selected format.  Only some formats are supported
 *    (see SetFormat_HexDisplayCopyAs()).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_HDBCFormatType GetFormat_HexDisplayCopyAs(void)
{
    e_HDBCFormatType RetValue;

    if(g_HexDisplayCopyAs==NULL)
        return e_HDBCFormat_HexDump;

    RetValue=e_HDBCFormat_HexDump;

    if(g_HexDisplayCopyAs->ui->radioButton_HexDump->isChecked())
        RetValue=e_HDBCFormat_HexDump;

    if(g_HexDisplayCopyAs->ui->radioButton_Hex->isChecked())
        RetValue=e_HDBCFormat_Hex;

    if(g_HexDisplayCopyAs->ui->radioButton_AscII->isChecked())
        RetValue=e_HDBCFormat_AscII;

    if(g_HexDisplayCopyAs->ui->radioButton_RAW->isChecked())
        RetValue=e_HDBCFormat_RAW;

    return RetValue;
}

