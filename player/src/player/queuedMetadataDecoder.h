/************************************************************************************
* Copyright (c) 2021 ONVIF.
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

#ifndef QUEUEDMETADATADECODER_H
#define QUEUEDMETADATADECODER_H
#include <QTreeWidgetItem>

#include "queuedVideoDecoder.h"
#include "videoContext.h"

#include "types.h"

class EventItem : public QTreeWidgetItem
{
public:
    EventItem(int t, size_t ha) : hash(ha) {
        m_time = t;
    }
    int m_time;
    size_t hash;
};

class MetadataDecoder : public QueuedVideoDecoder
{
public:
    MetadataDecoder(QueuedVideoDecoder* vc) : QueuedVideoDecoder(AVMEDIA_TYPE_DATA) { m_decoder = vc; }

    Queue<EventItem*> m_eventQueue;
protected:
    virtual void processPacket(AVPacket* packet, int timestamp_ms);

private:
    //! Try to parse metadata
    void parseMetadata(VideoFrame& frame, const unsigned char* buffer, size_t bytes, int time);
    QueuedVideoDecoder* m_decoder;
};

#endif // QUEUEDMETADATADECODER_H
