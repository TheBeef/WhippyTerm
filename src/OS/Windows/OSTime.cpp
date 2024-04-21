/*******************************************************************************
 * FILENAME: OSTime.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * CREATED BY:
 *    Paul Hutchinson (04 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/OSTime.h"
#include <windows.h>
#include <stdint.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

uint32_t GetElapsedTime_ms(void)
{
    FILETIME CurrentTime;

    GetSystemTimeAsFileTime(&CurrentTime);

    return CurrentTime.dwLowDateTime;
}

