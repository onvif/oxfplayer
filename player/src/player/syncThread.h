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

#ifndef SYNCTHREAD_H
#define SYNCTHREAD_H

#include "crosscompilation_cxx11.h"

#include <QThread>

//! Base class for all threads that want's to have sync start and stop.
class SyncThread : public QThread
{
public:
    SyncThread(int sleep_timeout_ms, QThread::Priority priority = QThread::NormalPriority);

    virtual ~SyncThread();

    //! Start thread.
    virtual void start();

    //! Stop thread;
    virtual void stop();

protected:
    virtual void run();

    //! This will be called when thread started.
    virtual void threadStarted() {}

    //! Define this function. It will be called in infinity circle.
    virtual bool threadBody() = 0;

    //! This function will be called after thread end.
    virtual void threadFinished() {}

    //! Get started state.
    bool isRunning() const { return m_is_running; }

private:
    //! Flag that thread is running.
    bool                m_is_running;
    //! Flag that thread finished.
    bool                m_is_finished;
    //! Quit from thread.
    volatile bool       m_quit;
    //! How long should we sleep after each circle.
    int                 m_sleep_timeout_ms;
    //! Priority for this new thread.
    QThread::Priority   c_priority;
};

#endif // SYNCTHREAD_H
