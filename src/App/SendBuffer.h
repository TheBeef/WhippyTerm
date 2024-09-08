/*******************************************************************************
 * FILENAME: SendBuffer.h
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
 * HISTORY:
 *    Paul Hutchinson (17 Jul 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __SENDBUFFER_H_
#define __SENDBUFFER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Connections.h"
#include <stdint.h>

/***  DEFINES                          ***/
#define MAX_SEND_BUFFERS            (12+26)  // We support up to 38 buffers (12 Fn keys, and a-z)
#define MAX_QUICK_SEND_BUFFERS      12  // The number of buffers that are used for the quick send (12 because of the number of function keys we have).  Must be smaller than 'MAX_SEND_BUFFERS'

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class SendBuffer
{
    public:
        SendBuffer();
        ~SendBuffer();
        bool LoadBuffers(const char *Filename=nullptr);
        bool SaveBuffers(const char *Filename=nullptr);
        static bool SaveBuffer2File(const char *Filename,const char *BufferName,
                const uint8_t *Data,uint32_t DataSize);
        static bool LoadBufferFromFile(const char *Filename,char **BufferName,
                uint8_t **Data,uint32_t *DataSize);
        void ClearAllBuffers(void);

        void SetBuffer(int BufferIndex,const uint8_t *Memory,uint32_t BSize);
        void SetBufferName(int BufferIndex,const char *NewName);
        char *GetBufferName(int BufferIndex);
        bool GetBufferInfo(int BufferIndex,const uint8_t **Memory,
                uint32_t *BSize);

        bool Send(class Connection *Con,int BufferIndex);

    private:
        uint8_t *Buffer[MAX_SEND_BUFFERS];
        uint32_t BufferSize[MAX_SEND_BUFFERS];
        char *BufferName[MAX_SEND_BUFFERS];

        static void SaveBuffer2WTSBFile(class RIFF *Save,
                struct BufferInfo *BInfo,const char *BufferName,
                const uint8_t *Data,uint32_t DataSize);
        static void LoadBufferFromWTSBFile(class RIFF *Load,
                struct LoadBufferDiskData *LoadedDiskData);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern class SendBuffer g_SendBuffers;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
