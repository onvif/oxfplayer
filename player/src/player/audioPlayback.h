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

#ifndef AUDIOPLAYBACK_H
#define AUDIOPLAYBACK_H

#include "crosscompilation_cxx11.h"

#include "basePlayback.h"

#include "ffmpeg.h"
#include "types.h"
#include "decoder.h"

//! Audio playback implementation interface.
class AudioPlaybackImpl : public BasePlayback
{
private:
    Q_OBJECT

public:
    AudioPlaybackImpl() :
        BasePlayback(),
        m_audio_decoder(nullptr)
    {}

    virtual ~AudioPlaybackImpl()
    {}

    //! Set paramters that will be used to calculate some audio parameters.
    void setAudioParams(const AudioParams& audio_params) { m_audio_params = audio_params; }

    //! Set decoder to read from.
    void setAudioDecoder(Decoder<AudioFrame>* audio_decoder) { m_audio_decoder = audio_decoder; }

    virtual void clear()
    {
        stop();
        m_audio_params = AudioParams();
        m_audio_decoder = nullptr;
        m_is_playing = false;
    }

    //! Set volume level. Volume should be in [0.0 1.0] scope.
    virtual void setVolume(double volume) = 0;

protected:
    //! Audio parameters used to calculate audio played time.
    AudioParams             m_audio_params;
    //! Decoder that provides data.
    Decoder<AudioFrame>*    m_audio_decoder;
};

//! Main audio system.
/*!
 * \brief This class is responsible for audio playback.
 */
class AudioPlayback : public BasePlayback
{
private:
    Q_OBJECT

public:
    AudioPlayback();

    ~AudioPlayback();

    //! Set paramters that will be used to calculate some audio parameters.
    void setAudioParams(const AudioParams& audio_params) { m_impl->setAudioParams(audio_params); }

    //! Set decoder to read from.
    void setAudioDecoder(Decoder<AudioFrame>* audio_decoder) { m_impl->setAudioDecoder(audio_decoder); }

    virtual void start() { m_impl->start(); }

    virtual void pause() { m_impl->pause(); }

    virtual void resume() { m_impl->resume(); }

    virtual void stop() { m_impl->stop(); }

    virtual void startAndPause() { m_impl->startAndPause(); }

    virtual void clear() { m_impl->clear(); }

    virtual int getPlayingTime() const { return m_impl->getPlayingTime(); }

    //! Set volume level. Volume should be in [0.0 1.0] scope.
    void setVolume(double volume) { m_impl->setVolume(volume); }

private:
    AudioPlaybackImpl*  m_impl;
};

#endif // AUDIOPLAYBACK_H
