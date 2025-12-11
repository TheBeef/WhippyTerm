/*******************************************************************************
 * FILENAME: BPDS.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the .h file for the BPDS.cpp file.
 *
 * COPYRIGHT:
 *    Copyright 02 Dec 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (02 Dec 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __BPDS_H_
#define __BPDS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>
#include <stdbool.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_BPDSDataFormat_Unknown,
    e_BPDSDataFormat_Int8,
    e_BPDSDataFormat_UInt8,
    e_BPDSDataFormat_Int16,
    e_BPDSDataFormat_UInt16,
    e_BPDSDataFormat_Int32,
    e_BPDSDataFormat_UInt32,
    e_BPDSDataFormat_Int64,
    e_BPDSDataFormat_UInt64,
    e_BPDSDataFormat_Int128,
    e_BPDSDataFormat_UInt128,
    e_BPDSDataFormat_Float,
    e_BPDSDataFormat_Double,
    e_BPDSDataFormatMAX
} e_BPDSDataFormatType;

struct BPDSStringValueSet
{
    char *Str;
    unsigned int StrLen;
    void *UserData;
    struct BPDSStringValueSet *Next;
};

struct BPDSNumberValueSet
{
    uint64_t Value;
    void *UserData;
    struct BPDSNumberValueSet *Next;
};

struct BPDSField
{
    char Name[256];
    struct BPDSNumberValueSet *NumValues;
    struct BPDSStringValueSet *StrValues;
    int64_t Size;
    e_BPDSDataFormatType Format;        // Data Type
    bool MatchAny;
    struct BPDSField *SizeLinkedTo;    // If the size is a different symbol then this points to it

    void *UserData;

    struct BPDSField *Next;
};

struct BPDSDef
{
    char *DPDSDefStr;
    const char *ErrorStr;
    unsigned int ErrorOffset;
    bool HadError;
    struct BPDSField *FieldList;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
struct BPDSDef *BPDS_Parse(const char *BPDSStr);
void BPDS_FreeBPDSDef(struct BPDSDef *Def);

#endif   /* end of "#ifndef __BPDS_H_" */
