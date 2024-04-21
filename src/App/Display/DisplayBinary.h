/*******************************************************************************
 * FILENAME: DisplayBinary.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the binary version of the display base.
 *
 * COPYRIGHT:
 *    Copyright 2023 Paul Hutchinson.
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
 *    Paul Hutchinson (06 Aug 2023)
 *       Created
 *
 *******************************************************************************/
#ifndef __DISPLAYBINARY_H_
#define __DISPLAYBINARY_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Display/DisplayBase.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class DisplayBinary : public DisplayBase
{
    public:
        DisplayBinary();
        ~DisplayBinary();

        bool Init(void *ParentWidget,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData);
        void Reparent(void *NewParentWidget);
        void WriteChar(uint8_t *Chr);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
