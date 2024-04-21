/*******************************************************************************
 * FILENAME: UI_ESB_Fill.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (02 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UI_ESB_FILL_H_
#define __UI_ESB_FILL_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_Fill(void);
bool UIShow_ESB_Fill(void);
void UIFree_ESB_Fill(void);

void ESBF_SetFillValue(uint8_t Value);
uint8_t ESBF_GetFillValue(void);

#endif
