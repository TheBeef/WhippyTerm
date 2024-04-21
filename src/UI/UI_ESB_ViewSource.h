/*******************************************************************************
 * FILENAME: UI_ESB_ViewSource.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the API to the view CRC source dialog in the edit buffer dialog.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (31 Aug 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UI_ESB_VIEWSOURCE_H_
#define __UI_ESB_VIEWSOURCE_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ESB_ViewSource(void);
bool UIShow_ESB_ViewSource(void);
void UIFree_ESB_ViewSource(void);
void ESBVS_SetSourceText(const char *Source);

#endif
