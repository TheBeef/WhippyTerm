/*******************************************************************************
 * FILENAME: CRCSystem.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (31 Aug 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __CRCSYSTEM_H_
#define __CRCSYSTEM_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/* Don't forget to add to session if you add / remove CRC types */
typedef enum
{
    /* 8 Bit CRC's */
    e_CRC_CRC8,
    e_CRC_CRC8_SAE_J1850,
    e_CRC_CRC8_SAE_J1850_ZERO,
    e_CRC_CRC8_8H2F,
    e_CRC_CRC8_CDMA2000,
    e_CRC_CRC8_DARC,
    e_CRC_CRC8_DVB_S2,
    e_CRC_CRC8_EBU,
    e_CRC_CRC8_ICODE,
    e_CRC_CRC8_ITU,
    e_CRC_CRC8_MAXIM,
    e_CRC_CRC8_ROHC,
    e_CRC_CRC8_WCDMA,
    e_CRC_CRC8_BLUETOOTH,

    /* 16 bit CRC's */
    e_CRC_CRC16_CCIT_ZERO,
    e_CRC_CRC16_ARC,
    e_CRC_CRC16_AUG_CCITT,
    e_CRC_CRC16_BUYPASS,
    e_CRC_CRC16_CCITT_FALSE,
    e_CRC_CRC16_CDMA2000,
    e_CRC_CRC16_DDS_110,
    e_CRC_CRC16_DECT_R,
    e_CRC_CRC16_DECT_X,
    e_CRC_CRC16_DNP,
    e_CRC_CRC16_EN_13757,
    e_CRC_CRC16_GENIBUS,
    e_CRC_CRC16_MAXIM,
    e_CRC_CRC16_MCRF4XX,
    e_CRC_CRC16_RIELLO,
    e_CRC_CRC16_T10_DIF,
    e_CRC_CRC16_TELEDISK,
    e_CRC_CRC16_TMS37157,
    e_CRC_CRC16_USB,
    e_CRC_CRC16_A,
    e_CRC_CRC16_KERMIT,
    e_CRC_CRC16_MODBUS,
    e_CRC_CRC16_X_25,
    e_CRC_CRC16_XMODEM,

    /* 32 bit CRC's */
    e_CRC_CRC32,
    e_CRC_CRC32_BZIP2,
    e_CRC_CRC32_C,
    e_CRC_CRC32_D,
    e_CRC_CRC32_MPEG2,
    e_CRC_CRC32_POSIX,
    e_CRC_CRC32_Q,
    e_CRC_CRC32_JAMCRC,
    e_CRC_CRC32_XFER,

    /* 64 bit CRC's */
    e_CRC_CRC64_ECMA_182,
    e_CRC_CRC64_GO_ISO,
    e_CRC_CRC64_WE,
    e_CRC_CRC64_XZ,

    e_CRCMAX
} e_CRCType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void CRC_ShutDown(void);
void CRC_CalCRCStr(e_CRCType CRCType,const uint8_t *Data,int Bytes,char *buff);
int CRC_CalcCRC(e_CRCType CRCType,const uint8_t *Data,int Bytes,uint64_t *CRC);
bool CRC_GetListOfAvailableCRCs(t_ListViewItemListType &CRCList);
bool CRC_BuildSource4CRC(e_CRCType CRCType,std::string &OutStr);

#endif
