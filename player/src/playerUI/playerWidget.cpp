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

#include "playerWidget.h"
#include "ui_playerWidget.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>

#include "defines.h"

PlayerWidget::PlayerWidget(QWidget* parent) :
    QMainWindow(parent),
    PlayerWidgetInterface(),
    m_ui(new Ui::PlayerWidget)
{
    m_ui->setupUi(this);

#ifndef MEMORY_INFO
    m_ui->menuAdditional->menuAction()->setVisible(false);
#endif //MEMORY_INFO

    m_ui->frames_list_layout->addWidget(&m_fragments);
    m_ui->video_layout->addWidget(&m_video_frame);

    QObject::connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    QObject::connect(m_ui->actionFile_structure, SIGNAL(triggered()), this, SIGNAL(showFileStructure()));
    QObject::connect(m_ui->actionFile_signature, SIGNAL(triggered()), this, SIGNAL(verifyFileSignature()));
    QObject::connect(m_ui->actionCertificate_storage, SIGNAL(triggered()), this, SIGNAL(openCertificateStorage()));
    QObject::connect(m_ui->actionExit, SIGNAL(triggered()), this, SIGNAL(exit()));
	QObject::connect(m_ui->actionLocalTime, SIGNAL(triggered()), this, SLOT(showLocalTime()));
#ifdef MEMORY_INFO
    QObject::connect(m_ui->actionMemory, SIGNAL(triggered()), this, SIGNAL(memoryInfo()));
#endif //MEMORY_INFO
}

void PlayerWidget::setControls(ControlsWidget* controls)
{
    PlayerWidgetInterface::setControls(controls);
    m_ui->controls_layout->addWidget(m_controls);
}

void PlayerWidget::removeControls()
{
    m_ui->controls_layout->removeWidget(m_controls);
    PlayerWidgetInterface::removeControls();
}

void PlayerWidget::setStreamsInfo(int video_streams_count, int audio_streams_count)
{
    setStreamsMenu(m_ui->menuVideo_streams, video_streams_count, true);
    setStreamsMenu(m_ui->menuAudio_streams, audio_streams_count, false);
}

PlayerWidget::~PlayerWidget()
{
    delete m_ui;
}

void PlayerWidget::closeEvent(QCloseEvent* event)
{
    event->ignore();
    emit exit();
}

QString PlayerWidget::getLastOpenedFolder()
{
#ifdef WIN32
    QSettings settings(QDir::homePath() + WINP_APP_DATA_ROAMING + COMPANY_NAME + "/" + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat);
#endif //WIN32
#ifdef UNIX
    QSettings settings(QDir::homePath() + "/." + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat);
#endif //UNIX
    return settings.value("lastOpenedFolder", "").toString();
}

void PlayerWidget::saveLastOpenedFolder(const QString& folder)
{
#ifdef WIN32
    QSettings settings(QDir::homePath() + WINP_APP_DATA_ROAMING + COMPANY_NAME + "/" + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat);
#endif //WIN32
#ifdef UNIX
    QSettings settings(QDir::homePath() + "/." + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat);
#endif //UNIX
    settings.setValue("lastOpenedFolder", folder);
}

void PlayerWidget::setStreamsMenu(QMenu* menu, int count, bool video)
{
    menu->clear();
    for(int i = 0; i < count; ++i)
    {
        QAction* new_action = new QAction("Stream " + QString::number(i), menu);
        new_action->setCheckable(true);
        new_action->setChecked(i == 0);
        if(video)
            QObject::connect(new_action, SIGNAL(triggered()), this, SLOT(onVideoStreamSelected()));
        else
            QObject::connect(new_action, SIGNAL(triggered()), this, SLOT(onAudioStreamSelected()));
        menu->addAction(new_action);
    }
}

void PlayerWidget::onOpenFile()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open video", getLastOpenedFolder(), AVAILIBLE_EXTENTIONS);
    if(!file_name.isEmpty())
    {
        QFileInfo file_info(file_name);
        saveLastOpenedFolder(file_info.absolutePath());
        emit openFile(file_name);
    }
}

void PlayerWidget::onVideoStreamSelected()
{
    QAction* action = (QAction*)sender();
    for(int i = 0; i < m_ui->menuVideo_streams->actions().size(); ++i)
    {
        bool checked = ( m_ui->menuVideo_streams->actions().at(i) == action);
        m_ui->menuVideo_streams->actions().at(i)->setChecked(checked);
        if(checked)
            emit changeVideoStream(i);
    }
}

void PlayerWidget::onAudioStreamSelected()
{
    QAction* action = (QAction*)sender();
    for(int i = 0; i < m_ui->menuAudio_streams->actions().size(); ++i)
    {
        bool checked = (m_ui->menuAudio_streams->actions().at(i) == action);
        m_ui->menuAudio_streams->actions().at(i)->setChecked(checked);
        if(checked)
            emit changeAudioStream(i);
    }
}

void PlayerWidget::showLocalTime()
{
	emit showLocalTimeChanged((bool)((QAction*)sender())->isChecked());
}