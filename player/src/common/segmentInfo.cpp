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

#include "segmentInfo.h"

SegmentInfo::SegmentInfo(QString file_name /*= QString()*/)
    : m_segment_number(0)
    , m_file_name(file_name)
    , m_duration(0)
	, m_samples(0)
	, m_accumulatedSampleDuration(0)
	, m_firstSampleCompositionOffset(0)
	, m_lastSampleCompositionOffset(0)
	, m_videoTimescale(0)
	, m_defaultSampleDuration(0)
{
}

SegmentInfo::~SegmentInfo()
{
}

void SegmentInfo::readMovieHeaderBox(MovieHeaderBox *box)
{
    m_timescale = box->getTimeScale();
	int64_t delta = box->getDuration() * 1000 / m_timescale;
	m_segment_start = box->getCreationTime().addMSecs(-delta);
}

void SegmentInfo::read(MediaHeaderBox*box)
{
	// assume that first track is Video
    if (m_videoTimescale == 0) m_videoTimescale = box->getTimeScale();
}

void SegmentInfo::readAfIdentificationBox(AFIdentificationBox *box)
{
    m_duration = box->getDuration() / m_videoTimescale;
    m_predecessor_uuid = box->getPredecessorUUID().toString();
    m_segment_uuid = box->getFragmentUUID().toString();
    m_successor_uuid = box->getSuccessorUUID().toString();
    m_segment_start = box->getStartTime();
}

void SegmentInfo::readTrackHeaderBox(TrackHeaderBox *box)
{
	m_currentParserTrackId = box->getTrackID();
	if (m_valid_stream_ids.count() == 0) m_firstTrackId = m_currentParserTrackId;
    m_valid_stream_ids << m_currentParserTrackId;
}

void SegmentInfo::readCorrectionStartTimeBox(CorrectStartTimeBox * box)
{
    m_segment_start = box->getStartTime();
}

void SegmentInfo::read(TimeToSampleBox* box)
{
	if (m_currentParserTrackId != m_firstTrackId) return;		// only accumulate first track
	QListIterator<TimeToSampleEntry> it(box->getTable());
	while (it.hasNext()) {
		TimeToSampleEntry e = it.next();
		m_samples += std::get<0>(e);
		m_accumulatedSampleDuration += std::get<0>(e) * std::get<1>(e);
	}
}

void SegmentInfo::read(TrackRunBox* box)
{
	if (m_currentParserTrackId != m_firstTrackId) return;		// only accumulate first track
	QListIterator<TrackRunEntry> it(box->getTable());
	while (it.hasNext()) {
		TrackRunEntry e = it.next();
		auto duration = m_defaultSampleDuration;
		if (std::get<0>(e).hasValue()) duration = std::get<0>(e).value();
		if (duration > 0) {
			m_samples++;
			m_accumulatedSampleDuration += duration;
		}
	}
}

double SegmentInfo::getFpsFromSamples() const
{
	if (m_samples == 0 || m_videoTimescale == 0) return 0.0;
	double duration = (double)(m_accumulatedSampleDuration + m_lastSampleCompositionOffset - m_firstSampleCompositionOffset) / (double)m_videoTimescale;
	return (double)m_samples / duration;
}


void SegmentInfo::read(CompositionOffsetBox* box)
{
	if (m_currentParserTrackId != m_firstTrackId) return;		// only accumulate first track
	CompositionOffsetEntry last = box->getTable().last();
	m_lastSampleCompositionOffset = std::get<1>(last);
	CompositionOffsetEntry first = box->getTable().first();
	m_firstSampleCompositionOffset = std::get<1>(first);
}

bool SegmentInfo::isSurveillanceFragment() const
{
    return !(m_predecessor_uuid.isEmpty() || m_segment_uuid.isEmpty() || m_successor_uuid.isEmpty());
}

QDateTime SegmentInfo::getStartTime() const
{
    return m_segment_start.addMSecs(m_firstSampleCompositionOffset * 1000 / m_videoTimescale);
}

QDateTime SegmentInfo::getFinishTime() const
{
    return m_segment_start.addMSecs(getDuration());
}

QString SegmentInfo::getFileName() const
{
    return m_file_name;
}

QString SegmentInfo::getName() const
{
    if(m_name.isEmpty())
        createName();
    return m_name;
}

uint64_t SegmentInfo::getDuration() const
{
    // Will return 0 for non-surveillance files
    // In this case duration will be determined with the use of FFMpeg methods
	if (m_duration) return 1000 * m_duration / m_timescale;
	else if (m_videoTimescale) return 1000 * (m_accumulatedSampleDuration + m_lastSampleCompositionOffset - m_firstSampleCompositionOffset) / m_videoTimescale;
	return 0;
}

void SegmentInfo::setDuration(uint64_t duration)
{
    if(!duration)
        return;

    m_duration = duration * m_timescale / 1000;
}

bool SegmentInfo::isBeginning() const
{
    return (m_predecessor_uuid == m_segment_uuid);
}

bool SegmentInfo::isEnd() const
{
    return (m_successor_uuid == m_segment_uuid);
}

bool SegmentInfo::isSuccessor(const SegmentInfo & right) const
{
    return (m_successor_uuid == right.m_segment_uuid);
}

void SegmentInfo::setFragmentNumber(uint32_t fragment_number)
{
    m_segment_number = fragment_number;
}

uint32_t SegmentInfo::getFragmentNumber() const
{
    return m_segment_number;
}

QSet<int> SegmentInfo::getValidMediaStreamIds() const
{
    return m_valid_stream_ids;
}

void SegmentInfo::createName() const
{
    if(isSurveillanceFragment())
        m_name = QString("%1\n%2\n%3").arg(QFileInfo(m_file_name).fileName()).arg(getStartTime().toString(DATETIME_CONVERSION_FORMAT)).arg(getFinishTime().toString(DATETIME_CONVERSION_FORMAT));
    else
        m_name = QString("%1\nduration %2 seconds").arg(QFileInfo(m_file_name).fileName()).arg((int)(getDuration()/1000));
}
