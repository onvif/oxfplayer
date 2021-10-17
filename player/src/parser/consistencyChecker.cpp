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

#include "consistencyChecker.h"

#include "basic/unknownBox.h"
#include "templateContentBoxes.hpp"
#include "templateFullBoxes.hpp"
#include "templateSuperBoxes.hpp"
#include "templateSuperFullBoxes.hpp"
#include "templateTableBoxes.hpp"
#include "additionalUserInformation.hpp"
#include "afIdentificationBox.hpp"
#include "cameraMicrophoneIdentificationBox.hpp"
#include "certificateBox.hpp"
#include "compactSampleSizeBox.hpp"
#include "editListBox.hpp"
#include "fileTypeBox.hpp"
#include "mediaHeaderBox.hpp"
#include "movieExtendsHeaderBox.hpp"
#include "movieHeaderBox.hpp"
#include "sampleDependencyTypeBox.hpp"
#include "sampleSizeBox.hpp"
#include "signatureBox.hpp"
#include "signatureConfigurationBox.hpp"
#include "surveillanceExportBox.hpp"
#include "surveillanceMetadataSampleConfigBox.hpp"
#include "surveillanceMetadataSampleEntryBox.hpp"
#include "trackFragmentHeaderBox.hpp"
#include "trackFragmentRandomAccessBox.hpp"
#include "trackHeaderBox.hpp"
#include "trackRunBox.hpp"
#include "correctstarttimebox.hpp"

ConsistencyChecker::ConsistencyChecker()
    : m_current_box(nullptr)
{
    registerChecker<MovieBox>( [this] () {
        // Checks that MovieExtendsBox, if present, contains exactly the same count of TrackExtendsBox as there are TrackBox in a MovieBox
        if(m_counter_info[MovieExtendsBox::getFourCC()] == 1)
        {
            auto movie_extends_boxes = extractBoxes(dynamic_cast<ChildrenMixin *>(m_current_box), MovieExtendsBox::getFourCC());
            if(!movie_extends_boxes.isEmpty())
            {
                MovieExtendsBox * movie_extends_box = dynamic_cast<MovieExtendsBox *>(movie_extends_boxes.front());
                CounterInfo counter_info = countBoxes(movie_extends_box);

                FourCC four_cc = TrackExtendsBox::getFourCC();

                if(counter_info[four_cc] < m_counter_info[TrackBox::getFourCC()])
                {
                    emit mandatoryBoxIsMissing(movie_extends_box, four_cc);
                }
                else if(counter_info[four_cc] > m_counter_info[TrackBox::getFourCC()])
                {
                    emit boxCountIsExceeding(movie_extends_box, extractBoxes(movie_extends_box, four_cc));
                }
            }
        }

        this->exactlyOneCheck<MovieHeaderBox>();
        this->oneOrMoreCheck<TrackBox>();
        this->zeroOrOneCheck<AdditionalMetadataContainerBox>();
        this->zeroOrOneCheck<InitialObjectDescriptorBox>();
        this->zeroOrOneCheck<MetaBox>();
        this->zeroOrOneCheck<MovieExtendsBox>();
        this->zeroOrOneCheck<UserDataBox>();
    });

    registerChecker<TrackBox>( [this] () {
        this->exactlyOneCheck<MediaBox>();
        this->exactlyOneCheck<TrackHeaderBox>();
        this->zeroOrOneCheck<AdditionalMetadataContainerBox>();
        this->zeroOrOneCheck<EditBox>();
        this->zeroOrOneCheck<MetaBox>();
        this->zeroOrOneCheck<TrackReferenceBox>();
        this->zeroOrOneCheck<UserDataBox>();

        freeBoxesCheck();
    });

    registerChecker<MediaBox>( [this] () {
        this->exactlyOneCheck<MediaHeaderBox>();
        this->exactlyOneCheck<HandlerBox>();
        this->exactlyOneCheck<MediaInformationBox>();
    });

    registerChecker<MediaInformationBox>( [this] () {
        this->exactlyOneOfCheck(QList<FourCC>()
                          << HintMediaHeaderBox::getFourCC()
                          << NullMediaHeaderBox::getFourCC()
                          << SoundMediaHeaderBox::getFourCC()
                          << VideoMediaHeaderBox::getFourCC());
        this->exactlyOneCheck<DataInformationBox>();
        this->exactlyOneCheck<SampleTableBox>();
    });

    registerChecker<DataInformationBox>( [this] () {
        this->exactlyOneOfCheck(QList<FourCC>()
                          << DataEntryUrlBox::getFourCC()
                          << DataEntryUrnBox::getFourCC()
                          << DataReferenceBox::getFourCC());
    });

    registerChecker<DataReferenceBox>( [this] () {
        this->anyCountCheck<DataEntryUrlBox>();
        this->anyCountCheck<DataEntryUrnBox>();
    });

    registerChecker<SampleTableBox>( [this] () {
        this->exactlyOneOfCheck(QList<FourCC>()
                          << CompactSampleSizeBox::getFourCC()
                          << SampleSizeBox::getFourCC());
        this->exactlyOneOfCheck(QList<FourCC>()
                          << ChunkLargeOffsetBox::getFourCC()
                          << ChunkOffsetBox::getFourCC());
        this->exactlyOneCheck<SampleDescriptionBox>();
        this->exactlyOneCheck<SampleToChunkBox>();
        this->exactlyOneCheck<TimeToSampleBox>();
        this->zeroOrOneCheck<SyncSampleBox>();
        this->zeroOrOneCheck<CompositionOffsetBox>();
        this->zeroOrOneCheck<SampleDependencyTypeBox>();
    });

    registerChecker<SampleDescriptionBox>( [this] () {
        // Allows to store any boxes here, treating them as codec specific or standard extensions.
        m_counter_info.clear();
    });

    registerChecker<AdditionalMetadataContainerBox>( [this] () {
        this->oneOrMoreCheck<MetaBox>();
    });

    registerChecker<EditBox>( [this] () {
        this->zeroOrOneCheck<EditListBox>();
    });

    registerChecker<MetaBox>( [this] () {
        Box * parent = m_current_box->getParent();
        if( is_a<FileBox>(parent) )
        {
            this->exactlyOneCheck<AFIdentificationBox>();
            this->exactlyOneCheck<SurveillanceExportBox>();
            this->zeroOrOneCheck<DataInformationBox>();
            this->zeroOrOneCheck<FileDeliveryItemInformationBox>();
            this->zeroOrOneCheck<HandlerBox>();
            this->zeroOrOneCheck<ItemProtectionBox>();
        }
        else if( is_a<MovieBox>(parent) )
        {
            this->zeroOrOneCheck<DataInformationBox>();
            this->zeroOrOneCheck<HandlerBox>();
        }
        else if( is_a<TrackBox>(parent) )
        {
            //Camera Microphone Identification Box is mandatory only for media tracks
            auto media_boxes = extractBoxes( dynamic_cast<ChildrenMixin *>(parent), MediaBox::getFourCC() );
            if(!media_boxes.isEmpty())
            {
                ChildrenMixin * media_box = dynamic_cast<ChildrenMixin *>( media_boxes.front() );
                auto media_information_boxes = extractBoxes( media_box, MediaInformationBox::getFourCC() );
                if(!media_information_boxes.isEmpty())
                {
                    CounterInfo counter_info = countBoxes( dynamic_cast<ChildrenMixin *>(media_information_boxes.front()));

                    FourCC vmhd_four_cc = VideoMediaHeaderBox::getFourCC();
                    FourCC smhd_four_cc = SoundMediaHeaderBox::getFourCC();

                    if( (counter_info[vmhd_four_cc] > 0 ) || ( counter_info[smhd_four_cc] > 0) )
                    {
                        this->exactlyOneCheck<CameraMicrophoneIdentificationBox>();
                    }
                }
            }


            this->zeroOrOneCheck<DataInformationBox>();
            this->zeroOrOneCheck<HandlerBox>();
        }
    });

    registerChecker<FileDeliveryItemInformationBox>( [this] () {
        this->anyCountCheck<PartitionEntryBox>();
    });

    registerChecker<ItemProtectionBox>( [this] () {
        this->oneOrMoreCheck<ProtectionSchemeInfoBox>();
    });

    registerChecker<ProtectionSchemeInfoBox>( [this] () {
        this->zeroOrOneCheck<SchemeInformationBox>();
        this->zeroOrOneCheck<SchemeTypeBox>();
    });

    registerChecker<SchemeInformationBox>( [this] () {
        this->exactlyOneCheck<SignatureBox>();
        this->exactlyOneCheck<CertificateBox>();
        this->zeroOrOneCheck<SignatureConfigurationBox>();
        this->zeroOrOneCheck<AdditionalUserInformationBox>();
        this->zeroOrOneCheck<CorrectStartTimeBox>();
    });

    registerChecker<TrackReferenceBox>( [this] () {
        // Allows to store any boxes here, treating them as codec specific or standard extensions.
        m_counter_info.clear();
    });

    registerChecker<UserDataBox>( [this] () {
        // Allows to store any boxes here, treating them as user data.
        // Moreover there is a special udtaBoxesCheck that allows to store here any boxes (even registered in other checkers) with any hierarchy.
        m_counter_info.clear();
    });

    registerChecker<MovieExtendsBox>( [this] () {
        this->exactlyOneCheck<MovieExtendsHeaderBox>();
        this->anyCountCheck<TrackExtendsBox>();
        });

    registerChecker<MovieFragmentBox>( [this] () {
        this->exactlyOneCheck<MovieFragmentHeaderBox>();
        this->anyCountCheck<TrackFragmentBox>();
    });

    registerChecker<TrackFragmentBox>( [this] () {
        this->exactlyOneCheck<TrackFragmentHeaderBox>();
        this->anyCountCheck<TrackRunBox>();
		this->zeroOrOneCheck<TrackFragmentDecodeTimeBox>();
    });

    registerChecker<MovieFragmentRandomAccessBox>( [this] () {
        this->exactlyOneCheck<MovieFragmentRandomAccessOffsetBox>();
        this->anyCountCheck<TrackFragmentRandomAccessBox>();
    });
}

void ConsistencyChecker::checkFileBox(FileBox *box)
{
    m_counter_info = countBoxes(box);
    m_current_box = box;

    exactlyOneCheck<FileTypeBox>();
    exactlyOneCheck<MovieBox>();
    oneOrMoreCheck<MovieDataBox>();
    zeroOrOneCheck<AdditionalMetadataContainerBox>();
    zeroOrOneCheck<MetaBox>();
    zeroOrOneCheck<MovieFragmentRandomAccessBox>();
    zeroOrOneCheck<ProgressiveDownloadInfoBox>();
    anyCountCheck<MovieFragmentBox>();
    freeBoxesCheck();
    markUnexpectedBoxes();
}

void ConsistencyChecker::onBoxCreated(Box *box)
{
    FourCC four_cc = box->getBoxFourCC();
    if(m_checkers_map.contains(four_cc))
    {
        m_counter_info = countBoxes(dynamic_cast<ChildrenMixin*>(box));
        m_current_box = box;

        if( !udtaBoxesCheck() )
        {
            m_checkers_map[four_cc]();

            freeBoxesCheck();
            markUnexpectedBoxes();
        }
    }
}

void ConsistencyChecker::freeBoxesCheck()
{
    anyCountCheck<FreeSpaceBox>();
    anyCountCheck<SkipBox>();
}

bool ConsistencyChecker::udtaBoxesCheck()
{
    Box * parent = m_current_box->getParent();
    while(parent != nullptr)
    {
        if( is_a<UserDataBox>(parent) )
        {
            m_counter_info.clear();
            return true;
        }
        parent = parent->getParent();
    }
    return false;
}

void ConsistencyChecker::markUnexpectedBoxes()
{
    if(m_counter_info.size() > 0)
    {
        unexpectedBoxesMet(m_current_box, extractBoxes(dynamic_cast<ChildrenMixin *>(m_current_box), m_counter_info.keys()));
    }
}

ConsistencyChecker::CounterInfo ConsistencyChecker::countBoxes(ChildrenMixin * box)
{
    CounterInfo result;
    if (box) {
        auto box_children = box->getChildren();
        for (auto it = box_children.begin(), end = box_children.end(); it != end; ++it)
        {
            Box* child = *it;
            FourCC four_cc = child->getBoxFourCC();
            if (result.contains(four_cc))
            {
                result[four_cc]++;
            }
            else
            {
                result[four_cc] = 1;
            }
        }
    }
    return result;
}

QList<Box *> ConsistencyChecker::extractBoxes(ChildrenMixin * box, FourCC four_cc)
{
    return extractBoxes(box, QList<FourCC>() << four_cc);
}

QList<Box *> ConsistencyChecker::extractBoxes(ChildrenMixin * box, QList<FourCC> four_cc_codes)
{
    QList<Box *> result;
    auto box_children = box->getChildren();
    for(auto it = box_children.begin(), end = box_children.end(); it != end; ++it)
    {
		Box* child = *it;
        FourCC child_four_cc = child->getBoxFourCC();
        if(four_cc_codes.contains(child_four_cc))
        {
            result.append(child);
        }
    }
    return result;
}
