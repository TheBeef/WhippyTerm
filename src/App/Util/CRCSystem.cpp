/*******************************************************************************
 * FILENAME: CRCSystem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the system for calc'ing CRC's of all different types.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (31 Aug 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Util/CRCSystem.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static uint64_t CRC_DoCRCFromParam(const uint8_t *DataPtr,int Bytes,
        const uint64_t *CRCTable,int Bits,uint64_t Poly,uint64_t StartVal,
        uint64_t XorOut,bool RefIn,bool RefOut);
static uint64_t *CRC_CalcCRCTable(int Bits,uint64_t Poly);
static uint64_t CRC_DoReflect(uint64_t Value,int Bits);
static bool CRC_GetCRCParam(e_CRCType CRCType,int &Bits,uint64_t &Poly,
        uint64_t &Start,uint64_t &XOR,bool &RefIn,bool &RefOut);
static void CRC_Append2String(std::string &Str,const char *fmt,...);

/*** VARIABLE DEFINITIONS     ***/
uint64_t *m_CRCLookups[e_CRCMAX];

/*******************************************************************************
 * NAME:
 *    CRC_ShutDown
 *
 * SYNOPSIS:
 *    void CRC_ShutDown(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called at shut down.  It frees anything that the
 *    CRC system had allocated.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void CRC_ShutDown(void)
{
    int r;

    for(r=0;r<e_CRCMAX;r++)
    {
        if(m_CRCLookups[r]!=NULL)
        {
            free(m_CRCLookups[r]);
            m_CRCLookups[r]=NULL;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    CRC_GetListOfAvailableCRCs
 *
 * SYNOPSIS:
 *    bool CRC_GetListOfAvailableCRCs(t_ListViewItemListType &CRCList);
 *
 * PARAMETERS:
 *    CRCList [O] -- The list of CRC's that the system supports.
 *
 * FUNCTION:
 *    This function gets a list of CRC's and their names that the system
 *    supports.  This is ment for the UI.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool CRC_GetListOfAvailableCRCs(t_ListViewItemListType &CRCList)
{
    struct ListViewItem NewItem;

    try
    {
        CRCList.clear();

        NewItem.Label="CRC-8";NewItem.ID=e_CRC_CRC8;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/SAE-J1850";NewItem.ID=e_CRC_CRC8_SAE_J1850;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/SAE-J1850-ZERO";NewItem.ID=e_CRC_CRC8_SAE_J1850_ZERO;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/8H2F";NewItem.ID=e_CRC_CRC8_8H2F;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/CDMA2000";NewItem.ID=e_CRC_CRC8_CDMA2000;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/DARC";NewItem.ID=e_CRC_CRC8_DARC;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/DVB-S2";NewItem.ID=e_CRC_CRC8_DVB_S2;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/EBU";NewItem.ID=e_CRC_CRC8_EBU;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/I-CODE";NewItem.ID=e_CRC_CRC8_ICODE;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/ITU";NewItem.ID=e_CRC_CRC8_ITU;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/MAXIM-DOW";NewItem.ID=e_CRC_CRC8_MAXIM;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/ROHC";NewItem.ID=e_CRC_CRC8_ROHC;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/WCDMA";NewItem.ID=e_CRC_CRC8_WCDMA;CRCList.push_back(NewItem);
        NewItem.Label="CRC-8/BLUETOOTH";NewItem.ID=e_CRC_CRC8_BLUETOOTH;CRCList.push_back(NewItem);

        NewItem.Label="CRC-16/CCIT-ZERO";NewItem.ID=e_CRC_CRC16_CCIT_ZERO;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/ARC";NewItem.ID=e_CRC_CRC16_ARC;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/AUG-CCITT";NewItem.ID=e_CRC_CRC16_AUG_CCITT;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/BUYPASS";NewItem.ID=e_CRC_CRC16_BUYPASS;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/CCITT-FALSE";NewItem.ID=e_CRC_CRC16_CCITT_FALSE;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/CDMA2000";NewItem.ID=e_CRC_CRC16_CDMA2000;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/DDS-110";NewItem.ID=e_CRC_CRC16_DDS_110;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/DECT-R";NewItem.ID=e_CRC_CRC16_DECT_R;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/DECT-X";NewItem.ID=e_CRC_CRC16_DECT_X;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/DNP";NewItem.ID=e_CRC_CRC16_DNP;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/EN-13757";NewItem.ID=e_CRC_CRC16_EN_13757;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/GENIBUS";NewItem.ID=e_CRC_CRC16_GENIBUS;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/MAXIM-DOW";NewItem.ID=e_CRC_CRC16_MAXIM;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/MCRF4XX";NewItem.ID=e_CRC_CRC16_MCRF4XX;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/RIELLO";NewItem.ID=e_CRC_CRC16_RIELLO;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/T10-DEF";NewItem.ID=e_CRC_CRC16_T10_DIF;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/TELEDISK";NewItem.ID=e_CRC_CRC16_TELEDISK;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/TMS37157";NewItem.ID=e_CRC_CRC16_TMS37157;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/USB";NewItem.ID=e_CRC_CRC16_USB;CRCList.push_back(NewItem);
        NewItem.Label="CRC-A";NewItem.ID=e_CRC_CRC16_A;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/KERMIT";NewItem.ID=e_CRC_CRC16_KERMIT;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/MODBUS";NewItem.ID=e_CRC_CRC16_MODBUS;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/X-25";NewItem.ID=e_CRC_CRC16_X_25;CRCList.push_back(NewItem);
        NewItem.Label="CRC-16/XMODEM";NewItem.ID=e_CRC_CRC16_XMODEM;CRCList.push_back(NewItem);

        NewItem.Label="CRC-32";NewItem.ID=e_CRC_CRC32;CRCList.push_back(NewItem);   // CRC-32/ISO-HDLC
        NewItem.Label="CRC-32/BZIP2";NewItem.ID=e_CRC_CRC32_BZIP2;CRCList.push_back(NewItem);
        NewItem.Label="CRC-32C";NewItem.ID=e_CRC_CRC32_C;CRCList.push_back(NewItem);
        NewItem.Label="CRC-32D";NewItem.ID=e_CRC_CRC32_D;CRCList.push_back(NewItem);
        NewItem.Label="CRC-32/MPEG-2";NewItem.ID=e_CRC_CRC32_MPEG2;CRCList.push_back(NewItem);
        NewItem.Label="CRC-32/POSIX";NewItem.ID=e_CRC_CRC32_POSIX;CRCList.push_back(NewItem);
        NewItem.Label="CRC-32Q";NewItem.ID=e_CRC_CRC32_Q;CRCList.push_back(NewItem);
        NewItem.Label="CRC-32/JAMCRC";NewItem.ID=e_CRC_CRC32_JAMCRC;CRCList.push_back(NewItem);
        NewItem.Label="CRC-32/XFER";NewItem.ID=e_CRC_CRC32_XFER;CRCList.push_back(NewItem);

        NewItem.Label="CRC-64/ECMA-182";NewItem.ID=e_CRC_CRC64_ECMA_182;CRCList.push_back(NewItem);
        NewItem.Label="CRC-64/ISO";NewItem.ID=e_CRC_CRC64_GO_ISO;CRCList.push_back(NewItem);
        NewItem.Label="CRC-64/WE";NewItem.ID=e_CRC_CRC64_WE;CRCList.push_back(NewItem);
        NewItem.Label="CRC-64/XZ";NewItem.ID=e_CRC_CRC64_XZ;CRCList.push_back(NewItem);
    }
    catch(...)
    {
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    CRC_CalCRCStr
 *
 * SYNOPSIS:
 *    void CRC_CalCRCStr(const uint8_t *Data,int Bytes,char *OutStr);
 *
 * PARAMETERS:
 *    Data [I] -- A pointer to the bytes to calc the CRC for
 *    Bytes [I] -- The number of bytes in 'Data'
 *    OutStr [O] -- A buffer to store the CRC into.  This will be string and
 *                  must be 17 bytes.
 *
 * FUNCTION:
 *    This function takes a block of bytes and calc's the CRC for it.
 *    'm_CRC_UseCRCType' will be used for what CRC to use.
 *
 *    The return value is a string with hex digits of the CRC in it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void CRC_CalCRCStr(e_CRCType CRCType,const uint8_t *Data,int Bytes,char *buff)
{
    uint64_t CRC;
    int Bits;
    int OutDig;

    Bits=CRC_CalcCRC(CRCType,Data,Bytes,&CRC);
    if(Bits==0)
    {
        buff[0]=0;
        return;
    }

    OutDig=Bits/4;

    sprintf(buff,"%0*" PRIX64,OutDig,CRC);
}

/*******************************************************************************
 * NAME:
 *    CRC_CalcCRC
 *
 * SYNOPSIS:
 *    int CRC_CalcCRC(e_CRCType CRCType,const uint8_t *Data,int Bytes,
 *              uint64_t *CRC);
 *
 * PARAMETERS:
 *    CRCType [I] -- The type of CRC to calc
 *    Data [I] -- The data to calc the CRC from
 *    Bytes [I] -- The number of bytes in 'Data'
 *    CRC [O] -- The CRC we calc'ed.  This is 64 bit number but only the
 *               lower bits are valid.  This function returns the number of
 *               valid bits.
 *
 * FUNCTION:
 *    This function calc's a CRC for some data.
 *
 * RETURNS:
 *    The number of bits in the CRC.  This will be 0 if there was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int CRC_CalcCRC(e_CRCType CRCType,const uint8_t *Data,int Bytes,uint64_t *CRC)
{
    int Bits;
    uint64_t Poly;
    uint64_t Start;
    uint64_t XOR;
    bool RefIn;
    bool RefOut;

    if(!CRC_GetCRCParam(CRCType,Bits,Poly,Start,XOR,RefIn,RefOut))
        return 0;

    if(m_CRCLookups[CRCType]==NULL)
        m_CRCLookups[CRCType]=CRC_CalcCRCTable(Bits,Poly);
    if(m_CRCLookups[CRCType]==NULL)
        return 0;

    *CRC=CRC_DoCRCFromParam(Data,Bytes,m_CRCLookups[CRCType],Bits,Poly,
            Start,XOR,RefIn,RefOut);

    return Bits;
}

/*******************************************************************************
 * NAME:
 *    CRC_GetCRCParam
 *
 * SYNOPSIS:
 *    static bool CRC_GetCRCParam(e_CRCType CRCType,int &Bits,uint64_t &Poly,
 *          uint64_t &Start,uint64_t &XOR,bool &RefIn,bool &RefOut);
 *
 * PARAMETERS:
 *    CRCType [I] -- The type of CRC to get the param's for
 *    Bits [O] -- The number of bits in this CRC
 *    Poly [O] -- The polynomial to use for this CRC
 *    Start [O] -- The starting value
 *    XOR [O] -- The value to xor the result with
 *    RefIn [O] -- Do we do reflection on the data
 *    RefOut [O] -- Do we do reflection on the result
 *
 * FUNCTION:
 *    This function gets the params to be used on the CRC
 *
 * RETURNS:
 *    true -- We filled in the data
 *    false -- There was an error (unknown CRC type)
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool CRC_GetCRCParam(e_CRCType CRCType,int &Bits,uint64_t &Poly,
        uint64_t &Start,uint64_t &XOR,bool &RefIn,bool &RefOut)
{
    /* https://reveng.sourceforge.io/crc-catalogue/all.htm */
    switch(CRCType)
    {
        case e_CRC_CRC8:
            Bits=8;
            Poly=0x07;
            Start=0;
            XOR=0;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC8_SAE_J1850:
            Bits=8;
            Poly=0x1D;
            Start=0xFF;
            XOR=0xFF;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC8_SAE_J1850_ZERO:
            Bits=8;
            Poly=0x1D;
            Start=0x00;
            XOR=0x00;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC8_8H2F:
            Bits=8;
            Poly=0x2F;
            Start=0xFF;
            XOR=0xFF;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC8_CDMA2000:
            Bits=8;
            Poly=0x9B;
            Start=0xFF;
            XOR=0x00;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC8_DARC:
            Bits=8;
            Poly=0x39;
            Start=0x00;
            XOR=0x00;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC8_DVB_S2:
            Bits=8;
            Poly=0xD5;
            Start=0x00;
            XOR=0x00;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC8_EBU:
            Bits=8;
            Poly=0x1D;
            Start=0xFF;
            XOR=0x00;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC8_ICODE:
            Bits=8;
            Poly=0x1D;
            Start=0xFD;
            XOR=0x00;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC8_ITU:
            Bits=8;
            Poly=0x07;
            Start=0x00;
            XOR=0x55;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC8_MAXIM:
            Bits=8;
            Poly=0x31;
            Start=0x00;
            XOR=0x00;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC8_ROHC:
            Bits=8;
            Poly=0x07;
            Start=0xFF;
            XOR=0x00;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC8_WCDMA:
            Bits=8;
            Poly=0x9B;
            Start=0x00;
            XOR=0x00;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC8_BLUETOOTH:
            Bits=8;
            Poly=0xA7;
            Start=0x00;
            XOR=0x00;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_CCIT_ZERO:
            Bits=16;
            Poly=0x1021;
            Start=0x0000;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_ARC:
            Bits=16;
            Poly=0x8005;
            Start=0x0000;
            XOR=0x0000;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_AUG_CCITT:
            Bits=16;
            Poly=0x1021;
            Start=0x1D0F;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_BUYPASS:
            Bits=16;
            Poly=0x8005;
            Start=0x0000;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_CCITT_FALSE:
            Bits=16;
            Poly=0x1021;
            Start=0xFFFF;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_CDMA2000:
            Bits=16;
            Poly=0xC867;
            Start=0xFFFF;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_DDS_110:
            Bits=16;
            Poly=0x8005;
            Start=0x800D;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_DECT_R:
            Bits=16;
            Poly=0x0589;
            Start=0x0000;
            XOR=0x0001;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_DECT_X:
            Bits=16;
            Poly=0x0589;
            Start=0x0000;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_DNP:
            Bits=16;
            Poly=0x3D65;
            Start=0x0000;
            XOR=0xFFFF;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_EN_13757:
            Bits=16;
            Poly=0x3D65;
            Start=0x0000;
            XOR=0xFFFF;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_GENIBUS:
            Bits=16;
            Poly=0x1021;
            Start=0xFFFF;
            XOR=0xFFFF;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_MAXIM:
            Bits=16;
            Poly=0x8005;
            Start=0x0000;
            XOR=0xFFFF;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_MCRF4XX:
            Bits=16;
            Poly=0x1021;
            Start=0xFFFF;
            XOR=0x0000;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_RIELLO:
            Bits=16;
            Poly=0x1021;
            Start=0xB2AA;
            XOR=0x0000;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_T10_DIF:
            Bits=16;
            Poly=0x8BB7;
            Start=0x0000;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_TELEDISK:
            Bits=16;
            Poly=0xA097;
            Start=0x0000;
            XOR=0x0000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC16_TMS37157:
            Bits=16;
            Poly=0x1021;
            Start=0x89EC;
            XOR=0x0000;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_USB:
            Bits=16;
            Poly=0x8005;
            Start=0xFFFF;
            XOR=0xFFFF;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_A:
            Bits=16;
            Poly=0x1021;
            Start=0xC6C6;
            XOR=0x0000;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_KERMIT:
            Bits=16;
            Poly=0x1021;
            Start=0x0000;
            XOR=0x0000;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_MODBUS:
            Bits=16;
            Poly=0x8005;
            Start=0xFFFF;
            XOR=0x0000;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_X_25:
            Bits=16;
            Poly=0x1021;
            Start=0xFFFF;
            XOR=0xFFFF;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC16_XMODEM:
            Bits=16;
            Poly=0x1021;
            Start=0;
            XOR=0;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC32:
            Bits=32;
            Poly=0x4C11DB7;
            Start=0xFFFFFFFF;
            XOR=0xFFFFFFFF;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC32_BZIP2:
            Bits=32;
            Poly=0x4C11DB7;
            Start=0xFFFFFFFF;
            XOR=0xFFFFFFFF;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC32_C:
            Bits=32;
            Poly=0x1EDC6F41;
            Start=0xFFFFFFFF;
            XOR=0xFFFFFFFF;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC32_D:
            Bits=32;
            Poly=0xA833982B;
            Start=0xFFFFFFFF;
            XOR=0xFFFFFFFF;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC32_MPEG2:
            Bits=32;
            Poly=0x4C11DB7;
            Start=0xFFFFFFFF;
            XOR=0x00000000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC32_POSIX:
            Bits=32;
            Poly=0x4C11DB7;
            Start=0x00000000;
            XOR=0xFFFFFFFF;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC32_Q:
            Bits=32;
            Poly=0x814141AB;
            Start=0x00000000;
            XOR=0x00000000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC32_JAMCRC:
            Bits=32;
            Poly=0x4C11DB7;
            Start=0xFFFFFFFF;
            XOR=0x00000000;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC32_XFER:
            Bits=32;
            Poly=0xAF;
            Start=0x00000000;
            XOR=0x00000000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC64_ECMA_182:
            Bits=64;
            Poly=0x42F0E1EBA9EA3693;
            Start=0x0000000000000000;
            XOR=0x0000000000000000;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC64_GO_ISO:
            Bits=64;
            Poly=0x000000000000001B;
            Start=0xFFFFFFFFFFFFFFFF;
            XOR=0xFFFFFFFFFFFFFFFF;
            RefIn=true;
            RefOut=true;
        break;
        case e_CRC_CRC64_WE:
            Bits=64;
            Poly=0x42F0E1EBA9EA3693;
            Start=0xFFFFFFFFFFFFFFFF;
            XOR=0xFFFFFFFFFFFFFFFF;
            RefIn=false;
            RefOut=false;
        break;
        case e_CRC_CRC64_XZ:
            Bits=64;
            Poly=0x42F0E1EBA9EA3693;
            Start=0xFFFFFFFFFFFFFFFF;
            XOR=0xFFFFFFFFFFFFFFFF;
            RefIn=true;
            RefOut=true;
        break;
        default:
        case e_CRCMAX:
            return false;
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    CRC_CalcCRCTable
 *
 * SYNOPSIS:
 *    static uint64_t *CRC_CalcCRCTable(int Bits,uint64_t Poly);
 *
 * PARAMETERS:
 *    Bits [I] -- The number of bits in this CRC (8,16,32,64)
 *    Poly [I] -- The polynomial to use to build this table
 *
 * FUNCTION:
 *    This function builds a CRC table for use with CRC_DoCRC().  It allocates
 *    the memory and you need to free it.
 *
 * RETURNS:
 *    A pointer to the allocated memory with the table in it or NULL on error.
 *
 * NOTES:
 *    You must free the memory with free()
 *
 * SEE ALSO:
 *    CRC_DoCRC()
 ******************************************************************************/
static uint64_t *CRC_CalcCRCTable(int Bits,uint64_t Poly)
{
    uint64_t *CRCTable;
    int Byte;
    int i;
    uint64_t Entry;
    uint64_t MSB;
    uint64_t Mask;

    CRCTable=(uint64_t *)malloc(256*sizeof(uint64_t));
    if(CRCTable==NULL)
        return NULL;

    MSB=1LL<<(Bits-1);
    if(Bits<64)
        Mask=(1LL<<Bits)-1;
    else
        Mask=0xFFFFFFFFFFFFFFFF;

    for(Byte=0;Byte<256;Byte++)
    {
        Entry=((uint64_t)Byte<<(Bits-8))&Mask;
        for(i=0;i<8;i++)
        {
            if((Entry&MSB))
                Entry=(Entry<<1)^Poly;
            else
                Entry=Entry<<1;
        }
        CRCTable[Byte]=(Entry&Mask);
    }
    return CRCTable;
}

/*******************************************************************************
 * NAME:
 *    CRC_DoCRCFromParam
 *
 * SYNOPSIS:
 *    static uint64_t CRC_DoCRCFromParam(const uint8_t *DataPtr,int Bytes,
 *          const uint64_t *CRCTable,int Bits,uint64_t Poly,
 *          uint64_t StartVal,uint64_t XorOut,bool RefIn,bool RefOut);
 *
 * PARAMETERS:
 *    DataPtr [I] -- The data to do the CRC on
 *    Bytes [I] -- The number of bytes in 'DataPtr'
 *    CRCTable [I] -- The lookup table with CRC values in it.  Made with
 *                    CRC_CalcCRCTable()
 *    Bits [I] -- The number of bits in this CRC (8,16,32,64)
 *    Poly [I] -- The polynomial to use with this CRC
 *    StartVal [I] -- The starting value to assign to the CRC
 *    XorOut [I] -- The value to xor the resulting CRC with before we return.
 *    RefIn [I] -- Do we reflect the incoming bytes.
 *    RefOut [I] -- Do we reflect the CRC before we return it.
 *
 * FUNCTION:
 *    This function does a CRC on a block of data.  This function supports
 *    many types of CRC.  It supports many standard CRC's as well custom
 *    CRC's if you want.
 *
 * RETURNS:
 *    The CRC.  This is in a 64 bit value but only the lower 'Bits' worth
 *    will be valid.
 *
 * NOTES:
 *    Based on the work by Bastian Molkenthin who does a great job on
 *    CRC's at:
 *      http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
 *
 * SEE ALSO:
 *    CRC_CalcCRCTable()
 ******************************************************************************/
static uint64_t CRC_DoCRCFromParam(const uint8_t *DataPtr,int Bytes,
        const uint64_t *CRCTable,int Bits,uint64_t Poly,uint64_t StartVal,
        uint64_t XorOut,bool RefIn,bool RefOut)
{
    int i;
    uint64_t crc;
    uint8_t curByte;
    uint64_t Mask;
    int LookupIndex;

    if(Bits<64)
        Mask=(1LL<<Bits)-1;
    else
        Mask=0xFFFFFFFFFFFFFFFF;

    crc=StartVal;
    for(i=0;i<Bytes;i++)
    {
        curByte=DataPtr[i]&0xFF;
        if(RefIn)
            curByte=CRC_DoReflect(curByte,8);

        crc=(crc^((uint64_t)curByte<<(Bits-8)))&Mask;
        LookupIndex=(crc>>(Bits-8))&0xFF;
        crc=(crc<<8)&Mask;
        crc=(crc^CRCTable[LookupIndex])&Mask;
    }

    if(RefOut)
        crc=CRC_DoReflect(crc,Bits);
    return (crc^XorOut)&Mask;
}

/*******************************************************************************
 * NAME:
 *    CRC_DoReflect
 *
 * SYNOPSIS:
 *    static uint64_t CRC_DoReflect(uint64_t Value,int Bits);
 *
 * PARAMETERS:
 *    Value [I] -- The value to reflect
 *    Bits [I] -- The number of bits to swap
 *
 * FUNCTION:
 *    This function does the reflect for the CRC.
 *
 * RETURNS:
 *    The reflected value
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static uint64_t CRC_DoReflect(uint64_t Value,int Bits)
{
    uint64_t RetValue;
    int i;

    RetValue=0;

    for(i=0;i<Bits;i++)
        if((Value&(1LL<<i)))
            RetValue|=(1LL<<((Bits-1)-i));

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    CRC_BuildSource4CRC
 *
 * SYNOPSIS:
 *    bool CRC_BuildSource4CRC(e_CRCType CRCType,std::string &OutStr);
 *
 * PARAMETERS:
 *    CRCType [I] -- The type of CRC to build the source code for.
 *    OutStr [O] -- The string that was built
 *
 * FUNCTION:
 *    This function makes the source code to calc a CRC.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool CRC_BuildSource4CRC(e_CRCType CRCType,std::string &OutStr)
{
    int Bits;
    uint64_t Poly;
    uint64_t Start;
    uint64_t XOR;
    bool RefIn;
    bool RefOut;
    char buff[100];
    int r;
    int OutDig;
    int len;
    const char *CTypeStr;
    const char *OneTypeStr;

    try
    {
        OutStr="";

        if(!CRC_GetCRCParam(CRCType,Bits,Poly,Start,XOR,RefIn,RefOut))
            throw(0);

        if(m_CRCLookups[CRCType]==NULL)
            m_CRCLookups[CRCType]=CRC_CalcCRCTable(Bits,Poly);
        if(m_CRCLookups[CRCType]==NULL)
            throw(0);

        switch(Bits)
        {
            case 8:
                CTypeStr="uint8_t";
                OneTypeStr="";
            break;
            case 16:
                CTypeStr="uint16_t";
                OneTypeStr="";
            break;
            case 32:
                CTypeStr="uint32_t";
                OneTypeStr="L";
            break;
            case 64:
                CTypeStr="uint64_t";
                OneTypeStr="LL";
            break;
            default:
                throw(0);
        }
        OutDig=Bits/4;

        CRC_Append2String(OutStr,"#include <stdint.h>\n");
        CRC_Append2String(OutStr,"\n");

        /* Output the CRC table for this CRC */
        CRC_Append2String(OutStr,"%s LookupTable[256]=\n",CTypeStr);
        CRC_Append2String(OutStr,"{\n");
        len=0;
        for(r=0;r<256;r++)
        {
            if(len==0)
            {
                OutStr+="    ";
                len+=4;
            }
            sprintf(buff,"0x%0*" PRIX64,OutDig,m_CRCLookups[CRCType][r]);
            OutStr+=buff;
            len+=2+OutDig;

            if(r==255)
                OutStr+="\n";
            else
                OutStr+=",";
            len++;
            if(len>70)
            {
                OutStr+="\n";
                len=0;
            }
        }
        CRC_Append2String(OutStr,"};\n");
        CRC_Append2String(OutStr,"\n");

        if(RefIn || (RefOut && Bits==8))
            CRC_Append2String(OutStr,"uint8_t CRC_DoReflect8(uint8_t Value);\n");

        if(RefOut && Bits!=8)
            CRC_Append2String(OutStr,"%s CRC_DoReflect%d(%s Value);\n",CTypeStr,Bits,CTypeStr);

        if(RefIn || RefOut)
            CRC_Append2String(OutStr,"\n");

        CRC_Append2String(OutStr,"%s CRC(const uint8_t *DataPtr,int Bytes)\n",CTypeStr);
        CRC_Append2String(OutStr,"{\n");
        CRC_Append2String(OutStr,"    int i;\n");
        CRC_Append2String(OutStr,"    %s crc;\n",CTypeStr);
        CRC_Append2String(OutStr,"    uint8_t curByte;\n");
        CRC_Append2String(OutStr,"    int LookupIndex;\n");
        CRC_Append2String(OutStr,"\n");
        CRC_Append2String(OutStr,"    crc=0x%0*" PRIX64 ";\n",OutDig,Start);
        CRC_Append2String(OutStr,"    for(i=0;i<Bytes;i++)\n");
        CRC_Append2String(OutStr,"    {\n");
        CRC_Append2String(OutStr,"        curByte=DataPtr[i]&0xFF;\n");
        if(RefIn)
            CRC_Append2String(OutStr,"        curByte=CRC_DoReflect8(curByte);\n");
        CRC_Append2String(OutStr,"        crc=(crc^((%s)curByte<<%d));\n",CTypeStr,Bits-8);
        CRC_Append2String(OutStr,"        LookupIndex=(crc>>%d)&0xFF;\n",Bits-8);
        CRC_Append2String(OutStr,"        crc=crc<<8;\n");
        CRC_Append2String(OutStr,"        crc=crc^LookupTable[LookupIndex];\n");
        CRC_Append2String(OutStr,"    }\n");
        if(RefOut)
            CRC_Append2String(OutStr,"    crc=CRC_DoReflect%d(crc);\n",Bits);
        if(XOR!=0)
            CRC_Append2String(OutStr,"    return (crc^0x%0*" PRIX64 ");\n",OutDig,XOR);
        else
            CRC_Append2String(OutStr,"    return crc;\n");
        CRC_Append2String(OutStr,"};\n");

        if(RefIn || (RefOut && Bits==8))
        {
            CRC_Append2String(OutStr,"\n");
            CRC_Append2String(OutStr,"uint8_t CRC_DoReflect8(uint8_t Value)\n");
            CRC_Append2String(OutStr,"{\n");
            CRC_Append2String(OutStr,"    uint8_t RetValue;\n");
            CRC_Append2String(OutStr,"    int i;\n");
            CRC_Append2String(OutStr,"\n");
            CRC_Append2String(OutStr,"    RetValue=0;\n");
            CRC_Append2String(OutStr,"\n");
            CRC_Append2String(OutStr,"    for(i=0;i<8;i++)\n");
            CRC_Append2String(OutStr,"        if((Value&(1<<i)))\n");
            CRC_Append2String(OutStr,"            RetValue|=(1<<(7-i));\n");
            CRC_Append2String(OutStr,"\n");
            CRC_Append2String(OutStr,"    return RetValue;\n");
            CRC_Append2String(OutStr,"}\n");
        }

        if(RefOut && Bits!=8)
        {
            CRC_Append2String(OutStr,"\n");
            CRC_Append2String(OutStr,"%s CRC_DoReflect%d(%s Value)\n",CTypeStr,Bits,CTypeStr);
            CRC_Append2String(OutStr,"{\n");
            CRC_Append2String(OutStr,"    %s RetValue;\n",CTypeStr);
            CRC_Append2String(OutStr,"    int i;\n");
            CRC_Append2String(OutStr,"\n");
            CRC_Append2String(OutStr,"    RetValue=0;\n");
            CRC_Append2String(OutStr,"\n");
            CRC_Append2String(OutStr,"    for(i=0;i<%d;i++)\n",Bits);
            CRC_Append2String(OutStr,"        if((Value&(1%s<<i)))\n",OneTypeStr);
            CRC_Append2String(OutStr,"            RetValue|=(1%s<<(%d-i));\n",OneTypeStr,Bits-1);
            CRC_Append2String(OutStr,"\n");
            CRC_Append2String(OutStr,"    return RetValue;\n");
            CRC_Append2String(OutStr,"}\n");
        }

        CRC_Append2String(OutStr,"/* This code is public domain do what you want with it just don't blame me if it messes up.\n\n");
        CRC_Append2String(OutStr,"   THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */\n");

//        CRC_Append2String(OutStr,"/* Copyright %s Paul Hutchinson\n","2021");
//        CRC_Append2String(OutStr,"\n");
//        CRC_Append2String(OutStr,"Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n");
//        CRC_Append2String(OutStr,"\n");
//        CRC_Append2String(OutStr,"The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n");
//        CRC_Append2String(OutStr,"\n");
//        CRC_Append2String(OutStr,"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */\n");
//        CRC_Append2String(OutStr,"\n");

    }
    catch(...)
    {
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    CRC_Append2String
 *
 * SYNOPSIS:
 *    static CRC_Append2String(std::string &Str,const char *fmt,...);
 *
 * PARAMETERS:
 *    Str [I/O] -- The string to append to
 *    fmt [I] -- printf() style format
 *    ... [I] -- args for printf()
 *
 * FUNCTION:
 *    This function is a helper function that appends a printf() formated
 *    string to a string.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This will throw out of memory.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void CRC_Append2String(std::string &Str,const char *fmt,...)
{
    char buff[500];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buff,sizeof(buff),fmt,args);
    Str+=buff;
    va_end(args);
}

/*******************************************************************************
 * NAME:
 *    CRC_FindCRC
 *
 * SYNOPSIS:
 *    t_CRCListType CRC_FindCRC(const uint8_t *Data,int DataSize,
 *              const char *CRCstr);
 *
 * PARAMETERS:
 *    Data [I] -- The data to build CRC's from
 *    DataSize [I] -- The number of bytes in 'Data'
 *    CRCstr [I] -- The CRC to try to match in string format (and hex).
 *
 * FUNCTION:
 *    This function searches for a CRC alg that will calc the Data and
 *    CRC provided.
 *
 * RETURNS:
 *    A list of CRC alg's found or a blank list if no matches found.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_CRCListType CRC_FindCRC(const uint8_t *Data,int DataSize,const char *CRCstr)
{
    t_CRCListType FoundCRCs;
    int Len;
    int SearchBits;
    uint64_t SearchCRC;
    uint64_t CalcCRC;
    int Bits;
    uint64_t Poly;
    uint64_t Start;
    uint64_t XOR;
    bool RefIn;
    bool RefOut;
    int alg;
    e_CRCType CRCType;

    /* First select what types of CRC we are look through based on the length
       of the CRC */
    Len=strlen(CRCstr);
    switch(Len)
    {
        case 2:  // 8 bit
            SearchBits=8;
        break;
        case 4:  // 16 bit
            SearchBits=16;
        break;
        case 8:  // 32 bit
            SearchBits=32;
        break;
        case 16: // 64 bit
            SearchBits=64;
        break;
        break;
        default:
            /* Can't convert */
            return FoundCRCs;
    }

    SearchCRC=strtoull(CRCstr,NULL,16);

    /* Try all the CRC alg's we have and see if we get a hit */
    for(alg=0;alg<e_CRCMAX;alg++)
    {
        CRCType=(e_CRCType)alg;
        if(!CRC_GetCRCParam(CRCType,Bits,Poly,Start,XOR,RefIn,RefOut))
            continue;

        /* If it's the wrong size, skip */
        if(Bits!=SearchBits)
            continue;

        /* Find the CRC for this data */
        if(m_CRCLookups[CRCType]==NULL)
            m_CRCLookups[CRCType]=CRC_CalcCRCTable(Bits,Poly);
        if(m_CRCLookups[CRCType]==NULL)
            continue;

        CalcCRC=CRC_DoCRCFromParam(Data,DataSize,m_CRCLookups[CRCType],Bits,
                Poly,Start,XOR,RefIn,RefOut);
        if(CalcCRC==SearchCRC)
        {
            /* Found it, add it to the list */
            FoundCRCs.push_back(CRCType);
        }
    }

    return FoundCRCs;
}

