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

#include "videoFrameWidget.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

VideoFrameWidget::VideoFrameWidget(QWidget* parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

VideoFrameWidget::~VideoFrameWidget()
{

}

void VideoFrameWidget::setDrawImage(const QImage& image)
{
    m_source_image = image;
    if(!m_source_image.isNull())
    {
        if(m_source_image.size().width() == size().width() ||
           m_source_image.size().height() == size().height())
            m_draw_image = m_source_image;
        else
            m_draw_image = m_source_image.scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation);
    }
    else
        m_draw_image = m_source_image;

    repaint();
}

void VideoFrameWidget::clear()
{
    m_source_image = m_draw_image = QImage();

    repaint();
}

void VideoFrameWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    //draw black background
    painter.fillRect(rect(), Qt::black);
    if(!m_draw_image.isNull())
    {
        //find out where to place image
        int x_pos = (size().width() - m_draw_image.size().width()) / 2;
        int y_pos = (size().height() - m_draw_image.size().height()) / 2;
        painter.drawImage(x_pos, y_pos, m_draw_image);
    }
}

void VideoFrameWidget::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    //just resize existing picture till we get next frame
    if(!m_source_image.isNull())
        m_draw_image = m_source_image.scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation);
}

void VideoFrameWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    emit doubleClick();
}

void VideoFrameWidget::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);

    if(event->key() == Qt::Key_Escape)
        emit escapePressed();
    else if(event->key() == Qt::Key_Space)
        emit spacePressed();
}
