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

#ifndef ENGINE_H
#define ENGINE_H

#include "basePlayback.h"
#include <QTimer>

#include "enums.h"
#include "types.h"
#include "streamReader.h"
#include "videoContext.h"
#include "audioContext.h"
#include "decoder.h"
#include "audioPlayback.h"
#include "videoPlayback.h"
#include "segmentInfo.h"
#include "queuedAudioDecoder.h"
#include "queuedVideoDecoder.h"
#include "queuedMetadataDecoder.h"

class VideoFrameWidget;
class SegmentInfo;

//! Base class that will manage video and audio playback.
class Engine : public BasePlayback
{
private:
    Q_OBJECT

public:
    Engine();

    ~Engine();

    //! Set widget that will be used to present video.
    void setVideoWidget(VideoFrameWidget* video_widget, QTreeWidget* event_widget);

    //! Init engine with some file.
    bool init(const QString& file_name, SegmentInfo& fragment);

    virtual void start();

    virtual void pause();

    virtual void resume();

    virtual void stop();

    virtual void startAndPause();

    virtual void clear();

    virtual int getPlayingTime() const;

    //! Seek to some position.
    //! Seek in play state will continue play.
    //! Seek in pause state will pause.
    //! Seek in stop state will pause too.
    void seek(int time_ms);

	//! Show next frame in pause mode.
	int showNextFrame();

	//! Get player state.
    PlayerState getState() const { return m_player_state; }

    //! Video decoder.
    QueuedVideoDecoder m_video_decoder;
    //! Audio decoder.
    QueuedAudioDecoder m_audio_decoder;
    //! Metadata decoder.
    MetadataDecoder m_metadata_decoder;

    //! Set new stream index for audio.
    void setAudioStreamIndex(int index);

public slots:
    //! Set volume. Volume should be between 0 and 100.
    void setVolume(int volume);

#ifdef MEMORY_INFO
public:
    //! Debug. Get memory info.
    void memoryInfo(int& video_memory, int& audio_memory) const;
#endif //MEMORY_INFO

private:
    //! Init MainContext.
    bool initDecoders(const QString& file_name, SegmentInfo *segment);

    //! Init audio and video playback.
    bool initPlayback();

    //! Private function that will stop playback.
    void stopPlayback();

    //! Seek to some position.
    void doSeek(int time_ms);

	private slots:
    //! This slot will be called when video or audio playback finished.
    void onFinished();

private:
    //! Widget to present video.
    VideoFrameWidget*   m_video_widget;

    //! Widget to present events.
    QTreeWidget* m_event_widget;

    //! Video playback.
    VideoPlayback   m_video_playback;
    //! Audio playback.
    AudioPlayback   m_audio_playback;

    //! Is Engine initialized.
    bool            m_is_initialized;
    //! Player state.
    PlayerState     m_player_state;

    //! Playing time.
    mutable int     m_playing_time;
};

#endif //ENGINE_H
