/*******************************************************************************
 * FILENAME: UIHexDisplayCopyAs.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI access for the hex display copy as form in it.
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
 *    Paul Hutchinson (14 Jun 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIHEXDISPLAYCOPYAS_H_
#define __UIHEXDISPLAYCOPYAS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Display/HexDisplayBuffers.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_HexDisplayCopyAs(void);
bool UIShow_HexDisplayCopyAs(void);
void UIFree_HexDisplayCopyAs(void);
void SetFormat_HexDisplayCopyAs(e_HDBCFormatType SelectedFormat);
e_HDBCFormatType GetFormat_HexDisplayCopyAs(void);

#endif
