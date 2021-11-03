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

#ifndef QUEUEDVIDEODECODER_H
#define QUEUEDVIDEODECODER_H

#include "queuedDecoder.h"

#include "types.h"

class QueuedVideoDecoder : public QueuedDecoder<VideoFrame>
{
public:
    QueuedVideoDecoder();

    ~QueuedVideoDecoder();

    virtual void clear();
    int frameWidth() const { return m_frame_width;}
    int frameHeight() const { return m_frame_height; }


protected:
    virtual void processPacket(AVPacket* packet, int* readed_frames);

private:
    //! Init scale context.
    void initSwsContext(AVFrame* frame);

    //! Clear scale context.
    void clearSwsContext();

protected:
    //! Scale context.
    SwsContext* m_sws_context;
    //! RGB frame used for conversion.
    AVFrame*    m_frame_RGB;
    //! Temp buffer size.
    int         m_buffer_size;
    //! Temp buffer for conversion.
    uint8_t*    m_buffer;
    int m_frame_width, m_frame_height;
};

class MetadataDecoder : public QueuedVideoDecoder
{
public:
    MetadataDecoder(QueuedVideoDecoder* vc) { m_decoder = vc; }
protected:
    virtual void processPacket(AVPacket* packet, int* readed_frames);

private:
    //! Try to parse metadata
    QImage parseMetadata(const unsigned char* buffer, size_t bytes);
    QueuedVideoDecoder* m_decoder;
};

#endif // QUEUEDVIDEODECODER_H
