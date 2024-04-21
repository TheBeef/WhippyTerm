/*******************************************************************************
 * FILENAME: ClipboardHelpers.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the code for the clipboard helpers in it.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (15 Oct 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "ClipboardHelpers.h"
#include "UI/UIPasteData.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static const char *ProcessHexDumpLine(const char *p,uint8_t **DestBuff,unsigned int *BytesOnLine);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    ConvertClipboardData2Format
 *
 * SYNOPSIS:
 *    uint8_t *ConvertClipboardData2Format(std::string &ClipboardData,
 *              e_PasteDataType DataFormat,unsigned int *Bytes);
 *
 * PARAMETERS:
 *    ClipboardData [I] -- A string with the clipboard data to convert in it
 *    DataFormat [I] -- What format do you want to convert the data into.
 *                      e_PasteData_Text -- We basicly just copy the data
 *                          from the 'ClipboardData' into a cstring.
 *                      e_PasteData_HexDump -- Convert hex dump data into
 *                          binary data.  The system will try it's best to
 *                          figure out the format, but will return NULL
 *                          if it can't figure it out.
 *                      e_PasteData_Cancel -- We just return NULL.
 *    Bytes [O] -- The number of bytes in the returned buffer
 *
 * FUNCTION:
 *    This function takes clipboard data and converts it to data that can be
 *    send.  The convertion depends on how it was told to convert the data.
 *
 * RETURNS:
 *    NULL if there was an error or the data could not be converted else
 *    a buffer with the converted data in it.  You must free() this buffer.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
uint8_t *ConvertClipboardData2Format(std::string &ClipboardData,
        e_PasteDataType DataFormat,unsigned int *Bytes)
{
    uint8_t *RetBuff;
    uint8_t *d;
    const char *p;
    const char *StartOfStr;
    int Count;
    uint8_t Number;
    int r;
    unsigned int BytesOnLine;

    *Bytes=0;
    if(DataFormat>=e_PasteData_Cancel)
        return NULL;

    if(DataFormat==e_PasteData_Text)
    {
        /* Basicly a 1:1 copy */
        RetBuff=(uint8_t *)malloc(ClipboardData.length()+1);
        if(RetBuff==NULL)
            return NULL;
        memcpy(RetBuff,ClipboardData.c_str(),ClipboardData.length()+1); // +1 for the \0
        *Bytes=ClipboardData.length()+1;
        return RetBuff;
    }

    if(DataFormat==e_PasteData_HexDump)
    {
        /* Ok, see if we can make sense of the clipboard data */
        StartOfStr=ClipboardData.c_str();
        /* We always assume the hex dump is in hex without any prefix.
           It may have an offset at the start of the line maybe with a char
           spliting the offset from the hex data.
           It may have an AscII dump after the data as well.
           So formats:
               01 02 03 04
               01 02 03 04 ....
               0000: 01 02 03 04
               0000: 01 02 03 04 ....
           or something complex:
               4048 ->   01 02   03 04     ....

            We also support a line of hex digits with no spaces:
            01020304\n
            05060708

        */

        /* See if we are in the squished hex digits */
        p=StartOfStr;
        while(*p!=0)
        {
            if(*p!='\n' && *p!='\r' && !isxdigit(*p))
                break;
            p++;
        }
        if(*p==0)
        {
            /* Ok, it's in this format, but it must be even number of bytes */
            if((p-StartOfStr)&1)
            {
                /* It's odd, we don't know this is */
                return NULL;
            }
            Count=(p-StartOfStr)/2;
            RetBuff=(uint8_t *)malloc(Count);
            if(RetBuff==NULL)
                return NULL;

            p=StartOfStr;
            d=RetBuff;
            for(r=0;r<Count;r++)
            {
                Number=0;

                if(*p>='a' && *p<='f')
                    Number+=(*p)-'a'+10;
                else if(*p>='A' && *p<='F')
                    Number+=(*p)-'A'+10;
                else
                    Number+=(*p)-'0';
                Number<<=4;
                p++;

                if(*p>='a' && *p<='f')
                    Number+=(*p)-'a'+10;
                else if(*p>='A' && *p<='F')
                    Number+=(*p)-'A'+10;
                else
                    Number+=(*p)-'0';
                p++;

                *d++=Number;
            }
            *Bytes=Count;
            return RetBuff;
        }

        /* Count how many bytes this will be */
        p=StartOfStr;
        Count=0;
        while(*p!=0)
        {
            p=ProcessHexDumpLine(p,NULL,&BytesOnLine);
            if(p==NULL)
                return NULL;
            Count+=BytesOnLine;
        }

        if(Count==0)
        {
            /* Ok, we didn't find any data to convert */
            return NULL;
        }

        RetBuff=(uint8_t *)malloc(Count);
        if(RetBuff==NULL)
            return NULL;

        p=StartOfStr;
        d=RetBuff;
        while(*p!=0)
        {
            p=ProcessHexDumpLine(p,&d,&BytesOnLine);
            if(p==NULL)
                return NULL;
        }

        *Bytes=Count;
        return RetBuff;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    ProcessHexDumpLine
 *
 * SYNOPSIS:
 *    static const char *ProcessHexDumpLine(const char *p,uint8_t **DestBuff,
 *          unsigned int *BytesOnLine);
 *
 * PARAMETERS:
 *    p [I] -- The point in the buffer that has the start of the line to process
 *    DestBuff [O] -- A pointer to the pointer of the buffer to store the
 *                    data into.  Ie. char buff[10],*d=buff;xxx(&d);.
 *                    This can be NULL if all you want it to count the bytes.
 *    BytesOnLine [O] -- The number of bytes on this line.
 *
 * FUNCTION:
 *    This function processes a line of hex dump data into bytes.  It will
 *    count the number of binary bytes on this line.  It ignores trailing
 *    AscII and leading offsets.
 *
 * RETURNS:
 *    A pointer to the next line in the buffer, or NULL if there was an error
 *    processing the line.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static const char *ProcessHexDumpLine(const char *p,uint8_t **DestBuff,
        unsigned int *BytesOnLine)
{
    const char *s;
    char c;
    int Count;
    uint8_t Number;

    *BytesOnLine=0;

    /* Skip any non hex digits */
    while(!isxdigit(*p))
        p++;

    /* See if this line starts with an offset */
    s=p;
    while(isxdigit(*p))
        p++;
    if(p-s>2)
    {
        /* We have more than 2 digits, it must be a offset, skip until we find
           a hex dig */
        while(!isxdigit(*p) && *p!=0)
            p++;
        s=p;
    }
    if(*p==0)
    {
        *BytesOnLine=0;
        return p;
    }

    /* Ok, process any hex digits */
    Count=0;
    p=s;
    while(*p!=0)
    {
        if(*(p+1)==0)
        {
            /* Odd number of digits */
            return NULL;
        }
        c=*(p+2);
        if(c!=0 && !isspace(c))
        {
            /* It's not a hex diget so it's likely the AscII */
            while(*p!=0 && *p!='\n')
                p++;
            if(*p=='\n')
                p++;
            *BytesOnLine=Count;
            return p;
        }

        if(DestBuff!=NULL)
        {
            Number=0;

            if(*p>='a' && *p<='f')
                Number+=(*p)-'a'+10;
            else if(*p>='A' && *p<='F')
                Number+=(*p)-'A'+10;
            else
                Number+=(*p)-'0';
            Number<<=4;
            p++;

            if(*p>='a' && *p<='f')
                Number+=(*p)-'a'+10;
            else if(*p>='A' && *p<='F')
                Number+=(*p)-'A'+10;
            else
                Number+=(*p)-'0';
            p++;

            **DestBuff=Number;
            *DestBuff=(*DestBuff)+1;
        }
        else
        {
            /* We are just counting */
            p+=2;
        }
        Count++;

        /* skip any spaces */
        while(isspace(*p))
        {
            if(*p=='\n')
            {
                /* We are done with this line */
                *BytesOnLine=Count;
                return p+1;
            }
            p++;
        }
    }
    *BytesOnLine=Count;
    return p;
}
