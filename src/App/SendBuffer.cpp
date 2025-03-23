/*******************************************************************************
 * FILENAME: SendBuffer.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the send buffer system in it.  A send buffer allocates
 *    'MAX_SEND_BUFFERS' buffer that can be sent out a connection.
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
 *    Paul Hutchinson (17 Jul 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/SendBuffer.h"
#include "App/MainWindow.h"
#include "ThirdParty/SimpleRIFF/RIFF.h"
#include "UI/UIAsk.h"
#include "OS/FilePaths.h"
#include "OS/Directorys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/
#define BUFFERS_FILE        "Buffers.dat"

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
/* Disk structure */
struct BufferInfo
{
    uint8_t BufferIndex;
    uint8_t Padding;
};

struct LoadBufferDiskData
{
    uint8_t *Data;
    uint32_t DataSize;
    struct BufferInfo BInfo;
    char *BufferName;
    bool DataFound;
    bool BInfoFound;
    bool NameFound;
};

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class SendBuffer g_SendBuffers;

SendBuffer::SendBuffer()
{
    int r;

    for(r=0;r<MAX_SEND_BUFFERS;r++)
    {
        Buffer[r]=NULL;
        BufferSize[r]=0;
        BufferName[r]=NULL;
    }

    ClearAllBuffers();
}

SendBuffer::~SendBuffer()
{
    int r;

    for(r=0;r<MAX_SEND_BUFFERS;r++)
    {
        if(Buffer[r]!=NULL)
            free(Buffer[r]);
        if(BufferName[r]!=NULL)
            free(BufferName[r]);
    }
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::LoadBuffers
 *
 * SYNOPSIS:
 *    bool SendBuffer::LoadBuffers(const char *Filename=NULL);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to load.  If NULL use the default in the
 *                    WhippyTerm settings directory.
 *
 * FUNCTION:
 *    This function goes to the disk and loads all the buffers.  All existing
 *    buffers are freed before the load.
 *
 * RETURNS:
 *    true -- Load worked
 *    false -- There was an error
 *
 * SEE ALSO:
 *    SaveBuffers()
 ******************************************************************************/
bool SendBuffer::LoadBuffers(const char *Filename)
{
    class RIFF Load;
    char ChunkID[5];
    uint32_t ChunkSize;
    string LoadFilename;
    struct BufferInfo BInfo;
    bool BufferInfoFound;
    uint8_t *Memory;
    uint32_t MemorySize;
    int r;
    bool FileOpen;
    bool RetValue;
    char *NewName;

    FileOpen=false;
    NewName=NULL;
    Memory=NULL;
    try
    {
        if(Filename==NULL)
        {
            if(GetAppDataPath(LoadFilename)==false)
                throw("Failed to get load path");
            LoadFilename+=BUFFERS_FILE;
        }
        else
        {
            LoadFilename=Filename;
        }

        /* Free any existing buffers */
        for(r=0;r<MAX_SEND_BUFFERS;r++)
        {
            free(Buffer[r]);
            free(BufferName[r]);
            Buffer[r]=NULL;
            BufferSize[r]=0;
            BufferName[r]=NULL;
        }

        Load.SetIFFMode(true);
        Load.Open(LoadFilename.c_str(),e_RIFFOpen_Read,"WTSB");
        while(Load.ReadNextDataBlock(ChunkID,&ChunkSize))
        {
            if(strcmp(ChunkID,"BUFF")==0)
            {
                BufferInfoFound=false;
                Memory=NULL;

                Load.StartReadingGroup();
                while(Load.ReadNextDataBlock(ChunkID,&ChunkSize))
                {
                    if(strcmp(ChunkID,"INFO")==0)
                    {
                        if(ChunkSize>=sizeof(BInfo))
                        {
                            Load.Read(&BInfo,sizeof(BInfo));
                            BufferInfoFound=true;
                        }
                    }
                    if(strcmp(ChunkID,"NAME")==0)
                    {
                        NewName=(char *)malloc(ChunkSize+1);
                        if(NewName==NULL)
                            throw("Out of memory for buffer name");
                        Load.Read(NewName,ChunkSize);
                        NewName[ChunkSize]=0;
                    }
                    else if(strcmp(ChunkID,"BIN ")==0)
                    {
                        Memory=(uint8_t *)malloc(ChunkSize);
                        if(Memory==NULL)
                            throw("Out of memory");
                        Load.Read(Memory,ChunkSize);
                        MemorySize=ChunkSize;
                    }
                }
                if(Memory==NULL || !BufferInfoFound)
                    throw("Can't load buffer.  Missing data");

                if(BInfo.BufferIndex>=MAX_SEND_BUFFERS)
                {
                    /* Can't load this because we don't support this many
                       buffers */
                    free(Memory);
                    free(NewName);
                }
                else
                {
                    Buffer[BInfo.BufferIndex]=Memory;
                    BufferSize[BInfo.BufferIndex]=MemorySize;
                    BufferName[BInfo.BufferIndex]=NewName;
                }
                Memory=NULL;
                NewName=NULL;

                Load.DoneReadingGroup();
            }
        }

        /* Tell the main windows that we changed all the bookmarks */
        for(r=0;r<MAX_SEND_BUFFERS;r++)
            MW_InformOfSendBufferChange(r);

        RetValue=true;
    }
    catch(e_RIFFErrorType err)
    {
        if(NewName!=NULL)
            free(NewName);

        if(Memory!=NULL)
            free(Memory);

        ClearAllBuffers();

        RetValue=false;
    }

    if(FileOpen)
        Load.Close();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::SaveBuffers
 *
 * SYNOPSIS:
 *    bool SendBuffer::SaveBuffers(const char *Filename=NULL);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to load.  If NULL use the default in the
 *                    WhippyTerm settings directory.
 *
 * FUNCTION:
 *    This function saves the current buffers to disk.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool SendBuffer::SaveBuffers(const char *Filename)
{
    class RIFF Save;
    int b;
    struct BufferInfo BInfo;
    string SaveFilename;
    char *UseName;
    char NameBuff[100];

    try
    {
        if(Filename==NULL)
        {
            if(GetAppDataPath(SaveFilename)==false)
                throw("Failed to get save path");

            /* See if this path exists */
            if(!PathExists(SaveFilename.c_str()))
            {
                /* Try making it */
                if(!MakePathDir(SaveFilename.c_str()))
                    throw("Failed to make save path");
            }

            SaveFilename+=BUFFERS_FILE;
        }
        else
        {
            SaveFilename=Filename;
        }

        Save.SetIFFMode(true);
        Save.Open(SaveFilename.c_str(),e_RIFFOpen_Write,"WTSB");

        for(b=0;b<MAX_SEND_BUFFERS;b++)
        {
            if(Buffer[b]!=NULL && BufferSize[b]>0)
            {
                memset(&BInfo,0x00,sizeof(BInfo));
                BInfo.BufferIndex=b;

                if(BufferName[b]==NULL)
                {
                    sprintf(NameBuff,"Buffer %d",b+1);
                    UseName=NameBuff;
                }
                else
                {
                    UseName=BufferName[b];
                }

                Save.StartGroup("BUFF");
                SaveBuffer2WTSBFile(&Save,&BInfo,UseName,Buffer[b],
                        BufferSize[b]);
                Save.EndGroup();
            }
        }

        Save.Close();
    }
    catch(e_RIFFErrorType err)
    {
        string ErrMsg;

        ErrMsg="Failed to save \"";
        ErrMsg+=SaveFilename;
        ErrMsg+="\":\n";
        ErrMsg+=Save.Error2Str(err);
        UIAsk("Error",ErrMsg.c_str(),e_AskBox_Error);

        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::LoadBufferFromFile
 *
 * SYNOPSIS:
 *    static bool SendBuffer::LoadBufferFromFile(const char *Filename,
 *              char *BufferName,uint8_t *Data,int *DataSize);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to load from
 *    BufferName [O] -- The name of the buffer.  You will need to free this.
 *    Data [O] -- The data to put in the file.  You will need to free this.
 *    DataSize [O] -- The number of bytes in 'Data'
 *
 * FUNCTION:
 *    This function load a buffer from disk.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool SendBuffer::LoadBufferFromFile(const char *Filename,char **BufferName,
        uint8_t **Data,uint32_t *DataSize)
{
    class RIFF Load;
    bool FileOpen;
    bool RetValue;
    struct LoadBufferDiskData DiskData;

    FileOpen=false;
    try
    {
        Load.SetIFFMode(true);
        Load.Open(Filename,e_RIFFOpen_Read,"WTSB");
        FileOpen=true;

        LoadBufferFromWTSBFile(&Load,&DiskData);

        if(!DiskData.NameFound)
        {
            DiskData.BufferName=(char *)malloc(1);
            if(DiskData.BufferName==NULL)
                throw("Out of memory");
            *DiskData.BufferName='\0';
        }
        if(!DiskData.DataFound)
            throw("Malformed buffer file");

        *BufferName=DiskData.BufferName;
        *Data=DiskData.Data;
        *DataSize=DiskData.DataSize;

        RetValue=true;
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error);
        RetValue=false;
    }
    catch(e_RIFFErrorType err)
    {
        UIAsk("Error","Failed to load buffer",e_AskBox_Error);
        RetValue=false;
    }

    if(!RetValue)
    {
        if(DiskData.BufferName!=NULL)
            free(DiskData.BufferName);

        if(DiskData.Data!=NULL)
            free(DiskData.Data);
    }

    if(FileOpen)
        Load.Close();

    return RetValue;
}


/*******************************************************************************
 * NAME:
 *    SendBuffer::SaveBuffer2File
 *
 * SYNOPSIS:
 *    static bool SendBuffer::SaveBuffer2File(const char *Filename,
 *              const char *BufferName,const uint8_t *Data,int DataSize);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to save to
 *    BufferName [I] -- The name of the buffer
 *    Data [I] -- The data to put in the file
 *    DataSize [I] -- The number of bytes in 'Data'
 *
 * FUNCTION:
 *    This function saves data for a buffer to the disk in buffer format.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool SendBuffer::SaveBuffer2File(const char *Filename,const char *BufferName,
        const uint8_t *Data,uint32_t DataSize)
{
    class RIFF Save;

    try
    {
        Save.SetIFFMode(true);
        Save.Open(Filename,e_RIFFOpen_Write,"WTSB");

        SaveBuffer2WTSBFile(&Save,NULL,BufferName,Data,DataSize);

        Save.Close();
    }
    catch(e_RIFFErrorType err)
    {
        string ErrMsg;

        ErrMsg="Failed to save \"";
        ErrMsg+=Filename;
        ErrMsg+="\":\n";
        ErrMsg+=Save.Error2Str(err);
        UIAsk("Error",ErrMsg.c_str(),e_AskBox_Error);

        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::SaveBuffer2WTSBFile
 *
 * SYNOPSIS:
 *    static void SendBuffer::SaveBuffer2WTSBFile(class RIFF *Save,
 *          struct BufferInfo *BInfo,const char *BufferName,const uint8_t *Data,
 *          uint32_t DataSize);
 *
 * PARAMETERS:
 *    Save [I] -- The RIFF object to save to
 *    BInfo [I] -- Info about this buffer.  If NULL it is skipped
 *    BufferName [I] -- The name of this buffer
 *    Data [I] -- The binary data to store in this buffer
 *    DataSize [I] -- The number of bytes in 'Data'
 *
 * FUNCTION:
 *    This function saves buffer data into a file.
 *
 * RETURNS:
 *    NONE
 *
 * THROWS:
 *    e_RIFFErrorType from RIFF.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void SendBuffer::SaveBuffer2WTSBFile(class RIFF *Save,struct BufferInfo *BInfo,
        const char *BufferName,const uint8_t *Data,uint32_t DataSize)
{
    /* buffer info */
    if(BInfo!=NULL)
    {
        Save->StartDataBlock("INFO");
        Save->Write(BInfo,sizeof(struct BufferInfo));
        Save->EndDataBlock();
    }

    if(BufferName!=NULL)
    {
        Save->StartDataBlock("NAME");
        Save->Write(BufferName,strlen(BufferName));
        Save->EndDataBlock();
    }

    /* Save the buffer */
    Save->StartDataBlock("BIN ");
    Save->Write(Data,DataSize);
    Save->EndDataBlock();
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::LoadBufferFromWTSBFile
 *
 * SYNOPSIS:
 *    void SendBuffer::LoadBufferFromWTSBFile(class RIFF *Load,
 *              struct LoadBufferDiskData *LoadedDiskData);
 *
 * PARAMETERS:
 *    Load [I] -- The RIFF to read the data out of
 *    LoadedDiskData [O] -- This will be filled in with the data we read from
 *                          the RIFF file.
 *
 * FUNCTION:
 *    This function reads the data chunks out of a RIFF file.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    SendBuffer::SaveBuffer2WTSBFile()
 ******************************************************************************/
void SendBuffer::LoadBufferFromWTSBFile(class RIFF *Load,
        struct LoadBufferDiskData *LoadedDiskData)
{
    char ChunkID[5];
    uint32_t ChunkSize;

    memset(LoadedDiskData,0x00,sizeof(struct LoadBufferDiskData));

    while(Load->ReadNextDataBlock(ChunkID,&ChunkSize))
    {
        if(strcmp(ChunkID,"INFO")==0)
        {
            if(ChunkSize>=sizeof(struct BufferInfo))
            {
                Load->Read(&LoadedDiskData->BInfo,sizeof(struct BufferInfo));
                LoadedDiskData->BInfoFound=true;
            }
        }
        if(strcmp(ChunkID,"NAME")==0)
        {
            LoadedDiskData->BufferName=(char *)malloc(ChunkSize+1);
            if(LoadedDiskData->BufferName==NULL)
                throw("Out of memory for buffer name");
            Load->Read(LoadedDiskData->BufferName,ChunkSize);
            LoadedDiskData->BufferName[ChunkSize]=0;
            LoadedDiskData->NameFound=true;
        }
        else if(strcmp(ChunkID,"BIN ")==0)
        {
            LoadedDiskData->Data=(uint8_t *)malloc(ChunkSize);
            if(LoadedDiskData->Data==NULL)
                throw("Out of memory");
            Load->Read(LoadedDiskData->Data,ChunkSize);
            LoadedDiskData->DataSize=ChunkSize;
            LoadedDiskData->DataFound=true;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::SetBuffer
 *
 * SYNOPSIS:
 *    void SendBuffer::SetBuffer(int BufferIndex,const uint8_t *Memory,
 *              uint32_t BSize);
 *
 * PARAMETERS:
 *    BufferIndex [I] -- The buffer to replace the data for
 *    Memory [I] -- The new memory to use for this buffer.  This will be copied.
 *    BSize [I] -- The number of bytes in 'Memory'
 *
 * FUNCTION:
 *    This function free the current contents of a buffer and replaces it with
 *    new data.  The data will be copied.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    SendBuffer::SetBufferName()
 ******************************************************************************/
void SendBuffer::SetBuffer(int BufferIndex,const uint8_t *Memory,uint32_t BSize)
{
    if(BufferIndex>=MAX_SEND_BUFFERS)
        return;

    if(Buffer[BufferIndex]!=NULL)
        free(Buffer[BufferIndex]);
    Buffer[BufferIndex]=NULL;
    BufferSize[BufferIndex]=0;

    Buffer[BufferIndex]=NULL;
    if(BSize>0)
    {
        Buffer[BufferIndex]=(uint8_t *)malloc(BSize);
        if(Buffer[BufferIndex]==NULL)
            return;
        memcpy(Buffer[BufferIndex],Memory,BSize);
    }

    BufferSize[BufferIndex]=BSize;

    MW_InformOfSendBufferChange(BufferIndex);
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::SetBufferName
 *
 * SYNOPSIS:
 *    void SendBuffer::SetBufferName(int BufferIndex,const char *NewName);
 *
 * PARAMETERS:
 *    BufferIndex [I] -- The buffer to replace the data for
 *    NewName [I] -- A C-string with the new name of this buffer in it
 *
 * FUNCTION:
 *    This function sets the name of a buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    SendBuffer::SetBuffer(), SendBuffer::GetBufferName()
 ******************************************************************************/
void SendBuffer::SetBufferName(int BufferIndex,const char *NewName)
{
    if(BufferIndex>=MAX_SEND_BUFFERS)
        return;

    free(BufferName[BufferIndex]);

    BufferName[BufferIndex]=(char *)malloc(strlen(NewName)+1);
    strcpy(BufferName[BufferIndex],NewName);

    MW_InformOfSendBufferChange(BufferIndex);
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::GetBufferName
 *
 * SYNOPSIS:
 *    char *SendBuffer::GetBufferName(int BufferIndex);
 *
 * PARAMETERS:
 *    BufferIndex [I] -- The buffer to get the data for
 *
 * FUNCTION:
 *    This function gets the name of a buffer.  If the buffer is not set
 *    then it returns "Buffer xx" with the buffer number in it.
 *
 * RETURNS:
 *    A string with the name of the buffer in it.  This is valid until shutdown
 *    or a new name is assigned (by loading or calling SetBufferName())
 *
 *    If 'BufferIndex' is invalid this function returns NULL.  This function
 *    will also return NULL if there is an error.
 *
 * SEE ALSO:
 *    SendBuffer::SetBufferName()
 ******************************************************************************/
char *SendBuffer::GetBufferName(int BufferIndex)
{
    char buff[100];

    if(BufferIndex>=MAX_SEND_BUFFERS)
        return NULL;

    if(BufferName[BufferIndex]==NULL)
    {
        sprintf(buff,"Buffer %d",BufferIndex+1);
        BufferName[BufferIndex]=(char *)malloc(strlen(buff)+1);
        strcpy(BufferName[BufferIndex],buff);
    }
    return BufferName[BufferIndex];
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::GetBufferInfo
 *
 * SYNOPSIS:
 *    bool SendBuffer::GetBufferInfo(int BufferIndex,const uint8_t **Memory,
 *              uint32_t *BSize);
 *
 * PARAMETERS:
 *    BufferIndex [I] -- The buffer to get the data for
 *    Memory [O] -- A pointer to the memory for this buffer.  This can be
 *                  set to NULL if the buffer has not been set.
 *    BSize [O] -- The number of bytes in the buffer.
 *
 * FUNCTION:
 *    This function gets access to the buffer's memory.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- The memory could not be fetched.
 *
 * NOTES:
 *    'Memory' remains valid until this class is freed or the buffer is
 *    changed.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool SendBuffer::GetBufferInfo(int BufferIndex,const uint8_t **Memory,
        uint32_t *BSize)
{
    *Memory=NULL;
    *BSize=0;

    if(BufferIndex>=MAX_SEND_BUFFERS)
        return false;

    *Memory=Buffer[BufferIndex];
    *BSize=BufferSize[BufferIndex];

    return true;
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::ClearAllBuffers
 *
 * SYNOPSIS:
 *    void SendBuffer::ClearAllBuffers(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function resets all the send buffers to blank and resets their names.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void SendBuffer::ClearAllBuffers(void)
{
    int r;
    char buff[100];

    for(r=0;r<MAX_SEND_BUFFERS;r++)
    {
        free(Buffer[r]);
        free(BufferName[r]);
        Buffer[r]=NULL;
        BufferSize[r]=0;
        BufferName[r]=NULL;
    }

    for(r=0;r<MAX_SEND_BUFFERS;r++)
    {
        if(r<MAX_QUICK_SEND_BUFFERS)
            sprintf(buff,"Send Buffer #%d",r+1);
        else
            sprintf(buff,"%c",'a'+(r-MAX_QUICK_SEND_BUFFERS));
        SetBuffer(r,(uint8_t *)buff,strlen(buff));

        MW_InformOfSendBufferChange(r);
    }
}

/*******************************************************************************
 * NAME:
 *    SendBuffer::SendBuffer
 *
 * SYNOPSIS:
 *    bool SendBuffer::Send(class Connection *Con,int BufferIndex);
 *
 * PARAMETERS:
 *    Con [I] -- The connection to send the buffer out
 *    BufferIndex [I] -- The buffer number to send
 *
 * FUNCTION:
 *    This function sends a buffer out a connection.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool SendBuffer::Send(class Connection *Con,int BufferIndex)
{
    if(BufferIndex>=MAX_SEND_BUFFERS)
        return false;

    if(Buffer[BufferIndex]==NULL || BufferSize[BufferIndex]==0)
        return true;

    if(Con->WriteData(Buffer[BufferIndex],BufferSize[BufferIndex],
            e_ConWriteSource_Buffers)!=e_ConWrite_Success)
    {
        return false;
    }

    /* Force the send as if we are doing a block send device we want to send
       everything we just queued */
    Con->TransmitQueuedData();

    return true;
}
