/*******************************************************************************
 * FILENAME: UITransmitDelay.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the UI definitions for the transmit delay dialog.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (12 Feb 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __UITRANSMITDELAY_H_
#define __UITRANSMITDELAY_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_TransmitDelay(void);
void UIFree_TransmitDelay(void);
bool UIShow_TransmitDelay(void);

void UITD_SetByteDelay(unsigned int ms);
unsigned int UITD_GetByteDelay(void);
void UITD_SetLineDelay(unsigned int ms);
unsigned int UITD_GetLineDelay(void);

#endif
