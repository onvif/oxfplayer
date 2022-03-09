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

#include "syncThread.h"

#include "defines.h"

SyncThread::SyncThread(int sleep_timeout_ms, QThread::Priority priority) :
    QThread(),
    m_is_running(false),
    m_is_finished(false),
    m_quit(false),
    m_sleep_timeout_ms(sleep_timeout_ms),
    c_priority(priority)
{
    if(m_sleep_timeout_ms < 0)
        m_sleep_timeout_ms = 0;
}

SyncThread::~SyncThread()
{
    stop();
}

void SyncThread::start()
{
    if(m_is_running)
        return;
    m_quit = false;
    m_is_running = true;
    m_is_finished = false;
    QThread::start(c_priority);
}

void SyncThread::stop()
{
    if(!m_is_running)
        return;
    m_quit = true;
    wait();
    while(m_is_running)
        msleep(WAIT_TREAD);
}

void SyncThread::run()
{

    threadStarted();

    for(; m_quit != true; )
    {
        if(!threadBody())
            m_quit = true;
        else if(m_sleep_timeout_ms)
            msleep(m_sleep_timeout_ms);
    }

    threadFinished();

    m_is_running = false;
    m_is_finished = true;
}
