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

#ifndef FRAGMENTINFO_H
#define FRAGMENTINFO_H

#include "crosscompilation_cxx11.h"

#include <QString>
#include <QList>
#include <QFileInfo>
#include <QDateTime>
#include <QMultiMap>
#include <QPair>
#include <QSet>

#include "afIdentificationBox.hpp"
#include "movieHeaderBox.hpp"
#include "mediaHeaderBox.hpp"
#include "trackHeaderBox.hpp"
#include "trackRunBox.hpp"
#include "correctstarttimebox.hpp"
#include "helpers/optional.hpp"
#include "templateTableBoxes.hpp"

//! Class that descibes one frament in video.
class FragmentInfo
{
public:
    FragmentInfo(QString file_name = QString());

    ~FragmentInfo();

    //! Try to extract duration from movie header box
    void readMovieHeaderBox(MovieHeaderBox * box);

    //! Try to extract duration from Surveillance identification box
    void readAfIdentificationBox(AFIdentificationBox * box);

    //! Try to get id of media stream from track header box
    void readTrackHeaderBox(TrackHeaderBox * box);
    void readCorrectionStartTimeBox(CorrectStartTimeBox * box);
    void read(TimeToSampleBox* box);
	void read(CompositionOffsetBox* box);
	void read(TrackRunBox* box);
	void read(MediaHeaderBox* box);

    //! Returns if a fragment is a Surveillance file.
    bool isSurveillanceFragment() const;

    //! Returns fragment start time
    QDateTime getStartTime() const;

    //! Returns fragment finish time
    QDateTime getFinishTime() const;

    //! Returns fragment file path.
    QString getFileName() const;

    //! Returns name, computed in createName function.
    QString getName() const;

    //! Compute a duration of a fragment in milliseconds.
    uint64_t getDuration() const;
	bool hasDuration() const { return m_duration != 0;}

    //! Set duration of a fragment if it is not filled from SUMI box.
    void setDuration(uint64_t duration);

    //! Checks, if a fragment is a beginning fragment. Valid only for Surveillance files.
    bool isBeginning() const;

    //! Checks, if a fragment is an ending fragment. Valid only for Surveillance files.
    bool isEnd() const;

    //! Checks, if a fragment is a successor of a fragment. Valid only for Surveillance files.
    bool isSuccessor(const FragmentInfo & right) const;

    //! Sets the fragment number
    void setFragmentNumber(uint32_t fragment_number);

    //! Gets the fragment number
    uint32_t getFragmentNumber() const;

    //! Returns ids of valid media streams.
    QSet<int> getValidMediaStreamIds() const;

    //! Gets the estimated fps from sample information
    double getFpsFromSamples() const;
private:
    //! Compute name for a fragment. This name will be shown in UI.
    void createName() const;

private:
    //! Fragment number
    uint32_t                        m_fragment_number;
    //! File name.
    QString                         m_file_name;
    //! Predecessor fragment UUID
    QString                         m_predecessor_uuid;
    //! Current fragment UUID
    QString                         m_fragment_uuid;
    //! Successor fragment UUID
    QString                         m_successor_uuid;
    //! Fragment start timestamp in UTC.
    QDateTime                       m_fragment_start;
    //! Set of known media stream ids.
    QSet<int>                       m_valid_stream_ids;
    //! Timescale of the fragment.
    uint32_t                        m_timescale;
    //! Video timescale of the fragment.
    uint32_t                        m_videoTimescale;
    //! Duration of the surveillance fragment in timescales, if present.
    uint64_t                        m_duration;
    //! Name that will be shown in UI.
    mutable QString                 m_name;
    //! Number of samples.
    uint32_t                        m_samples;
    //! Accumulated sample duration
    uint64_t                        m_accumulatedSampleDuration;
	//! Optional Composition offset of last sample
	uint64_t                        m_firstSampleCompositionOffset;
	//! Optional Composition offset of last sample
    uint64_t                        m_lastSampleCompositionOffset;
	uint32_t						m_firstTrackId;
public:
	uint32_t						m_currentParserTrackId;		///< track id currently beingparsed
};

typedef QList<FragmentInfo> FragmentsList;

#endif // FRAGMENTINFO_H
