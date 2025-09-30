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
        return SelColor.rgba()&0xFFFFFF;
    return 0xFFFFFFFF;
}
