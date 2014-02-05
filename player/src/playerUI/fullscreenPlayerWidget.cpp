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

#include "fullscreenPlayerWidget.h"

#include <QCloseEvent>
#include <QResizeEvent>

FullscreenPlayerWidget::FullscreenPlayerWidget(QWidget*parent) :
    QWidget(parent),
    PlayerWidgetInterface(),
    m_video_frame(this),
    m_controls_moving_area(0, Up, this)
{
    m_video_frame.show();
    m_controls_moving_area.move(0, 0);
    m_controls_moving_area.resize(0, 0);
    m_controls_moving_area.show();
}

FullscreenPlayerWidget::~FullscreenPlayerWidget()
{

}

void FullscreenPlayerWidget::setControls(ControlsWidget* controls)
{
    PlayerWidgetInterface::setControls(controls);
    m_controls_moving_area.setSubWidget(m_controls);
}

void FullscreenPlayerWidget::removeControls()
{
    m_controls_moving_area.removeSubWidget();
    PlayerWidgetInterface::removeControls();
}

void FullscreenPlayerWidget::relocateControls()
{
    m_controls_moving_area.resize(width(), 150);
    m_controls_moving_area.move(0, height() - 150);
}

void FullscreenPlayerWidget::closeEvent(QCloseEvent* event)
{
    event->ignore();
}

void FullscreenPlayerWidget::resizeEvent(QResizeEvent* event)
{
    m_video_frame.resize(event->size());
    m_video_frame.move(0, 0);
    relocateControls();
}

void FullscreenPlayerWidget::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);

    m_video_frame.grabKeyboard();
}

void FullscreenPlayerWidget::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event);

    m_video_frame.releaseKeyboard();
}
