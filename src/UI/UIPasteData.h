/*******************************************************************************
 * FILENAME: UIPasteData.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the API to the paste data dialog in it.
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
#ifndef __UIPASTEDATA_H_
#define __UIPASTEDATA_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_PasteData_Text,
    e_PasteData_HexDump,
    e_PasteData_Cancel,
    e_PasteDataMAX
} e_PasteDataType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_PasteData(void);
e_PasteDataType UIShow_PasteData(void);
void UIFree_PasteData(void);

#endif
