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

#include "nonqueuedAudioDecoder.h"

#include "avFrameWrapper.h"

#include <QDebug>

NonQueuedAudioDecoder::NonQueuedAudioDecoder() :
    NonQueuedDecoder<AudioFrame>(),
    m_swr_context(nullptr)
{
}

NonQueuedAudioDecoder::~NonQueuedAudioDecoder()
{
}

bool NonQueuedAudioDecoder::getNextFrame(AudioFrame& decoded_frame, void* additional_data)
{
    decoded_frame.clear();

    bool ret = false;
    if(m_frames.size())
    {
        decoded_frame = m_frames.dequeue();
        ret = true;
    }
    else
    {
        //do some decode
        ret = NonQueuedDecoder<AudioFrame>::getNextFrame(decoded_frame, additional_data);
        if(ret)
            decoded_frame = m_frames.dequeue();
    }

    return ret;
}

void NonQueuedAudioDecoder::clearBuffers()
{
    NonQueuedDecoder<AudioFrame>::clearBuffers();
    m_frames.clear();
}

void NonQueuedAudioDecoder::clear()
{
    NonQueuedDecoder<AudioFrame>::clear();
    clearSwrContext();
    m_frames.clear();
    m_audio_params = AudioParams();
}

bool NonQueuedAudioDecoder::processPacket(AVPacket* packet, AVStream* stream, AudioFrame& decoded_frame, void* additional_data)
{
    Q_UNUSED(decoded_frame);
    Q_UNUSED(additional_data);

    AVFrameWrapperPtr frame_wrapper(new AVFrameWrapper());
    AVFrame* frame = frame_wrapper->get();

    bool ret = false;

    int packet_size = packet->size;

    while(packet_size > 0)
    {
        av_frame_unref(frame);

        int got_frame = 0;
        int len1 = avcodec_decode_audio4(stream->codec, frame, &got_frame, packet);
        if(len1 > 0 &&
           got_frame)
        {
            packet_size -= len1;

            int data_size = av_samples_get_buffer_size(0, frame->channels, frame->nb_samples, (AVSampleFormat)frame->format, 1);

            if(data_size > 0)
            {
                AudioFrame audio_frame;
                audio_frame.calcTime(packet->pts, stream->time_base);

                if(m_skip_threshold == -1 ||
                   audio_frame.m_selected_pts >= m_skip_threshold)
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
                        int out_count = (int64_t)frame->nb_samples * m_audio_params.m_freq / frame->sample_rate + 256;
                        int out_size  = av_samples_get_buffer_size(NULL, m_audio_params.m_channels, out_count, m_audio_params.m_fmt, 0);

                        av_fast_malloc(&out_buffer, &outBufferSize, out_size);

                        int len2 = swr_convert(m_swr_context, out, out_count, in, frame->nb_samples);

                        if(len2 > 0 &&
                           len2 != out_count)
                        {
                            int new_data_size = len2 * m_audio_params.m_channels * m_audio_params.m_fmt_size;
                            audio_frame.m_data = QByteArray((const char*)out_buffer, new_data_size);
                            m_frames.enqueue(audio_frame);
                            ret = true;
                        }

                        av_freep(&out_buffer);
                    }
                }
                else
                    qDebug() << "Skipping due to threshold";
            }
        }
        else
            break;
    }

    return ret;
}

void NonQueuedAudioDecoder::initSwrContext(AVFrame* frame)
{
    m_swr_context = swr_alloc_set_opts(0,
                                       m_audio_params.m_channel_layout, m_audio_params.m_fmt, m_audio_params.m_freq,
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

void NonQueuedAudioDecoder::clearSwrContext()
{
    if(m_swr_context != nullptr)
    {
        swr_free(&m_swr_context);
        m_swr_context = nullptr;
    }
}
