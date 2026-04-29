/*******************************************************************************
 * FILENAME: Sockets.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the OS socket interface in it.  This is a generic interface
 *    to the sockets that used by the main code (plugins may have their own
 *    version of the sockets).
 *
 *    Plugins should not use this.
 *
 * COPYRIGHT:
 *    Copyright 23 Apr 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (23 Apr 2026)
 *       Created
 *
 *******************************************************************************/
#ifndef __SOCKETS_H_
#define __SOCKETS_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
struct OSSocket *OpenSocket(const char *Dest,unsigned int Port,volatile bool *AbortFlag);
bool SendSocket(struct OSSocket *Sock,const void *Buffer,unsigned int Bytes);
int ReadSocket(struct OSSocket *Sock,void *Buffer,unsigned int MaxBytes);
unsigned int AvailSocket(struct OSSocket *Sock);
void CloseSocket(struct OSSocket *Sock);

#endif   /* end of "#ifndef __SOCKETS_H_" */
