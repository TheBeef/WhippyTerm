/*******************************************************************************
 * FILENAME: Dialog_ESB_InsertProp.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (03 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __DIALOG_ESB_INSERTNUMBER_H_
#define __DIALOG_ESB_INSERTNUMBER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_INEndian_Big,
    e_INEndian_Little,
    e_INEndianMAX
} e_INEndianType;

typedef enum
{
    e_INBits_8,
    e_INBits_16,
    e_INBits_32,
    e_INBits_64,
    e_INBitsMAX
} e_INBitsType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool RunESB_InsertPropDialog(e_INEndianType &Endian,e_INBitsType &Bits,
        bool BitsLocked);

#endif
