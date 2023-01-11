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

#include "segmentExtractor.h"

#include "afIdentificationBox.hpp"
#include "mediaHeaderBox.hpp"
#include "movieHeaderBox.hpp"
#include "trackHeaderBox.hpp"
#include "trackFragmentHeaderBox.hpp"
#include "correctstarttimebox.hpp"

SegmentExtractor::SegmentExtractor(QObject *parent) :
    QObject(parent)
{
}

SegmentList SegmentExtractor::getSegments()
{
    if(m_fragments_have_surveillance_boxes)
    {
        uint32_t fragment_number = 0;
        auto beginning_fragment = std::find_if(m_segments.begin(), m_segments.end(), [] (SegmentInfo & fi) -> bool {return fi.isBeginning();});
        if(beginning_fragment != m_segments.end())
        {
            beginning_fragment->setFragmentNumber(fragment_number++);
            m_segments.move(beginning_fragment - m_segments.begin(), 0);
        }
        for(int i = 0; i < m_segments.size() - 1; ++i)
        {
            const SegmentInfo & current_fragment = m_segments.at(i);
            auto next_fragment = std::find_if(m_segments.begin() + i + 1, m_segments.end(), [&current_fragment] (SegmentInfo & fi) -> bool {return current_fragment.isSuccessor(fi);});
            if(next_fragment != m_segments.end())
            {
                next_fragment->setFragmentNumber(fragment_number++);
                m_segments.move(next_fragment - m_segments.begin(), i + 1);
            }
        }
    }
    return m_segments;
}

void SegmentExtractor::onContentsCleared()
{
    m_segments.clear();
    m_fragments_have_surveillance_boxes = true;
}

void SegmentExtractor::onFileOpened(QString path)
{
    m_segments.append(SegmentInfo(path));
}

void SegmentExtractor::onFileClosed()
{
    if(m_fragments_have_surveillance_boxes)
        m_fragments_have_surveillance_boxes = m_segments.back().isSurveillanceFragment();
}

void SegmentExtractor::onBoxCreated(Box *box)
{
	switch((uint32_t)box->getBoxFourCC()) {
	case 'mvhd':
		m_segments.back().readMovieHeaderBox(dynamic_cast<MovieHeaderBox *>(box));
		break;
	case 'sumi':
		m_segments.back().readAfIdentificationBox(dynamic_cast<AFIdentificationBox *>(box));
		break;
	case 'tkhd':
		m_segments.back().readTrackHeaderBox(dynamic_cast<TrackHeaderBox *>(box));
		break;
	case 'cstb':
		m_segments.back().readCorrectionStartTimeBox(dynamic_cast<CorrectStartTimeBox *>(box));
		break;
	case 'stts':
		m_segments.back().read(dynamic_cast<TimeToSampleBox*>(box));
		break;
	case 'ctts':
		m_segments.back().read(dynamic_cast<CompositionOffsetBox*>(box));
		break;
	case 'tfhd':
	{
		auto tfhd = dynamic_cast<TrackFragmentHeaderBox*>(box);
		m_segments.back().m_currentParserTrackId = tfhd->getTrackID();
		m_segments.back().m_defaultSampleDuration = tfhd->getDefaultSampleDuration().hasValue() ? tfhd->getDefaultSampleDuration().value() : 0;
		break;
	}
	case 'trun':
		m_segments.back().read(dynamic_cast<TrackRunBox*>(box));
		break;
	case 'mdhd':
		m_segments.back().read(dynamic_cast<MediaHeaderBox*>(box));
		break;
	}
}
