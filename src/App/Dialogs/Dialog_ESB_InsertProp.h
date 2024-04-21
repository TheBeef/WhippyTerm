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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
