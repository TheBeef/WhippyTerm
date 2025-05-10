/*******************************************************************************
 * FILENAME: UITextDefs.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has definitions for text that is rendered to the screen.
 *    These are used to talk to the text displays and are also stored
 *    in the main code as the attributes of text that will be displayed.
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
 *    Paul Hutchinson (14 Aug 2023)
 *       Created
 *
 *******************************************************************************/
#ifndef __UITEXTDEFS_H_
#define __UITEXTDEFS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/DataProcessors.h"   // Include for the TXT_ATTRIB_ defines
#include "App/Util/TextStyleHelpers.h"

/***  DEFINES                          ***/
#define MAX_BYTES_PER_CHAR  32

#define UITD_DRAWMASK_BOLD                  0x0001
#define UITD_DRAWMASK_UNDERLINE             0x0002
#define UITD_DRAWMASK_UNDERLINE_DOUBLE      0x0004
#define UITD_DRAWMASK_UNDERLINE_DOTTED      0x0008
#define UITD_DRAWMASK_UNDERLINE_DASHED      0x0010
#define UITD_DRAWMASK_UNDERLINE_WAVY        0x0020
#define UITD_DRAWMASK_OVERLINE              0x0040
#define UITD_DRAWMASK_LINETHROUGH           0x0080
#define UITD_DRAWMASK_ITALIC                0x0100
#define UITD_DRAWMASK_REVERSE               0x0200
#define UITD_DRAWMASK_OUTLINE               0x0400
#define UITD_DRAWMASK_BOX                   0x0800
#define UITD_DRAWMASK_ROUNDBOX              0x1000
#define UITD_DRAWMASK_COLOR_ATTRIB          0x2000

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct TextCanvasFrag
{
    e_TextCanvasFragType FragType;
    const char *Text;
    struct CharStyling Styling;
    int Value;
    void *Data;
};

enum e_TextCursorStyleType
{
    e_TextCursorStyle_Block,
    e_TextCursorStyle_UnderLine,
    e_TextCursorStyle_Line,
    e_TextCursorStyle_Box,
    e_TextCursorStyle_Dotted,
    e_TextCursorStyle_Hidden,
    e_TextCursorStyleMAX
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
