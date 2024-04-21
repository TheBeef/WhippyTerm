/*******************************************************************************
 * FILENAME: Dialog_ESB_CRCType.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file for the Dialog_ESB_CRCType.cpp file.
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
 * HISTORY:
 *    Paul Hutchinson (30 Aug 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __DIALOG_ESB_CRCTYPE_H_
#define __DIALOG_ESB_CRCTYPE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Dialogs/Dialog_EditSendBuffer.h"
#include "App/Util/CRCSystem.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool RunESB_CRCTypeDialog(e_CRCType &CRCType);

#endif
