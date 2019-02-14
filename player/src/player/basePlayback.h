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

#ifndef BASEPLAYBACK_H
#define BASEPLAYBACK_H

#include "crosscompilation_cxx11.h"

#include <QObject>

//! Base class for all playback classes.
class BasePlayback : public QObject
{
private:
    Q_OBJECT

public:
    BasePlayback() : QObject(), m_is_playing(false) {}

    virtual ~BasePlayback() {}

    //! Start playback.
    virtual void start() = 0;

    //! Pause playback.
    virtual void pause() = 0;

    //! Resume playback after pause.
    virtual void resume() = 0;

    //! Stop playback.
    virtual void stop() = 0;

    //! Start and then pause playback.
    virtual void startAndPause() = 0;

    //! Clear playback data.
    virtual void clear() = 0;

    //! Get current time.
    virtual int getPlayingTime() const = 0;

signals:
    //! This signal will be seent time by time.
    void played(BasePlayback* playback);

    //! This signal will be emitted when audio playback has reached the end of the file.
    void playbackFinished();

protected:
    bool    m_is_playing;
};

#endif // BASEPLAYBACK_H
