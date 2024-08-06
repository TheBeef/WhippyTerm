/*******************************************************************************
 * FILENAME: Form_DebugPrintLogAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has debug function for a printf() style log.
 *
 * COPYRIGHT:
 *    Copyright 05 Aug 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (05 Aug 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIDebug.h"
#include "Form_DebugPrintLog.h"
#include <stdarg.h>
#include <stdlib.h>

/*** DEFINES                  ***/
#define BUFFER_GROW_SIZE            1000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_DebugPrintLog *m_DebugPrintfForm;

void DB_printf(const char *fmt,...)
{
    static char *Buffer;
    static int BufferSize;
    va_list args;
    int Len;

    try
    {
        if(m_DebugPrintfForm==NULL)
            m_DebugPrintfForm=new Form_DebugPrintLog(NULL);

        if(Buffer==NULL)
        {
            Buffer=(char *)malloc(BUFFER_GROW_SIZE);
            if(Buffer==NULL)
                return;
            BufferSize=BUFFER_GROW_SIZE;
        }

        va_start(args,fmt);
        Len=vsnprintf(Buffer,BufferSize,fmt,args);
        if(Len>BufferSize)
        {
            Buffer=(char *)realloc(Buffer,BUFFER_GROW_SIZE+Len);
            if(Buffer==NULL)
                return;
            vsnprintf(Buffer,BufferSize,fmt,args);
        }
        va_end(args);

        m_DebugPrintfForm->AddText(Buffer);
    }
    catch(...)
    {
    }
}

void DB_ShowDebugPrintLogForm(void)
{
    try
    {
        if(m_DebugPrintfForm==NULL)
            m_DebugPrintfForm=new Form_DebugPrintLog(NULL);

        m_DebugPrintfForm->show();
    }
    catch(...)
    {
    }
}

void DB_printf_clear(void)
{
    m_DebugPrintfForm->ClearText();
}
