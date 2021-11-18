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

#ifndef QUEUEDAUDIODECODER_H
#define QUEUEDAUDIODECODER_H

#include "queuedDecoder.h"
#include "audioContext.h"

#include "types.h"

class QueuedAudioDecoder : public QueuedDecoder<AudioFrame>
{
public:
    QueuedAudioDecoder();

    ~QueuedAudioDecoder();

    void setIndex(int index = -1);
    //! Set audio parameters used resample readed audio data.
    const AudioParams &getParams() { return m_context.m_audio_params; }
    virtual void clear();

    int getIndex() const { return m_index; }

protected:
    virtual void processPacket(AVPacket* packet, int* readed_frames);

private:
    //! Init resample context.
    void initSwrContext(AVFrame* frame);

    //! Clear resample context.
    void cleatSwrContext();

private:
    //! Audio context.
    AudioContext    m_context;
    //! Resampling context.
    SwrContext* m_swr_context;
    int m_index;
};

#endif // QUEUEDAUDIODECODER_H
