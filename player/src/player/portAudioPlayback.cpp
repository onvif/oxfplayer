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

#include "portAudioPlayback.h"

#include <QDebug>

PortAudioPlayback::PortAudioPlayback() :
    AudioPlaybackImpl()
{
    QObject::connect(&m_audio_thread, SIGNAL(played()), this, SLOT(onPlayed()));
    QObject::connect(&m_audio_thread, SIGNAL(playbackFinished()), this, SIGNAL(playbackFinished()));
}

PortAudioPlayback::~PortAudioPlayback()
{
    stop();
    clear();
}

void PortAudioPlayback::start()
{
    if(m_audio_decoder == nullptr ||
       m_is_playing)
        return;

    m_audio_thread.setAudioDecoder(m_audio_decoder);
    m_audio_thread.setAudioParams(m_audio_params);

    m_audio_thread.start();
    m_is_playing = true;
}

void PortAudioPlayback::pause()
{
    if(m_audio_decoder == nullptr ||
       !m_is_playing)
        return;

    m_audio_thread.stop();
    m_is_playing = false;
}

void PortAudioPlayback::resume()
{
    if(m_audio_decoder == nullptr ||
       m_is_playing)
        return;

    m_audio_thread.start();
    m_is_playing = true;
}

void PortAudioPlayback::stop()
{
    if(m_audio_decoder == nullptr)
        return;

    m_audio_thread.stop();
    m_is_playing = false;
}

void PortAudioPlayback::startAndPause()
{
    if(m_audio_decoder == nullptr ||
       m_is_playing)
        return;

    m_audio_thread.setAudioDecoder(m_audio_decoder);
    m_audio_thread.setAudioParams(m_audio_params);

    m_is_playing = false;
}

void PortAudioPlayback::clear()
{
    AudioPlaybackImpl::clear();
    m_audio_thread.setAudioDecoder(nullptr);
}

int PortAudioPlayback::getPlayingTime() const
{
    return m_audio_thread.getPlayingTime();
}

void PortAudioPlayback::setVolume(double volume)
{
    if(volume < 0.0)
        volume = 0.0;
    if(volume > 100.0)
        volume = 100.0;

    m_audio_thread.setVolume(volume);
}

void PortAudioPlayback::onPlayed()
{
    emit played(this);
}
