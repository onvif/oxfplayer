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

#ifndef NONQUEUEDDECODER_H
#define NONQUEUEDDECODER_H

#include "decoder.h"

template<typename T>
class NonQueuedDecoder : public Decoder<T>
{
public:
    NonQueuedDecoder() :
        Decoder<T>()
    {}

    virtual ~NonQueuedDecoder()
    {}

    virtual bool getNextFrame(T& decoded_frame, void* additional_data = 0)
    {
        decoded_frame.clear();

        if(!Decoder<T>::m_main_context ||
           !Decoder<T>::m_stream)
            return false;

        bool ret = true;

        AVPacket *packet = av_packet_alloc();

        while(true)
        {
            int read_result = av_read_frame(Decoder<T>::m_main_context->getFormatContext(), packet);
            if(read_result == 0)
            {
                //packet readed
                if(packet->stream_index == Decoder<T>::m_stream->index)
                {
                    if (processPacket(packet, Decoder<T>::m_stream, decoded_frame, additional_data))
                        break;
                }
            }
            else
            {
                //file ended.
                ret = false;
                break;
            }
        }
        av_packet_unref(packet);

        return ret;
    }

protected:
    //! Main method to decode video packet.
    /*!
     * \param packet packet to decode
     * \param stream video stream where packet from
     * \param frame frame where to place decoded data
     * \param additional_data additional data used for/after decoding
     */
    virtual bool processPacket(AVPacket* packet, AVStream* stream, T& decoded_frame, void* additional_data = 0) = 0;
};

#endif // NONQUEUEDDECODER_H
