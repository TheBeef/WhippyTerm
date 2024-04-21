/*******************************************************************************
 * FILENAME: UI_ESB_SetBufferName.h
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
 *    Paul Hutchinson (08 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UI_ESB_SETBUFFERNAME_H_
#define __UI_ESB_SETBUFFERNAME_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_SetBufferName(void);
bool UIShow_ESB_SetBufferName(void);
void UIFree_ESB_SetBufferName(void);

void ESBSBN_SetBufferName(std::string &Name);
void ESBSBN_GetBufferName(std::string &Name);

#endif
