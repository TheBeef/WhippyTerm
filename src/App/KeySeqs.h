/*******************************************************************************
 * FILENAME: KeySeqs.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (16 Jul 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __KEYSEQS_H_
#define __KEYSEQS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/KeyDefines.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct CommandKeySeq
{
    uint8_t Mod;
    e_UIKeys Key;
    char Letter;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
e_UIKeys ConvertKeyName2ENum(const char *Name);
const char *ConvertENum2KeyName(e_UIKeys Key);
const char *ConvertKeySeq2String(struct CommandKeySeq *KeySeq);
bool ConvertString2KeySeq(struct CommandKeySeq *KeySeq,const char *Str);
bool KeySeqMapped(struct CommandKeySeq *KeySeq);

#endif
