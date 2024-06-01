/*******************************************************************************
 * FILENAME: URLHighlighter.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Highlights URLs in the text.
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "URLHighlighter.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct URLHighlighterData
{
    unsigned int CharsInURLSeen;
    bool DoingHighlight;
    bool AttribAlreadySet;
};

/*** FUNCTION PROTOTYPES      ***/
t_DataProcessorHandleType *URLHighlighter_AllocateData(void);
void URLHighlighter_FreeData(t_DataProcessorHandleType *DataHandle);
void URLHighlighter_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);

/*** VARIABLE DEFINITIONS     ***/
struct DataProcessorAPI m_URLHighlighterCBs=
{
    URLHighlighter_AllocateData,
    URLHighlighter_FreeData,
    NULL,
    URLHighlighter_ProcessByte,
};
struct DataProcessorInfo m_URLHighlighter_Info=
{
    "URLHighlighter",
    "URL Highlighter",
    "Underlines URL's starting with http://",
    "Underlines URL's starting with http://",
    &m_URLHighlighterCBs,
    e_DataProcessorType_Text,
    e_TextDataProcessorClass_Highlighter
};

static const struct PI_UIAPI *m_UIAPI;
static const struct PI_SystemAPI *m_System;
static const struct DPS_API *m_DPS;

/*******************************************************************************
 * NAME:
 *    URLHighlighter_RegisterPlugin
 *
 * SYNOPSIS:
 *    void URLHighlighter_RegisterPlugin(const struct PI_SystemAPI *SysAPI);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function registers this plugin with the system.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This function is normally is called from the RegisterPlugin() when
 *    it is being used as a normal plugin.  As a std plugin it is called
 *    from RegisterStdPlugins() instead.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void URLHighlighter_RegisterPlugin(const struct PI_SystemAPI *SysAPI)
{
    m_System=SysAPI;
    m_DPS=SysAPI->GetAPI_DataProcessors();
    m_UIAPI=m_DPS->GetAPI_UI();

    m_DPS->RegisterDataProcessor(&m_URLHighlighter_Info);
}

/*******************************************************************************
 * NAME:
 *    URLHighlighter_AllocateData
 *
 * SYNOPSIS:
 *    t_DataProcessorHandleType *URLHighlighter_AllocateData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates any needed data for this input filter.
 *
 * RETURNS:
 *    A pointer to the data, NULL if there was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_DataProcessorHandleType *URLHighlighter_AllocateData(void)
{
    struct URLHighlighterData *Data;
    Data=(struct URLHighlighterData *)malloc(sizeof(struct URLHighlighterData));
    if(Data==NULL)
        return NULL;

    Data->CharsInURLSeen=0;
    Data->DoingHighlight=false;

    return (t_DataProcessorHandleType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    URLHighlighter_FreeData
 *
 *  SYNOPSIS:
 *    void URLHighlighter_FreeData(t_DataProcessorHandleType *DataHandle);
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to free.  This will need to be
 *                      case to your internal data type before you use it.
 *
 *  FUNCTION:
 *    This function frees the memory allocated with AllocateData().
 *
 *  RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void URLHighlighter_FreeData(t_DataProcessorHandleType *DataHandle)
{
    struct URLHighlighterData *Data=(struct URLHighlighterData *)DataHandle;

    free(Data);
}

/*******************************************************************************
 *  NAME:
 *    URLHighlighter_ProcessByte
 *
 *  SYNOPSIS:
 *    void URLHighlighter_ProcessByte(t_DataProcessorHandleType *DataHandle,
 *          const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
 *          PG_BOOL *Consumed)
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to free.  This will need to be
 *                      case to your internal data type before you use it.
 *    ConnectionID [I] -- The connection to work on
 *    RawByte [I] -- The raw byte to process.  This is the byte that came in.
 *    ProcessedChar [I/O] -- This is a unicode char that has already been
 *                         processed by some of the other input filters.  You
 *                         can change this as you need.  It must remain only
 *                         one unicode char.
 *    CharLen [I/O] -- This number of bytes in 'ProcessedChar'
 *    Consumed [I/O] -- This tells the system (and other filters) if the
 *                      char has been used up and will not be added to the
 *                      screen.
 *    Style [I/O] -- This is the current style that the char will be added
 *                   with.  You can change this to change how the char will
 *                   be added.
 *
 *  FUNCTION:
 *    This function is called for each byte that comes in.
 *
 *  RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void URLHighlighter_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    struct URLHighlighterData *Data=(struct URLHighlighterData *)DataHandle;
    const char *SearchStr="http://";
    unsigned char c;
    unsigned int r;
    unsigned int SearchStrLen;
    uint16_t Attribs;

    /* DEBUG PAUL: This really doesn't work.  This is because this processor
       really needs to work on chars, however control chars are being striped
       (as they should).  So this processor wants to work on chars, but also
       wants to work on char chars.
       In the case of a processor that takes chars that might also include
       control chars (UTF16 maybe?) we need to ignore them when they aren't
       control chars but handle them when they are.... Don't know how....
        */
//    if(*Consumed)
//        return;

    if(Data->DoingHighlight)
    {
        /* Waiting for a invalid char */
        if(*CharLen==1)
        {
            c=ProcessedChar[0];
            if(c<'!' || c>'~')
            {
                /* Char out of valid range */
                Data->CharsInURLSeen=0;
                Data->DoingHighlight=false;

                /* Stop highlighting */
                if(!Data->AttribAlreadySet)
                {
                    Attribs=m_DPS->GetAttribs();
                    Attribs&=~TXT_ATTRIB_UNDERLINE;
                    m_DPS->SetAttribs(Attribs);
                }
            }
        }
    }
    else
    {
        if(*CharLen==1 && ProcessedChar[0]==SearchStr[Data->CharsInURLSeen])
        {
            /* Found the next char */
            Data->CharsInURLSeen++;
            SearchStrLen=strlen(SearchStr);
            if(Data->CharsInURLSeen==SearchStrLen)
            {
                /* Ok, this is a URL */
                Data->DoingHighlight=true;

                Data->AttribAlreadySet=false;
                if(m_DPS->GetAttribs()&TXT_ATTRIB_UNDERLINE)
                    Data->AttribAlreadySet=true;

                m_DPS->SetAttribs(m_DPS->GetAttribs()|TXT_ATTRIB_UNDERLINE);

                /* We now have to back and highlight the http:// part */
                for(r=0;r<SearchStrLen-1;r++)
                    m_DPS->DoBackspace();

                m_DPS->InsertString((uint8_t *)SearchStr,SearchStrLen-1); // -1 because we haven't added the last char yet
            }
        }
        else
        {
            /* Non matching char, start over */
            Data->CharsInURLSeen=0;
        }
    }
}
