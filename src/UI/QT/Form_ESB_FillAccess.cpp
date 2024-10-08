/*******************************************************************************
 * FILENAME: Form_ESB_FillAccess.cpp
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
 *    Paul Hutchinson (02 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_ESB_Fill.h"
#include "ui_Form_ESB_Fill.h"
#include "UI/UI_ESB_Fill.h"
#include "main.h"
#include <stdio.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

class Form_ESB_Fill *g_ESB_Fill;

/*******************************************************************************
 * NAME:
 *    UIAlloc_ESB_Fill
 *
 * SYNOPSIS:
 *    bool UIAlloc_ESB_Fill(void);
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
bool UIAlloc_ESB_Fill(void)
{
    try
    {
        g_ESB_Fill=new Form_ESB_Fill(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ESB_Fill=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_ESB_Fill
 *
 * SYNOPSIS:
 *    bool UIShow_ESB_Fill(void);
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
bool UIShow_ESB_Fill(void)
{
    return g_ESB_Fill->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_ESB_Fill
 *
 * SYNOPSIS:
 *    void UIFree_ESB_Fill(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_ESB_Fill()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_ESB_Fill(void)
{
    delete g_ESB_Fill;

    g_ESB_Fill=NULL;
}

void ESBF_SetFillValue(uint8_t Value)
{
    char buff[100];
    sprintf(buff,"0x%0X",Value);

    g_ESB_Fill->ui->FillValue_lineEdit->setText(buff);
}

uint8_t ESBF_GetFillValue(void)
{
    const char *Str;
    long Value;

    Str=qPrintable(g_ESB_Fill->ui->FillValue_lineEdit->text());

    Value=strtol(Str,NULL,0);
    if(Value>255)
        Value=255;
    if(Value<0)
        Value=0;

    return Value;
}

