/*******************************************************************************
 * FILENAME: UISound.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the API for making sound
 *
 * COPYRIGHT:
 *    Copyright 08 Feb 2025 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (08 Feb 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __UISOUND_H_
#define __UISOUND_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

void UIPlayWav(const char *Filename);
void UIPlayBuiltInBeep(void);
void UI_Beep(void);

#endif   /* end of "#ifndef __UISOUND_H_" */
