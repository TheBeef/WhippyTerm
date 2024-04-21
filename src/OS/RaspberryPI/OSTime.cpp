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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

uint32_t GetElapsedTime_ms(void)
{
    struct timeval tv;
    uint64_t MillSec;

    gettimeofday(&tv,NULL);
    MillSec=tv.tv_sec*1000;
    MillSec+=tv.tv_usec/1000;

    return MillSec;
}
