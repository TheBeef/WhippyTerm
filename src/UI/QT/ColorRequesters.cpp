/*******************************************************************************
 * FILENAME: ColorRequesters.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (24 Jun 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIFileReq.h"
#include "Form_MainWindow.h"
#include <QColorDialog>
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UIGetColor
 *
 * SYNOPSIS:
 *    uint32_t UIGetColor(uint32_t StartingColor);
 *
 * PARAMETERS:
 *    StartingColor [I] -- The color to select when the window opens.
 *
 * FUNCTION:
 *    This function opens a color requester and lets the user select a color.
 *
 * RETURNS:
 *    A color in RRGGBB format or 0xFFFFFFFF if the user selects cancel.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
uint32_t UIGetColor(uint32_t StartingColor)
{
    QColor SelColor;

    SelColor=QColorDialog::getColor(QColor(StartingColor),
            g_MainApp->activeWindow());
    if(SelColor.isValid())
        return SelColor.rgba();
    return 0xFFFFFFFF;
}
