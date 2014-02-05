/************************************************************************************
* Copyright (c) 2013 ONVIF.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*    * Neither the name of ONVIF nor the names of its contributors may be
*      used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ONVIF BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************/

#include "portAudioThread.h"

#include "defines.h"

template<typename T>
void modifyVolumeLevel(QByteArray& data, double factor)
{
    T* ptr = (T*)data.data();
    int count = data.size() / sizeof(T);
    double value = 0.0;
    for(int i = 0; i < count; ++i, ++ptr)
    {
        value = (double)(*ptr);
        value *= factor;
        *ptr = (T)value;
    }
}

PortAudioThread::PortAudioThread() :
    SyncThread(0),
    m_audio_decoder(nullptr),
    m_is_initialized(false),
    m_stream(0),
    m_current_time(0),
    m_sent_time(-1),
    m_volume(1.0)
{
    PaError error = Pa_Initialize();
    m_is_initialized = (error == paNoError &&
                        Pa_GetDefaultOutputDevice() != paNoDevice);
}

PortAudioThread::~PortAudioThread()
{
    stop();
    Pa_Terminate();
}

void PortAudioThread::setAudioDecoder(Decoder<AudioFrame>* audio_decoder)
{
    stop();
    m_audio_decoder = audio_decoder;
}

void PortAudioThread::setAudioParams(const AudioParams& audio_params)
{
    stop();
    m_audio_params = audio_params;
}

void PortAudioThread::start()
{
    if(!m_is_initialized ||
       m_audio_decoder == nullptr)
        return;

    PaError error = Pa_OpenDefaultStream(&m_stream,
                                         0, m_audio_params.m_channels,
                                         getFormat(),
                                         m_audio_params.m_freq,
                                         paFramesPerBufferUnspecified,
                                         0, 0);
    if(error != paNoError)
        return;

    Pa_StartStream(m_stream);

    m_current_time = 0;
    m_sent_time = -1;

    SyncThread::start();
}

void PortAudioThread::stop()
{
    SyncThread::stop();

    if(m_stream)
    {
        Pa_StopStream(m_stream);
        Pa_CloseStream(m_stream);
        m_stream = 0;
    }

    m_sent_time = -1;
}

bool PortAudioThread::threadBody()
{
    AudioFrame audio_data;
    if(m_audio_decoder->getNextFrame(audio_data))
    {
        m_current_time = audio_data.m_time;
        if(m_sent_time == -1 ||
           audio_data.m_time - m_sent_time >= AUDIO_NOTIFY_TIMEOUT)
        {
            m_sent_time = audio_data.m_time;
            emit played();
        }
        if(m_volume != 1.0)
        {
            if(m_volume == 0.0)
                memset(audio_data.m_data.data(), 0, audio_data.m_data.size());
            else
            {
                double factor = -1.0 * log10(1.0 - m_volume);

                PaSampleFormat sample_format = getFormat();
                if(sample_format == paInt16)
                    modifyVolumeLevel<qint16>(audio_data.m_data, factor);
                else if(sample_format == paInt32)
                    modifyVolumeLevel<qint32>(audio_data.m_data, factor);
                else if(sample_format == paUInt8)
                    modifyVolumeLevel<quint8>(audio_data.m_data, factor);
                else if(sample_format == paFloat32)
                    modifyVolumeLevel<float>(audio_data.m_data, factor);
            }
        }
        Pa_WriteStream(m_stream, audio_data.m_data.data(), audio_data.m_data.size() / m_audio_params.m_channels / m_audio_params.m_fmt_size);
        return true;
    }

    emit playbackFinished();

    return false;
}

void PortAudioThread::threadFinished()
{
    if(m_stream)
    {
        Pa_StopStream(m_stream);
        Pa_CloseStream(m_stream);
        m_stream = 0;
    }
}

PaSampleFormat PortAudioThread::getFormat()
{
    PaSampleFormat sample_format = paInt16;
    if(m_audio_params.m_fmt == AV_SAMPLE_FMT_S16)
        sample_format = paInt16;
    else if(m_audio_params.m_fmt == AV_SAMPLE_FMT_S32)
        sample_format = paInt32;
    else if(m_audio_params.m_fmt == AV_SAMPLE_FMT_U8)
        sample_format = paUInt8;
    else if(m_audio_params.m_fmt == AV_SAMPLE_FMT_FLT)
        sample_format = paFloat32;
    return sample_format;
}
