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

#include "boxFactory.h"

#include "basic/box.h"
#include "basic/mandatoryBox.h"
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

BoxFactory::BoxFactory()
{
    // -- template content boxes --
    registerCreator<FreeSpaceBox>();
    registerCreator<MovieDataBox>();
    registerCreator<SkipBox>();
    // -- template content full boxes --
    registerCreator<InitialObjectDescriptorBox>();
    // -- template full boxes --
    registerCreator<DataEntryUrlBox>();
    registerCreator<DataEntryUrnBox>();
    registerCreator<HandlerBox>();
    registerCreator<HintMediaHeaderBox>();
    registerCreator<MovieFragmentHeaderBox>();
    registerCreator<MovieFragmentRandomAccessOffsetBox>();
    registerCreator<NullMediaHeaderBox>();
    registerCreator<SchemeTypeBox>();
    registerCreator<SoundMediaHeaderBox>();
    registerCreator<TrackExtendsBox>();
    registerCreator<VideoMediaHeaderBox>();
    registerCreator<TrackFragmentDecodeTimeBox>();
    // -- template super boxes --
    registerCreator<AdditionalMetadataContainerBox>();
    registerCreator<DataInformationBox>();
    registerCreator<EditBox>();
    registerCreator<MediaBox>();
    registerCreator<MediaInformationBox>();
    registerCreator<MovieBox>();
    registerCreator<MovieExtendsBox>();
    registerCreator<MovieFragmentBox>();
    registerCreator<MovieFragmentRandomAccessBox>();
    registerCreator<PartitionEntryBox>();
    registerCreator<ProtectionSchemeInfoBox>();
    registerCreator<SampleTableBox>();
    registerCreator<SchemeInformationBox>();
    registerCreator<TrackBox>();
    registerCreator<TrackFragmentBox>();
    registerCreator<TrackReferenceBox>();
    registerCreator<UserDataBox>();
    // -- template super full boxes --
    registerCreator<DataReferenceBox>();
    registerCreator<FileDeliveryItemInformationBox>();
    registerCreator<ItemProtectionBox>();
    registerCreator<MetaBox>();
    registerCreator<SampleDescriptionBox>();
    // -- template table boxes --
    registerCreator<ChunkLargeOffsetBox>();
    registerCreator<ChunkOffsetBox>();
    registerCreator<CompositionOffsetBox>();
    registerCreator<ProgressiveDownloadInfoBox>();
    registerCreator<SampleToChunkBox>();
    registerCreator<SyncSampleBox>();
    registerCreator<TimeToSampleBox>();
    // -- individual boxes --
    registerCreator<AdditionalUserInformationBox>();
    registerCreator<AFIdentificationBox>();
    registerCreator<CameraMicrophoneIdentificationBox>();
    registerCreator<CertificateBox>();
    registerCreator<CompactSampleSizeBox>();
    registerCreator<EditListBox>();
    registerCreator<FileTypeBox>();
    registerCreator<MediaHeaderBox>();
    registerCreator<MovieExtendsHeaderBox>();
    registerCreator<MovieHeaderBox>();
    registerCreator<SampleDependencyTypeBox>();
    registerCreator<SampleSizeBox>();
    registerCreator<SignatureBox>();
    registerCreator<SignatureConfigurationBox>();
    registerCreator<SurveillanceExportBox>();
    registerCreator<SurveillanceMetadataSampleConfigBox>();
    registerCreator<SurveillanceMetadataSampleEntryBox>();
    registerCreator<TrackFragmentHeaderBox>();
    registerCreator<TrackFragmentRandomAccessBox>();
    registerCreator<TrackHeaderBox>();
    registerCreator<TrackRunBox>();
    registerCreator<CorrectStartTimeBox>();
}

BoxFactory & BoxFactory::instance()
{
    static BoxFactory s_instance;

    return s_instance;
}

bool BoxFactory::parseBox(LimitedStreamReader & stream, ChildrenMixin * parent /*= nullptr*/)
{
    bool result = false;
    m_debug_tab_count++;
    std::string debug_tab_string((m_debug_tab_count-1)*2, ' ');
    if(stream)
    {
        try
        {
            Box * box = nullptr;
            FourCC four_cc;
            optional<QUuid> uuid;
            uint64_t offset = stream.getCurrentOffset();
            uint32_t size;
            stream.read(size).read(four_cc);
            uint64_t large_size = 0;
            if(1 == size)
            {
                stream.read(large_size);
            }

            //qDebug() << debug_tab_string.c_str() << "Offset:" << offset << "Size:" << QString("%1 (%2)").arg(size).arg(large_size) << "FourCC:" << four_cc;

            if(four_cc.isUuid())
            {
                uuid = QUuid();
                stream.read(uuid->data1).read(uuid->data2).read(uuid->data3).read(uuid->data4);
                box = createBox(four_cc, parent);
            }
            else
            {
                box = createBox(four_cc, parent);
            }
            LimitedStreamReader limited_stream(stream.makeNew(offset, size, large_size));
            box->initialize(limited_stream);

            switch(limited_stream.getStreamState())
            {
            case SS_Undersized:
                {
                    box->setSizeError(Box::SizeInsufficient);
                    //qDebug() << debug_tab_string.c_str() << "Stream reports that actual box size is less, than expected.";
                }
                break;
            case SS_Oversized:
                {
                    box->setSizeError(Box::SizeExceeding);
                    //qDebug() << debug_tab_string.c_str() << "Stream reports that actual box size is more, than expected.";
                }
                break;
            case SS_Ok:
                {
                    box->setSizeError(Box::SizeOk);
                    //qDebug() << debug_tab_string.c_str() << "Stream reports that actual box size is OK.";
                }
                break;
            }

            emit boxCreated(box);

            limited_stream.rewindToFinish();
            result = (bool)stream;

            //qDebug() << debug_tab_string.c_str() << "Moved to:" << limited_stream.getFinalOffset();
        }
        catch(...)
        {}
    }
    m_debug_tab_count--;
    return result;
}

void BoxFactory::onMandatoryBoxIsMissing(Box *source, FourCC four_cc)
{
    new MandatoryBox(four_cc, dynamic_cast<ChildrenMixin *>(source));
    source->setConsistencyError(Box::HasNotEnoughBoxes);
}

void BoxFactory::onMandatoryBoxesAreMissing(Box *source, QList<FourCC> four_cc_codes)
{
    new MandatoryBoxes(four_cc_codes, dynamic_cast<ChildrenMixin *>(source));
    source->setConsistencyError(Box::HasNotEnoughBoxes);
}

void BoxFactory::onBoxCountIsExceeding(Box *source, QList<Box *> boxes)
{
    bool boxes_have_same_four_cc = true;
    FourCC four_cc = boxes.front()->getBoxFourCC();
    for(auto it = boxes.begin(), end = boxes.end(); it != end; ++it)
    {
		Box * box = *it;
        FourCC box_four_cc = box->getBoxFourCC();
        if(box_four_cc == four_cc)
        {
            boxes_have_same_four_cc = false;
            break;
        }
    }
    if(boxes_have_same_four_cc)
    {
        source->setConsistencyError(Box::HasTooManyBoxes);
        for(auto it = boxes.begin(), end = boxes.end(); it != end; ++it)
		{
			Box * box = *it;
            box->setConsistencyError(Box::IsBoxWithExceedingCount);
        }
    }
    else
    {
        source->setConsistencyError(Box::HasConflictingBoxes);
        for(auto it = boxes.begin(), end = boxes.end(); it != end; ++it)
		{
			Box * box = *it;
            box->setConsistencyError(Box::IsConflictingBox);
        }
    }
}

void BoxFactory::onUnexpectedBoxesMet(Box *source, QList<Box *> boxes)
{
    source->setConsistencyError(Box::HasUnexpectedBoxes);
    for(auto it = boxes.begin(), end = boxes.end(); it != end; ++it)
    {
		Box * box = *it;
        box->setConsistencyError(Box::IsUnexpectedBox);
    }
}

Box * BoxFactory::createBox(const FourCC & four_cc, ChildrenMixin * parent /*= nullptr*/)
{
    auto it = m_creators.find(four_cc);
    if(m_creators.end() != it)
         return (*it)(parent);
    return new UnknownBox(four_cc, parent);
}
