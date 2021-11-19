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

#ifndef MAINCONTEXT_H
#define MAINCONTEXT_H

#include "crosscompilation_cxx11.h"

#include "ffmpeg.h"

#include <QString>
#include <QSet>
#include <QVector>

/**
 * Class that contains main information about file in terms of ffmpeg. 
 * Each instance contains information about one type of stream (Video, Audio, Metadata).
 * If multiple streams per type are available these can be selected by zero based instance
 * which is internally translated to the track ID.
 */
class StreamReader
{
public:
    StreamReader(AVMediaType stream_type);

    ~StreamReader();

    //! Init MainContext with file.
    bool open(const QString& file_name, const QSet<int>& valid_streams = QSet<int>());

    //! Clear MainContext;
    void clear();

    AVFormatContext* getFormatContext() { return m_format_context; }

    //! Get streams count.
    int getStreamsCount() const { return m_streams.size(); }

    //! Get stream by index.
    AVStream* getStream(int index) const;

    //! Get codec context by stream index.
    AVCodecContext* getCodecContext(int index);

    //! Seek to some time with some deviation.
    /*!
     * \param timestamp_ms time to seek to
     * \return
     */
    bool seek(int timestamp_ms);

    //! Conver time in ms to pts for supported streams.
    void timeToPTS(int timestamp_ms, QVector<int>& pts_vector);

private:
    //! Init with file.
    bool init(const QString& file_name, const QSet<int>& valid_streams = QSet<int>());

    //! Init stream by index.
    bool openStream(int index, AVStream*& stream, AVCodecContext*& codec_context, AVCodec*& codec);

private:
    //! Structure that describes one stream in video file.
    struct StreamInfo
    {
        //! Stream index.
        int             m_index;
        //! Stream.
        AVStream*       m_stream;
        //! Codec context.
        AVCodecContext* m_codec_context;
        //! Codec.
        AVCodec*        m_codec;

        StreamInfo() :
            m_index(-1),
            m_stream(nullptr),
            m_codec_context(nullptr),
            m_codec(nullptr)
        {}

        //! Close codec.
        void clear()
        {
            if(m_codec != nullptr)
                avcodec_close(m_codec_context);
        }

        //! Convert time in ms to pts.
        int timeMsToPts(int timestamp_ms) const;
    };

    //! Stream type
    AVMediaType         m_stream_type;
    //! Main format context.
    AVFormatContext*    m_format_context;
    //! QVector of streams.
    QVector<StreamInfo> m_streams;
};

#endif // MAINCONTEXT_H
