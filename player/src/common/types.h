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

#ifndef TYPES_H
#define TYPES_H

#include "ffmpeg.h"

#include <QImage>
#include <QByteArray>

//! Structure that describes audio format.
/*!
 * \brief This structure describes format acceptable for hardware.
 *        This parameters used to convert audio data from file format to hardware format.
 */
struct AudioParams
{
    //! Frequency.
    int             m_freq;
    //! Channels count.
    int             m_channels;
    //! Channel layout.
    int64_t         m_channel_layout;
    //! Format of audio data.
    AVSampleFormat  m_fmt;
    //! Size of one sample in specified format.
    int64_t         m_fmt_size;

    AudioParams() :
        m_freq(0),
        m_channels(0),
        m_channel_layout(0),
        m_fmt(AV_SAMPLE_FMT_NONE),
        m_fmt_size(0)
    {}
};

//! Base structure for decoded data.
struct DecodedFrame
{
    DecodedFrame() :
        m_time(0),
        m_selected_pts(0)
    {}

    virtual ~DecodedFrame()
    {}

    //! Present time.
    int         m_time;

    //! Selected pts. Calculated used pkt_pts or pkt_dts.
    int64_t     m_selected_pts;

    //! Calculate presentation time.
    void calcTime(int64_t pkt_dts, int64_t pkt_pts, AVRational time_base)
    {
        m_selected_pts = pkt_pts;
        if(m_selected_pts == 0)
            m_selected_pts = pkt_dts;
        if(m_selected_pts == AV_NOPTS_VALUE)
            m_selected_pts = 0;
        m_time = (int)((double)m_selected_pts * av_q2d(time_base) * 1000.0);
    }

    //! Get occupied memory.
    virtual int size() const = 0;

    //! Clear frame.
    virtual void clear()
    {
        m_time = 0;
        m_selected_pts = 0;
    }
};

//! Structure that describes one decoded video frame.
struct VideoFrame : public DecodedFrame
{
    //! Qt image containing decode video frame.
    QImage      m_image;

    VideoFrame() :
        DecodedFrame()
    {}

    virtual int size() const
    {
        return sizeof(VideoFrame) + m_image.byteCount();
    }

    virtual void clear()
    {
        DecodedFrame::clear();
        m_image = QImage();
    }
};

//! Structure that describes one decoded audio frame.
struct AudioFrame : public DecodedFrame
{
    //! Decoded data.
    QByteArray  m_data;

    AudioFrame() :
        DecodedFrame()
    {}

    int size() const
    {
        return sizeof(AudioFrame) + m_data.size();
    }

    virtual void clear()
    {
        DecodedFrame::clear();
        m_data.clear();
    }
};

#endif // TYPES_H
