/*******************************************************************************
 * FILENAME: UI_ESB_InsertProp.h
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
 *    Paul Hutchinson (03 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UI_ESB_INSERTNUMBER_H_
#define __UI_ESB_INSERTNUMBER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/
enum e_ESBIP_RadioButton
{
    e_ESBIP_RadioButton_Endian_Big,
    e_ESBIP_RadioButton_Endian_Little,
    e_ESBIP_RadioButton_Bits_8,
    e_ESBIP_RadioButton_Bits_16,
    e_ESBIP_RadioButton_Bits_32,
    e_ESBIP_RadioButton_Bits_64,
    e_ESBIP_RadioButtonMAX
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_InsertProp(void);
bool UIShow_ESB_InsertProp(void);
void UIFree_ESB_InsertProp(void);
t_UIRadioBttnCtrl *UIESBIP_GetRadioButton(e_ESBIP_RadioButton bttn);

#endif
