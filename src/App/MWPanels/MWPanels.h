/*******************************************************************************
 * FILENAME: MWPanels.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has a list of the available main window panels.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (07 Mar 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __MWPANELS_H_
#define __MWPANELS_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_MWPanels_ConnectionOptions,
    e_MWPanels_StopWatch,
    e_MWPanels_Display,             // TBD
    e_MWPanels_Capture,
    e_MWPanels_Upload,              // TBD
    e_MWPanels_Downloadload,        // TBD
    e_MWPanels_Bridge,
    e_MWPanels_Hex,                 // TBD
    e_MWPanels_Injection,           // TBD
    e_MWPanels_Buffers,             // TBD
    e_MWPanelsMAX
} e_MWPanelsType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
