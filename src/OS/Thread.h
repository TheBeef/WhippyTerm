/*******************************************************************************
 * FILENAME: Thread.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the threads interface in it.
 *
 * COPYRIGHT:
 *    Copyright 06 Apr 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (06 Apr 2026)
 *       Created
 *
 *******************************************************************************/
#ifndef __THREAD_H_
#define __THREAD_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct ThreadMutex;
struct ThreadHandle;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
struct ThreadHandle *StartThread(void (*ThreadFn)(void *),void *Arg);
void Wait4ThreadToExit(struct ThreadHandle *Thread);
struct ThreadMutex *AllocMutex(void);
void FreeMutex(struct ThreadMutex *);
void LockMutex(struct ThreadMutex *Mutex);
void UnLockMutex(struct ThreadMutex *Mutex);

#endif   /* end of "#ifndef __THREAD_H_" */
