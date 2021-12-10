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

#ifndef QUEUEDDECODER_H
#define QUEUEDDECODER_H

#include "decoder.h"
#include "syncThread.h"

#include "defines.h"
#include "queue.h"

template<typename T>
class QueuedDecoder : public Decoder<T>, public SyncThread
{
public:
    QueuedDecoder(AVMediaType type) :
        Decoder<T>(type),
        SyncThread(DECODE_SLEEP_TIMEOUT, QThread::HighPriority)
    {}

    virtual ~QueuedDecoder()
    {}

    virtual bool getNextFrame(T& decoded_frame, void* additional_data = 0)
    {
        Q_UNUSED(additional_data);

        decoded_frame.clear();

        if(m_queue.size())
        {
            decoded_frame = m_queue.pop();
            return true;
        }

        return false;
    }

    virtual void start()
    {
        //do not start if no context set or no streams added
        if (Decoder<T>::m_stream == nullptr)
            return;

        SyncThread::start();
    }
    void wait() {
        while(isRunning() &&
              m_queue.size() < MINIMUM_FRAMES_IN_QUEUE_TO_START)
            msleep(WAIT_TREAD);
    }

    virtual void stop()
    {
        SyncThread::stop();

        Decoder<T>::m_skip_threshold = -1;
    }

    virtual void clearBuffers()
    {
        Decoder<T>::clearBuffers();
        m_queue.clear();
    }

    virtual void clear()
    {
        Decoder<T>::clear();
        m_queue.clear();
    }

protected:
    //!  Process function. Decode here.
    virtual void processPacket(AVPacket* packet, int* readed_frames) = 0;

    //! Thread body.
    virtual bool threadBody()
    {
        //check do we need to decode some more frames
        if(m_queue.size() < MINIMUM_FRAMES_IN_QUEUE)
        {
            //read another frame
            //exit if file ended
            AVPacket *packet = av_packet_alloc();
            int readed_frames = 0;
            for(;readed_frames < FRAMES_TO_READ;)
            {
                auto ctx = getFormatContext();
                if (ctx == 0) return false;

                int read_result = av_read_frame(ctx, packet);

                if(read_result == 0)
                {
                    //packet read normally
                    if(packet->stream_index == Decoder<T>::m_stream->index)
                    {
                        processPacket(packet, &readed_frames);
                    }
                }
                else
                {
                    //end of file
                    return false;
                }
            }
            av_packet_unref(packet);
        }

        return true;
    }

public:
    //! Queue with decoded frames.
    Queue<T>        m_queue;
};

#endif // QUEUEDDECODER_H
