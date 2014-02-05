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

#ifndef DECODER_H
#define DECODER_H

#include "ffmpeg.h"
#include "mainContext.h"

#include <QQueue>

//! Base class for decoder.
template<typename T>
class Decoder
{
public:
    Decoder() :
        m_main_context(nullptr),
        m_skip_threshold(-1)
    {}

    virtual ~Decoder()
    {
        stop();
        clear();
    }

    //! Set main context.
    void setMainContext(MainContext* main_context) { m_main_context = main_context; }

    //! Set stream.
    void setStream(AVStream* stream) { m_stream = stream; }

    //! Set skip threshold.
    void setSkipThreshold(int skip_threshold) { m_skip_threshold = skip_threshold; }

    //! Start decoder.
    virtual void start()
    {}

    //! Get (read and decode) next frame.
    virtual bool getNextFrame(T& decoded_frame, void* additional_data = 0) = 0;

    //! Stop decoder.
    virtual void stop()
    {}

    //! Get internal buffers size.
    virtual int buffersSize() const
    {
        return 0;
    }

    //! Clear temporally buffers if exists.
    virtual void clearBuffers()
    {}

    //! Clear.
    virtual void clear()
    {
        stop();

        m_main_context = nullptr;
        m_stream = nullptr;
        m_skip_threshold = -1;
    }

protected:
    //! Main context to read from.
    MainContext*    m_main_context;
    //! Stream to read from.
    AVStream*       m_stream;
    //! Skip threshols.
    int             m_skip_threshold;
};

#endif // DECODER_H
