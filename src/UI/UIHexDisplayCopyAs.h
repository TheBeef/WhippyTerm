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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
