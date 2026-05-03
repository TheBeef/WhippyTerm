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
    FILETIME CurrentTime;

    GetSystemTimeAsFileTime(&CurrentTime);

    return CurrentTime.dwLowDateTime/10000;
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
    Sleep(ms);
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
    FILETIME ft;
    ULARGE_INTEGER uli;

    GetSystemTimeAsFileTime(&ft);
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    /* Seconds between 1601-01-01 and 1970-01-01 = 11644473600,
       times 10,000,000 (100-ns ticks per second). */
    const uint64_t EPOCH_DIFF_100NS = 116444736000000000ULL;

    /* Divide by 10,000,000 to convert 100-ns ticks to seconds. */
    return (uli.QuadPart - EPOCH_DIFF_100NS) / 10000000ULL;
}
