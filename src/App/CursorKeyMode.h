/*******************************************************************************
 * FILENAME: CursorKeyMode.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file.
 *
 * COPYRIGHT:
 *    Copyright 13 Oct 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (13 Oct 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __CURSORKEYMODE_H_
#define __CURSORKEYMODE_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void CursorKeyMode_ApplySettings(void);
void SetCursorKeyModeLocal(bool Local);
bool GetCurrentCursorKeyModeIsLocal(void);

#endif   /* end of "#ifndef __CURSORKEYMODE_H_" */
