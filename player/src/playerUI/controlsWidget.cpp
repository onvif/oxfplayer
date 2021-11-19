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

#include "controlsWidget.h"
#include "ui_controlsWidget.h"

#include <QApplication>

#include "controller.h"

#include "defines.h"

#include <QTime>
#include <QDateTime>
#include <QPainter>

ControlsWidget::ControlsWidget(QWidget* parent) :
    QWidget(parent),
    m_ui(new Ui::ControlsWidget),
    m_player_state(Stopped),
    m_playing_fragment_index(-1),
    m_fragment_played(0),
    m_mute(false),
    m_old_volume(0),
    m_fullscreen_mode(false),
	m_showLocalTime(true)
{
    m_ui->setupUi(this);

    setPlayBtnIcon();
    m_ui->stop_btn->setIcon(QIcon(":/stop"));
    m_ui->prev_btn->setIcon(QIcon(":/prev"));
    m_ui->next_btn->setIcon(QIcon(":/next"));
    setMuteBtnIcon();
    m_ui->fullscreen_btn->setIcon(QIcon(":/fullscreen"));

    m_ui->stop_btn->setToolTip("Stop");
    m_ui->prev_btn->setToolTip("Previous fragment");
    m_ui->next_btn->setToolTip("Next fragment");
    m_ui->fullscreen_btn->setToolTip("Switch to fullscreen mode");


    setTimePositions();
    setTimeLabels();

    QObject::connect(m_ui->play_btn, SIGNAL(clicked()), this, SLOT(onPlayBtn()));
    QObject::connect(m_ui->stop_btn, SIGNAL(clicked()), this, SLOT(onStopBtn()));
    QObject::connect(m_ui->volume, SIGNAL(valueChanged(int)), this, SLOT(onVolume(int)));
    QObject::connect(m_ui->mute_btn, SIGNAL(clicked()), this, SLOT(onMute()));
    QObject::connect(m_ui->prev_btn, SIGNAL(clicked()), this, SIGNAL(prevFragment()));
    QObject::connect(m_ui->next_btn, SIGNAL(clicked()), this, SIGNAL(nextFragment()));
    QObject::connect(m_ui->total_position, SIGNAL(newValue(int)), this, SLOT(onTotalValue(int)));
    QObject::connect(m_ui->fullscreen_btn, SIGNAL(clicked()), this, SIGNAL(fullscreen()));

    enableUI(false);

    setMouseTracking(true);
}

ControlsWidget::~ControlsWidget()
{
    delete m_ui;
}

void ControlsWidget::setFragmentsList(const FragmentsList& fragments_list)
{
    m_fragments_list = fragments_list;
    m_playing_fragment_index = 0;
    m_fragment_played = 0;
    if(m_fragments_list.size() == 1)
    {
        m_ui->prev_btn->setEnabled(false);
        m_ui->next_btn->setEnabled(false);
    }
    else
    {
        m_ui->prev_btn->setEnabled(false);
    }
    m_ui->total_position->setFragmentsList(m_fragments_list);
}

void ControlsWidget::startFragment(int fragment_index)
{
    if(fragment_index < 0 ||
       fragment_index >= m_fragments_list.size())
        return;

    m_playing_fragment_index = fragment_index;
    m_fragment_played = 0;
    if(m_playing_fragment_index == 0)
    {
        m_ui->prev_btn->setEnabled(false);
        m_ui->next_btn->setEnabled(true);
    }
    else if(m_playing_fragment_index == m_fragments_list.size() - 1)
    {
        m_ui->prev_btn->setEnabled(true);
        m_ui->next_btn->setEnabled(false);
    }
    else
    {
        m_ui->prev_btn->setEnabled(true);
        m_ui->next_btn->setEnabled(true);
    }
}

void ControlsWidget::setPlayedTime(BasePlayback* playback)
{
    m_fragment_played = playback->getPlayingTime();
}

void ControlsWidget::clear()
{
    m_player_state = Stopped;
    setPlayBtnIcon();
    m_fragments_list.clear();
    m_playing_fragment_index = -1;
    m_mute = false;
    setMuteBtnIcon();
    m_old_volume = 0;
    m_fullscreen_mode = false;
}

void ControlsWidget::startPlayback()
{
    m_player_state = Playing;
    setPlayBtnIcon();
}

void ControlsWidget::pausePlayback()
{
    m_player_state = Paused;
    setPlayBtnIcon();
}

void ControlsWidget::stopPlayback()
{
    m_player_state = Stopped;
    setPlayBtnIcon();
    m_fragment_played = 0;
}

void ControlsWidget::enableUI(bool enable)
{
    m_ui->play_btn->setEnabled(enable);
    m_ui->stop_btn->setEnabled(enable);
    m_ui->total_position->setEnabled(enable);
    m_ui->mute_btn->setEnabled(enable);
    m_ui->volume->setEnabled(enable);
    m_ui->prev_btn->setEnabled(enable);
    m_ui->next_btn->setEnabled(enable);
    m_ui->fullscreen_btn->setEnabled(enable);
}

void ControlsWidget::fullscreenMode(bool on)
{
    if(on)
    {
        m_fullscreen_mode = true;
        m_ui->fullscreen_btn->hide();
        QFont font = m_ui->current_time->font();
        font.setBold(true);
        m_ui->current_time->setFont(font);
    }
    else
    {
        m_fullscreen_mode = false;
        m_ui->fullscreen_btn->show();
        QFont font = m_ui->current_time->font();
        font.setBold(false);
        m_ui->current_time->setFont(font);
    }
}

void ControlsWidget::updateUI()
{
    setTimePositions();
    setTimeLabels();
}

void ControlsWidget::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    emit mouseMove();
}

void ControlsWidget::paintEvent(QPaintEvent* event)
{
    if(m_fullscreen_mode)
    {
        QPainter painter(this);
        QColor background_color = Qt::lightGray;
        background_color.setAlpha(100);
        painter.setPen(background_color);
        painter.setBrush(background_color);
        painter.drawRect(rect());
    }
    QWidget::paintEvent(event);
}

void ControlsWidget::setPlayBtnIcon()
{
    switch(m_player_state)
    {
    case Stopped:
        m_ui->play_btn->setIcon(QIcon(":/play"));
        m_ui->play_btn->setToolTip("Play");
        break;
    case Playing:
        m_ui->play_btn->setIcon(QIcon(":/pause"));
        m_ui->play_btn->setToolTip("Pause");
        break;
    case Paused:
        m_ui->play_btn->setIcon(QIcon(":/play"));
        m_ui->play_btn->setToolTip("Play");
        break;
    }
    m_ui->play_btn->repaint();
}

void ControlsWidget::setMuteBtnIcon()
{
    if(m_mute)
    {
        m_ui->mute_btn->setIcon(QIcon(":/mute"));
        m_ui->mute_btn->setToolTip("Unmute");
    }
    else
    {
        m_ui->mute_btn->setIcon(QIcon(":/volume"));
        m_ui->mute_btn->setToolTip("Mute");
    }
}

int ControlsWidget::calcTotalMaximum()
{
    int total_length = 0;
    for(int i = 0; i < m_fragments_list.size(); ++i)
        total_length += (int)m_fragments_list[i].getDuration();
    return total_length;
}

int ControlsWidget::calcTotalCurrent()
{
    int total_current = 0;
    for(int i = 0; i < m_fragments_list.size(); ++i)
        if(i < m_playing_fragment_index)
            total_current += (int)m_fragments_list[i].getDuration();
    total_current += m_fragment_played;
    return total_current;
}


void ControlsWidget::setTimePositions()
{
    m_ui->total_position->setMaximum(calcTotalMaximum());
    m_ui->total_position->setValue(calcTotalCurrent());
}

void ControlsWidget::setTimeLabels()
{
    if(m_fragments_list.size() &&
       m_fragments_list[0].getStartTime().isValid())
    {
        //use UTC time
        QString current_time   = "";
        if(m_fragments_list.size())
        {
            QDateTime cur_time = m_fragments_list[m_playing_fragment_index].getStartTime().addMSecs(m_fragment_played);
            if (m_showLocalTime) current_time = cur_time.toLocalTime().toString(DATETIME_CONVERSION_FORMAT);
			else current_time = cur_time.toString("dd-MM-yyyy hh:mm:ss.zzz");
        }
        m_ui->current_time->setText(current_time);
    }
    else
    {
        //use played time
        QTime cur_time;
        cur_time.setHMS(0, 0, 0);
        cur_time = cur_time.addMSecs(m_fragment_played);
        m_ui->current_time->setText(cur_time.toString(DATETIME_SHORT_CONVERSION_FORMAT));
    }
}

void ControlsWidget::onPlayBtn()
{
    switch(m_player_state)
    {
    case Stopped:
        m_player_state = Playing;
		m_ui->next_btn->setEnabled(false);
		emit started();
        break;
    case Playing:
        m_player_state = Paused;
		m_ui->next_btn->setEnabled(true);
		emit paused();
        break;
    case Paused:
        m_player_state = Playing;
		m_ui->next_btn->setEnabled(false);
		emit started();
        break;
    }
    setPlayBtnIcon();
}

void ControlsWidget::onStopBtn()
{
    m_player_state = Stopped;
    setPlayBtnIcon();
    m_fragment_played = 0;
    emit stopped();
}

void ControlsWidget::onVolume(int value)
{
    if(m_mute)
    {
        m_mute = false;
        setMuteBtnIcon();
    }
    emit(volume(value));
}

void ControlsWidget::onFragmentValue(int value)
{
    int old_value = m_fragment_played;
    m_fragment_played = value;
    m_ui->total_position->setValue(m_ui->total_position->value() + m_fragment_played - old_value);
    emit fragment(value);
}

void ControlsWidget::onTotalValue(int value)
{
    //find out fragment index
    int fragment_index = -1;
    int ms = -1;
    int length = 0;
    for(int i = 0; i < m_fragments_list.size(); ++i)
    {
        if(length + (int)m_fragments_list[i].getDuration() > value)
        {
            fragment_index = i;
            ms = value - length;
            break;
        }
        else
            length += (int)m_fragments_list[i].getDuration();
    }
    m_fragment_played = ms;
    if(fragment_index == m_playing_fragment_index)
        emit fragment(ms);
    else
        emit total(fragment_index, ms);
}

void ControlsWidget::onMute()
{
    m_mute = !m_mute;
    if(m_mute)
    {
        m_old_volume = m_ui->volume->value();
        emit(volume(0));
    }
    else
    {
        emit(volume(m_old_volume));
    }
    setMuteBtnIcon();
}
