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

#ifndef VIDEOPLAYBACK_H
#define VIDEOPLAYBACK_H

#include "basePlayback.h"

#include "ffmpeg.h"
#include "types.h"
#include "videoContext.h"
#include "decoder.h"
#include "videoFrameWidget.h"

//! Main video system.
/*!
 * \brief This class is responsible for video playback.
 */
class VideoPlayback : public BasePlayback
{
private:
    Q_OBJECT

public:
    VideoPlayback();

    ~VideoPlayback();

    //! Set video context. VideoContext used to get delay from it.
    void setVideoContext(VideoContext* video_context) { m_video_context = video_context; }

    //! Set decoder to read from.
    void setVideoDecoder(Decoder<VideoFrame>* video_decoder, Decoder<VideoFrame>* meta_decoder) { 
        m_video_decoder = video_decoder; 
        m_metadata_decoder = meta_decoder;
    }

    //! Set widget to draw on.
    void setVideoWidget(VideoFrameWidget* video_widget);

    virtual void start();

    virtual void pause();

    virtual void resume();

    virtual void stop();

    virtual void startAndPause();

    virtual void clear();

    virtual int getPlayingTime() const;

public slots:
    //! Synchronize video to audio by audio timestamps.
    void syncWithAudio(BasePlayback* playback);

protected:
    //! Timer event to draw new frame.
    virtual void timerEvent(QTimerEvent* event);

private:
    //! Draw new frame.
    void showFrame(bool single_frame = false);

private:
    //! VideoContext.
    VideoContext*           m_video_context;
    //! Decoder that provides data.
    Decoder<VideoFrame>*    m_video_decoder;
    Decoder<VideoFrame>*    m_metadata_decoder;
    //! Widget to draw on.
    VideoFrameWidget*       m_video_widget;
    //! Timer till next frame drawing.
    int                     m_timer;
    //! Currently drawing frame.
    VideoFrame              m_current_frame;
    //! Current delay till next frame.
    int                     m_current_delay;
    //! Image with metadata overlay information
    VideoFrame              m_overlay;
};

#endif // VIDEOPLAYBACK_H
