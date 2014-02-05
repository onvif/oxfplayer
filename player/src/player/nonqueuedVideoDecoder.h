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

#ifndef NONQUEUEDVIDEODECODER_H
#define NONQUEUEDVIDEODECODER_H

#include "nonqueuedDecoder.h"

#include "types.h"

class NonQueuedVideoDecoder : public NonQueuedDecoder<VideoFrame>
{
public:
    NonQueuedVideoDecoder();

    ~NonQueuedVideoDecoder();

    virtual void clear();

protected:
    virtual bool processPacket(AVPacket* packet, AVStream* stream, VideoFrame& decoded_frame, void* additional_data = 0);

private:
    //! Init scale context.
    void initSwsContext(AVFrame* frame, const QSize& widget_size);

    //! Clear scale context.
    void clearSwsContext();

private:
    //! Previous size of window.
    QSize               m_widget_size;
    //! Size of picture - frame size scaled to widget size.
    QSize               m_image_size;
    //! Scale context.
    SwsContext*         m_sws_context;
    //! RGB frame used for conversion.
    AVFrame*            m_frame_RGB;
    //! Temp buffer size.
    int                 m_buffer_size;
    //! Temp buffer for conversion.
    uint8_t*            m_buffer;
};

#endif // NONQUEUEDVIDEODECODER_H
