/*******************************************************************************
 * FILENAME: DebugUtils.cpp
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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (26 May 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIDebug.h"
#include <QElapsedTimer>
#include <QInputDialog>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
QElapsedTimer *m_Debug_ElapsedTimer=NULL;
uint64_t g_DBTimers[e_DBTMAX];
uint32_t g_DBTimes[e_DBTMAX];
uint32_t g_DBMaxTimes[e_DBTMAX];
uint32_t g_DBAvgTimes[e_DBTMAX];
uint32_t g_DBAvgCounts[e_DBTMAX];

/*******************************************************************************
 * NAME:
 *    DEBUG_GetTimerElapsed
 *
 * SYNOPSIS:
 *    uint64_t DEBUG_GetTimerElapsed(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the number of nanoseconds since the first call to
 *    this function.
 *
 * RETURNS:
 *    The number of nano seconds since the first call to this function.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
uint64_t DEBUG_GetTimerElapsed(void)
{
    if(m_Debug_ElapsedTimer==NULL)
    {
        m_Debug_ElapsedTimer=new QElapsedTimer();
        m_Debug_ElapsedTimer->start();
    }
    return m_Debug_ElapsedTimer->nsecsElapsed();
}

uint32_t DEBUG_GetInt(const char *Msg,int Start)
{
    return QInputDialog::getInt(NULL, "Input Int",Msg,Start);
}

void DB_StartTimer(e_DBTType Timer)
{
    g_DBTimers[Timer]=DEBUG_GetTimerElapsed();
}

void DB_StopTimer(e_DBTType Timer)
{
    g_DBTimes[Timer]=DEBUG_GetTimerElapsed()-g_DBTimers[Timer];

    if(g_DBTimes[Timer]>g_DBMaxTimes[Timer])
        g_DBMaxTimes[Timer]=g_DBTimes[Timer];

    g_DBAvgCounts[Timer]++;
    g_DBAvgTimes[Timer]+=g_DBTimes[Timer];
}

