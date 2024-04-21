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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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

typedef enum
{
    e_TextCanvasFrag_String,
    e_TextCanvasFrag_NonPrintableChar,
    e_TextCanvasFrag_SoftRet,
    e_TextCanvasFrag_HardRet,
    e_TextCanvasFrag_RetText,
//    e_TextCanvasFrag_HR,
    e_TextCanvasFragMAX
} e_TextCanvasFragType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool CmpCharStyle(const struct CharStyling *A,const struct CharStyling *B);
#endif
