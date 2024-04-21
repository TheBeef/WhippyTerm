/*******************************************************************************
 * FILENAME: KeyValueSupport.h
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
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __KEYVALUESUPPORT_H_
#define __KEYVALUESUPPORT_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/
#define PIS_ConvertKVList2PIKVList(KVList) (t_PIKVList *)&(KVList)

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void PI_KVClear(t_PIKVList *Handle);
PG_BOOL PI_KVAddItem(t_PIKVList *Handle,const char *Key,const char *Value);
const char *PI_KVGetItem(const t_PIKVList *Handle,const char *Key);

#endif
