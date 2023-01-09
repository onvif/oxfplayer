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

#include "videoPlayback.h"

#include <QTimerEvent>

#include <QDebug>

VideoPlayback::VideoPlayback() :
    BasePlayback(),
    m_video_context(nullptr),
    m_video_decoder(nullptr),
    m_video_widget(nullptr),
    m_timer(-1),
    m_current_delay(-1)
{
}

VideoPlayback::~VideoPlayback()
{
    stop();
    clear();
}

void VideoPlayback::setVideoWidget(VideoFrameWidget* video_widget, QTreeWidget* event_widget)
{
    m_video_widget = video_widget;
    m_event_widget = event_widget;
    if(m_video_widget != nullptr)
        m_video_widget->setDrawImage(m_current_frame.m_image);
}

void VideoPlayback::start()
{
    if(m_video_context == nullptr ||
       m_video_decoder == nullptr ||
       m_video_widget == nullptr ||
       m_is_playing)
        return;

    showFrame();
    m_current_delay = m_video_context->getTimerDelay();
    m_timer = startTimer(m_current_delay ,Qt::PreciseTimer);
    m_overlay.clear();

    m_is_playing = true;
}

void VideoPlayback::pause()
{
    if(m_video_context == nullptr ||
       m_video_decoder == nullptr ||
       m_video_widget == nullptr ||
       !m_is_playing)
        return;

    if(m_timer != -1)
    {
        killTimer(m_timer);
        m_timer = -1;
    }

    m_is_playing = false;
}

void VideoPlayback::resume()
{
    if(m_video_context == nullptr ||
       m_video_decoder == nullptr ||
       m_video_widget == nullptr ||
       m_is_playing)
        return;

    m_current_delay = m_video_context->getTimerDelay();
    m_timer = startTimer(m_current_delay ,Qt::PreciseTimer);

    m_is_playing = true;
}

void VideoPlayback::stop()
{
    if(m_video_context == nullptr ||
       m_video_decoder == nullptr ||
       m_video_widget == nullptr)
        return;

    if(m_timer != -1)
    {
        killTimer(m_timer);
        m_timer = -1;
    }

    m_current_frame = VideoFrame();
    m_overlay = VideoFrame();
    m_current_delay = -1;
    m_is_playing = false;

    m_video_widget->setDrawImage(m_current_frame.m_image);
    m_video_context->flushCurrentFps();
}

void VideoPlayback::startAndPause()
{
    if(m_video_context == nullptr ||
       m_video_decoder == nullptr ||
       m_video_widget == nullptr ||
       m_is_playing)
        return;

    showFrame(true);

    m_is_playing = false;
}

void VideoPlayback::clear()
{
    stop();
    m_video_context = nullptr;
    m_video_decoder = nullptr;
    m_video_widget = nullptr;
    if(m_timer != -1)
    {
        killTimer(m_timer);
        m_timer = -1;
    }
    m_current_frame = VideoFrame();
    m_overlay.clear();
    m_current_delay = -1;
    m_is_playing = false;
}

int VideoPlayback::getPlayingTime() const
{
    if(!m_current_frame.m_image.isNull())
        return m_current_frame.m_time;

    return 0;
}

void VideoPlayback::syncWithAudio(BasePlayback* playback)
{
    if(m_video_context == nullptr ||
       m_video_decoder == nullptr ||
       m_video_widget == nullptr)
        return;

    int audio_time = playback->getPlayingTime();

    qDebug() << "VideoPlayback::syncWithAudio" << audio_time;

    int video_time = m_current_frame.m_time;
    int delta = qAbs(video_time - audio_time);
    if(delta < m_video_context->getTimerDelay())
    {
        //do nothing - video equal to audio
    }
    else
    {
        if(video_time < audio_time)
        {
            //move video forward - ckip some frames to get frame time closer to audio time
            bool current_frame_changed = false;
            QSize widget_size = m_video_widget->size();
            while(true)
            {
                if(m_video_decoder->getNextFrame(m_current_frame, &widget_size))
                {
                    if(m_current_frame.m_time < audio_time)
                        qDebug() << "Skipping frame" << m_current_frame.m_time << audio_time;
                    else
                        //close enough
                        break;
                    current_frame_changed = true;
                }
                else
                {
                    //end of the video queue riched
                    qDebug() << "Video ended";
                    pause();
                    current_frame_changed = false;
                    emit playbackFinished();
                    break;
                }
            }
            if(current_frame_changed)
            {
                //new frame selected - show it
                m_video_widget->setDrawImage(m_current_frame.m_image);
                m_video_context->increaseFps();

                emit played(this);
            }
        }
        else
        {
            //slow down video
            qDebug() << "Slow FPS";
            m_video_context->decreaseFps();
        }
    }
}

void VideoPlayback::timerEvent(QTimerEvent* event)
{
    if(event->timerId() != m_timer)
        return;
    showFrame();
}

void VideoPlayback::showFrame(bool single_frame)
{
    //
    // Update events
    //
    while (!m_metadata_decoder->m_eventQueue.empty()) {
        auto _event = m_metadata_decoder->m_eventQueue.pop();
        for (int i = 0; i < m_event_widget->invisibleRootItem()->childCount(); i++) {
            auto ev = (EventItem*)m_event_widget->invisibleRootItem()->child(i);
            if (ev->hash == _event->hash) {     // skip duplicates
                delete _event; 
                _event = 0;
                break;
            }
        }
        if (_event) m_event_widget->addTopLevelItem(_event);
        else break;
    }
    //
    // Show video frame optionally with overlay
    //
    QSize widget_size = m_video_widget->size();
    VideoFrame frame;
    m_current_frame.clear();
    if (m_video_decoder->getNextFrame(frame, &widget_size))
    {
        //
        // Advance to nearest overlay
        //
        m_current_frame = frame;
        int delta = abs(m_overlay.m_time - m_current_frame.m_time);
        while (!m_metadata_decoder->m_queue.empty()) {
            int d = abs(m_metadata_decoder->m_queue.headTime() - m_current_frame.m_time);
            if (d > delta) break;
            m_overlay = m_metadata_decoder->m_queue.pop();
            delta = d;
        }
        //
        // Show overlay if within 500 milliseconds
        //
        if (delta < 500 && m_overlay) {
            int height = m_current_frame.m_image.height(), width = m_current_frame.m_image.width();
            int oheight = m_overlay.m_image.height(), owidth = m_overlay.m_image.width();
            if (height == oheight && width == owidth) {     // ensure that both have same size
                for (int y = 0; y < height; y++)
                {
                    uint* lmeta = (uint*)m_overlay.m_image.scanLine(y);
                    uint* lvideo = (uint*)m_current_frame.m_image.scanLine(y);
                    for (int x = 0; x < width; x++)
                    {
                        if (lmeta[x] & 0xffffff) lvideo[x] = lmeta[x];
                    }
                }
            }
        }
        m_video_widget->setDrawImage(m_current_frame.m_image);

        emit played(this);

        if(!single_frame)
        {
            //maybe we must change timeout
            if(m_video_context->getTimerDelay() != m_current_delay)
            {
                if(m_timer != -1)
                {
                    killTimer(m_timer);
                    m_timer = -1;
                }
                m_current_delay = m_video_context->getTimerDelay();
                m_timer = startTimer(m_current_delay ,Qt::PreciseTimer);
            }
        }
    }
    else
    {
        qDebug() << "Video ended";
        pause();
        emit playbackFinished();
    }
}
