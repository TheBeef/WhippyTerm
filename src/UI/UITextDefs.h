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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
