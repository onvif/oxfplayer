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

#ifndef MOVINGOUTAREA_H
#define MOVINGOUTAREA_H

#include <QWidget>
#include <QTimer>

#include "enums.h"

//! This widget will slowly show sub_widget by mouse move.
//! Do not put this widget to layout.
//! Sub widget can be NULL and can be setuped or removed later.
class MovingOutArea : public QWidget
{
private:
    Q_OBJECT

public:
    MovingOutArea(QWidget* sub_widget, MovingDirection moving_direction, QWidget* parent = 0);

    ~MovingOutArea();

    //! Setup sub widget.
    void setSubWidget(QWidget* sub_widget);

    //! Remove sub widget.
    void removeSubWidget();

protected:
    virtual void mouseMoveEvent(QMouseEvent* event);

    virtual void resizeEvent(QResizeEvent* event);

private:
    //! Set sub widget start position.
    void setSubWidgetPosition();

private slots:
    //! On in timer.
    void onInTimer();

    //! On out timer.
    void onOutTimer();

    //! On show timer.
    void onShowTimer();

    //! When mouse moved over sub widget.
    void onMouseMove();

private:
    //! Kill all timers.
    void killTimers();

private:
    //! Sub widget.
    QWidget*        m_sub_widget;
    //! Moving direction.
    MovingDirection m_direction;
    //! Moving in timer.
    QTimer          m_move_in_timer;
    //! Moving out timer.
    QTimer          m_move_out_timer;
    //! Showing timer.
    QTimer          m_show_timer;
};

#endif // MOVINGOUTAREA_H
