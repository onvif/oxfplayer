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

#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "crosscompilation_cxx11.h"

#include <QMainWindow>
#include "playerWidgetInterface.h"

#include "fragmentListWidget.h"
#include "controlsWidget.h"
#include "videoFrameWidget.h"

namespace Ui {
class PlayerWidget;
}

//! Main component uinting all UI.
class PlayerWidget : public QMainWindow, public PlayerWidgetInterface
{
private:
    Q_OBJECT

public:
    PlayerWidget(QWidget* parent = 0);

    ~PlayerWidget();

    virtual FragmentListWidget* getFragmentList() { return &m_fragments; }

    virtual VideoFrameWidget* getVideoWidget() { return &m_video_frame; }

    virtual void setControls(ControlsWidget* controls);

    virtual void removeControls();

    //! Fill streams info
    void setStreamsInfo(int video_streams_count, int audio_streams_count);

signals:
    //! Some file selected in Open File dialog.
    void openFile(const QString& fileName);

    //! Some directory selected in Open File dialog.
    void openDir(const QString& dirName);

    //! Verify File structure item seleceted.
    void showFileStructure();

    //! Verify File signature.
    void verifyFileSignature();

    //! Open certificate storage.
    void openCertificateStorage();

    //! Change video stream.
    void changeVideoStream(int index);

    //! Change audio stream.
    void changeAudioStream(int index);

    //! Exit menu item selected.
    void exit();

    //! Debug. Memory info.
    void memoryInfo();

	//! Use local or utc time
	void showLocalTimeChanged(bool on);

protected:
    //! On close event.
    virtual void closeEvent(QCloseEvent* event);

private:
    //! Get last opened folder
    QString getLastOpenedFolder();

    //! Save last opened folder.
    void saveLastOpenedFolder(const QString& folder);

    //! Set streams menu.
    void setStreamsMenu(QMenu* menu, int count, bool video);

private slots:
    //! Process open file menu selection.
    void onOpenFile();

    //! Process open directory menu selection.
    void onOpenDir();

    //! Select video stream signal.
    void onVideoStreamSelected();

    //! Select audio stream signal.
    void onAudioStreamSelected();

	//! Use local or utc time
	void showLocalTime();

private:
    //! UI.
    Ui::PlayerWidget*   m_ui;
    //! Fragments UI.
    FragmentListWidget  m_fragments;
    //! Video view UI.
    VideoFrameWidget    m_video_frame;
};

#endif // PLAYERWIDGET_H
