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

#include "streamReader.h"

#include "videoContext.h"

#include <QDebug>

VideoContext::VideoContext()
{
    clear();
}

VideoContext::~VideoContext()
{

}

bool VideoContext::open(AVStream* stream, double fpsHint)
{
    m_video_stream = stream;
    if(m_video_stream == nullptr) return false;
	if (fpsHint != 0.0) m_fps = fpsHint;
	if (m_video_stream->avg_frame_rate.num != 0) {
	    m_fps = av_q2d(m_video_stream->avg_frame_rate);
	}
    m_current_fps = m_fps;

    qDebug() << "FPS" << m_fps;

    return true;
}

void VideoContext::clear()
{
    m_video_stream = nullptr;
    m_fps = 1.0;
    m_current_fps = 1.0;
}

void VideoContext::increaseFps()
{
    m_current_fps += 1.0;
}

void VideoContext::decreaseFps()
{
    m_current_fps -= 1.0;
    if(m_current_fps < 1.0)
        m_current_fps += 1.0;
}

void VideoContext::flushCurrentFps()
{
    m_current_fps = m_fps;
}

int VideoContext::getTimerDelay()
{
    return (int)(1000.0 / m_current_fps);
}
