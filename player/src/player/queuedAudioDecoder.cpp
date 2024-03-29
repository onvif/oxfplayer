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

#include "queuedAudioDecoder.h"

#include "ffmpeg.h"
#include "avFrameWrapper.h"

#include <QDebug>

QueuedAudioDecoder::QueuedAudioDecoder() :
    QueuedDecoder<AudioFrame>(AVMEDIA_TYPE_AUDIO)
    , m_swr_context(nullptr)
{
}

QueuedAudioDecoder::~QueuedAudioDecoder()
{
}

void QueuedAudioDecoder::clear()
{
    QueuedDecoder<AudioFrame>::clear();
    cleatSwrContext();
}

void QueuedAudioDecoder::setIndex(int index)
{
    if (index >= getStreamsCount()) return;
    if (index < 0) index = m_streamIndex;
    m_context.clear();
    if (m_streamIndex >= 0) {
        m_context.init(getCodecContext(m_streamIndex));
        m_stream = getStream(m_streamIndex);
    }
}

void QueuedAudioDecoder::processPacket(AVPacket* packet, int timestamp_ms)
{
    AVFrame* frame = av_frame_alloc();

    avcodec_send_packet(m_stream->codec, packet);

    while (avcodec_receive_frame(m_stream->codec, frame) == 0)
    {
        int data_size = av_samples_get_buffer_size(0, frame->channels, frame->nb_samples, (AVSampleFormat)frame->format, 1);

        if(data_size > 0)
        {
            if(timestamp_ms >= lastSeekTime())
            {
                //resample audio
                if(m_swr_context == nullptr)
                    initSwrContext(frame);

                if(m_swr_context != nullptr)
                {
                    const uint8_t **in = (const uint8_t **)frame->extended_data;
                    uint8_t* out_buffer = 0;
                    unsigned int outBufferSize = 0;
                    uint8_t **out = &out_buffer;
                    int out_count = (int64_t)frame->nb_samples * m_context.m_audio_params.m_freq / frame->sample_rate + 256;
                    int out_size  = av_samples_get_buffer_size(NULL, m_context.m_audio_params.m_channels, out_count, m_context.m_audio_params.m_fmt, 0);

                    av_fast_malloc(&out_buffer, &outBufferSize, out_size);

                    int len2 = swr_convert(m_swr_context, out, out_count, in, frame->nb_samples);

                    if(len2 > 0 &&
                        len2 != out_count)
                    {
                        int new_data_size = len2 * m_context.m_audio_params.m_channels * m_context.m_audio_params.m_fmt_size;
                        AudioFrame audio_frame(timestamp_ms);
                        audio_frame.m_data = QByteArray((const char*)out_buffer, new_data_size);
                        m_queue.push(audio_frame);
                    }

                    av_freep(&out_buffer);
                }
            }
            else
                qDebug() << "Skipping due to threshold";
        }
    }
    av_frame_unref(frame);
}

void QueuedAudioDecoder::initSwrContext(AVFrame* frame)
{
    m_swr_context = swr_alloc_set_opts(0,
        m_context.m_audio_params.m_channel_layout, m_context.m_audio_params.m_fmt, m_context.m_audio_params.m_freq,
                                       frame->channel_layout, (AVSampleFormat)frame->format, frame->sample_rate,
                                       0, 0);
    if(m_swr_context != nullptr)
    {
       if(swr_init(m_swr_context) < 0)
       {
           swr_free(&m_swr_context);
           m_swr_context = 0;
       }
    }
}

void QueuedAudioDecoder::cleatSwrContext()
{
    if(m_swr_context != nullptr)
    {
        swr_free(&m_swr_context);
        m_swr_context = nullptr;
    }
}
