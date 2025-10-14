/*******************************************************************************
 * FILENAME: UIKeyboard.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access to the keyboard.  This is only for things
 *    that fall outside of the normal keyboard handling (like scroll lock).
 *
 * COPYRIGHT:
 *    Copyright 12 Oct 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (12 Oct 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIKEYBOARD_H_
#define __UIKEYBOARD_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void EnableHandlingOfScrollLock(bool Enabled);
bool IsScrollLockOn(void);

#endif   /* end of "#ifndef __UIKEYBOARD_H_" */
