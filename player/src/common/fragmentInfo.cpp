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

#include "fragmentInfo.h"

FragmentInfo::FragmentInfo(QString file_name /*= QString()*/)
    : m_fragment_number(0)
    , m_file_name(file_name)
    , m_duration(0)
{
}

FragmentInfo::~FragmentInfo()
{
}

void FragmentInfo::readMovieHeaderBox(MovieHeaderBox *box)
{
    m_timescale = box->getTimeScale();
}

void FragmentInfo::readAfIdentificationBox(AFIdentificationBox *box)
{
    m_duration = box->getDuration();
    m_predecessor_uuid = box->getPredecessorUUID().toString();
    m_fragment_uuid = box->getFragmentUUID().toString();
    m_successor_uuid = box->getSuccessorUUID().toString();
    m_fragment_start = box->getStartTime();
}

void FragmentInfo::readTrackHeaderBox(TrackHeaderBox *box)
{
    m_valid_stream_ids << box->getTrackID();
}

bool FragmentInfo::isSurveillanceFragment() const
{
    return !(m_predecessor_uuid.isEmpty() || m_fragment_uuid.isEmpty() || m_successor_uuid.isEmpty());
}

QDateTime FragmentInfo::getStartTime() const
{
    return m_fragment_start;
}

QDateTime FragmentInfo::getFinishTime() const
{
    return m_fragment_start.addMSecs(getDuration());
}

QString FragmentInfo::getFileName() const
{
    return m_file_name;
}

QString FragmentInfo::getName() const
{
    if(m_name.isEmpty())
        createName();
    return m_name;
}

uint64_t FragmentInfo::getDuration() const
{
    // Will return 0 for non-surveillance files
    // In this case duration will be determined with the use of FFMpeg methods
    return 1000 * m_duration / m_timescale;
}

void FragmentInfo::setDuration(uint64_t duration)
{
    if(!duration)
        return;

    m_duration = duration * m_timescale / 1000;
}

bool FragmentInfo::isBeginning() const
{
    return (m_predecessor_uuid == m_fragment_uuid);
}

bool FragmentInfo::isEnd() const
{
    return (m_successor_uuid == m_fragment_uuid);
}

bool FragmentInfo::isSuccessor(const FragmentInfo & right) const
{
    return (m_successor_uuid == right.m_fragment_uuid);
}

void FragmentInfo::setFragmentNumber(uint32_t fragment_number)
{
    m_fragment_number = fragment_number;
}

uint32_t FragmentInfo::getFragmentNumber() const
{
    return m_fragment_number;
}

QSet<int> FragmentInfo::getValidMediaStreamIds() const
{
    return m_valid_stream_ids;
}

void FragmentInfo::createName() const
{
    if(isSurveillanceFragment())
        m_name = QString("%1\n%2\n%3").arg(QFileInfo(m_file_name).fileName()).arg(getStartTime().toString(DATETIME_CONVERSION_FORMAT)).arg(getFinishTime().toString(DATETIME_CONVERSION_FORMAT));
    else
        m_name = QString("%1\nduration %2 seconds").arg(QFileInfo(m_file_name).fileName()).arg((int)(getDuration()/1000));
}
