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

#ifndef PORTAUDIOTHREAD_H
#define PORTAUDIOTHREAD_H

#include "syncThread.h"

#include "ffmpeg.h"
#include "types.h"
#include "decoder.h"
#include "portaudio.h"

//! Class that will play sound using PortAudio in separate thread.
class PortAudioThread : public SyncThread
{
private:
    Q_OBJECT

public:
    PortAudioThread();

    ~PortAudioThread();

    //! Set decoder to read from.
    void setAudioDecoder(Decoder<AudioFrame>* audio_decoder);

    //! Set audio parameters of hardware.
    void setAudioParams(const AudioParams& audio_params);

    //! Start playback.
    virtual void start();

    //! Stop playback;
    virtual void stop();

    //! Set volume level. Volume should be in [0.0 1.0] scope.
    void setVolume(double volume) { m_volume = volume; }

    //! Get current time of playing audio part.
    int getPlayingTime() const { return m_current_time; }

signals:
    //! This signal will be emitted time by time, informing how much audio data have been played.
    void played();

    //! This signal will be emitted when playback finished - no more data in queue.
    void playbackFinished();

protected:
    virtual bool threadBody();

    virtual void threadFinished();

private:
    PaSampleFormat getFormat();

private:
    //! Set decoder to read from.
    Decoder<AudioFrame>*    m_audio_decoder;
    //! Parameters that will be used to init audio device;
    AudioParams             m_audio_params;
    //! Is initialized.
    bool                    m_is_initialized;
    //! Audio stream;
    PaStream*               m_stream;
    //! Currently playing tyme.
    volatile int            m_current_time;
    //! Last time value send in played signal.
    int                     m_sent_time;
    //! Volume level.
    volatile double         m_volume;
};

#endif //PORTAUDIOTHREAD_H
