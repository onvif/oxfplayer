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

#include "fragmentExtractor.h"

#include "afIdentificationBox.hpp"
#include "mediaHeaderBox.hpp"
#include "movieHeaderBox.hpp"
#include "trackHeaderBox.hpp"

FragmentExtractor::FragmentExtractor(QObject *parent) :
    QObject(parent)
{
}

FragmentsList FragmentExtractor::getFragmentsList()
{
    if(m_fragments_have_surveillance_boxes)
    {
        uint32_t fragment_number = 0;
        auto beginning_fragment = std::find_if(m_fragments_list.begin(), m_fragments_list.end(), [] (FragmentInfo & fi) -> bool {return fi.isBeginning();});
        if(beginning_fragment != m_fragments_list.end())
        {
            beginning_fragment->setFragmentNumber(fragment_number++);
            m_fragments_list.move(beginning_fragment - m_fragments_list.begin(), 0);
        }
        for(int i = 0; i < m_fragments_list.size() - 1; ++i)
        {
            const FragmentInfo & current_fragment = m_fragments_list.at(i);
            auto next_fragment = std::find_if(m_fragments_list.begin() + i + 1, m_fragments_list.end(), [&current_fragment] (FragmentInfo & fi) -> bool {return current_fragment.isSuccessor(fi);});
            if(next_fragment != m_fragments_list.end())
            {
                next_fragment->setFragmentNumber(fragment_number++);
                m_fragments_list.move(next_fragment - m_fragments_list.begin(), i + 1);
            }
        }
    }
    return m_fragments_list;
}

void FragmentExtractor::onContentsCleared()
{
    m_fragments_list.clear();
    m_fragments_have_surveillance_boxes = true;
}

void FragmentExtractor::onFileOpened(QString path)
{
    m_fragments_list.append(FragmentInfo(path));
}

void FragmentExtractor::onFileClosed()
{
    if(m_fragments_have_surveillance_boxes)
        m_fragments_have_surveillance_boxes = m_fragments_list.back().isSurveillanceFragment();
}

void FragmentExtractor::onBoxCreated(Box *box)
{
    static const FourCC sc_mvhd_four_cc(MovieHeaderBox::getFourCC()),
            sc_sumi_four_cc(AFIdentificationBox::getFourCC()),
            sc_tkhd_four_cc(TrackHeaderBox::getFourCC());

    FourCC four_cc = box->getBoxFourCC();

    if(!m_fragments_list.isEmpty())
    {
        if(four_cc == sc_mvhd_four_cc)
        {
            m_fragments_list.back().readMovieHeaderBox(dynamic_cast<MovieHeaderBox *>(box));
        }
        else if(four_cc == sc_sumi_four_cc)
        {
            m_fragments_list.back().readAfIdentificationBox(dynamic_cast<AFIdentificationBox *>(box));
        }
        else if(four_cc == sc_tkhd_four_cc)
        {
            m_fragments_list.back().readTrackHeaderBox(dynamic_cast<TrackHeaderBox *>(box));
        }
    }
}
