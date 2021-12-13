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
    m_metadata_decoder(&m_video_decoder),
    m_video_widget(nullptr),
    m_is_initialized(false),
    m_player_state(Stopped),
    m_playing_time(0)
{
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
}

/*********************************************************************************************/

void Engine::setVideoWidget(VideoFrameWidget* video_widget, QTreeWidget* event_widget)
{
    m_video_widget = video_widget;
    m_event_widget = event_widget;
    m_video_playback.setVideoWidget(m_video_widget, event_widget);
}

bool Engine::init(const QString& file_name, SegmentInfo& fragment)
{
	bool res = true;
	res = res && initDecoders(file_name, fragment.getFpsFromSamples());
    res = res && initPlayback();
    res = res && m_video_decoder.getStreamsCount();

    m_is_initialized = res;

    return res;
}

void Engine::start()
{
    if(!m_is_initialized ||
       m_player_state != Stopped)
        return;

    bool video = m_video_decoder.getStreamsCount();
    bool audio = m_audio_decoder.getStreamsCount();

    if(!video)
        return;

    m_video_decoder.start();
    m_audio_decoder.start();
    m_metadata_decoder.start();
    m_video_decoder.wait();
    if (audio) m_audio_decoder.wait();
    m_metadata_decoder.wait();

    m_video_playback.start();
    if (audio) m_audio_playback.start();

    m_player_state = Playing;
}

void Engine::pause()
{
    if(!m_is_initialized ||
       m_player_state != Playing)
        return;

    if(m_audio_decoder.getStreamsCount())
        m_audio_playback.pause();
    m_video_playback.pause();

    m_player_state = Paused;
}

void Engine::resume()
{
    if(!m_is_initialized ||
       m_player_state != Paused)
        return;

    if(m_audio_decoder.getStreamsCount())
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

    bool video = m_video_decoder.getStreamsCount();
    bool audio = m_audio_decoder.getStreamsCount();

    if(!video)
        return;

    m_video_decoder.start();
    m_audio_decoder.start();
    m_metadata_decoder.start();
    m_video_decoder.wait(true);
    m_metadata_decoder.wait(true);
    if (audio) m_audio_decoder.wait(true);
    m_video_playback.startAndPause();
    if (audio) m_audio_playback.startAndPause();

    m_player_state = Paused;
}

void Engine::clear()
{
    m_video_decoder.clear();
    m_audio_decoder.clear();
    m_metadata_decoder.clear();
    m_video_playback.clear();
    m_audio_playback.clear();
    m_player_state = Stopped;
    m_playing_time = 0;
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

void Engine::setAudioStreamIndex(int index)
{
    m_audio_decoder.setIndex(index);
    m_audio_playback.setAudioParams(m_audio_decoder.getParams());
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

    if(m_audio_decoder.getStreamsCount())
        m_audio_playback.setVolume(vol);
}

#ifdef MEMORY_INFO
void Engine::memoryInfo(int& video_memory, int& audio_memory) const
{
    video_memory = m_video_decoder.buffersSize();
    audio_memory = m_audio_decoder.buffersSize();
}
#endif //MEMORY_INFO

/*********************************************************************************************/

bool Engine::initDecoders(const QString& file_name, double fps)
{
    bool res = true;

    res = res && m_video_decoder.open(file_name);
    res = res && m_audio_decoder.open(file_name);
    res = res && m_metadata_decoder.open(file_name);
    res = res && m_video_decoder.getStreamsCount();
    m_video_decoder.setStream(0, fps);
    m_metadata_decoder.setStream(0);
    m_audio_decoder.setIndex(0);

    return res;
}

bool Engine::initPlayback()
{
    m_video_playback.setVideoContext(&m_video_decoder.m_context);
    m_video_playback.setVideoDecoder(&m_video_decoder, &m_metadata_decoder);
    m_video_playback.setVideoWidget(m_video_widget, m_event_widget);

    m_audio_playback.setAudioDecoder(&m_audio_decoder);
    m_audio_playback.setAudioParams(m_audio_decoder.getParams());
    return true;
}

void Engine::stopPlayback()
{
    if(m_audio_decoder.getStreamsCount())
        m_audio_playback.stop();
    m_video_playback.stop();

    m_audio_decoder.stop();
    m_video_decoder.stop();
    m_metadata_decoder.stop();

    m_audio_decoder.clearBuffers();
    m_video_decoder.clearBuffers();
    m_metadata_decoder.clearBuffers();

    m_playing_time = 0;

    m_player_state = Stopped;
}

void Engine::doSeek(int time_ms)
{
    m_playing_time = time_ms;

    m_video_decoder.seek(time_ms);
    //skip threshold
    m_audio_decoder.seek(time_ms);
    m_metadata_decoder.seek(time_ms);

    //clear video context fps
    m_video_decoder.m_context.flushCurrentFps();
}

int Engine::showNextFrame()
{
	VideoFrame video_frame;
	m_video_decoder.getNextFrame(video_frame);
	m_video_widget->setDrawImage(video_frame.m_image);
	return video_frame.m_time;
}

void Engine::onFinished()
{
    pause();

    emit playbackFinished();
}
