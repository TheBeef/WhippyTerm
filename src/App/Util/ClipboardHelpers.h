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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
