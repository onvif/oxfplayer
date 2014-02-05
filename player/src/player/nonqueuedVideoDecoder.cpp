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

#include "nonqueuedVideoDecoder.h"

#include "avFrameWrapper.h"

#include <QDebug>

NonQueuedVideoDecoder::NonQueuedVideoDecoder() :
    NonQueuedDecoder<VideoFrame>(),
    m_sws_context(0),
    m_frame_RGB(0),
    m_buffer_size(-1),
    m_buffer(0)
{
}

NonQueuedVideoDecoder::~NonQueuedVideoDecoder()
{
}

void NonQueuedVideoDecoder::clear()
{
    NonQueuedDecoder<VideoFrame>::clear();
    m_widget_size = QSize();
    m_image_size = QSize();
    clearSwsContext();
}

bool NonQueuedVideoDecoder::processPacket(AVPacket* packet, AVStream* stream, VideoFrame& decoded_frame, void* additional_data)
{
    QSize widget_size;
    if(additional_data)
        widget_size = *((QSize*)additional_data);

    AVFrameWrapperPtr frame_wrapper(new AVFrameWrapper());
    AVFrame* frame = frame_wrapper->get();

    bool ret = false;
    int got_frame = 0;
    avcodec_decode_video2(stream->codec, frame, &got_frame, packet);
    if(got_frame)
    {
        decoded_frame.calcTime(frame->pkt_dts, frame->pkt_pts, stream->time_base);

        //do we need to skip or not?
        if(m_skip_threshold == -1 ||
           decoded_frame.m_selected_pts >= m_skip_threshold)
        {
            //conver frame to RGB frame and create image
            if(m_sws_context == nullptr &&
               additional_data)
                initSwsContext(frame, widget_size);
            else if(m_sws_context != nullptr &&
                    additional_data &&
                    widget_size != m_widget_size)
            {
                clearSwsContext();
                initSwsContext(frame, widget_size);
            }

            if(m_sws_context != nullptr)
            {
                //scale image
                sws_scale(m_sws_context, (uint8_t**)frame->data, frame->linesize, 0, frame->height, m_frame_RGB->data, m_frame_RGB->linesize);

                //conert to image
                QImage image(m_image_size.width(), m_image_size.height(), QImage::Format_RGB32);
                for(int y = 0; y < m_image_size.height(); ++y)
                    memcpy(image.scanLine(y), m_frame_RGB->data[0] + y * m_frame_RGB->linesize[0], m_image_size.width() * 4);

                //fill other fields
                decoded_frame.m_image = image;
            }

            ret = true;
        }
        else
            qDebug() << "Skipping due to threshold";
    }

    return ret;
}

void NonQueuedVideoDecoder::initSwsContext(AVFrame* frame, const QSize& widget_size)
{
    //save new size
    m_widget_size = widget_size;

    //calc image size
    m_image_size = QSize(frame->width, frame->height);
    m_image_size.scale(m_widget_size, Qt::KeepAspectRatio);

    //create scale context
    m_sws_context = sws_getCachedContext(0,
                                         frame->width, frame->height, (AVPixelFormat)frame->format,
                                         m_image_size.width(), m_image_size.height(), PIX_FMT_RGB32,
                                         SWS_BICUBIC, 0, 0, 0);

    //create RGB frame
    m_frame_RGB = avcodec_alloc_frame();

    //allocate buffer for RGBFrame
    m_buffer_size = avpicture_get_size(PIX_FMT_RGB32, m_image_size.width(), m_image_size.height());
    m_buffer = (uint8_t*)av_malloc(m_buffer_size * sizeof(uint8_t));

    //assign buffer with frame
    avpicture_fill((AVPicture*)m_frame_RGB, m_buffer, PIX_FMT_RGB32, m_image_size.width(), m_image_size.height());
}

void NonQueuedVideoDecoder::clearSwsContext()
{
    if(m_sws_context != nullptr)
    {
        av_free(m_buffer);
        avcodec_free_frame(&m_frame_RGB);
        sws_freeContext(m_sws_context);
        m_sws_context = 0;
        m_frame_RGB = 0;
        m_buffer_size = -1;
        m_buffer = 0;
        m_widget_size = QSize();
        m_image_size = QSize();
    }
}
