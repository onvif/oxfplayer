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

#include "mainContext.h"

#include <QFile>

MainContext::MainContext(AVMediaType stream_type) :
    m_stream_type(stream_type),
    m_format_context(nullptr),
    m_metadataIndex(-1)
{
}

MainContext::~MainContext()
{
    clear();
}

bool MainContext::open(const QString& file_name, const QSet<int>& valid_streams)
{
    clear();
    if(!init(file_name, valid_streams))
    {
        clear();
        return false;
    }

    return true;
}

void MainContext::clear()
{
    if(m_format_context != nullptr)
    {
        for(int i = 0; i < m_streams.size(); ++i)
            m_streams[i].clear();
        m_streams.clear();
        avformat_close_input(&m_format_context);
        m_format_context = nullptr;
    }
}

AVStream* MainContext::getStream(int index)
{
    if(index >= 0 &&
       index < m_streams.size())
        return m_streams[index].m_stream;
    return 0;
}

AVCodecContext* MainContext::getCodecContext(int index)
{
    if(index >= 0 &&
       index < m_streams.size())
        return m_streams[index].m_codec_context;
    return 0;
}

bool MainContext::seek(int timestamp_ms)
{
    if(!m_format_context)
        return false;

    int pos = 0, res = 0;
    for(QVector<StreamInfo>::const_iterator cIter = m_streams.constBegin(); cIter != m_streams.constEnd(); ++cIter)
    {
        pos = cIter->timeMsToPts(timestamp_ms);
        res = avformat_seek_file(m_format_context, cIter->m_stream->index, 0, pos, pos, 0);
        if(res < 0)
            res = avformat_seek_file(m_format_context, cIter->m_stream->index, 0, pos, pos, AVSEEK_FLAG_ANY);
        avcodec_flush_buffers(cIter->m_codec_context);
    }
    return true;
}

void MainContext::timeToPTS(int timestamp_ms, QVector<int>& pts_vector)
{
    pts_vector.clear();
    for(QVector<StreamInfo>::const_iterator cIter = m_streams.constBegin(); cIter != m_streams.constEnd(); ++cIter)
        pts_vector.push_back(cIter->timeMsToPts(timestamp_ms));
}

bool MainContext::init(const QString& file_name, const QSet<int>& valid_streams)
{
    if(file_name.isEmpty() ||
       !QFile::exists(file_name))
        return false;

    if(avformat_open_input(&m_format_context, file_name.toUtf8().data(), 0, 0) != 0)
        return false;

    if(avformat_find_stream_info(m_format_context, 0) < 0)
        return false;

    for(unsigned int index = 0; index < m_format_context->nb_streams; ++index)
    {
        auto codec_type = m_format_context->streams[index]->codec->codec_type;
        if(m_format_context->streams[index]->codec->codec_type != m_stream_type
            && !(m_stream_type == AVMEDIA_TYPE_VIDEO && codec_type == AVMEDIA_TYPE_DATA) ||
           (valid_streams.size() &&
            !valid_streams.contains(m_format_context->streams[index]->id))
          )
            continue;
        StreamInfo si;
        si.m_type = m_stream_type;
        si.m_index = index;
        if(openStream(index, si.m_stream, si.m_codec_context, si.m_codec))
            m_streams.push_back(si);

        if (m_stream_type == AVMEDIA_TYPE_VIDEO && codec_type == AVMEDIA_TYPE_DATA) {
            m_metadataIndex = index;
        }
    }

    return true;
}

bool MainContext::openStream(int index, AVStream*& stream, AVCodecContext*& codec_context, AVCodec*& codec)
{
    stream = m_format_context->streams[index];

    if(stream == nullptr)
        return false;

    codec_context = stream->codec;

    codec = avcodec_find_decoder(codec_context->codec_id);
    if(codec == nullptr)
        return false;

    if(avcodec_open2(codec_context, codec, 0) != 0)
        return false;

    return true;
}

int MainContext::StreamInfo::timeMsToPts(int timestamp_ms) const
{
    return (int)(((double)timestamp_ms / av_q2d(m_stream->time_base)) / 1000.0);
}
