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
    e_MWPanels_Capture,
    e_MWPanels_Upload,
    e_MWPanels_Downloadload,
    e_MWPanels_Bridge,
    e_MWPanels_Hex,
    e_MWPanels_Injection,           // TBD
    e_MWPanels_Buffers,
    e_MWPanels_AuxControls,
    e_MWPanelsMAX
} e_MWPanelsType;

/*
== Panel design thoughts ==
This is the basic rules WhippyTerm tries to follow (these are guide lines
not "can never break these rules").

Left:
    This are things that effect the connection it's self.  This includes
    things that send in/out of the byte stream.

Right:
    The right panel is for tools / misc stuff.

Bottom:
    The bottom panel is for messing / monitoring the byte stream.

*/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
