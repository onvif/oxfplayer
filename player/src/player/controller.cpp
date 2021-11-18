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

#include "controller.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>

#include "certificateStorage.h"
#include "certificateStorageDialog.h"

Controller::Controller(Engine& engine,
                       PlayerWidget& player_widget, FullscreenPlayerWidget& fullscreen_player_widget, ControlsWidget& controls_widget,
                       ParserWidget& parser_widget, VerifyerDialog& verifyer_dlg, MediaParser & media_parser) :
    QObject(),
    m_engine(engine),
    m_player_widget(player_widget),
    m_fullscreen_player_widget(fullscreen_player_widget),
    m_controls_widget(controls_widget),
    m_parser_widget(parser_widget),
    m_verifyer_dialog(verifyer_dlg),
    m_media_parser(media_parser),
    m_playing_fragment_index(-1)
{
    QObject::connect(&m_player_widget, SIGNAL(openFile(QString)), this, SLOT(openFile(QString)));
    QObject::connect(&m_player_widget, SIGNAL(openDir(QString)), this, SLOT(openDir(QString)));
    QObject::connect(&m_player_widget, SIGNAL(changeVideoStream(int)), this, SLOT(onVideoStreamIndexChanged(int)));
    QObject::connect(&m_player_widget, SIGNAL(changeAudioStream(int)), this, SLOT(onAudioStreamIndexChanged(int)));
    QObject::connect(&m_player_widget, SIGNAL(showFileStructure()), this, SLOT(showFileStructure()));
    QObject::connect(&m_player_widget, SIGNAL(verifyFileSignature()), this, SLOT(verifyFileSignature()));
    QObject::connect(&m_player_widget, SIGNAL(openCertificateStorage()), this, SLOT(openCertificateStorage()));
    QObject::connect(&m_player_widget, SIGNAL(exit()), this, SLOT(exit()));
    QObject::connect(&m_player_widget, SIGNAL(showLocalTimeChanged(bool)), this, SLOT(onshowLocalTimeChanged(bool)));

    QObject::connect(&m_engine, SIGNAL(playbackFinished()), this, SLOT(onPlaybackFinished()));    

    QObject::connect(&m_controls_widget, SIGNAL(started()), this, SLOT(onPlay()), Qt::QueuedConnection);
    QObject::connect(&m_controls_widget, SIGNAL(paused()), this, SLOT(onPause()), Qt::QueuedConnection);
    QObject::connect(&m_controls_widget, SIGNAL(stopped()), this, SLOT(onStop()), Qt::QueuedConnection);
    QObject::connect(&m_controls_widget, SIGNAL(volume(int)), &m_engine, SLOT(setVolume(int)), Qt::QueuedConnection);
    QObject::connect(&m_controls_widget, SIGNAL(fragment(int)), this, SLOT(onSeek(int)));
    QObject::connect(&m_controls_widget, SIGNAL(total(int,int)), this, SLOT(onSeek(int,int)));
    QObject::connect(&m_controls_widget, SIGNAL(nextFragment()), this, SLOT(onNextFragment()));
    QObject::connect(&m_controls_widget, SIGNAL(prevFragment()), this, SLOT(onPrevFragment()));
    QObject::connect(&m_controls_widget, SIGNAL(fullscreen()), this, SLOT(toFullScreenMode()));

    QObject::connect(m_player_widget.getFragmentList(), SIGNAL(fragmentSelected(FragmentInfo)), this, SLOT(onFragmentSelected(FragmentInfo)));

    QObject::connect(m_player_widget.getVideoWidget(), SIGNAL(doubleClick()), this, SLOT(toFullScreenMode()));
    QObject::connect(m_fullscreen_player_widget.getVideoWidget(), SIGNAL(doubleClick()), this, SLOT(fromFullScreenMode()));
    QObject::connect(m_fullscreen_player_widget.getVideoWidget(), SIGNAL(escapePressed()), this, SLOT(fromFullScreenMode()));
    QObject::connect(m_fullscreen_player_widget.getVideoWidget(), SIGNAL(spacePressed()), this, SLOT(onSpace()));

    QObject::connect(&m_engine, SIGNAL(played(BasePlayback*)), this, SLOT(onPlayed(BasePlayback*)));

#ifdef MEMORY_INFO
    //Debug
    QObject::connect(&m_player_widget, SIGNAL(memoryInfo()), this, SLOT(showMemoryInfo()));
#endif //MEMORY_INFO

    m_player_widget.show();
    m_fullscreen_player_widget.hide();

    m_player_widget.setControls(&m_controls_widget);

    engine.setVideoWidget(m_player_widget.getVideoWidget());
}

Controller::~Controller()
{
    m_engine.stop();
    m_engine.clear();
}

void Controller::openFile(const QString& file_name)
{
    m_engine.stop();
    m_engine.clear();
    m_player_widget.getFragmentList()->clear();
    m_player_widget.getVideoWidget()->clear();
    m_controls_widget.enableUI(true);
    m_parser_widget.clearContents();
    m_verifyer_dialog.clearContent();
    m_media_parser.clearContents();
    m_fragments_list.clear();

    m_media_parser.addFile(file_name);

    if(!m_media_parser.isValidISO())
    {
        QMessageBox message_box(QMessageBox::Information,
                               m_player_widget.windowTitle(),
                               QString("This file does not conform ISO Base Media format"),
                               QMessageBox::Ok,
                               &m_player_widget);
        message_box.exec();
        m_media_parser.clearContents();
        return;
    }

    m_fragments_list = m_media_parser.getFragmentsList();
    updateFragmentsList(m_fragments_list);

    if(!m_engine.init(file_name, m_fragments_list.front()))
    {
        QMessageBox message_box(QMessageBox::Information,
                               m_player_widget.windowTitle(),
                               QString("Error to open this file using FFMPEG"),
                               QMessageBox::Ok,
                               &m_player_widget);
        message_box.exec();
        m_media_parser.clearContents();
        return;
    }

    m_parser_widget.showFilesetInformation(m_media_parser.getFilesetInformation());
    m_verifyer_dialog.initialize(m_media_parser);

    m_playing_fragment_index = 0;
    m_player_widget.getFragmentList()->setFragmentsList(m_fragments_list);
    m_player_widget.setStreamsInfo(m_engine.getVideoStreamsCount(), m_engine.audioDecoder()->getStreamsCount());
    m_controls_widget.setFragmentsList(m_fragments_list);
    m_controls_widget.startPlayback();
    m_controls_widget.updateUI();
    m_engine.start();
}

void Controller::openDir(const QString& dir_name)
{
    m_engine.stop();
    m_engine.clear();
    m_player_widget.getVideoWidget()->clear();
    m_player_widget.getFragmentList()->clear();
    m_controls_widget.enableUI(true);
    m_parser_widget.clearContents();
    m_verifyer_dialog.clearContent();
    m_media_parser.clearContents();
    m_fragments_list.clear();

    QDir dir(dir_name, "*.mp4", QDir::SortFlags(QDir::Name | QDir::IgnoreCase), QDir::Files);
    QDirIterator it(dir);
    bool bContainsFiles = false;
    while(it.hasNext())
    {
        it.next();
        bContainsFiles = true;
        m_media_parser.addFile(it.filePath());
    }

    QString errorMessage;

    if(bContainsFiles)
    {
        SurveillanceConformanceType Surveillance_conformance = m_media_parser.isValidSurveillance();

        if(m_media_parser.isValidISOFileset())
        {
            if(Surveillance_conformance == SurveillanceConformant)
            {
                m_fragments_list = m_media_parser.getFragmentsList();
                updateFragmentsList(m_fragments_list);
                if(m_engine.init(m_fragments_list.front().getFileName(), m_fragments_list.front()))
                {
                    m_parser_widget.showFilesetInformation(m_media_parser.getFilesetInformation());
                    m_verifyer_dialog.initialize(m_media_parser);

                    m_playing_fragment_index = 0;
                    m_player_widget.getFragmentList()->setFragmentsList(m_fragments_list);
                    m_player_widget.setStreamsInfo(m_engine.getVideoStreamsCount(), m_engine.audioDecoder()->getStreamsCount());
                    m_controls_widget.setFragmentsList(m_fragments_list);
                    m_controls_widget.startPlayback();
                    m_controls_widget.updateUI();
                    m_engine.start();
                }
            }
            else
            {
                if(Surveillance_conformance == IsNotSurveillanceFormat)
                {
                    errorMessage = "This fileset contains files that do not conform ISO Surveillance format";
                }
                else if(Surveillance_conformance == IsNotSurveillanceFileset)
                {
                    errorMessage = "The selected directory contains an inconsistent fileset";
                }
            }
        }
        else
        {
            errorMessage = "This fileset contains files that do not conform ISO Base Media format";
        }
    }
    else
    {
        errorMessage = "Specified directory does not contain media files";
    }

    if(!errorMessage.isEmpty())
    {
        QMessageBox message_box(QMessageBox::Information,
                               m_player_widget.windowTitle(),
                               errorMessage,
                               QMessageBox::Ok,
                               &m_player_widget);
        message_box.exec();
        m_media_parser.clearContents();
    }
}

void Controller::showFileStructure()
{
    m_parser_widget.hide();
    m_parser_widget.showFilesetInformation(m_media_parser.getFilesetInformation());
    m_parser_widget.show();
}

void Controller::verifyFileSignature()
{
    m_verifyer_dialog.hide();
    m_verifyer_dialog.initialize( m_media_parser );
    m_verifyer_dialog.show();
}

void Controller::openCertificateStorage()
{
    CertificateStorageDialog certificate_storage_dialog(&m_player_widget);
    certificate_storage_dialog.exec();
}

void Controller::exit()
{
    m_engine.stop();
    m_engine.clear();
    qApp->quit();
}

void Controller::onPlay()
{
    if(m_engine.getState() == Paused)
        m_engine.resume();
    else //state == Stopped
        m_engine.start();
    m_controls_widget.startPlayback();
    m_controls_widget.updateUI();
}

void Controller::onPause()
{
    m_engine.pause();
}

void Controller::onStop()
{
    m_engine.stop();
    m_controls_widget.setPlayedTime(&m_engine);
    m_controls_widget.updateUI();
}

void Controller::onPlayed(BasePlayback* playback)
{
    m_controls_widget.setPlayedTime(playback);
    m_controls_widget.updateUI();
}

void Controller::onSeek(int time_ms)
{
    m_engine.seek(time_ms);
    m_controls_widget.setPlayedTime(&m_engine);
    m_controls_widget.updateUI();
}

void Controller::onSeek(int fragment_index, int time_ms)
{
    PlayerState old_engine_state = m_engine.getState();
    m_controls_widget.stopPlayback();
    m_engine.stop();
    m_engine.clear();
    FragmentInfo fragment_info = m_fragments_list[fragment_index];
    if(!m_engine.init(fragment_info.getFileName(), fragment_info))
    {
        m_controls_widget.updateUI();
        //TODO - common function for this message
        QMessageBox message_box(QMessageBox::Information,
                               m_player_widget.windowTitle(),
                               QString("Error to open this file using FFMPEG"),
                               QMessageBox::Ok,
                               &m_player_widget);
        message_box.exec();
        return;
    }
    m_playing_fragment_index = fragment_index;
    m_player_widget.getFragmentList()->selectFragment(fragment_info);
    m_player_widget.setStreamsInfo(m_engine.getVideoStreamsCount(), m_engine.audioDecoder()->getStreamsCount());
    m_controls_widget.startFragment(fragment_index);
    m_engine.seek(time_ms);
    switch(old_engine_state)
    {
    case Stopped:
        //due to pause state in engine after seek in stop state do nothing.
        m_controls_widget.pausePlayback();
        break;
    case Playing:
        m_engine.resume(); //due to seek in stop state we must resume, not start playback
        m_controls_widget.startPlayback();
        break;
    case Paused:
        m_controls_widget.pausePlayback();
        break;
    }
    m_controls_widget.updateUI();
}

void Controller::onVideoStreamIndexChanged(int index)
{
    if(index == m_engine.getCurrentVideoStreamIndex())
        return;
    changeStreamIndex(index, true);
}

void Controller::onAudioStreamIndexChanged(int index)
{
    if(index == m_engine.audioDecoder()->getIndex())
        return;
    changeStreamIndex(index, false);
}

void Controller::onFragmentSelected(FragmentInfo fragment_info)
{
    PlayerState old_engine_state = m_engine.getState();
    m_engine.stop();
    m_engine.clear();
    m_controls_widget.stopPlayback();
    if(!m_engine.init(fragment_info.getFileName(), fragment_info))
    {
        m_controls_widget.updateUI();
        //TODO - common function for this message
        QMessageBox message_box(QMessageBox::Information,
                               m_player_widget.windowTitle(),
                               QString("Error to open this file using FFMPEG"),
                               QMessageBox::Ok,
                               &m_player_widget);
        message_box.exec();
        return;
    }
    m_playing_fragment_index = fragment_info.getFragmentNumber();
    m_player_widget.getFragmentList()->selectFragment(fragment_info);
    m_player_widget.setStreamsInfo(m_engine.getVideoStreamsCount(), m_engine.audioDecoder()->getStreamsCount());
    m_controls_widget.startFragment(fragment_info.getFragmentNumber());
    switch(old_engine_state)
    {
    case Stopped:
    case Playing:
        m_engine.start();
        m_controls_widget.startPlayback();
        break;
    case Paused:
        m_engine.startAndPause();
        m_controls_widget.pausePlayback();
        break;
    }
    m_controls_widget.setPlayedTime(&m_engine);
    m_controls_widget.updateUI();
}

void Controller::onNextFragment()
{
	if (m_fragments_list.size() == 1 ||
		m_playing_fragment_index == m_fragments_list.size() - 1) {
		m_controls_widget.setPlayedTime(m_engine.showNextFrame());
		m_controls_widget.setTimeLabels();
		return;
	}
	else {
		onFragmentSelected(m_fragments_list[m_playing_fragment_index + 1]);
	}
}

void Controller::onPrevFragment()
{
    if(m_fragments_list.size() == 1 ||
       m_playing_fragment_index == 0)
        return;

    onFragmentSelected(m_fragments_list[m_playing_fragment_index - 1]);
}

void Controller::toFullScreenMode()
{
    m_player_widget.hide();
    m_player_widget.removeControls();
    m_engine.setVideoWidget(m_fullscreen_player_widget.getVideoWidget());
    m_controls_widget.fullscreenMode(true);
    m_fullscreen_player_widget.setControls(&m_controls_widget);
    m_fullscreen_player_widget.showFullScreen();
    m_fullscreen_player_widget.relocateControls();
}

void Controller::fromFullScreenMode()
{
    m_fullscreen_player_widget.hide();
    m_fullscreen_player_widget.removeControls();
    m_engine.setVideoWidget(m_player_widget.getVideoWidget());
    m_controls_widget.fullscreenMode(false);
    m_player_widget.setControls(&m_controls_widget);
    m_player_widget.show();
}

void Controller::onSpace()
{
    if(m_engine.getState() == Playing)
    {
        m_engine.pause();
        m_controls_widget.pausePlayback();
    }
    else if(m_engine.getState() == Paused)
    {
        m_engine.resume();
        m_controls_widget.startPlayback();
    }
    m_controls_widget.updateUI();
}

void Controller::onshowLocalTimeChanged(bool on)
{
	m_controls_widget.m_showLocalTime = on;
	m_controls_widget.setTimeLabels();
}

void Controller::onPlaybackFinished()
{
    if(m_fragments_list.size() == 1 ||
       (m_fragments_list.size() > 1 &&
        m_playing_fragment_index == m_fragments_list.size() - 1))
    {
        m_controls_widget.pausePlayback();
        m_controls_widget.updateUI();
    }
    else
        onNextFragment();
}

#ifdef MEMORY_INFO
void Controller::showMemoryInfo()
{
    int video_memory, audio_memory;
    m_engine.memoryInfo(video_memory, audio_memory);
    QString memory_string = QString("Video queue size ") + QString::number(video_memory / 1024 / 1024) + QString(" MB\n") +
                            QString("Audio queue size ") + QString::number(audio_memory / 1024) + QString(" KB\n");
    QMessageBox msg_box(QMessageBox::Information, "Memory info", memory_string,QMessageBox::Ok);
    msg_box.exec();
}
#endif //MEMORY_INFO

void Controller::updateFragmentsList(FragmentsList&)
{
	// currently nothing tbd
}

void Controller::changeStreamIndex(int index, bool video)
{
    PlayerState old_engine_state = m_engine.getState();
    int current_position_ms = m_engine.getPlayingTime();
    m_engine.stop();
    if(video)
        m_engine.setVideoStreamIndex(index);
    else
        m_engine.setAudioStreamIndex(index);
    m_engine.seek(current_position_ms);
    switch(old_engine_state)
    {
    case Stopped:
        //do nothing
        break;
    case Playing:
        m_engine.resume(); //due to seek in stop state will lead us to pause state
        m_controls_widget.startPlayback();
        break;
    case Paused:
        m_engine.startAndPause();
        m_controls_widget.pausePlayback();
        break;
    }
    m_controls_widget.updateUI();
}
