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

#ifndef CLICKABLESLIDER_H
#define CLICKABLESLIDER_H

#include "crosscompilation_cxx11.h"

#include <QSlider>

#include "segmentInfo.h"

//! Custom slider that can send signal by mose click.
class ClickableSlider : public QSlider
{
private:
    Q_OBJECT

public:
    ClickableSlider(QWidget* parent = 0);

    ~ClickableSlider();

    void setFragmentsList(const SegmentList& fragments_list, int fragment_index = -1);

signals:
    void newValue(int value);

protected:
    virtual void mousePressEvent(QMouseEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);

    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    int calcValue(QMouseEvent* event);

private:
    //! List of fragments - used to define tooltip message.
    SegmentList   m_segments;
    //! Index of fragment to use to define tooltip message.
    int             m_segment_index;
    //! Was mouse pressed or not.
    bool            m_mouse_pressed;
};

#endif // CLICKABLESLIDER_H
