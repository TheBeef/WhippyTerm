/*******************************************************************************
 * FILENAME: UIChangeConnectionName.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI access functions for the change connection name
 *    dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (19 Sep 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICHANGECONNECTIONNAME_H_
#define __UICHANGECONNECTIONNAME_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_UICCN_TxtInput
{
    e_UICCN_TxtInput_Name,
    e_UICCN_TxtInputMAX
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ChangeConnectionName(void);
void UIShow_ChangeConnectionName(void);
void UIFree_ChangeConnectionName(void);
t_UITextInputCtrl *UICCN_GetTxtInputHandle(e_UICCN_TxtInput TInput);

#endif
