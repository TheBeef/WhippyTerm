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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
