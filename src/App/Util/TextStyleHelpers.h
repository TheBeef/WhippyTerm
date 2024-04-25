/*******************************************************************************
 * FILENAME: TextStyleHelpers.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the def's for char styling and fragment types as well as
 *    util functions for the styling in it.
 *
 * COPYRIGHT:
 *    Copyright 2023 Paul Hutchinson.
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
 *    Paul Hutchinson (26 Aug 2023)
 *       Created
 *
 *******************************************************************************/
#ifndef __TEXTSTYLEHELPERS_H_
#define __TEXTSTYLEHELPERS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct CharStyling
{
    uint32_t FGColor;
    uint32_t BGColor;
    uint32_t ULineColor;
    uint16_t Attribs;
};

/*
  e_TextCanvasFragType types:
    * e_TextCanvasFrag_String -- This is a normal UTF-8 C string.  You can
            find the string in 'TextLineFrag.Text'
    * e_TextCanvasFrag_NonPrintableChar -- This is a control char.  You can
            find the name of the control char in 'TextLineFrag.Text'
    * e_TextCanvasFrag_SoftRet -- This is a 'soft' end of line.  A soft end of
            line is one the program added on it's own (wrap because it hit the
            edge of the screen, word wrap, etc).
    * e_TextCanvasFrag_HardRet -- This is a 'hard' end of line.  A hard end of
            line is where a control char made the line end (\n for example).
    * e_TextCanvasFrag_RetText -- This is an end of line that we drawing.  The
            text to draw as the end of line is in 'TextLineFrag.Text'.
            NOTE: This isn't used so it may no longer work (turns out figuring
            out if a \r or \r\n was used or if it's a line end that was made
            by moving the cursor was VERY hard to get right).
    * e_TextCanvasFrag_HR -- Horizontal rule that uses up the whole line.
*/
typedef enum
{
    e_TextCanvasFrag_String,
    e_TextCanvasFrag_NonPrintableChar,
    e_TextCanvasFrag_SoftRet,
    e_TextCanvasFrag_HardRet,
    e_TextCanvasFrag_RetText,
    e_TextCanvasFrag_HR,
    e_TextCanvasFragMAX
} e_TextCanvasFragType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool CmpCharStyle(const struct CharStyling *A,const struct CharStyling *B);
#endif
