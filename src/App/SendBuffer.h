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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (17 Jul 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __SENDBUFFER_H_
#define __SENDBUFFER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>

/***  DEFINES                          ***/
#define MAX_SEND_BUFFERS    12          // We support up to 12 buffers (12 because there are 12 function keys)

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

        void SetBuffer(int BufferIndex,const uint8_t *Memory,uint32_t BSize);
        void SetBufferName(int BufferIndex,const char *NewName);
        char *GetBufferName(int BufferIndex);
        bool GetBufferInfo(int BufferIndex,const uint8_t **Memory,
                uint32_t *BSize);

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
