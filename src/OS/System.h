/*******************************************************************************
 * FILENAME: System.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has OS system related functions in it.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (21 Feb 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct DLLHandle;

typedef enum
{
    e_OS_Linux,
    e_OS_Windows,
    e_OS_MacOSX,
    e_OS_RaspberryPI,
    e_OSMAX
} e_OSType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void InitOS(void);
struct DLLHandle *LoadDLL(const char *Filename);
void *DLL2Function(struct DLLHandle *Handle,const char *FnName);
void CloseDLL(struct DLLHandle *Handle);
const char *LastDLLError(void);
e_OSType RunningOS(void);
int RunningExeBits(void);
#endif
