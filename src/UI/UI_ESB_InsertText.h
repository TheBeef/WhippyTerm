/*******************************************************************************
 * FILENAME: UI_ESB_InsertText.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (28 May 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __UI_ESB_INSERTTEXT_H_
#define __UI_ESB_INSERTTEXT_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_ESBIT_MuliLineTextInput
{
    e_ESBIT_MuliLineTextInput_InsertText,
    e_ESBIT_MuliLineTextInputMAX
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_InsertText(void);
bool UIShow_ESB_InsertText(void);
void UIFree_ESB_InsertText(void);
t_UIMuliLineTextInputCtrl *UIESBIT_GetMuliLineTextInput(e_ESBIT_MuliLineTextInput input);

#endif
