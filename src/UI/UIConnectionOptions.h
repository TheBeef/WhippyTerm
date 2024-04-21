/*******************************************************************************
 * FILENAME: UIConnectionOptions.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI access functions for the connection options
 *    dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (10 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICONNECTIONOPTIONS_H_
#define __UICONNECTIONOPTIONS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ConnectionOptions(void);
void UIFree_ConnectionOptions(void);
bool UIShow_ConnectionOptions(void);

t_UIContainerCtrl *UI_CO_GetOptionsFrameContainer(void);

#endif
