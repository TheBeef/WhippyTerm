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
 *    This program is free software: you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation, either version 3 of the License, or (at your
 *    option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program. If not, see https://www.gnu.org/licenses/.
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
