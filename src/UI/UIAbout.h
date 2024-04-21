/*******************************************************************************
 * FILENAME: UIAbout.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the about dialog access functions in it.
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIABOUT_H_
#define __UIABOUT_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_UIAboutTab_About,
    e_UIAboutTab_Author,
    e_UIAboutTab_License,
    e_UIAboutTab_Libraries,
    e_UIAboutTabMAX
} e_UIAboutTabType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_About(void);
void UIShow_About(void);
void UIFree_About(void);
void UISetAboutName(const char *Name);
void UISetAboutText(const char *Text,e_UIAboutTabType Tab);
const char *GetGUIEngineName(void);
const char *GetGUIEngineVersion(void);

#endif   /* end of "#ifndef __UIABOUT_H_" */
