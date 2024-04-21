/*******************************************************************************
 * FILENAME: RIFF.cpp
 *
 * PROJECT:
 *    Simple RIFF
 *
 * FILE DESCRIPTION:
 *    This has a class to read / write simple RIFF / IFF files.
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
 * CREATED BY:
 *    Paul Hutchinson (17 Jul 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "RIFF.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RIFF::RIFF
 *
 * SYNOPSIS:
 *    RIFF::RIFF(bool UseIFF=false);
 *
 * PARAMETERS:
 *    UseIFF [I] -- If true then use IFF mode, else use RIFF.
 *
 * FUNCTION:
 *    This is the constructor for this class.
 *
 *    The system can work with IFF or RIFF files.  IFF is the orginal
 *    Electronic Arts 85, RIFF is the Microsoft 91 version of IFF.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This class can throw the following errors:
 *      e_RIFFError_None -- Class don't actually throw this
 *      e_RIFFError_InternalError_BadChunkIDSize -- The chunk / form type
 *              that was passed in is not valid.  It needs to be 4 byte string.
 *      e_RIFFError_FailedToOpenFile -- fopen() failed.
 *      e_RIFFError_WriteError -- We had an error writing to the file
 *      e_RIFFError_ReadError -- We had an error reading from the file
 *      e_RIFFError_FileNotOpen -- You didn't open the file before trying
 *              to read/write to it.
 *      e_RIFFError_ChunkToSmall -- You tried to read more data from a chunk
 *              than there are in the chunk.
 *      e_RIFFError_BadFormat -- File is not a supported RIFF or IFF file.
 *      e_RIFFError_FormTypeMismatch -- The form type of this file does not
 *              match the type you sent into Open()
 *      e_RIFFError_UnsupportedFileMod -- You tried to open a file with a mode
 *              that this class does not support.
 *
 * LIMITATIONS:
 *    Does not support all the IFF / RIFF spec.  It does not support
 *    CAT or LIST
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
RIFF::RIFF(bool UseIFF)
{
    IFFMode=UseIFF;
}

/*******************************************************************************
 * NAME:
 *    RIFF::~RIFF
 *
 * SYNOPSIS:
 *    RIFF::~RIFF();
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This is the deconstructure for this class.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
RIFF::~RIFF()
{
    if(FileHandle!=NULL)
        fclose(FileHandle);
}

/*******************************************************************************
 * NAME:
 *    RIFF::Error2Str
 *
 * SYNOPSIS:
 *    const char *RIFF::Error2Str(e_RIFFErrorType err);
 *
 * PARAMETERS:
 *    err [I] -- The error code to convert to a string.
 *
 * FUNCTION:
 *    This function converts one of the error codes that can be thrown to
 *    a string.
 *
 * RETURNS:
 *    A static string with the text for this error code in it.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const char *RIFF::Error2Str(e_RIFFErrorType err)
{
    switch(err)
    {
        case e_RIFFError_None:
            return "Internal Error.  Code doesn't think there is an error";
        case e_RIFFError_InternalError_BadChunkIDSize:
            return "Internal Error.  Chunk must be 4 bytes";
        case e_RIFFError_FailedToOpenFile:
            return "Failed to open file";
        case e_RIFFError_WriteError:
            return "Write error";
        case e_RIFFError_ReadError:
            return "Read error";
        case e_RIFFError_FileNotOpen:
            return "File not open";
        case e_RIFFError_ChunkToSmall:
            return "Chunk to small";
        case e_RIFFError_BadFormat:
            return "Invalid RIFF / IFF";
        case e_RIFFError_FormTypeMismatch:
            return "The form type in the file did not match the requested form type";
        case e_RIFFError_UnsupportedFileMode:
            return "Open() was called with an invalid 'Mode'";
    }
    return "Internal Error.  Can not convert error code to text";
}

/*******************************************************************************
 * NAME:
 *    RIFF::SetIFFMode
 *
 * SYNOPSIS:
 *    void RIFF::SetIFFMode(bool UseIFF);
 *
 * PARAMETERS:
 *    UseIFF [I] -- If true then use IFF mode, else use RIFF.
 *
 * FUNCTION:
 *    This function changes the mode we are working from IFF to RIFF or back.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::SetIFFMode(bool UseIFF)
{
    IFFMode=UseIFF;
}

/*******************************************************************************
 * NAME:
 *    RIFF::Open
 *
 * SYNOPSIS:
 *    void RIFF::Open(const char *Filename,e_RIFFOpenType Mode,
 *          const char *FormType);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to open for read/write
 *    Mode [I] -- Open for read or writing a file.  Supported values:
 *                  e_RIFFOpen_Read -- Read an existing RIFF file.
 *                  e_RIFFOpen_Write -- Write a new RIFF file.
 *    FormType [I] -- The type of the file we are going to be opening / creating
 *
 * FUNCTION:
 *    This function opens an RIFF file for loading or saving.  If the file
 *    does not exist a new one will be made if you are saving.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::Open(const char *Filename,e_RIFFOpenType Mode,const char *FormType)
{
    if(Mode==e_RIFFOpen_Write)
        OpenWrite(Filename,FormType);
    else if(Mode==e_RIFFOpen_Read)
        OpenRead(Filename,FormType);
    else
        throw(e_RIFFError_UnsupportedFileMode);
}

/*******************************************************************************
 * NAME:
 *    RIFF::Close
 *
 * SYNOPSIS:
 *    void RIFF::Close(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function closes the file opened with Open().  It will finalize
 *    the file if opened for writing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::Close(void)
{
    if(Opened4Read)
        CloseRead();
    else
        CloseWrite();
}

/*******************************************************************************
 * NAME:
 *    RIFF::OpenRead
 *
 * SYNOPSIS:
 *    void RIFF::OpenRead(const char *Filename,const char *FormType);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to open.
 *    FormType [I] -- Form type of this file.  This will be checked against
 *                    the form type in the file.
 *
 * FUNCTION:
 *    This function opens a file for reading and does some basic checking of
 *    the file.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::OpenRead(const char *Filename,const char *FormType)
{
    char ChunkID[5];
    const char *FORMName;

    ChunkID[4]=0;

    FORMName="RIFF";
    if(IFFMode)
        FORMName="FORM";

    FileHandle=fopen(Filename,"rb");
    if(FileHandle==NULL)
        throw(e_RIFFError_FailedToOpenFile);

    if(fread(ChunkID,4,1,FileHandle)!=1)
    {
        fclose(FileHandle);
        FileHandle=NULL;
        throw(e_RIFFError_ReadError);
    }

    if(strcmp(ChunkID,FORMName)!=0)
    {
        fclose(FileHandle);
        FileHandle=NULL;
        throw(e_RIFFError_BadFormat);
    }

    TotalFileSize=ReadSize();

    if(fread(ChunkID,4,1,FileHandle)!=1)
    {
        fclose(FileHandle);
        FileHandle=NULL;
        throw(e_RIFFError_ReadError);
    }

    if(strcmp(ChunkID,FormType)!=0)
    {
        fclose(FileHandle);
        FileHandle=NULL;
        throw(e_RIFFError_FormTypeMismatch);
    }

    Opened4Read=true;

    BlockSize=TotalFileSize-4;   // Total file size - the FormType
    DataLeft=0;
    ReadFromBlock=0;
}

/*******************************************************************************
 * NAME:
 *    RIFF::CloseRead
 *
 * SYNOPSIS:
 *    void RIFF::CloseRead(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This closes a file opened with OpenRead().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::CloseRead(void)
{
    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    fclose(FileHandle);
    FileHandle=NULL;
}

/*******************************************************************************
 * NAME:
 *    RIFF::ReadNextDataBlock
 *
 * SYNOPSIS:
 *    bool RIFF::ReadNextDataBlock(char *ChunkID,uint32_t *ChunkSize);
 *    bool RIFF::ReadNextDataBlock(char *ChunkID,uint32_t *ChunkSize,
 *              e_RIFFDataType &DataType);
 *
 * PARAMETERS:
 *    ChunkID [O] -- A buffer at least 5 bytes in size.  This is where
 *                   the name of the next chunk we find the file is copyed.
 *                   A '\0' is added so you can do a strcmp() on it.
 *    ChunkSize [O] -- The number of bytes in the chunk.
 *    DataType [O] -- The type of block we just read.
 *                      e_RIFFData_DataBlock -- A normal chunk type
 *                      e_RIFFData_Group -- It a group.
 *
 * FUNCTION:
 *    This function finds and reads the next data block (chunk) in the file
 *    opened for reading.
 *
 *    If you want to skip the returned chunk just call ReadNextDataBlock()
 *    again.
 *
 * RETURNS:
 *    true -- Another chunk was found.  'ChunkID' and 'ChunkSize' have
 *            been set.
 *    false -- We are at the end of the file.  No chunk found.
 *
 * EXAMPLE:
 *    char Chunk[5];
 *    uint32_t Size;
 *
 *    while(RIFF.ReadNextDataBlock(Chunk,&Size))
 *    {
 *        if(strcmp(Chunk,"fmt ")==0)
 *            RIFF.Read(&WaveFormatHeader,sizeof(WaveFormatHeader));
 *        else if(strcmp(Chunk,"AUTH")==0)
 *            RIFF.Read(&Author,sizeof(Author));
 *        // More strcmp's here for more chunks we support
 *        RIFF.FinishReadChunk();
 *    }
 *
 * SEE ALSO:
 *    FinishReadChunk()
 ******************************************************************************/
bool RIFF::ReadNextDataBlock(char *ChunkID,uint32_t *ChunkSize,e_RIFFDataType &DataType)
{
    ChunkID[4]=0;

    DataType=e_RIFFData_DataBlock;

    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    if(DataLeft>0)
    {
        fseek(FileHandle,DataLeft,SEEK_CUR);
        ReadFromBlock+=DataLeft;
    }

    /* See if we are at the end of the file */
    if(ReadFromBlock>=BlockSize)
        return false;

    if(fread(ChunkID,4,1,FileHandle)!=1)
        throw(e_RIFFError_ReadError);
    ReadFromBlock+=4;

    CurrentChunkSize=ReadSize();
    ReadFromBlock+=4;

    *ChunkSize=CurrentChunkSize;

    if(CurrentChunkSize&1)
    {
        /* Add in the padding 0 */
        CurrentChunkSize++;
    }

    DataLeft=CurrentChunkSize;

    if(strcmp(ChunkID,"LIST")==0)
    {
        DataType=e_RIFFData_DataBlock;

        if(fread(ChunkID,4,1,FileHandle)!=1)
            throw(e_RIFFError_ReadError);
        ReadFromBlock+=4;

        DataLeft-=4;
    }

    return true;
}
bool RIFF::ReadNextDataBlock(char *ChunkID,uint32_t *ChunkSize)
{
    e_RIFFDataType DataType;

    return ReadNextDataBlock(ChunkID,ChunkSize,DataType);
}

/*******************************************************************************
 * NAME:
 *    RIFF::StartReadingGroup
 *
 * SYNOPSIS:
 *    void RIFF::StartReadingGroup(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function needs to be called when you are about to start reading
 *    in a group.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    RIFF::DoneReadingGroup()
 ******************************************************************************/
void RIFF::StartReadingGroup(void)
{
    struct ListHistory Old;

    Old.BlockSize=BlockSize;
    Old.ReadFromBlock=ReadFromBlock;
    Old.DataLeft=DataLeft;  //??

    PrevPos.push(Old);

    BlockSize=CurrentChunkSize-4;   // We have already removed 'type'
    ReadFromBlock=0;
    DataLeft=0;
}

/*******************************************************************************
 * NAME:
 *    RIFF::DoneReadingGroup
 *
 * SYNOPSIS:
 *    void RIFF::DoneReadingGroup(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called after you are done reading in a group.
 *    It moves up 1 group.  Any unread data / chunks will be skipped.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    RIFF::StartReadingGroup()
 ******************************************************************************/
void RIFF::DoneReadingGroup(void)
{
    struct ListHistory Old;
    uint32_t BytesInBlockLeft;

    /* Ok, we need to skip whatever wasn't read */
    BytesInBlockLeft=BlockSize-ReadFromBlock;
    if(BytesInBlockLeft>0)
    {
        fseek(FileHandle,BytesInBlockLeft,SEEK_CUR);
        ReadFromBlock+=BytesInBlockLeft;

        if(BlockSize&1)
        {
            /* We need to skip the padding byte */
            fseek(FileHandle,1,SEEK_CUR);
            ReadFromBlock+=1;
        }
    }

    Old=PrevPos.top();
    PrevPos.pop();

    BlockSize=Old.BlockSize;
    ReadFromBlock+=Old.ReadFromBlock;
    DataLeft=0;
}

/*******************************************************************************
 * NAME:
 *    RIFF::Read
 *
 * SYNOPSIS:
 *    void RIFF::Read(void *Data,uint32_t Bytes);
 *
 * PARAMETERS:
 *    Data [I] -- The buffer to read the data from the file into
 *    Bytes [I] -- The number of bytes to read from the file.
 *
 * FUNCTION:
 *    This function reads bytes from the file opened with Open().  It will
 *    read from the current chunk only.  If there isn't enough bytes left in
 *    the chunk to full fill this request then a 'e_RIFFError_ChunkToSmall'
 *    exception is thrown.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::Read(void *Data,uint32_t Bytes)
{
    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    if(Bytes>DataLeft)
        throw(e_RIFFError_ChunkToSmall);

    if(fread(Data,Bytes,1,FileHandle)!=1)
        throw(e_RIFFError_ReadError);

    DataLeft-=Bytes;
    ReadFromBlock+=Bytes;
}

/*******************************************************************************
 * NAME:
 *    RIFF::OpenWrite
 *
 * SYNOPSIS:
 *    void RIFF::OpenWrite(const char *Filename,const char *FormType);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to open.
 *    FormType [I] -- Form type of this file.
 *
 * FUNCTION:
 *    This function will create a new file and add the headers to the open of
 *    it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::OpenWrite(const char *Filename,const char *FormType)
{
    const char *FORMName;

    if(strlen(FormType)!=4)
        throw(e_RIFFError_InternalError_BadChunkIDSize);

    FileHandle=fopen(Filename,"wb");
    if(FileHandle==NULL)
        throw(e_RIFFError_FailedToOpenFile);

    FORMName="RIFF";
    if(IFFMode)
        FORMName="FORM";
    fwrite(FORMName,4,1,FileHandle);
    TotalFileSize=0;
    WriteSize(TotalFileSize);
    if(fwrite(FormType,4,1,FileHandle)!=1)
        throw(e_RIFFError_WriteError);

    TotalFileSize=4;    // The FormType counts
    CurrentChunkSize=0;
    CurrentListSize=0;

    Opened4Read=false;
}

/*******************************************************************************
 * NAME:
 *    RIFF::CloseWrite
 *
 * SYNOPSIS:
 *    void RIFF::CloseWrite(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function closes the file opened with OpenWrite().  It finalizes
 *    the file.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::CloseWrite(void)
{
    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    fseek(FileHandle,4,SEEK_SET);
    WriteSize(TotalFileSize);

    fclose(FileHandle);
    FileHandle=NULL;
}

/*******************************************************************************
 * NAME:
 *    RIFF::StartDataBlock
 *
 * SYNOPSIS:
 *    void RIFF::StartDataBlock(const char *ChunkID);
 *
 * PARAMETERS:
 *    ChunkID [I] -- The ID of the data block to start. This is an identifier
 *                   used to find this data when reading the file back in.
 *                   This must be 4 letters long and be in the AscII
 *                   range of [a-z][A-Z][0-9] and space.  Only the length is
 *                   checked.
 *
 * FUNCTION:
 *    This function starts a new block of data when writing a file.  This
 *    will work out to an IFF chunk.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::StartDataBlock(const char *ChunkID)
{
    uint32_t ChunkSize;
    uint32_t ChunkStart;

    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    if(strlen(ChunkID)!=4)
        throw(e_RIFFError_InternalError_BadChunkIDSize);

    fwrite(ChunkID,4,1,FileHandle);

    ChunkStart=ftell(FileHandle);
    ChunkSize=0;
    WriteSize(ChunkSize);

    SavedChuckOffset=ChunkStart;
    SavedChuckSize=CurrentChunkSize;

    CurrentChunkSize=0;
    TotalFileSize+=4+4;
    if(!ListSizes.empty())
        CurrentListSize+=4+4;
}

/*******************************************************************************
 * NAME:
 *    RIFF::EndDataBlock
 *
 * SYNOPSIS:
 *    void RIFF::EndDataBlock(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    When writing a file this function closes the data block opened with
 *    StartDataBlock().  It will write the size of the data to the correct
 *    place in the file.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::EndDataBlock(void)
{
    uint32_t ChunkOffset;
    uint32_t ClosingChunkSize;
    uint32_t TotalByteSizeOfChunk;

    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    ClosingChunkSize=CurrentChunkSize;
    TotalByteSizeOfChunk=ClosingChunkSize;

    ChunkOffset=SavedChuckOffset;

    /* Write the size of the closeing chunk */
    fseek(FileHandle,ChunkOffset,SEEK_SET);
    WriteSize(ClosingChunkSize);
    fseek(FileHandle,0,SEEK_END);

    /* Pad if needed */
    if(ClosingChunkSize&1)
    {
        uint8_t ZeroPadding;

        ZeroPadding=0;
        fwrite(&ZeroPadding,1,1,FileHandle);
        TotalFileSize++;
        TotalByteSizeOfChunk++; // Add to the amount of the new chunk
        if(!ListSizes.empty())
            CurrentListSize+=1;
    }

    CurrentChunkSize=SavedChuckSize;
}

/*******************************************************************************
 * NAME:
 *    RIFF::Write
 *
 * SYNOPSIS:
 *    void RIFF::Write(const void *Data,uint32_t Bytes);
 *
 * PARAMETERS:
 *    Data [I] -- The binary data to write to the open chunk.
 *    Bytes [I] -- The number of bytes to write to the chunk.
 *
 * FUNCTION:
 *    This function writes bytes to the open chunk.  You can call this as
 *    many times as you would like inside the chunk and it will append the
 *    data to the open chunk.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::Write(const void *Data,uint32_t Bytes)
{
    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    if(Bytes>0)
        if(fwrite(Data,Bytes,1,FileHandle)!=1)
            throw(e_RIFFError_WriteError);

    TotalFileSize+=Bytes;
    CurrentChunkSize+=Bytes;
    if(!ListSizes.empty())
        CurrentListSize+=Bytes;
}

/*******************************************************************************
 * NAME:
 *    RIFF::StartGroup
 *
 * SYNOPSIS:
 *    void RIFF::StartGroup(const char *ListID);
 *
 * PARAMETERS:
 *    ListID [I] -- The ID of the group to start.  This is the ID used to
 *                  identify this group of data blocks when reading.
 *                  This must be 4 letters long and be in the AscII range
 *                  of [a-z][A-Z][0-9] and space.  Only the length is checked.
 *
 * FUNCTION:
 *    This function starts group of data blocks.  Groups let you put data
 *    blocks inside of other data blocks.  You can place groups inside of
 *    other groups.
 *
 *    This works out to an IFF LIST chunk.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    You must call EndGroup() to end the group.
 *
 * SEE ALSO:
 *    EndGroup()
 ******************************************************************************/
void RIFF::StartGroup(const char *ListID)
{
    uint32_t ListSize;
    uint32_t ListStart;

    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    if(strlen(ListID)!=4)
        throw(e_RIFFError_InternalError_BadChunkIDSize);

    fwrite("LIST",4,1,FileHandle);
    ListStart=ftell(FileHandle);
    ListSize=0;
    WriteSize(ListSize);
    fwrite(ListID,4,1,FileHandle);

    ListOffsets.push(ListStart);
    ListSizes.push(CurrentListSize);

    CurrentListSize=0;          // Reset the current list block size

    CurrentListSize+=4;         // Add in ListID
    TotalFileSize+=4+4+4;       // Add in 'LIST' + Size + ListID
}

/*******************************************************************************
 * NAME:
 *    RIFF::EndGroup
 *
 * SYNOPSIS:
 *    void RIFF::EndGroup(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This ends the current group started with StartGroup().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    StartGroup()
 ******************************************************************************/
void RIFF::EndGroup(void)
{
    uint32_t ListOffset;
    uint32_t ClosingListSize;

    if(FileHandle==NULL)
        throw(e_RIFFError_FileNotOpen);

    ClosingListSize=CurrentListSize;

    ListOffset=ListOffsets.top();
    ListOffsets.pop();

    /* Write the size of the closeing chunk */
    fseek(FileHandle,ListOffset,SEEK_SET);
    WriteSize(ClosingListSize);
    fseek(FileHandle,0,SEEK_END);

    /* Pad if needed */
    if(ClosingListSize&1)
    {
        uint8_t ZeroPadding;

        ZeroPadding=0;
        fwrite(&ZeroPadding,1,1,FileHandle);
        TotalFileSize++;
        CurrentListSize++;  // Add this to the size of the current list
    }

    /* Move back up 1 level in the list */
    /* Pop where we where then add in the block we just wrote, and add in
       'LIST' and size */
    CurrentListSize=ListSizes.top()+CurrentListSize+4+4;
    ListSizes.pop();
}

/*******************************************************************************
 * NAME:
 *    RIFF::ReadSize
 *
 * SYNOPSIS:
 *    uint32_t RIFF::ReadSize(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function reads a 32bit var from the file and converts it
 *    from IFF or RIFF as needed.
 *
 * RETURNS:
 *    The read value.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
uint32_t RIFF::ReadSize(void)
{
    uint8_t SizeBuff[4];
    uint32_t Var;

    if(fread(SizeBuff,4,1,FileHandle)!=1)
    {
        fclose(FileHandle);
        FileHandle=NULL;
        throw(e_RIFFError_ReadError);
    }

    if(IFFMode)
    {
        Var =SizeBuff[3]<<0;
        Var|=SizeBuff[2]<<8;
        Var|=SizeBuff[1]<<16;
        Var|=SizeBuff[0]<<24;
    }
    else
    {
        Var =SizeBuff[0]<<0;
        Var|=SizeBuff[1]<<8;
        Var|=SizeBuff[2]<<16;
        Var|=SizeBuff[3]<<24;
    }

    return Var;
}

/*******************************************************************************
 * NAME:
 *    RIFF::WriteSize
 *
 * SYNOPSIS:
 *    void RIFF::WriteSize(uint32_t Size);
 *
 * PARAMETERS:
 *    Size [I] -- The size to write out
 *
 * FUNCTION:
 *    This function writes a 32bit size out to the file converting for IFF and
 *    RIFF as needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RIFF::WriteSize(uint32_t Size)
{
    uint8_t SizeBuff[4];

    if(IFFMode)
    {
        SizeBuff[0]=(Size>>24)&0xFF;
        SizeBuff[1]=(Size>>16)&0xFF;
        SizeBuff[2]=(Size>>8)&0xFF;
        SizeBuff[3]=(Size>>0)&0xFF;
    }
    else
    {
        SizeBuff[0]=(Size>>0)&0xFF;
        SizeBuff[1]=(Size>>8)&0xFF;
        SizeBuff[2]=(Size>>16)&0xFF;
        SizeBuff[3]=(Size>>24)&0xFF;
    }

    if(fwrite(SizeBuff,4,1,FileHandle)!=1)
    {
        fclose(FileHandle);
        FileHandle=NULL;
        throw(e_RIFFError_WriteError);
    }
}
