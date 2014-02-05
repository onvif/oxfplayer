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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "crosscompilation_cxx11.h"

#include <QObject>

#include "engine.h"
#include "playerWidget.h"
#include "fullscreenPlayerWidget.h"
#include "parserWidget.h"
#include "verifyerdialog.h"
#include "videoFrameWidget.h"
#include "mediaParser.h"

//! Main class that controls all work.
class Controller : public QObject
{
private:
    Q_OBJECT

public:
    Controller(Engine& engine,
               PlayerWidget& player_widget, FullscreenPlayerWidget& fullscreen_player_widget, ControlsWidget& controls_widget,
               ParserWidget& parser_widget, VerifyerDialog& verifyer_dlg, MediaParser & media_parser);

    ~Controller();

private slots:
    //! This slot will be called when new file is selected to be opened.
    void openFile(const QString& file_name);

    //! This slot will be called when new folder is selected to be opened.
    void openDir(const QString& dir_name);

    //! This slot will be called when file structure needs to be shown.
    void showFileStructure();

    //! This slot will be called when file signature needs to be verified.
    void verifyFileSignature();

    //! This slot will be called when we want to work with certificates.
    void openCertificateStorage();

    //! This slot will be called when new Exit selected in menu.
    void exit();

    //! Play button pressed.
    void onPlay();

    //! Pause button pressed.
    void onPause();

    //! Stop button pressed.
    void onStop();

    //! Engine send new played time.
    void onPlayed(BasePlayback* playback);

    //! Fragment slider released - seek to some position.
    void onSeek(int time_ms);

    //! Fragment slider released - switch fragment and seek to some position.
    void onSeek(int fragment_index, int time_ms);

    //! Video stream index changed.
    void onVideoStreamIndexChanged(int index);

    //! Audio stream index changed.
    void onAudioStreamIndexChanged(int index);

    //! Switch to new selected fragment.
    void onFragmentSelected(FragmentInfo fragment_info);

    //! Next fragment button pressed.
    void onNextFragment();

    //! Previous fragment button pressed.
    void onPrevFragment();

    //! Switch to full screen.
    void toFullScreenMode();

    //! Switch to normal screen.
    void fromFullScreenMode();

    //! Space pressed - pause or play video.
    void onSpace();

    //! When engine says that playback finished.
    void onPlaybackFinished();

#ifdef MEMORY_INFO
    //!Debug. Show memory info.
    void showMemoryInfo();
#endif //MEMORY_INFO

private:
    //! Update duration of fragments if it is unknown from SUMI box.
    void updateFragmentsList(FragmentsList& fragments_list);

    //! Change stream index of audio or video.
    void changeStreamIndex(int index, bool video);

private:
    //! Engine.
    Engine&                 m_engine;
    //! Player UI.
    PlayerWidget&           m_player_widget;
    //! Fullscreen player widget.
    FullscreenPlayerWidget& m_fullscreen_player_widget;
    //! Controls widget - will be used by simple and fullscreen widget at the same time.
    ControlsWidget&         m_controls_widget;
    //! Parser UI.
    ParserWidget&           m_parser_widget;
    //! Verification dialog.
    VerifyerDialog&         m_verifyer_dialog;
    //! Media parser to validate file.
    MediaParser&            m_media_parser;
    //! Fragments info of opened file/file set.
    FragmentsList           m_fragments_list;
    //! Currently playing fragment.
    int                     m_playing_fragment_index;
};

#endif // CONTROLLER_H
