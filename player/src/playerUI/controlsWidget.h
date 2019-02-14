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

#ifndef CONTROLSWIDGET_H
#define CONTROLSWIDGET_H

#include "crosscompilation_cxx11.h"

#include <QWidget>

#include "enums.h"
#include "fragmentInfo.h"
#include "basePlayback.h"

namespace Ui {
class ControlsWidget;
}

//! UI component with controls.
class ControlsWidget : public QWidget
{
private:
    Q_OBJECT
    
public:
    ControlsWidget(QWidget* parent = 0);

    ~ControlsWidget();

    //! Setup fragments.
    void setFragmentsList(const FragmentsList& fragments_list);

    //! Start playback with selected length at some total position.
    void startFragment(int fragment_index);

	//! Setup time labels.
	void setTimeLabels();

	public slots:
    //! Set played time for fragment.
    void setPlayedTime(BasePlayback* playback);

public:
    //! Clear.
    void clear();

    //! Change UI state to playing state.
    void startPlayback();

    //! Change UI state to pauseds state.
    void pausePlayback();

    //! Change UI state to stopped state.
    void stopPlayback();

    //! Enable/disable UI (due to is file opened or not)
    void enableUI(bool enable);

    //! Switch fullscreen on/off.
    void fullscreenMode(bool on);

    //! Update UI. Call this function after all updates.
    void updateUI();

	//! Set played time for fragment.
	void setPlayedTime(int time) { m_fragment_played = time; }

signals:
    //! Started.
    void started();

    //! Paused.
    void paused();

    //! Stopped.
    void stopped();

    //! Previous fragment needed.
    void prevFragment();

    //! Next fragment needed.
    void nextFragment();

    //! Volume level changed.
    void volume(int value);

    //! Fragment scrolled to some position.
    void fragment(int ms);

    //! Total time scrolled to some position.
    void total(int fragment_index, int ms);

    //! Fullscreen button was pressed.
    void fullscreen();

    //! Mouse moved over controls.
    void mouseMove();

protected:
    virtual void mouseMoveEvent(QMouseEvent* event);

    virtual void paintEvent(QPaintEvent* event);

private:
    //! Set up button icons.
    void setPlayBtnIcon();

    //! Set up mute icon.
    void setMuteBtnIcon();

    //! Calc total slider maximum.
    int calcTotalMaximum();

    //! Calc total slider current value.
    int calcTotalCurrent();

    //! Set up time sliders position.
    void setTimePositions();

private slots:
    //! Play button pressed.
    void onPlayBtn();

    //! Stop button pressed.
    void onStopBtn();

    //! Volume changed.
    void onVolume(int value);

    //! Fragment slider changed his value.
    void onFragmentValue(int value);

    //! Total slider changed (seeking).
    void onTotalValue(int value);

    //! Mute button pressed.
    void onMute();

private:
    //! UI.
    Ui::ControlsWidget* m_ui;
    //! Player state.
    PlayerState         m_player_state;
    //! Fragments list.
    FragmentsList       m_fragments_list;
    //! Currently playing fragment.
    int                 m_playing_fragment_index;
    //! Current position in fragment.
    int                 m_fragment_played;
    //! Is mute on.
    bool                m_mute;
    //! Old volume level for mute function.
    int                 m_old_volume;
    //! Is in fullscreen mode.
    bool                m_fullscreen_mode;
public:
	bool				m_showLocalTime;
};

#endif // CONTROLSWIDGET_H
