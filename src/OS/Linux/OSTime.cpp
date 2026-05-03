/*******************************************************************************
 * FILENAME: OSTime.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (04 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/OSTime.h"
#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    GetElapsedTime_ms
 *
 * SYNOPSIS:
 *    uint32_t GetElapsedTime_ms(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the number of milliseconds from the system clock.
 *
 * RETURNS:
 *    A count of milliseconds taken from the system clock. The absolute value
 *    is not meaningful on its own; only the difference between two calls is.
 *
 * LIMITATIONS:
 *    The returned value is a 32 bit unsigned integer and will wrap back to
 *    zero approximately every 49.7 days. Code that uses this function to
 *    measure elapsed time across long intervals must account for this wrap.
 *
 * SEE ALSO:
 *    OS_Sleep(), GetCurrentTime()
 ******************************************************************************/
uint32_t GetElapsedTime_ms(void)
{
    struct timeval tv;
    uint64_t MillSec;

    gettimeofday(&tv,NULL);
    MillSec=tv.tv_sec*1000;
    MillSec+=tv.tv_usec/1000;

    return MillSec;
}

/*******************************************************************************
 * NAME:
 *    OS_Sleep
 *
 * SYNOPSIS:
 *    void OS_Sleep(unsigned int ms);
 *
 * PARAMETERS:
 *    ms [I] -- The number of milliseconds to pause the calling thread for.
 *
 * FUNCTION:
 *    This function pauses execution of the calling thread for the requested
 *    number of milliseconds. The actual delay may be slightly longer than
 *    requested due to operating system scheduling.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    GetElapsedTime_ms()
 ******************************************************************************/
void OS_Sleep(unsigned int ms)
{
    usleep(ms*1000);
}

/*******************************************************************************
 * NAME:
 *    GetCurrentTime
 *
 * SYNOPSIS:
 *    uint64_t GetCurrentTime(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns the current wall clock time as the number of
 *    seconds that have elapsed since the Unix epoch (00:00:00 UTC on
 *    1 January 1970).
 *
 * RETURNS:
 *    The current time, in seconds, since the Unix epoch.
 *
 * NOTES:
 *    Unlike GetElapsedTime_ms() the returned value is an absolute time and
 *    is suitable for time stamping or comparing against other epoch times.
 *
 * SEE ALSO:
 *    GetElapsedTime_ms()
 ******************************************************************************/
uint64_t GetCurrentTime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec;
}
