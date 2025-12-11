/*******************************************************************************
 * FILENAME: ColorStream.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the decoder that will take a BPDS and apply it to the
 *    incoming stream, styling it as it goes.
 *
 * COPYRIGHT:
 *    Copyright 03 Dec 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (03 Dec 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "ColorStream.h"
#include "HexDump.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
extern "C"
{
 #include "BPDS.h"
}

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct ColorStreamData
{
    struct BPDSDef *Def;
    struct BPDSField *CurField;
    bool MatchAny;
    uint64_t BytesCount;
    uint64_t Search4BytesCount;
    uint64_t CollectedValue;
    struct BPDSField *MarkStartOfField;
    unsigned int MarkSize;
    bool NewStyle;
    bool Reset2Start;
    struct StyleData NextStyle;

    bool BigEndian;
};

struct BPDSParsedData
{
    struct StyleData Style;
    uint64_t Value;
};

struct BPDSParsedValueSetData
{
    unsigned int MatchCount;
};

/*** FUNCTION PROTOTYPES      ***/
static void BasicHexDecoder_FreeBPDSUserData(struct BPDSDef *Def);
static void ResetStream2StartOfProtocol(struct ColorStreamData *CSD,bool DidStyling);
static void ResetColorStream(struct ColorStreamData *CSD);
static uint64_t FixEndianAndClip(struct ColorStreamData *CSD,uint64_t Value,unsigned int Bytes);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    AllocColorStream
 *
 * SYNOPSIS:
 *    struct ColorStreamData *AllocColorStream(const char *BPDSDefStr);
 *
 * PARAMETERS:
 *    BPDSDefStr [I] -- The BPDS definition string to use with this stream.
 *
 * FUNCTION:
 *    This function allocates and gets a color stream ready for processing
 *    an incoming stream.
 *
 * RETURNS:
 *    A pointer to the color stream data or NULL if there was an error.
 *
 * SEE ALSO:
 *    FreeColorStream()
 ******************************************************************************/
struct ColorStreamData *AllocColorStream(const char *BPDSDefStr)
{
    struct ColorStreamData *NewCSD;
    struct BPDSField *Cur;
    struct BPDSStringValueSet *svs;

    NewCSD=NULL;
    try
    {
        NewCSD=(struct ColorStreamData *)malloc(sizeof(struct ColorStreamData));
        if(NewCSD==NULL)
            throw(0);

        NewCSD->Def=BPDS_Parse(BPDSDefStr);
        if(NewCSD->Def==NULL)
        {
            throw(0);
        }

        /* Now walk the fields and allocate user data for our use */
        for(Cur=NewCSD->Def->FieldList;Cur!=NULL;Cur=Cur->Next)
        {
            Cur->UserData=malloc(sizeof(struct BPDSParsedData));
            if(Cur->UserData==NULL)
                throw(0);
            memset(Cur->UserData,0x00,sizeof(struct BPDSParsedData));

            for(svs=Cur->StrValues;svs!=NULL;svs=svs->Next)
            {
                svs->UserData=malloc(sizeof(struct BPDSParsedValueSetData));
                if(svs->UserData==NULL)
                    throw(0);
                memset(svs->UserData,0x00,sizeof(struct BPDSParsedValueSetData));
            }
        }

        NewCSD->BigEndian=false;

        ResetColorStream(NewCSD);
    }
    catch(...)
    {
        if(NewCSD!=NULL)
            FreeColorStream(NewCSD);

        NewCSD=NULL;
    }

    return NewCSD;
}

/*******************************************************************************
 * NAME:
 *    FreeColorStream
 *
 * SYNOPSIS:
 *    void FreeColorStream(struct ColorStreamData *CSD);
 *
 * PARAMETERS:
 *    CSD [I] -- The color stream data to free.
 *
 * FUNCTION:
 *    This function frees a color stream data allocated with AllocColorStream()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocColorStream()
 ******************************************************************************/
void FreeColorStream(struct ColorStreamData *CSD)
{
    if(CSD->Def!=NULL)
    {
        BasicHexDecoder_FreeBPDSUserData(CSD->Def);
        BPDS_FreeBPDSDef(CSD->Def);
    }

    free(CSD);
}

/*******************************************************************************
 * NAME:
 *    BasicHexDecoder_FreeBPDSUserData
 *
 * SYNOPSIS:
 *    static void BasicHexDecoder_FreeBPDSUserData(struct BPDSDef *Def);
 *
 * PARAMETERS:
 *    Def [I] -- The BPDS definition to free the user data for
 *
 * FUNCTION:
 *    This function walks the BPDS and frees any user data that was allocated.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void BasicHexDecoder_FreeBPDSUserData(struct BPDSDef *Def)
{
    struct BPDSField *Cur;
    struct BPDSNumberValueSet *nvs;
    struct BPDSStringValueSet *svs;

    for(Cur=Def->FieldList;Cur!=NULL;Cur=Cur->Next)
    {
        if(Cur->UserData!=NULL)
        {
            free(Cur->UserData);
            Cur->UserData=NULL;
        }
        for(nvs=Cur->NumValues;nvs!=NULL;nvs=nvs->Next)
        {
            if(nvs->UserData!=NULL)
            {
                free(nvs->UserData);
                nvs->UserData=NULL;
            }
        }
        for(svs=Cur->StrValues;svs!=NULL;svs=svs->Next)
        {
            if(svs->UserData!=NULL)
            {
                free(svs->UserData);
                svs->UserData=NULL;
            }
        }
    }
}

bool SetColorStreamFieldStyling(struct ColorStreamData *CSD,
        unsigned int FieldNum,struct StyleData *SD)
{
    struct BPDSField *Cur;
    unsigned index;
    struct BPDSParsedData *ReadData;

    /* Find the field they are talking about */
    for(Cur=CSD->Def->FieldList,index=0;Cur!=NULL && index<FieldNum;
            Cur=Cur->Next)
    {
        index++;
    }
    if(Cur==NULL)
    {
        /* Not found */
        return false;
    }

    ReadData=(struct BPDSParsedData *)Cur->UserData;
    ReadData->Style=*SD;

    return true;
}

void SetColorStreamEndian(struct ColorStreamData *CSD,bool Big)
{
    CSD->BigEndian=Big;
}

void ResetColorStream(struct ColorStreamData *CSD)
{
    if(CSD->Def->HadError)
        return;

    CSD->CurField=CSD->Def->FieldList;
    CSD->BytesCount=0;
    CSD->Search4BytesCount=CSD->CurField->Size;
    CSD->CollectedValue=0;
    CSD->MatchAny=true;
    CSD->MarkStartOfField=NULL;

    ResetStream2StartOfProtocol(CSD,false);
}


bool ColorStreamProcessIncomingByte(struct ColorStreamData *CSD,uint8_t Byte,bool DidStyling)
{
    bool Move2NextField;
    struct BPDSNumberValueSet *nvs;
    struct BPDSStringValueSet *svs;
    bool StrPartMatch;
//    bool NumMatch;
    struct BPDSParsedData *ReadData;
    struct BPDSParsedValueSetData *SetReadData;

    if(CSD->Def->HadError)
        return false;

    CSD->CollectedValue=CSD->CollectedValue<<8;
    CSD->CollectedValue|=Byte;

    CSD->NewStyle=false;
    Move2NextField=false;
    CSD->Reset2Start=false;
    StrPartMatch=false;
//    NumMatch=false;

    /* Handle string values */
    for(svs=CSD->CurField->StrValues;svs!=NULL;svs=svs->Next)
    {
        SetReadData=(struct BPDSParsedValueSetData *)svs->UserData;
        if(SetReadData->MatchCount==CSD->BytesCount)
        {
            /* Ok, see if this one matches */
            if(CSD->BytesCount<svs->StrLen)
            {
                if(svs->Str[CSD->BytesCount]==Byte)
                {
                    /* Match */
                    SetReadData->MatchCount++;
                    StrPartMatch=true;
                    if(SetReadData->MatchCount==svs->StrLen)
                    {
                        /* Ok, we found a match */
                        Move2NextField=true;
                    }
                }
                else
                {
                    SetReadData->MatchCount=0;
                }
            }
        }
    }

    if(CSD->CurField->StrValues!=NULL && !StrPartMatch)
    {
        ResetStream2StartOfProtocol(CSD,DidStyling);
        CSD->Reset2Start=true;
    }

    /* Handle number values */
    CSD->BytesCount++;
    if(CSD->CurField->NumValues!=NULL)
    {
        if(CSD->BytesCount==CSD->Search4BytesCount || CSD->MatchAny)
        {
            /* Ok, we have collected enough data */
            CSD->BytesCount=0;

            /* Check if it matches any values */
            for(nvs=CSD->CurField->NumValues;nvs!=NULL;nvs=nvs->Next)
            {
                if(nvs->Value==FixEndianAndClip(CSD,CSD->CollectedValue,
                        CSD->CurField->Size))
                {
                    ReadData=(struct BPDSParsedData *)CSD->CurField->UserData;
                    ReadData->Value=FixEndianAndClip(CSD,CSD->CollectedValue,
                            CSD->CurField->Size);
//                    NumMatch=true;
                    Move2NextField=true;
                    break;
                }
            }
            if(nvs==NULL)
            {
                /* No matching numbers */
                if(!CSD->MatchAny)
                {
                    ResetStream2StartOfProtocol(CSD,DidStyling);
                    CSD->Reset2Start=true;
                }
            }
        }
    }

    if(CSD->CurField->NumValues==NULL && CSD->CurField->StrValues==NULL)
    {
        /* We are not matching a value, so we move on as soon as we get
           the correct number of bytes */
        if(CSD->BytesCount==CSD->Search4BytesCount)
        {
            ReadData=(struct BPDSParsedData *)CSD->CurField->UserData;
            ReadData->Value=FixEndianAndClip(CSD,CSD->CollectedValue,
                    CSD->CurField->Size);
            Move2NextField=true;
        }
    }

    if(Move2NextField)
    {
        if(CSD->CurField==CSD->Def->FieldList)
        {
            /* We are at the start of the packet we need to use the
               marks to back color it (before we move on to a new
               field) */
            CSD->MarkStartOfField=CSD->Def->FieldList;
            CSD->MarkSize=CSD->Def->FieldList->Size;
            if(CSD->CurField->StrValues!=NULL)
                CSD->MarkSize=CSD->BytesCount;
        }
        else if(CSD->MatchAny)
        {
            CSD->MarkStartOfField=CSD->CurField;
            CSD->MarkSize=CSD->Def->FieldList->Size;
        }

        /* Move to next field */
        CSD->CurField=CSD->CurField->Next;
        if(CSD->CurField==NULL)
        {
            CSD->CurField=CSD->Def->FieldList;
            CSD->Reset2Start=true;
        }

//        strcpy(NextName,CurField->Name);
        ReadData=(struct BPDSParsedData *)CSD->CurField->UserData;
        CSD->NextStyle=ReadData->Style;
        CSD->NewStyle=true;
        CSD->MatchAny=false;

        if(CSD->CurField->MatchAny && !CSD->Reset2Start)
        {
            /* Ok, we move to the next field here but set the MatchAny flag */

            /* Apply this style before we change to the next field */
//            strcpy(NextName,CurField->Name);

            CSD->CurField=CSD->CurField->Next;
            if(CSD->CurField==NULL)
            {
                CSD->CurField=CSD->Def->FieldList;
                CSD->Reset2Start=true;
            }

            CSD->MatchAny=true;
        }

        /* Reset the parse matches (for strings) */
        for(svs=CSD->CurField->StrValues;svs!=NULL;svs=svs->Next)
            memset(&svs->UserData,0x00,sizeof(struct BPDSParsedValueSetData));

        /* If we are back at the start, set match any again */
        if(CSD->CurField==CSD->Def->FieldList)
            CSD->MatchAny=true;

        if(CSD->CurField->SizeLinkedTo!=NULL)
        {
            ReadData=(struct BPDSParsedData *)CSD->CurField->SizeLinkedTo->
                    UserData;
            CSD->Search4BytesCount=ReadData->Value;
        }
        else
        {
            CSD->Search4BytesCount=CSD->CurField->Size;
        }

        CSD->BytesCount=0;
        CSD->CollectedValue=0;
    }

    return CSD->NewStyle;
}

void ColorStreamProcessIncomingByteFinish(struct ColorStreamData *CSD,uint8_t Byte,bool DidStyling)
{
    struct BPDSParsedData *ReadData;

    if(CSD->MarkStartOfField!=NULL)
    {
        t_DataProMark *Mark;

        Mark=g_DPS->AllocateMark();
        if(Mark!=NULL)
        {
            g_DPS->MoveMark(Mark,-(CSD->MarkSize));

            ReadData=(struct BPDSParsedData *)CSD->MarkStartOfField->UserData;
            g_DPS->ApplyAttrib2Mark(Mark,ReadData->Style.Attribs,0,CSD->MarkSize);
            g_DPS->ApplyFGColor2Mark(Mark,ReadData->Style.FGColor,0,CSD->MarkSize);
            g_DPS->ApplyBGColor2Mark(Mark,ReadData->Style.BGColor,0,CSD->MarkSize);

            g_DPS->FreeMark(Mark);
        }

        CSD->MarkStartOfField=NULL;
    }

    if(CSD->NewStyle)
    {
        g_DPS->SetFGColor(CSD->NextStyle.FGColor);
        g_DPS->SetULineColor(CSD->NextStyle.FGColor);
        g_DPS->SetBGColor(CSD->NextStyle.BGColor);
        g_DPS->SetAttribs(CSD->NextStyle.Attribs);
        CSD->NewStyle=false;
    }

    if(CSD->Reset2Start)
    {
        ResetStream2StartOfProtocol(CSD,DidStyling);
    }
}

static void ResetStream2StartOfProtocol(struct ColorStreamData *CSD,bool SkipApplyStyle)
{
    struct BPDSStringValueSet *svs;
    struct BPDSParsedData *ReadData;

    /* We aren't moving on and we didn't get a part match on a string
       so we reset and start for the top */
    CSD->MatchAny=true;
    CSD->CurField=CSD->Def->FieldList;
    CSD->BytesCount=0;

    if(!SkipApplyStyle)
    {
        g_DPS->SetFGColor(g_DPS->GetSysDefaultColor(e_DefaultColors_FG));
        g_DPS->SetBGColor(g_DPS->GetSysDefaultColor(e_DefaultColors_BG));
        g_DPS->SetAttribs(0);
        g_DPS->SetULineColor(g_DPS->GetSysDefaultColor(e_DefaultColors_FG));
    }

    if(CSD->CurField->MatchAny)
    {
        /* Ok, we move to the next field here */
        ReadData=(struct BPDSParsedData *)CSD->CurField->UserData;

        if(!SkipApplyStyle)
        {
            g_DPS->SetFGColor(ReadData->Style.FGColor);
            g_DPS->SetULineColor(ReadData->Style.FGColor);
            g_DPS->SetBGColor(ReadData->Style.BGColor);
            g_DPS->SetAttribs(ReadData->Style.Attribs);
        }

        CSD->CurField=CSD->CurField->Next;
        if(CSD->CurField==NULL)
            CSD->CurField=CSD->Def->FieldList;
    }

    /* Reset the parse matches (for strings) */
    for(svs=CSD->CurField->StrValues;svs!=NULL;svs=svs->Next)
        memset(&svs->UserData,0x00,sizeof(struct BPDSParsedValueSetData));
}

static uint64_t FixEndianAndClip(struct ColorStreamData *CSD,uint64_t Value,
        unsigned int Bytes)
{
    uint8_t *SPtr;
    uint8_t *DPtr;
    uint64_t RetValue;
    unsigned int r;

    /* Internally we store numbers as little */
    if(!CSD->BigEndian)
        return Value&((1<<(Bytes*8))-1);

    RetValue=0;

    SPtr=(uint8_t *)&Value;
    DPtr=(uint8_t *)&RetValue;

    SPtr+=Bytes-1;
    for(r=0;r<Bytes;r++)
        *DPtr++=*SPtr--;

    return RetValue;
}
