/*******************************************************************************
 * FILENAME: Form_TransmitDelayAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (12 Feb 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_TransmitDelay.h"
#include "ui_Form_TransmitDelay.h"
#include "UI/UITransmitDelay.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

class Form_TransmitDelay *g_TransmitDelay;

/*******************************************************************************
 * NAME:
 *    UIAlloc_TransmitDelay
 *
 * SYNOPSIS:
 *    bool UIAlloc_TransmitDelay(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the transmit delay dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_TransmitDelay(void)
{
    try
    {
        g_TransmitDelay=new Form_TransmitDelay(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_TransmitDelay=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_TransmitDelay
 *
 * SYNOPSIS:
 *    bool UIShow_TransmitDelay(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the transmit delay dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_TransmitDelay(void)
{
    return g_TransmitDelay->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_TransmitDelay
 *
 * SYNOPSIS:
 *    void UIFree_TransmitDelay(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_TransmitDelay()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_TransmitDelay(void)
{
    delete g_TransmitDelay;

    g_TransmitDelay=NULL;
}

/*******************************************************************************
 * NAME:
 *    UITD_SetByteDelay
 *
 * SYNOPSIS:
 *    void UITD_SetByteDelay(unsigned int ms);
 *
 * PARAMETERS:
 *    ms [I] -- The number of ms between bytes going out
 *
 * FUNCTION:
 *    This function sets the number of ms between bytes going to fill into
 *    the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UITD_GetByteDelay()
 ******************************************************************************/
void UITD_SetByteDelay(unsigned int ms)
{
    g_TransmitDelay->ui->ByteDelay_spinBox->setValue(ms);
}

/*******************************************************************************
 * NAME:
 *    UITD_GetByteDelay
 *
 * SYNOPSIS:
 *    unsigned int UITD_GetByteDelay(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the value that the UI has been set to for the
 *    byte delay.
 *
 * RETURNS:
 *    The number of ms between bytes that the UI has been set to.
 *
 * SEE ALSO:
 *    UITD_SetByteDelay()
 ******************************************************************************/
unsigned int UITD_GetByteDelay(void)
{
    return g_TransmitDelay->ui->ByteDelay_spinBox->value();
}

/*******************************************************************************
 * NAME:
 *    UITD_SetLineDelay
 *
 * SYNOPSIS:
 *    void UITD_SetLineDelay(unsigned int ms);
 *
 * PARAMETERS:
 *    ms [I] -- The number of ms between lines going out
 *
 * FUNCTION:
 *    This function sets the number of ms between lines going to fill into
 *    the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UITD_GetLineDelay()
 ******************************************************************************/
void UITD_SetLineDelay(unsigned int ms)
{
    g_TransmitDelay->ui->LineDelay_spinBox->setValue(ms);
}

/*******************************************************************************
 * NAME:
 *    UITD_GetLineDelay
 *
 * SYNOPSIS:
 *    unsigned int UITD_GetLineDelay(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the value that the UI has been set to for the
 *    line delay.
 *
 * RETURNS:
 *    The number of ms between lines that the UI has been set to.
 *
 * SEE ALSO:
 *    TD_SetByteDelay()
 ******************************************************************************/
unsigned int UITD_GetLineDelay(void)
{
    return g_TransmitDelay->ui->LineDelay_spinBox->value();
}


