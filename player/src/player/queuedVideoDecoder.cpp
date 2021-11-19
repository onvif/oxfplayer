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

#include "queuedVideoDecoder.h"

#include "ffmpeg.h"
#include "avFrameWrapper.h"

#include <QDebug>

QueuedVideoDecoder::QueuedVideoDecoder() :
    QueuedDecoder<VideoFrame>(AVMEDIA_TYPE_VIDEO),
    m_sws_context(0),
    m_frame_RGB(0),
    m_buffer_size(-1),
    m_buffer(0)
{

}

QueuedVideoDecoder::~QueuedVideoDecoder()
{
}

void QueuedVideoDecoder::clear()
{
    QueuedDecoder<VideoFrame>::clear();
    clearSwsContext();
}

void QueuedVideoDecoder::setStream(int index, double fps)
{
    if (index >= 0) m_streamIndex = index;
    m_context.clear();
    m_context.open(getStream(m_streamIndex), fps);
    m_stream = getStream(m_streamIndex);

}

void QueuedVideoDecoder::processPacket(AVPacket* packet, int* readed_frames)
{
    AVFrame* frame = av_frame_alloc();

    avcodec_send_packet(m_stream->codec, packet);

    if (avcodec_receive_frame(m_stream->codec, frame) == 0)
    {
        //create frame
        VideoFrame video_frame;
        video_frame.calcTime(packet->pts, m_stream->time_base);

        //do we need to skip or not?
        if(m_skip_threshold == -1 ||
           video_frame.m_selected_pts >= m_skip_threshold)
        {
            //conver frame to RGB frame and create image
            if(m_sws_context == nullptr)
                initSwsContext(frame);

            if(m_sws_context != nullptr)
            {
                //scale image
                sws_scale(m_sws_context, (uint8_t**)frame->data, frame->linesize, 0, frame->height, m_frame_RGB->data, m_frame_RGB->linesize);

                //conert to image
                QImage image(frame->width, frame->height, QImage::Format_RGB32);
                for(int y = 0; y < frame->height; ++y)
                    memcpy(image.scanLine(y), m_frame_RGB->data[0] + y * m_frame_RGB->linesize[0], frame->width * 4);

                //fill other fields
                video_frame.m_image = image;

                //put into queue
                m_queue.push(video_frame);
                ++(*readed_frames);
            }
        }
        else
            qDebug() << "Skipping due to threshold";
    }
    av_frame_unref(frame);
}

void QueuedVideoDecoder::initSwsContext(AVFrame* frame)
{
    //create scale context
    m_sws_context = sws_getCachedContext(0,
                                         frame->width, frame->height, (AVPixelFormat)frame->format,
                                         frame->width, frame->height, AV_PIX_FMT_RGB32,
                                         SWS_BICUBIC, 0, 0, 0);

    //create RGB frame
    m_frame_RGB = av_frame_alloc();

    //allocate buffer for RGBFrame
    m_buffer_size = avpicture_get_size(AV_PIX_FMT_RGB32, frame->width, frame->height);
    m_buffer = (uint8_t*)av_malloc(m_buffer_size * sizeof(uint8_t));

    //assign buffer with frame
    avpicture_fill((AVPicture*)m_frame_RGB, m_buffer, AV_PIX_FMT_RGB32, frame->width, frame->height);
}

void QueuedVideoDecoder::clearSwsContext()
{
    if(m_sws_context != nullptr)
    {
        av_free(m_buffer);
        av_frame_free(&m_frame_RGB);
        sws_freeContext(m_sws_context);
        m_sws_context = 0;
        m_frame_RGB = 0;
        m_buffer_size = -1;
        m_buffer = 0;
    }
}
