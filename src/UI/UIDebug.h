/*******************************************************************************
 * FILENAME: UIDebug.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has interfaces to some debug functions in it.
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (26 May 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIDEBUG_H_
#define __UIDEBUG_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>
#include <QDebug>

/***  DEFINES                          ***/

/***  MACROS                           ***/
#define D1() {uint64_t Start;Start=DEBUG_GetTimerElapsed();
#define D2(x) qDebug("%s:%fms",x,(DEBUG_GetTimerElapsed()-Start)/1000000.0);}

/***  TYPE DEFINITIONS                 ***/
#define FOREACH_DBT(ENTRY) \
        ENTRY(e_DBT_DriverAvailData)     \
        ENTRY(e_DBT_AddChar2Display)     \
        ENTRY(e_DBT_SignalFromThread)    \
        ENTRY(e_DBTMAX)                  \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum
{
    FOREACH_DBT(GENERATE_ENUM)
} e_DBTType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
uint64_t DEBUG_GetTimerElapsed(void);
void DebugMsg(const char *fmt,...);
uint32_t DEBUG_GetInt(const char *Msg,int Start);

/* DisplayBuffer debugging */
void AllocateDisplayBufferDebugForm(void);
void UIDBD_SetDB(class DisplayBuffer *usedb);
void UIDBD_Update(void);

void DB_StartTimer(e_DBTType Timer);
void DB_StopTimer(e_DBTType Timer);

#endif
