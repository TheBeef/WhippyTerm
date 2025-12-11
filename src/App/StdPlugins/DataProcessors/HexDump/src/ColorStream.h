/*******************************************************************************
 * FILENAME: ColorStream.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 03 Dec 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (03 Dec 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __COLORSTREAM_H_
#define __COLORSTREAM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
struct ColorStreamData *AllocColorStream(const char *BPDSDefStr);
void FreeColorStream(struct ColorStreamData *CSD);
bool ColorStreamProcessIncomingByte(struct ColorStreamData *CSD,uint8_t Byte,bool DidStyling);
void SetColorStreamEndian(struct ColorStreamData *CSD,bool Big);
void ColorStreamProcessIncomingByteFinish(struct ColorStreamData *CSD,uint8_t Byte,bool DidStyling);
bool SetColorStreamFieldStyling(struct ColorStreamData *CSD,
        unsigned int FieldNum,struct StyleData *SD);

#endif   /* end of "#ifndef __COLORSTREAM_H_" */
