/*******************************************************************************
 * FILENAME: ClipboardHelpers.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the API to the clipboard helpers in it.  Clipboard helpers
 *    are functions that take data from the clipboard and process it, or
 *    format data for sending in to clipboard.
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
 * HISTORY:
 *    Paul Hutchinson (15 Oct 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __CLIPBOARDHELPERS_H_
#define __CLIPBOARDHELPERS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIPasteData.h"
#include <stdint.h>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
uint8_t *ConvertClipboardData2Format(std::string &ClipboardData,
        e_PasteDataType DataFormat,unsigned int *Bytes);

#endif
