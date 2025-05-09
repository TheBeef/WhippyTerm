/*******************************************************************************
 * FILENAME: Dialog_NewConnectionFromURI.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file does .h file stuff.
 *
 * COPYRIGHT:
 *    Copyright 2025 Paul Hutchinson.
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
 * HISTORY:
 *    Paul Hutchinson (10 Feb 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __DIALOG_NEWCONNECTIONFROMURI_H_
#define __DIALOG_NEWCONNECTIONFROMURI_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <string>
#include "App/MainWindow.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool RunNewConnectionFromURIDialog(class TheMainWindow *MW,std::string *URI);

#endif   /* end of "#ifndef __DIALOG_NEWCONNECTIONFROMURI_H_" */
