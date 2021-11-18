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

#include "videoFrameWidget.h"

#include "defines.h"
#include "engine.h"

#include "queuedVideoDecoder.h"
#include "queuedAudioDecoder.h"

#include <QDebug>

Engine::Engine() :
    BasePlayback(),
    m_video_widget(nullptr),
    m_video_decoder(new QueuedVideoDecoder()),
    m_audio_decoder(new QueuedAudioDecoder()),
    m_is_initialized(false),
    m_player_state(Stopped),
    m_playing_time(0),
    m_selected_video_stream_index(0){
    QObject::connect(&m_video_playback, SIGNAL(played(BasePlayback*)), this, SIGNAL(played(BasePlayback*)));
    QObject::connect(&m_video_playback, SIGNAL(playbackFinished()), this, SLOT(onFinished()));
    //direct connection used to prevent receiving messages from previously opened file
    QObject::connect(&m_audio_playback, SIGNAL(played(BasePlayback*)), &m_video_playback, SLOT(syncWithAudio(BasePlayback*)), Qt::DirectConnection);
    QObject::connect(&m_audio_playback, SIGNAL(playbackFinished()), this, SLOT(onFinished()));
}

Engine::~Engine()
{
    stop();
    clear();
    delete m_video_decoder;
    delete m_audio_decoder;
}

/*********************************************************************************************/

void Engine::setVideoWidget(VideoFrameWidget* video_widget)
{
    m_video_widget = video_widget;
    m_video_playback.setVideoWidget(m_video_widget);
}

bool Engine::init(const QString& file_name, FragmentInfo& fragment)
{
	m_currentFragment = &fragment;
	bool res = true;
	res = res && initMainContext(file_name, fragment);
    res = res && initDecoders();
    res = res && initVideoPlayback();
    res = res && initAudioPlayback();
    res = res && m_video_decoder->getStreamsCount();

    m_is_initialized = res;

    return res;
}

void Engine::start()
{
    if(!m_is_initialized ||
       m_player_state != Stopped)
        return;

    bool have_video = m_video_decoder->getStreamsCount();
    bool have_audio = m_audio_decoder->getStreamsCount();

    if(!have_video)
        return;

    if(!have_audio)
    {
        //just video
        m_video_decoder->start();
        m_video_playback.start();
    }
    else
    {
        //video and audio - modify fps
        m_video_decoder->start();
        m_audio_decoder->start();
        m_video_playback.start();
        m_audio_playback.start();
    }

    m_player_state = Playing;
}

void Engine::pause()
{
    if(!m_is_initialized ||
       m_player_state != Playing)
        return;

    if(m_audio_decoder->getStreamsCount())
        m_audio_playback.pause();
    m_video_playback.pause();

    m_player_state = Paused;
}

void Engine::resume()
{
    if(!m_is_initialized ||
       m_player_state != Paused)
        return;

    if(m_audio_decoder->getStreamsCount())
        m_audio_playback.resume();
    m_video_playback.resume();

    m_player_state = Playing;
}

void Engine::stop()
{
    if(!m_is_initialized)
        return;

    //do stop actions
    stopPlayback();

    //seek to the beginning
    doSeek(0);

    m_player_state = Stopped;
}

void Engine::startAndPause()
{
    if(!m_is_initialized ||
       m_player_state != Stopped)
        return;

    bool have_video = m_video_decoder->getStreamsCount();
    bool have_audio = m_audio_decoder->getStreamsCount();

    if(!have_video)
        return;

    if(!have_audio)
    {
        //just video
        m_video_decoder->start();
        m_video_playback.startAndPause();
    }
    else
    {
        //video and audio
        if(m_video_context.getTimerDelay() > AUDIO_NOTIFY_TIMEOUT)
        {
            //vide fps is to low - connect video to audio by notify
            //TODO
        }
        else
        {
            //video is faster then audio - modify fps
            m_video_decoder->start();
            m_audio_decoder->start();
            m_video_playback.startAndPause();
            m_audio_playback.startAndPause();
        }
    }

    m_player_state = Paused;
}

void Engine::clear()
{
    clearDecoders();
    clearVideoPlayback();
    clearAudioPlayback();
    m_player_state = Stopped;
    m_playing_time = 0;
    m_selected_video_stream_index = 0;
}

int Engine::getPlayingTime() const
{
    if(m_player_state != Stopped)
        m_playing_time = m_video_playback.getPlayingTime();
    return m_playing_time;
}

void Engine::seek(int time_ms)
{
    if(!m_is_initialized)
        return;

    qDebug() << "Seek to" << time_ms;

    switch(m_player_state)
    {
    case Stopped:
        //seeking in stop state
        doSeek(time_ms);
        startAndPause();
        break;
    case Playing:
        //seeking in playing state
        stopPlayback();
        doSeek(time_ms);
        start();
        break;
    case Paused:
        //seeking in paused state
        stopPlayback();
        doSeek(time_ms);
        startAndPause();
        break;
    }
}

void Engine::setVideoStreamIndex(int index)
{
    if(index < 0 ||
       index >= m_video_decoder->getStreamsCount())
        return;
    stop();
    m_selected_video_stream_index = index;
    m_video_context.clear();
	m_video_context.open(m_video_decoder->getStream(m_selected_video_stream_index), m_currentFragment->getFpsFromSamples());
    m_video_decoder->setStream(m_video_decoder->getStream(m_selected_video_stream_index));
}

void Engine::setAudioStreamIndex(int index)
{
    if(index < 0 ||
       index >= m_audio_decoder->getStreamsCount())
        return;
    stop();
    m_audio_decoder->setIndex(index);
    m_audio_playback.setAudioParams(m_audio_decoder->getParams());
}

void Engine::setVolume(int volume)
{
    if(!m_is_initialized)
        return;

    if(volume < 0)
        volume = 0;
    if(volume > 100)
        volume = 100;

    double vol = (double)volume / 100.0;

    if(m_audio_decoder->getStreamsCount())
        m_audio_playback.setVolume(vol);
}

#ifdef MEMORY_INFO
void Engine::memoryInfo(int& video_memory, int& audio_memory) const
{
    video_memory = m_video_decoder->buffersSize();
    audio_memory = m_audio_decoder->buffersSize();
}
#endif //MEMORY_INFO

/*********************************************************************************************/

bool Engine::initMainContext(const QString& file_name, FragmentInfo& fragment)
{
    bool res = true;

    res = res && m_video_decoder->open(file_name, fragment.getValidMediaStreamIds());
    res = res && m_audio_decoder->open(file_name, fragment.getValidMediaStreamIds());
    res = res && m_video_decoder->getStreamsCount();
	res = res && m_video_context.open(m_video_decoder->getStream(m_selected_video_stream_index), fragment.getFpsFromSamples());
    if (m_audio_decoder->getStreamsCount()) m_audio_decoder->setIndex();

    return res;
}

bool Engine::initDecoders()
{
    m_video_decoder->setStream(m_video_decoder->getStream(m_selected_video_stream_index));

    if(m_audio_decoder && m_audio_decoder->getStreamsCount())
    {
        m_audio_decoder->setIndex();
    }

    return true;
}

void Engine::clearDecoders()
{
    m_video_decoder->clear();
    m_audio_decoder->clear();
}

bool Engine::initVideoPlayback()
{
    m_video_playback.setVideoContext(&m_video_context);
    m_video_playback.setVideoDecoder(m_video_decoder);
    m_video_playback.setVideoWidget(m_video_widget);

    return true;
}

void Engine::clearVideoPlayback()
{
    m_video_playback.clear();
}

bool Engine::initAudioPlayback()
{
	m_audio_playback.setAudioDecoder(m_audio_decoder);
    m_audio_playback.setAudioParams(m_audio_decoder->getParams());

    return true;
}

void Engine::clearAudioPlayback()
{
    m_audio_playback.clear();
}

void Engine::stopPlayback()
{
    if(m_audio_decoder->getStreamsCount())
        m_audio_playback.stop();
    m_video_playback.stop();

    m_audio_decoder->stop();
    m_video_decoder->stop();

    m_audio_decoder->clearBuffers();
    m_video_decoder->clearBuffers();

    m_playing_time = 0;

    m_player_state = Stopped;
}

void Engine::doSeek(int time_ms)
{
    m_playing_time = time_ms;

    bool have_audio = m_audio_decoder->getStreamsCount();

    //seek video
    if(time_ms == 0)
    {
        //stop seek
        m_video_decoder->seek(0);
    }
    else
    {
        //step by step seek
        for(int step = 100; step <= SEEK_BACKSTEP; step *= 2)
        {
            int video_seek_time = time_ms - step;

            if(video_seek_time < 0)
            {
                video_seek_time = 0;
                //that's all - seek to start
                m_video_decoder->seek(video_seek_time);
                break;
            }

            m_video_decoder->seek(video_seek_time);

            //test that seek enough
            QueuedVideoDecoder decoder;
            decoder.setStream(m_video_decoder->getStream(m_selected_video_stream_index));
            VideoFrame video_frame;
            if(decoder.getNextFrame(video_frame) &&
               video_frame.m_time <= time_ms)
                break;
        }
    }

    //skip threshold
    QVector<int> pts_vector;
    m_video_decoder->timeToPTS(time_ms, pts_vector);
    if (pts_vector.size() > 0) {
        m_video_decoder->setSkipThreshold(pts_vector[m_selected_video_stream_index]);
    }
    //audio seek
    if(have_audio)
        m_audio_decoder->seek(time_ms);

    //clear video context fps
    m_video_context.flushCurrentFps();
}

int Engine::showNextFrame()
{
	VideoFrame video_frame;
	m_video_decoder->getNextFrame(video_frame);
	m_video_widget->setDrawImage(video_frame.m_image);
	return video_frame.m_time;
}

void Engine::onFinished()
{
    pause();

    emit playbackFinished();
}
