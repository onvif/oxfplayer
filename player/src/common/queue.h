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

#ifndef QUEUE_H
#define QUEUE_H

#include <QMutex>
#include <QMutexLocker>
#include <QQueue>

//! Template class for queue.
template<typename T>
class Queue
{
public:
    Queue()
    {

    }

    ~Queue()
    {

    }

    //! Put object at the end of a queue.
    void push(const T& t)
    {
        QMutexLocker locker(&m_mutex);

        m_queue.enqueue(t);
    }

    //! Get head element size in bytes.
    /*!
     *  Currently this fuction is valid and used only for T with size() method (Queue<QByteArray> for example).
     */
    int headSize()
    {
        if(!m_queue.isEmpty())
            return m_queue.head().size();

        return 0;
    }

    //! Get head element time mamber value.
    /*!
     *  Currently this fuction is valid and used only for T with m_time member (Queue<VideoFrame> for example).
     */
    int headTime()
    {
        if(!m_queue.isEmpty())
            return m_queue.head().m_time;

        return 0;
    }

    //! Get head element pts mamber value.
    /*!
     *  Currently this fuction is valid and used only for T with m_selectedPts member (Queue<VideoFrame> for example).
     */
    int64_t headPts()
    {
        if(!m_queue.isEmpty())
            return m_queue.head().m_selected_pts;

        return 0;
    }

    //! Get head element from queue.
    T pop()
    {
        QMutexLocker locker(&m_mutex);

        if(!m_queue.isEmpty())
            return m_queue.dequeue();

        return T();
    }

    bool empty() const { return m_queue.isEmpty(); }

    //! Count of elements in queue.
    int size()
    {
        QMutexLocker locker(&m_mutex);

        return m_queue.size();
    }

    //! Size in bytes of data, stored in queue.
    int dataSize() const
    {
        QMutexLocker locker(&m_mutex);

        int data_size = 0;

        for(typename QQueue<T>::const_iterator cIter = m_queue.constBegin(); cIter != m_queue.constEnd(); ++cIter)
            data_size += cIter->size();

        return data_size;
    }

    //! Clear queue.
    void clear()
    {
        QMutexLocker locker(&m_mutex);

        m_queue.clear();
    }

private:
    //! Qt mutext to guard queue operations.
    mutable QMutex  m_mutex;
    //! Qt queue used as container.
    QQueue<T>       m_queue;
};

#endif // QUEUE_H
