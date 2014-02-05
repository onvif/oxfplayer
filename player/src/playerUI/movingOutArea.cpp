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

#include "movingOutArea.h"

#include "defines.h"

#include <QTimerEvent>
#include <QResizeEvent>

MovingOutArea::MovingOutArea(QWidget* sub_widget, MovingDirection moving_direction, QWidget* parent) :
    QWidget(parent),
    m_sub_widget(0),
    m_direction(moving_direction)
{
    if(sub_widget)
        setSubWidget(sub_widget);

    setMouseTracking(true);

    QObject::connect(&m_move_in_timer, SIGNAL(timeout()), this, SLOT(onInTimer()));
    QObject::connect(&m_move_out_timer, SIGNAL(timeout()), this, SLOT(onOutTimer()));
    QObject::connect(&m_show_timer, SIGNAL(timeout()), this, SLOT(onShowTimer()));
}

MovingOutArea::~MovingOutArea()
{
    removeSubWidget();
}

void MovingOutArea::setSubWidget(QWidget* sub_widget)
{
    m_sub_widget = sub_widget;

    m_sub_widget->setParent(this);

    resize(sub_widget->size());
    setSubWidgetPosition();

    m_sub_widget->show();

    QObject::connect(m_sub_widget, SIGNAL(mouseMove()), this, SLOT(onMouseMove()));
}

void MovingOutArea::removeSubWidget()
{
    killTimers();

    if(m_sub_widget)
    {
        QObject::disconnect(m_sub_widget, SIGNAL(mouseMove()), this, SLOT(onMouseMove()));
        m_sub_widget->setParent(0);
        m_sub_widget = 0;
    }
}

void MovingOutArea::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    if(!m_move_in_timer.isActive() &&
       !m_move_out_timer.isActive() &&
       !m_show_timer.isActive())
    {
        m_move_in_timer.start(MOVING_AREA_SPEED);
    }
    else if(m_move_in_timer.isActive())
    {

    }
    else if(m_show_timer.isActive())
    {
        m_show_timer.stop();
        m_show_timer.start(MOVING_AREA_SHOW_TIME);
    }
    else if(m_move_out_timer.isActive())
    {
        m_move_out_timer.stop();
        m_move_in_timer.start(MOVING_AREA_SPEED);
    }
}

void MovingOutArea::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_sub_widget->resize(event->size());
    setSubWidgetPosition();
}

void MovingOutArea::setSubWidgetPosition()
{
    switch(m_direction)
    {
    case Up:
        m_sub_widget->move(0, height());
        break;
    case Right:
        m_sub_widget->move(-width(), 0);
        break;
    case Down:
        m_sub_widget->move(0, -height());
        break;
    case Left:
        m_sub_widget->move(width(), 0);
        break;
    }
}

void MovingOutArea::onInTimer()
{
    if(!m_sub_widget)
        return;
    bool stop = false;
    switch (m_direction)
    {
    case Up:
        m_sub_widget->move(m_sub_widget->x(), m_sub_widget->y() - MOVING_AREA_STEP);
        stop = (m_sub_widget->y() <= 0);
        break;
    case Right:
        m_sub_widget->move(m_sub_widget->x() + MOVING_AREA_STEP, m_sub_widget->y());
        stop = (m_sub_widget->x() >= 0);
        break;
    case Down:
        m_sub_widget->move(m_sub_widget->x(), m_sub_widget->y() + MOVING_AREA_STEP);
        stop = (m_sub_widget->y() >= 0);
        break;
    case Left:
        m_sub_widget->move(m_sub_widget->x() - MOVING_AREA_STEP, m_sub_widget->y());
        stop = (m_sub_widget->x() <= 0);
        break;
    }
    if(stop)
    {
        m_sub_widget->move(0, 0);
        m_move_in_timer.stop();
        m_show_timer.start(MOVING_AREA_SHOW_TIME);
    }
}

void MovingOutArea::onOutTimer()
{
    if(!m_sub_widget)
        return;
    bool stop = false;
    switch (m_direction)
    {
    case Up:
        m_sub_widget->move(m_sub_widget->x(), m_sub_widget->y() + MOVING_AREA_STEP);
        stop = (m_sub_widget->y() >= height());
        break;
    case Right:
        m_sub_widget->move(m_sub_widget->x() - MOVING_AREA_STEP, m_sub_widget->y());
        stop = (m_sub_widget->x() <= -width());
        break;
    case Down:
        m_sub_widget->move(m_sub_widget->x(), m_sub_widget->y() - MOVING_AREA_STEP);
        stop = (m_sub_widget->y() <= -height());
        break;
    case Left:
        m_sub_widget->move(m_sub_widget->x() + MOVING_AREA_STEP, m_sub_widget->y());
        stop = (m_sub_widget->x() >= width());
        break;
    }
    if(stop)
    {
        setSubWidgetPosition();
        m_move_out_timer.stop();
    }
}

void MovingOutArea::onShowTimer()
{
    if(!m_sub_widget)
        return;
    m_show_timer.stop();
    m_move_out_timer.start(MOVING_AREA_SPEED);
}

void MovingOutArea::onMouseMove()
{
    if(m_show_timer.isActive())
    {
        m_show_timer.stop();
        m_show_timer.start(MOVING_AREA_SHOW_TIME);
    }
}

void MovingOutArea::killTimers()
{
    m_move_in_timer.stop();
    m_move_out_timer.stop();
    m_show_timer.stop();
}
