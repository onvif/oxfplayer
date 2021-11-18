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

#include "mainContext.h"

#include "audioContext.h"

#include <qmediadevices.h>
#include <qaudiodevice.h>

AudioContext::AudioContext()
{
    clear();
}

AudioContext::~AudioContext()
{
    clear();
}

bool AudioContext::open(AVCodecContext* codecContext)
{
    AVCodecContext* audio_codec_context = codecContext;
    if(audio_codec_context == nullptr)
        return false;

    QAudioDevice default_device_info = QMediaDevices::defaultAudioOutput();
    QAudioFormat audio_format = default_device_info.preferredFormat();
    audio_format.setSampleRate(audio_codec_context->sample_rate);
    audio_format.setChannelCount(audio_codec_context->channels);
    if(!default_device_info.isFormatSupported(audio_format))
        audio_format = default_device_info.preferredFormat();

    //fill target audio params
    m_audio_params.m_freq = audio_format.sampleRate();
    m_audio_params.m_channels = audio_format.channelCount();
    m_audio_params.m_channel_layout = av_get_default_channel_layout(m_audio_params.m_channels);
    switch(audio_format.sampleFormat())
    {
    case QAudioFormat::Int16:
        m_audio_params.m_fmt = AV_SAMPLE_FMT_S16;
    case QAudioFormat::Int32:
        m_audio_params.m_fmt = AV_SAMPLE_FMT_S32;
        break;
    case QAudioFormat::UInt8:
        m_audio_params.m_fmt = AV_SAMPLE_FMT_U8;
        break;
    case QAudioFormat::Float:
        m_audio_params.m_fmt = AV_SAMPLE_FMT_FLT;
        break;
    default:
        break;
    }
    if(m_audio_params.m_fmt == AV_SAMPLE_FMT_NONE)
    {
        clear();
        return false;
    }
    m_audio_params.m_fmt_size = av_get_bytes_per_sample(m_audio_params.m_fmt);

    return true;
}

void AudioContext::clear()
{
    m_audio_params = AudioParams();
}
