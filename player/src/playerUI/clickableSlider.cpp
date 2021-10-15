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

#include "clickableSlider.h"

#include "defines.h"

#include <QMouseEvent>
#include <QToolTip>
#include <QTime>
#include <QDateTime>

ClickableSlider::ClickableSlider(QWidget* parent) :
    QSlider(parent),
    m_fragment_index(-1),
    m_mouse_pressed(false)
{
    setMouseTracking(true);
}

ClickableSlider::~ClickableSlider()
{

}

void ClickableSlider::setFragmentsList(const FragmentsList& fragments_list, int fragment_index)
{
    m_fragments_list = fragments_list;
    m_fragment_index = fragment_index;
}

void ClickableSlider::mousePressEvent(QMouseEvent* event)
{
    m_mouse_pressed = true;

    QSlider::mousePressEvent(event);
}

void ClickableSlider::mouseMoveEvent(QMouseEvent* event)
{
    int value = calcValue(event);

    if(m_fragments_list.size() &&
       m_fragments_list[0].getStartTime().isValid())
    {
        //use UTC time for tooltip
        QDateTime time;

        if(m_fragment_index != -1)
        {
            //single fragment
            time = m_fragments_list[m_fragment_index].getStartTime();
            time = time.addMSecs(value);
        }
        else
        {
            //find out in which fragment we are
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
            if(fragment_index == -1)
                fragment_index = m_fragments_list.size() - 1;
            time = m_fragments_list[fragment_index].getStartTime();
            time = time.addMSecs(ms);
        }

        if(isActiveWindow())
        {
            QString tool_tip_text = time.toString(DATETIME_CONVERSION_FORMAT);
            QToolTip::showText(mapToGlobal(event->pos()), tool_tip_text, this, rect());
        }
    }
    else if(m_fragments_list.size() == 0 ||
            (m_fragments_list.size()  &&
             !m_fragments_list[0].getStartTime().isValid()))
    {
        //simple playing time
        QTime time;
        time.setHMS(0 ,0 ,0);
        time = time.addMSecs(value);
        if(isActiveWindow())
            QToolTip::showText(mapToGlobal(event->pos()), time.toString(DATETIME_SHORT_CONVERSION_FORMAT), this, rect());
    }

    QSlider::mouseMoveEvent(event);
}

void ClickableSlider::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_mouse_pressed)
    {
        int value = calcValue(event);
        setValue(value);
        m_mouse_pressed = false;
        emit newValue(value);
    }

    QSlider::mouseReleaseEvent(event);
}

int ClickableSlider::calcValue(QMouseEvent* event)
{
    int value = 0;
    int min = minimum();
    int max = maximum();
    if (orientation() == Qt::Vertical)
        value = (int)((double)min + (double)(max - min) * (double)(height() - event->position().y())/(double)height());
    else
        value = (int)((double)min + (double)(max - min) * (double)(event->position().x()) / (double)width()) ;
    return value;
}
