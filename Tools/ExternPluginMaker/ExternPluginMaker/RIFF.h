/*******************************************************************************
 * FILENAME: RIFF.h
 * 
 * PROJECT:
 *    Simple RIFF
 *
 * FILE DESCRIPTION:
 *    This is the .h file for the RIFF class.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson
 *
 *    Permission is hereby granted, free of charge, to any person obtaining a
 *    copy of this software and associated documentation files (the "Software"),
 *    to deal in the Software without restriction, including without limitation
 *    the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *    and/or sell copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * HISTORY:
 *    Paul Hutchinson (17 Jul 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __RIFF_H_
#define __RIFF_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <list>
#include <stack>
#include <stdio.h>
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef std::stack<uint32_t> t_RIFFOffsets;
typedef std::stack<uint32_t> t_RIFFSizes;

typedef enum
{
    e_RIFFData_DataBlock,
    e_RIFFData_Group
} e_RIFFDataType;

typedef enum
{
    e_RIFFOpen_Read,
    e_RIFFOpen_Write
} e_RIFFOpenType;

enum e_RIFFErrorType
{
    e_RIFFError_None,
    e_RIFFError_InternalError_BadChunkIDSize,
    e_RIFFError_FailedToOpenFile,
    e_RIFFError_WriteError,
    e_RIFFError_ReadError,
    e_RIFFError_FileNotOpen,
    e_RIFFError_ChunkToSmall,
    e_RIFFError_BadFormat,
    e_RIFFError_FormTypeMismatch,
    e_RIFFError_UnsupportedFileMode
};

/***  CLASS DEFINITIONS                ***/
struct ListHistory
{
    uint32_t BlockSize;
    uint32_t ReadFromBlock;
    uint32_t DataLeft;
};

typedef std::stack<struct ListHistory> t_RIFFHistory;

class RIFF
{
    public:
        /* Misc */
        RIFF(bool UseIFF=false);
        ~RIFF();
        void SetIFFMode(bool UseIFF);
        static const char *Error2Str(e_RIFFErrorType err);

        /* File control */
        void Open(const char *Filename,e_RIFFOpenType Mode,const char *FormType);
        void Close(void);

        /* Write methods */
        void StartDataBlock(const char *ChunkID);
        void EndDataBlock(void);
        void Write(const void *Data,uint32_t Bytes);
        void StartGroup(const char *ListID);
        void EndGroup(void);

        /* Read methods */
        void Read(void *Data,uint32_t Bytes);
        bool ReadNextDataBlock(char *ChunkID,uint32_t *ChunkSize);
        bool ReadNextDataBlock(char *ChunkID,uint32_t *ChunkSize,e_RIFFDataType &DataType);
        void StartReadingGroup(void);
        void DoneReadingGroup(void);

    private:
        FILE *FileHandle;
        uint32_t CurrentChunkSize;
        uint32_t TotalFileSize;
        bool Opened4Read;
        bool IFFMode;

        /* Write */
        uint32_t SavedChuckOffset;
        uint32_t SavedChuckSize;
        uint32_t CurrentListSize;
        t_RIFFOffsets ListOffsets;
        t_RIFFSizes ListSizes;

        /* Read */
        uint32_t BlockSize;
        uint32_t ReadFromBlock;
        uint32_t DataLeft;
        t_RIFFHistory PrevPos;

        void OpenWrite(const char *Filename,const char *FormType);
        void CloseWrite(void);
        void OpenRead(const char *Filename,const char *FormType);
        void CloseRead(void);
        uint32_t ReadSize(void);
        void WriteSize(uint32_t Size);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
