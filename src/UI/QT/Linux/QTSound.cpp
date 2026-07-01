/*******************************************************************************
 * FILENAME: QTSound.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Has the sound play back system (mostly for playing bells) for the Linux
 *    OS.
 *
 *    In the end we needed to call the OS directly because the sound systems
 *    of QT just aren't up to the task (pauses, lock up, huge library files
 *    just to play a sound, etc), so we do this instead.
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
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <QApplication>
#include <QFile>
#include <QByteArray>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
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
    e_WavPlayError_PipeWire,
    e_WavPlayErrorMAX
} e_WavPlayErrorType;

struct PlaybackSample
{
    QByteArray WavFileContents;

    struct spa_audio_info_raw Info;
    const uint8_t *PcmStart;
    uint32_t PcmSize;
    uint32_t Stride;

    struct pw_thread_loop *Loop;    /* runs the PipeWire graph in a thread */
    struct pw_stream *Stream;       /* the playback stream */
    uint32_t Position;              /* read offset into PcmData in bytes */
    bool Draining;                  /* true once the last frame was queued */
    bool Finished;                  /* true once playback has fully drained */
};

/*** FUNCTION PROTOTYPES      ***/
static bool WavPlayStart(struct PlaybackSample *sample);
static void WavPlayStop(struct PlaybackSample *sample);
static e_WavPlayErrorType WavPlayParseWav(const uint8_t *WavData,
        size_t WavSize,const uint8_t **PcmStart,uint32_t *PcmSize,
        struct spa_audio_info_raw *Info,uint32_t *Stride);
static uint16_t WavPlayReadU16(const uint8_t *Data);
static uint32_t WavPlayReadU32(const uint8_t *Data);

static void WavPlayProcessCb(void *UserData);
static void WavPlayDrainedCb(void *UserData);
static void WavPlayStateChangedCb(void *UserData,enum pw_stream_state Old,
        enum pw_stream_state New,const char *Error);

/*** VARIABLE DEFINITIONS     ***/
struct PlaybackSample m_BuildInSample;
struct PlaybackSample m_UsrSelSample;

static const struct pw_stream_events m_StreamEvents=
{
    PW_VERSION_STREAM_EVENTS,
    .destroy=NULL,
    .state_changed=WavPlayStateChangedCb,
    .control_info=NULL,
    .io_changed=NULL,
    .param_changed=NULL,
    .add_buffer=NULL,
    .remove_buffer=NULL,
    .process=WavPlayProcessCb,
    .drained=WavPlayDrainedCb,
    .command=NULL,
    .trigger_done=NULL,
};

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
 *    This function init's the sound system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void InitQTSoundSystem(void)
{
    pw_init(NULL,NULL);
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
    QApplication::beep();
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
                &m_UsrSelSample.PcmSize,&m_UsrSelSample.Info,
                &m_UsrSelSample.Stride)!=e_WavPlayError_None)
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
                &m_BuildInSample.PcmSize,&m_BuildInSample.Info,
                &m_BuildInSample.Stride)!=e_WavPlayError_None)
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
 *    
 ******************************************************************************/
void FreeQTSoundSystem(void)
{
    /* Make sure we stop all samples (to make sure the memory is freed) */
    WavPlayStop(&m_BuildInSample);
    WavPlayStop(&m_UsrSelSample);

    pw_deinit();
}

/******************************************************************************
 * NAME:
 *    WavPlayParseWav
 *
 * SYNOPSIS:
 *    static e_WavPlayErrorType WavPlayParseWav(const uint8_t *WavData,
 *          size_t WavSize,const uint8_t **PcmStart,uint32_t *PcmSize,
 *          struct spa_audio_info_raw *Info,uint32_t *Stride);
 *
 * PARAMETERS:
 *    WavData [I] -- A complete .wav image.
 *    WavSize [I] -- The number of bytes in WavData.
 *    PcmStart [O] -- Set to point at the first PCM byte inside WavData.
 *    PcmSize [O] -- Set to the number of PCM bytes.
 *    Info [O] -- Filled in with the SPA audio format, rate, and channels.
 *    Stride [O] -- Set to the number of bytes in one audio frame.
 *
 * FUNCTION:
 *    Walks the RIFF chunks of a WAV image, pulling the format out of the "fmt "
 *    chunk and the sample location out of the "data" chunk.  The integer fields
 *    are read a byte at a time so the parse works regardless of host byte order
 *    or pointer alignment.
 *
 * RETURNS:
 *    e_WavPlayError_None on success.
 *    e_WavPlayError_BadFormat if the bytes are not a usable WAV image.
 *    e_WavPlayError_Unsupported if the sample format cannot be mapped to SPA.
 *
 * LIMITATIONS:
 *    Only WAVE_FORMAT_PCM (1) and WAVE_FORMAT_IEEE_FLOAT (3) are handled.
 *    WAVE_FORMAT_EXTENSIBLE (0xFFFE), commonly used by some 24/32 bit files,
 *    is reported as unsupported.
 *
 * SEE ALSO:
 *    WavPlayReadU16(), WavPlayReadU32()
 ******************************************************************************/
static e_WavPlayErrorType WavPlayParseWav(const uint8_t *WavData,
        size_t WavSize,const uint8_t **PcmStart,uint32_t *PcmSize,
        struct spa_audio_info_raw *Info,uint32_t *Stride)
{
    uint32_t Offset;
    uint16_t AudioFormat;
    uint16_t Channels;
    uint32_t SampleRate;
    uint16_t BitsPerSample;
    bool HaveFmt;
    bool HaveData;
    enum spa_audio_format Format;

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

    /* map the WAV sample format onto a SPA sample format */
    Format=SPA_AUDIO_FORMAT_UNKNOWN;
    if(AudioFormat==1)              /* integer PCM */
    {
        if(BitsPerSample==8)
            Format=SPA_AUDIO_FORMAT_U8;
        else if(BitsPerSample==16)
            Format=SPA_AUDIO_FORMAT_S16_LE;
        else if(BitsPerSample==24)
            Format=SPA_AUDIO_FORMAT_S24_LE;
        else if(BitsPerSample==32)
            Format=SPA_AUDIO_FORMAT_S32_LE;
    }
    else if(AudioFormat==3)         /* IEEE float */
    {
        if(BitsPerSample==32)
            Format=SPA_AUDIO_FORMAT_F32_LE;
        else if(BitsPerSample==64)
            Format=SPA_AUDIO_FORMAT_F64_LE;
    }

    if(Format==SPA_AUDIO_FORMAT_UNKNOWN)
        return e_WavPlayError_Unsupported;

    spa_zero(*Info);
    Info->format=Format;
    Info->channels=Channels;
    Info->rate=SampleRate;

    *Stride=(uint32_t)Channels*(BitsPerSample/8);
    if(*Stride==0)
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
 *    Reads a little endian 16 bit unsigned value without assuming alignment or
 *    host byte order.
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
 *    Reads a little endian 32 bit unsigned value without assuming alignment or
 *    host byte order.
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
 *    bool WavPlayStart(struct PlaybackSample *sample);
 *
 * PARAMETERS:
 *    sample [I] -- The sample to work on
 *
 * FUNCTION:
 *    This function starts a sample playing.  It needs to be stopped before
 *    starting the new sample.
 *
 * RETURNS:
 *    true -- The sample has been started
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool WavPlayStart(struct PlaybackSample *sample)
{
    struct spa_pod_builder Builder;
    uint8_t BuilderBuffer[1024];
    const struct spa_pod *Params[1];

    if(sample->Stream!=NULL || sample->Loop!=NULL)
        return false;

    sample->Position=0;
    sample->Draining=false;
    sample->Finished=false;

    sample->Loop=pw_thread_loop_new("wavplay",NULL);
    if(sample->Loop==NULL)
        return false;

    sample->Stream=pw_stream_new_simple(
            pw_thread_loop_get_loop(sample->Loop),"wavplay",
            pw_properties_new(
                    PW_KEY_MEDIA_TYPE,"Audio",
                    PW_KEY_MEDIA_CATEGORY,"Playback",
                    PW_KEY_MEDIA_ROLE,"Music",
                    NULL),
            &m_StreamEvents,
            sample);
    if(sample->Stream==NULL)
    {
        pw_thread_loop_destroy(sample->Loop);
        sample->Loop=NULL;
        return false;
    }

    spa_pod_builder_init(&Builder,BuilderBuffer,sizeof(BuilderBuffer));
    Params[0]=spa_format_audio_raw_build(&Builder,SPA_PARAM_EnumFormat,
            &sample->Info);

    if(pw_stream_connect(sample->Stream,PW_DIRECTION_OUTPUT,PW_ID_ANY,
            (pw_stream_flags)(PW_STREAM_FLAG_AUTOCONNECT |
                    PW_STREAM_FLAG_MAP_BUFFERS | PW_STREAM_FLAG_RT_PROCESS),
                    Params,1)<0)
    {
        pw_stream_destroy(sample->Stream);
        pw_thread_loop_destroy(sample->Loop);
        sample->Stream=NULL;
        sample->Loop=NULL;
        return false;
    }

    if(pw_thread_loop_start(sample->Loop)<0)
    {
        pw_stream_destroy(sample->Stream);
        pw_thread_loop_destroy(sample->Loop);
        sample->Stream=NULL;
        sample->Loop=NULL;
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    WavPlayStop
 *
 * SYNOPSIS:
 *    void WavPlayStop(struct PlaybackSample *sample);
 *
 * PARAMETERS:
 *    sample [I] -- The sample to work on
 *
 * FUNCTION:
 *    This function stops playing back a sample (and frees the playback eng
 *    for this sample)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void WavPlayStop(struct PlaybackSample *sample)
{
    if(sample->Stream==NULL)
        return;

    /* Stopping the loop joins its thread, so no callback can run after it */
    pw_thread_loop_stop(sample->Loop);
    pw_stream_destroy(sample->Stream);
    pw_thread_loop_destroy(sample->Loop);

    sample->Stream=NULL;
    sample->Loop=NULL;
}

/******************************************************************************
 * NAME:
 *    WavPlayProcessCb
 *
 * SYNOPSIS:
 *    static void WavPlayProcessCb(void *UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- The s_WavPlayState for the stream being served.
 *
 * FUNCTION:
 *    The PipeWire "process" callback.  It runs on the loop thread each time the
 *    graph needs more audio.  It copies the next slice of PCM into the supplied
 *    buffer.  When the last slice has been queued it asks the stream to drain
 *    so the tail of the sample is actually heard before playback stops.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    WavPlayDrainedCb()
 ******************************************************************************/
static void WavPlayProcessCb(void *UserData)
{
    struct PlaybackSample *sample=(struct PlaybackSample *)UserData;
    struct pw_buffer *PwBuffer;
    struct spa_buffer *SpaBuffer;
    uint8_t *Dest;
    uint32_t Request;
    uint32_t Size;
    uint32_t Available;

    PwBuffer=pw_stream_dequeue_buffer(sample->Stream);
    if(PwBuffer==NULL)
        return;

    SpaBuffer=PwBuffer->buffer;
    Dest=(uint8_t *)SpaBuffer->datas[0].data;
    if(Dest==NULL)
    {
        pw_stream_queue_buffer(sample->Stream,PwBuffer);
        return;
    }

    /* how much the graph wants this cycle, clamped to the buffer size */
    Request=(uint32_t)(PwBuffer->requested*sample->Stride);
    if(Request==0)
        Request=SpaBuffer->datas[0].maxsize;

    Size=Request;
    if(Size>SpaBuffer->datas[0].maxsize)
        Size=SpaBuffer->datas[0].maxsize;

    Available=sample->PcmSize-sample->Position;
    if(Size>Available)
        Size=Available;

    if(Size>0)
    {
        memcpy(Dest,sample->PcmStart+sample->Position,Size);
        sample->Position+=Size;
    }

    SpaBuffer->datas[0].chunk->offset=0;
    SpaBuffer->datas[0].chunk->stride=(int32_t)sample->Stride;
    SpaBuffer->datas[0].chunk->size=Size;

    pw_stream_queue_buffer(sample->Stream,PwBuffer);

    if(sample->Position>=sample->PcmSize && !sample->Draining)
    {
        sample->Draining=true;
        pw_stream_flush(sample->Stream,true);
    }
}

/******************************************************************************
 * NAME:
 *    WavPlayDrainedCb
 *
 * SYNOPSIS:
 *    static void WavPlayDrainedCb(void *UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- The s_WavPlayState for the stream being served.
 *
 * FUNCTION:
 *    The PipeWire "drained" callback.  It fires once the buffered tail of the
 *    sample has played out.  It marks the sample finished and wakes any thread
 *    blocked in WavPlayWait().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    WavPlayProcessCb(), WavPlayWait()
 ******************************************************************************/
static void WavPlayDrainedCb(void *UserData)
{
    struct PlaybackSample *sample=(struct PlaybackSample *)UserData;

    sample->Finished=true;
    pw_thread_loop_signal(sample->Loop,false);
}

/******************************************************************************
 * NAME:
 *    WavPlayStateChangedCb
 *
 * SYNOPSIS:
 *    static void WavPlayStateChangedCb(void *UserData,
 *    enum pw_stream_state Old,enum pw_stream_state New,const char *Error);
 *
 * PARAMETERS:
 *    UserData [I] -- The s_WavPlayState for the stream being served.
 *    Old [I] -- The previous stream state (not used).
 *    New [I] -- The new stream state.
 *    Error [I] -- An error string when New is the error state (not used).
 *
 * FUNCTION:
 *    The PipeWire "state_changed" callback.  If the stream lands in an error or
 *    unconnected state the sample can never finish on its own, so it is marked
 *    finished here and any waiter is woken.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    WavPlayDrainedCb()
 ******************************************************************************/
static void WavPlayStateChangedCb(void *UserData,enum pw_stream_state Old,
        enum pw_stream_state New,const char *Error)
{
    struct PlaybackSample *sample=(struct PlaybackSample *)UserData;

    if(New==PW_STREAM_STATE_ERROR || New==PW_STREAM_STATE_UNCONNECTED)
    {
        sample->Finished=true;
        pw_thread_loop_signal(sample->Loop,false);
    }
}
