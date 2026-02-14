/*

Need a call from the script thread when the script is done so the GUI can
update it's self (diable the stop button for example)

    x Abort
    x WriteCom
    x ReadCom
    x WriteScreen
    x ReadKeyboard
    x Sleep
    x Cls
---
Basic commands:
    * ChangeDir
    * CreateConnection
    * CloseConnection
    * OpenURI
    * OpenConnection
    * CloseConnection
    * MessageBox
    * FlushKeyboard
    * FlushCom
    * GetConnectionTitle
    * SetConnectionTitle
    * SendFile
    * Open
    * Close
    * Write
    * Read
    * RecvFile
    * SendFile
    * GetConnectionStatus   <- Open/Close
    * Wait <- Wait for one of the provided strings
    * mkdir
    * rmdir
    * deletefile
    * beep
    * CRC
    * Date
    * ChangeOptions
    * ChangeAuxControl
    * BridgeConnections
    * StartDownload
    * StartUpload
    * SetTranDelay
    * SetAutoConnect
    * StopWatch
    * SetZoom
    * InsertHR
    * ShowPanel
    * Capture
    * ClearHex
    * SetTerminalDataProcessorMode
    * Enable/Disable Data Processor
    * SetTerminalEmulation
    * SetCharacterEncoding
    * 


*/
/*******************************************************************************
 * FILENAME: WTB_OSTime.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 14 Feb 2026 Paul Hutchinson.
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
 * CREATED BY:
 *    Paul Hutchinson (14 Feb 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "../WTB_OSTime.h"
#include <unistd.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

void WTB_Sleep(unsigned int ms)
{
    usleep(ms*1000);
}

