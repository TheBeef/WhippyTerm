/*******************************************************************************
 * FILENAME: QTSound.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Has the sound play back system (mostly for playing bells) for the
 *    Windows (Win32) OS.
 *
 *    In the end we needed to call the OS directly because the sound systems
 *    of QT just aren't up to the task (pauses, lock up, huge library files
 *    just to play a sound, etc), so we use XAudio2 instead.
 *
 * COPYRIGHT:
 *    Copyright 08 Feb 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (08 Feb 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UISound.h"
#include "QTSound.h"
#include <QApplication>
#include <QFile>
#include <QByteArray>
#include <windows.h>
#include <xaudio2.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    e_WavPlayError_None=0,
    e_WavPlayError_OpenFile,
    e_WavPlayError_ReadFile,
    e_WavPlayError_BadFormat,
    e_WavPlayError_Unsupported,
    e_WavPlayError_OutOfMemory,
    e_WavPlayError_XAudio2,
    e_WavPlayErrorMAX
} e_WavPlayErrorType;

struct PlaybackSample
{
    QByteArray WavFileContents;

    WAVEFORMATEX Format;            /* the decoded WAV format */
    const uint8_t *PcmStart;        /* points into WavFileContents */
    uint32_t PcmSize;               /* number of PCM bytes */

    IXAudio2SourceVoice *SourceVoice;   /* the voice playing this sample */
};

/*** FUNCTION PROTOTYPES      ***/
static bool WavPlayStart(struct PlaybackSample *Sample);
static void WavPlayStop(struct PlaybackSample *Sample);
static e_WavPlayErrorType WavPlayParseWav(const uint8_t *WavData,
        size_t WavSize,const uint8_t **PcmStart,uint32_t *PcmSize,
        WAVEFORMATEX *Format);
static uint16_t WavPlayReadU16(const uint8_t *Data);
static uint32_t WavPlayReadU32(const uint8_t *Data);

/*** VARIABLE DEFINITIONS     ***/
struct PlaybackSample m_BuildInSample;
struct PlaybackSample m_UsrSelSample;

static IXAudio2 *m_XAudio2=NULL;                    /* the XAudio2 engine */
static IXAudio2MasteringVoice *m_MasterVoice=NULL;  /* the master voice */
static bool m_ComInitialized=false;    /* did we init COM in this module? */

/*******************************************************************************
 * NAME:
 *    InitQTSoundSystem
 *
 * SYNOPSIS:
 *    void InitQTSoundSystem(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the sound system.  It brings up the XAudio2 engine
 *    and its mastering voice.  If it fails the module is left in a state
 *    where the play functions do nothing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    FreeQTSoundSystem()
 ******************************************************************************/
void InitQTSoundSystem(void)
{
    HRESULT Result;

    m_XAudio2=NULL;
    m_MasterVoice=NULL;
    m_ComInitialized=false;

    /* XAudio2 talks to WASAPI under the hood so COM must be up.  A Qt app
       may already have done this on this thread, which is fine. */
    Result=CoInitializeEx(NULL,COINIT_MULTITHREADED);
    if(Result==S_OK || Result==S_FALSE)
        m_ComInitialized=true;

    if(FAILED(XAudio2Create(&m_XAudio2,0,XAUDIO2_DEFAULT_PROCESSOR)))
    {
        m_XAudio2=NULL;
        return;
    }

    if(FAILED(m_XAudio2->CreateMasteringVoice(&m_MasterVoice)))
    {
        m_XAudio2->Release();
        m_XAudio2=NULL;
        m_MasterVoice=NULL;
        return;
    }
}

/*******************************************************************************
 * NAME:
 *    UI_Beep
 *
 * SYNOPSIS:
 *    void UI_Beep(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does the OS "beep".
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UIPlayWav(), UIPlayBuiltInBeep(), FreeQTSoundSystem()
 ******************************************************************************/
void UI_Beep(void)
{
    MessageBeep(MB_OK);
}

/*******************************************************************************
 * NAME:
 *    UIPlayWav
 *
 * SYNOPSIS:
 *    void UIPlayWav(const char *Filename);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename (and path) to the sound (.wav) to play.
 *
 * FUNCTION:
 *    This function will load and play a sound from disk.  It will cache the
 *    last file loaded.  If you then load a different file the old one
 *    will be droped and the new one will be loaded.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UI_Beep()
 ******************************************************************************/
void UIPlayWav(const char *Filename)
{
    static string LastFilename;
    const uint8_t *WavData;
    int Length;

    if(LastFilename!=Filename || m_UsrSelSample.WavFileContents.isEmpty())
    {
        QFile FileBasedWav(Filename);

        /* Be sure to free the memory if we played something else */
        if(!m_UsrSelSample.WavFileContents.isEmpty())
        {
            WavPlayStop(&m_UsrSelSample);
            m_UsrSelSample.WavFileContents.clear();
        }

        LastFilename=Filename;

        if(FileBasedWav.open(QIODevice::ReadOnly))
        {
            m_UsrSelSample.WavFileContents=FileBasedWav.readAll();
            FileBasedWav.close();
        }
        else
        {
            return;
        }

        WavData=reinterpret_cast<const unsigned char *>
                (m_UsrSelSample.WavFileContents.constData());
        Length=m_UsrSelSample.WavFileContents.size();

        if(WavPlayParseWav(WavData,Length,&m_UsrSelSample.PcmStart,
                &m_UsrSelSample.PcmSize,&m_UsrSelSample.Format)!=
                e_WavPlayError_None)
        {
            m_UsrSelSample.WavFileContents.clear();
            return;
        }
    }

    /* Stop the currently playing sample (if there is one) */
    WavPlayStop(&m_UsrSelSample);
    WavPlayStart(&m_UsrSelSample);
}

/*******************************************************************************
 * NAME:
 *    UIPlayBuiltInBeep
 *
 * SYNOPSIS:
 *    void UIPlayBuiltInBeep(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function loads the built in "beep" sound (build into WhippyTerm)
 *    and plays it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UI_Beep()
 ******************************************************************************/
void UIPlayBuiltInBeep(void)
{
    const uint8_t *WavData;
    int Length;

    if(m_BuildInSample.WavFileContents.isEmpty())
    {
        QFile BuildInWav(":/S/Sounds/Computer_Error_Alert_Short.wav");
        if(BuildInWav.open(QIODevice::ReadOnly))
        {
            m_BuildInSample.WavFileContents=BuildInWav.readAll();
            BuildInWav.close();
        }
        else
        {
            return;
        }

        WavData=reinterpret_cast<const unsigned char *>
                (m_BuildInSample.WavFileContents.constData());
        Length=m_BuildInSample.WavFileContents.size();

        if(WavPlayParseWav(WavData,Length,&m_BuildInSample.PcmStart,
                &m_BuildInSample.PcmSize,&m_BuildInSample.Format)!=
                e_WavPlayError_None)
        {
            m_BuildInSample.WavFileContents.clear();
            return;
        }
    }

    /* Stop the currently playing sample (if there is one) */
    WavPlayStop(&m_BuildInSample);
    WavPlayStart(&m_BuildInSample);
}

/*******************************************************************************
 * NAME:
 *    FreeQTSoundSystem
 *
 * SYNOPSIS:
 *    void FreeQTSoundSystem(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees anything that was allocated with the sound system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    InitQTSoundSystem()
 ******************************************************************************/
void FreeQTSoundSystem(void)
{
    /* Make sure we stop all samples (to make sure the memory is freed) */
    WavPlayStop(&m_BuildInSample);
    WavPlayStop(&m_UsrSelSample);

    if(m_MasterVoice!=NULL)
    {
        m_MasterVoice->DestroyVoice();
        m_MasterVoice=NULL;
    }

    if(m_XAudio2!=NULL)
    {
        m_XAudio2->Release();
        m_XAudio2=NULL;
    }

    if(m_ComInitialized)
    {
        CoUninitialize();
        m_ComInitialized=false;
    }
}

/******************************************************************************
 * NAME:
 *    WavPlayParseWav
 *
 * SYNOPSIS:
 *    static e_WavPlayErrorType WavPlayParseWav(const uint8_t *WavData,
 *          size_t WavSize,const uint8_t **PcmStart,uint32_t *PcmSize,
 *          WAVEFORMATEX *Format);
 *
 * PARAMETERS:
 *    WavData [I] -- A complete .wav image.
 *    WavSize [I] -- The number of bytes in WavData.
 *    PcmStart [O] -- Set to point at the first PCM byte inside WavData.
 *    PcmSize [O] -- Set to the number of PCM bytes.
 *    Format [O] -- Filled in with the WAVEFORMATEX describing the samples.
 *
 * FUNCTION:
 *    Walks the RIFF chunks of a WAV image, pulling the format out of the
 *    "fmt " chunk and the sample location out of the "data" chunk.  The
 *    integer fields are read a byte at a time so the parse works regardless
 *    of host byte order or pointer alignment.
 *
 * RETURNS:
 *    e_WavPlayError_None on success.
 *    e_WavPlayError_BadFormat if the bytes are not a usable WAV image.
 *    e_WavPlayError_Unsupported if the sample format cannot be given to
 *    XAudio2.
 *
 * LIMITATIONS:
 *    Only WAVE_FORMAT_PCM (1) and WAVE_FORMAT_IEEE_FLOAT (3) are handled.
 *    WAVE_FORMAT_EXTENSIBLE (0xFFFE), commonly used by some 24/32 bit files,
 *    is reported as unsupported.  64 bit float is not supported by XAudio2
 *    and is also reported as unsupported.
 *
 * SEE ALSO:
 *    WavPlayReadU16(), WavPlayReadU32()
 ******************************************************************************/
static e_WavPlayErrorType WavPlayParseWav(const uint8_t *WavData,
        size_t WavSize,const uint8_t **PcmStart,uint32_t *PcmSize,
        WAVEFORMATEX *Format)
{
    uint32_t Offset;
    uint16_t AudioFormat;
    uint16_t Channels;
    uint32_t SampleRate;
    uint16_t BitsPerSample;
    uint16_t FormatTag;
    bool HaveFmt;
    bool HaveData;

    if(WavSize<12)
        return e_WavPlayError_BadFormat;
    if(memcmp(WavData,"RIFF",4)!=0)
        return e_WavPlayError_BadFormat;
    if(memcmp(WavData+8,"WAVE",4)!=0)
        return e_WavPlayError_BadFormat;

    HaveFmt=false;
    HaveData=false;
    AudioFormat=0;
    Channels=0;
    SampleRate=0;
    BitsPerSample=0;

    Offset=12;
    while(Offset+8<=WavSize)
    {
        const uint8_t *ChunkId;
        uint32_t ChunkSize;

        ChunkId=WavData+Offset;
        ChunkSize=WavPlayReadU32(WavData+Offset+4);
        Offset+=8;

        /* tolerate a final chunk whose declared size runs off the end */
        if(ChunkSize>WavSize-Offset)
            ChunkSize=(uint32_t)(WavSize-Offset);

        if(memcmp(ChunkId,"fmt ",4)==0 && ChunkSize>=16)
        {
            AudioFormat=WavPlayReadU16(WavData+Offset+0);
            Channels=WavPlayReadU16(WavData+Offset+2);
            SampleRate=WavPlayReadU32(WavData+Offset+4);
            BitsPerSample=WavPlayReadU16(WavData+Offset+14);
            HaveFmt=true;
        }
        else if(memcmp(ChunkId,"data",4)==0)
        {
            *PcmStart=WavData+Offset;
            *PcmSize=ChunkSize;
            HaveData=true;
        }

        /* RIFF chunks are padded to an even number of bytes */
        Offset+=ChunkSize;
        if((ChunkSize&1)!=0)
            Offset++;
    }

    if(!HaveFmt || !HaveData)
        return e_WavPlayError_BadFormat;
    if(Channels==0 || SampleRate==0)
        return e_WavPlayError_BadFormat;

    /* map the WAV sample format onto a WAVEFORMATEX tag for XAudio2 */
    FormatTag=0;
    if(AudioFormat==1)              /* integer PCM */
    {
        if(BitsPerSample==8 || BitsPerSample==16 ||
                BitsPerSample==24 || BitsPerSample==32)
        {
            FormatTag=WAVE_FORMAT_PCM;
        }
    }
    else if(AudioFormat==3)         /* IEEE float */
    {
        if(BitsPerSample==32)
            FormatTag=WAVE_FORMAT_IEEE_FLOAT;
    }

    if(FormatTag==0)
        return e_WavPlayError_Unsupported;

    memset(Format,0,sizeof(*Format));
    Format->wFormatTag=FormatTag;
    Format->nChannels=Channels;
    Format->nSamplesPerSec=SampleRate;
    Format->wBitsPerSample=BitsPerSample;
    Format->nBlockAlign=(WORD)(Channels*(BitsPerSample/8));
    Format->nAvgBytesPerSec=SampleRate*Format->nBlockAlign;
    Format->cbSize=0;

    if(Format->nBlockAlign==0)
        return e_WavPlayError_BadFormat;

    return e_WavPlayError_None;
}

/******************************************************************************
 * NAME:
 *    WavPlayReadU16
 *
 * SYNOPSIS:
 *    static uint16_t WavPlayReadU16(const uint8_t *Data);
 *
 * PARAMETERS:
 *    Data [I] -- A pointer to at least two bytes of little endian data.
 *
 * FUNCTION:
 *    Reads a little endian 16 bit unsigned value without assuming alignment
 *    or host byte order.
 *
 * RETURNS:
 *    The decoded value.
 *
 * SEE ALSO:
 *    WavPlayReadU32()
 ******************************************************************************/
static uint16_t WavPlayReadU16(const uint8_t *Data)
{
    return (uint16_t)((uint16_t)Data[0] | ((uint16_t)Data[1]<<8));
}

/******************************************************************************
 * NAME:
 *    WavPlayReadU32
 *
 * SYNOPSIS:
 *    static uint32_t WavPlayReadU32(const uint8_t *Data);
 *
 * PARAMETERS:
 *    Data [I] -- A pointer to at least four bytes of little endian data.
 *
 * FUNCTION:
 *    Reads a little endian 32 bit unsigned value without assuming alignment
 *    or host byte order.
 *
 * RETURNS:
 *    The decoded value.
 *
 * SEE ALSO:
 *    WavPlayReadU16()
 ******************************************************************************/
static uint32_t WavPlayReadU32(const uint8_t *Data)
{
    return (uint32_t)Data[0] | ((uint32_t)Data[1]<<8) |
            ((uint32_t)Data[2]<<16) | ((uint32_t)Data[3]<<24);
}

/*******************************************************************************
 * NAME:
 *    WavPlayStart
 *
 * SYNOPSIS:
 *    static bool WavPlayStart(struct PlaybackSample *Sample);
 *
 * PARAMETERS:
 *    Sample [I] -- The sample to work on
 *
 * FUNCTION:
 *    This function starts a sample playing.  It needs to be stopped before
 *    starting the new sample.  A fresh XAudio2 source voice is created for
 *    the sample's format and the whole PCM buffer is queued in one shot.
 *
 * RETURNS:
 *    true -- The sample has been started
 *    false -- There was an error.
 *
 * NOTES:
 *    XAudio2 does not copy the PCM data, it reads it straight out of
 *    Sample->WavFileContents while the voice plays.  That buffer must stay
 *    alive until WavPlayStop() is called (which is how this module works).
 *
 * SEE ALSO:
 *    WavPlayStop()
 ******************************************************************************/
static bool WavPlayStart(struct PlaybackSample *Sample)
{
    XAUDIO2_BUFFER Buffer;

    if(m_XAudio2==NULL)
        return false;
    if(Sample->SourceVoice!=NULL)
        return false;
    if(Sample->PcmStart==NULL || Sample->PcmSize==0)
        return false;

    if(FAILED(m_XAudio2->CreateSourceVoice(&Sample->SourceVoice,
            &Sample->Format)))
    {
        Sample->SourceVoice=NULL;
        return false;
    }

    memset(&Buffer,0,sizeof(Buffer));
    Buffer.AudioBytes=Sample->PcmSize;
    Buffer.pAudioData=Sample->PcmStart;
    Buffer.Flags=XAUDIO2_END_OF_STREAM;

    if(FAILED(Sample->SourceVoice->SubmitSourceBuffer(&Buffer)))
    {
        Sample->SourceVoice->DestroyVoice();
        Sample->SourceVoice=NULL;
        return false;
    }

    if(FAILED(Sample->SourceVoice->Start(0)))
    {
        Sample->SourceVoice->DestroyVoice();
        Sample->SourceVoice=NULL;
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    WavPlayStop
 *
 * SYNOPSIS:
 *    static void WavPlayStop(struct PlaybackSample *Sample);
 *
 * PARAMETERS:
 *    Sample [I] -- The sample to work on
 *
 * FUNCTION:
 *    This function stops playing back a sample (and frees the playback
 *    voice for this sample)
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    DestroyVoice() blocks until the XAudio2 audio thread is no longer using
 *    the voice, so it is safe to free the PCM buffer after this returns.
 *
 * SEE ALSO:
 *    WavPlayStart()
 ******************************************************************************/
static void WavPlayStop(struct PlaybackSample *Sample)
{
    if(Sample->SourceVoice==NULL)
        return;

    /* Stop the voice, throw away anything still queued, then free it */
    Sample->SourceVoice->Stop(0);
    Sample->SourceVoice->FlushSourceBuffers();
    Sample->SourceVoice->DestroyVoice();

    Sample->SourceVoice=NULL;
}
