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

#ifndef TEMPLATE_SUPER_BOX_H
#define TEMPLATE_SUPER_BOX_H

#include "basic/superBox.hpp"

//! Template macro for describing boxes containing other boxes.
//! \param THE_CLASS class name for the box
//! \param FOUR_CC FourCC for the box
//! \param DESCRIPTION description string for the box
#define TEMPLATE_SUPER_BOX(THE_CLASS, FOUR_CC, DESCRIPTION) \
    class THE_CLASS : public SuperBox \
    { \
        public: \
            explicit THE_CLASS(ChildrenMixin * parent = nullptr) \
                : SuperBox(parent) \
            { } \
        public: \
            BOX_INFO(FOUR_CC, DESCRIPTION) \
    }

//! Class describing Additional Metadata Container box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'meco' \n
 * Container: FileBox, MovieBox or TrackBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - one or more MetaBox
 *   .
 */
TEMPLATE_SUPER_BOX(AdditionalMetadataContainerBox, "meco", "Additional Metadata Container Box");

//! Class describing Data Information box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'dinf' \n
 * Container: MediaInformationBox or MetaBox \n
 * Mandatory: Yes for MediaInformationBox and No within MetaBox \n
 * Quantity: Exactly one \n
 * Contains:
 *   - exactly one variant of data entry boxes
 *      - DataEntryUrlBox
 *      - DataEntryUrnBox
 *      - DataReferenceBox
 *   .
 */
TEMPLATE_SUPER_BOX(DataInformationBox, "dinf", "Data Information Box");

//! Class describing Edit box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'edts' \n
 * Container: TrackBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - zero or one EditListBox
 *   .
 */
TEMPLATE_SUPER_BOX(EditBox, "edts", "Edit Box");

//! Class describing Media box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'mdia' \n
 * Container: TrackBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one \n
 * Contains:
 *   - exactly one MediaHeaderBox
 *   - exactly one HandlerBox
 *   - exactly one MediaInformationBox
 *   .
 */
TEMPLATE_SUPER_BOX(MediaBox, "mdia", "Media Box");

//! Class describing Media Information box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'minf' \n
 * Container: MediaBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one \n
 * Contains:
 *   - exactly one variant of media header boxes
 *      - HintMediaHeaderBox
 *      - NullMediaHeaderBox
 *      - SoundMediaHeaderBox
 *      - VideoMediaHeaderBox
 *   - exactly one DataInformationBox
 *   - exactly one SampleTableBox
 *   .
 */
TEMPLATE_SUPER_BOX(MediaInformationBox, "minf", "Media Information Box");

//! Class describing Movie box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'moov' \n
 * Container: FileBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one \n
 * Contains:
 *   - exactly one MovieHeaderBox
 *   - one or more TrackBox
 *   - zero or one AdditionalMetadataContainerBox
 *   - zero or one MetaBox
 *   - zero or one MovieExtendsBox
 *   - zero or one UserDataBox
 *   .
 */
TEMPLATE_SUPER_BOX(MovieBox, "moov", "Movie Box");

//! Class describing Movie Extends box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'mvex' \n
 * Container: MovieBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - exactly one MovieExtendsHeaderBox
 *   - one or more TrackExtendsBox (exactly one for each TrackBox in MovieBox)
 *   .
 */
TEMPLATE_SUPER_BOX(MovieExtendsBox, "mvex", "Movie Extends Box");

//! Class describing Movie Fragment box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'moof' \n
 * Container: FileBox \n
 * Mandatory: No \n
 * Quantity: Zero or more \n
 * Contains:
 *  - zero or more TrackFragmentBox
 *   .
 */
TEMPLATE_SUPER_BOX(MovieFragmentBox, "moof", "Movie Fragment Box");

//! Class describing Movie Fragment Random Access box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'mfra' \n
 * Container: FileBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - exactly one MovieFragmentRandomAccessOffsetBox
 *   - zero or more TrackFragmentRandomAccessBox (zero or one per track)
 *   .
 */
TEMPLATE_SUPER_BOX(MovieFragmentRandomAccessBox, "mfra", "Movie Fragment Random Access Box");

//! Class describing Partition Entry box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'paen' \n
 * Container: FileDeliveryItemInformationBox \n
 * Mandatory: Undefined \n
 * Quantity: Undefined \n
 * Contains:
 *   - Undefined
 *   .
 */
TEMPLATE_SUPER_BOX(PartitionEntryBox, "paen", "Partition Entry Box");

//! Class describing Protection Scheme Info box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'sinf' \n
 * Container: ProtectedSampleEntry or ItemProtectionBox \n
 * Mandatory: Yes \n
 * Quantity: One or more \n
 * Contains:
 *   - zero or one SchemeInformationBox
 *   - zero or one SchemeTypeBox
 *   .
 */
TEMPLATE_SUPER_BOX(ProtectionSchemeInfoBox, "sinf", "Protection Scheme Info Box");

//! Class describing Sample Table box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'stbl' \n
 * Container: MediaInformationBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one \n
 * Contains:
 *   - exactly one variant of sample size box
 *      - CompactSampleSizeBox
 *      - SampleSizeBox
 *   - exactly one variant of chunk offset box
 *      - ChunkLargeOffsetBox
 *      - ChunkOffsetBox
 *   - exactly one SampleToChunkBox
 *   - exactly one TimeToSampleBox
 *   - zero or one SyncSampleBox
 *   - zero or one CompositionOffsetBox
 *   - zero or one SampleDependencyTypeBox
 *   .
 */
TEMPLATE_SUPER_BOX(SampleTableBox, "stbl", "Sample Table Box");

//! Class describing Scheme Information box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'schi' \n
 * Container: ProtectionSchemeInfoBox, RestrictedSchemeInfoBox or SRTPProcessBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - [OXF extension] exactly one CertificateBox
 *   - [OXF extension] exactly one SignatureBox
 *   - [OXF extension] zero or one SignatureConfigurationBox
 *   - [OXF extension] zero or one AdditionalUserInformationBox
 *   .
 */
TEMPLATE_SUPER_BOX(SchemeInformationBox, "schi", "Scheme Information Box");

//! Class describing Track box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'trak' \n
 * Container: MovieBox \n
 * Mandatory: Yes \n
 * Quantity: One or more \n
 * Contains:
 *   - exactly one MediaBox
 *   - exactly one TrackHeaderBox
 *   - zero or one AdditionalMetadataContainerBox
 *   - zero or one EditBox
 *   - zero or one MetaBox
 *   - zero or one TrackReferenceBox
 *   - zero or one UserDataBox
 *   .
 */
TEMPLATE_SUPER_BOX(TrackBox, "trak", "Track Box");

//! Class describing Track Fragment box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'traf' \n
 * Container: MovieFragmentBox \n
 * Mandatory: No \n
 * Quantity: Zero or more \n
 * Contains:
 *   - exactly one TrackFragmentHeaderBox
 *   - zero or more TrackRunBox
 *   .
 */
TEMPLATE_SUPER_BOX(TrackFragmentBox, "traf", "Track Fragment Box");

//! Class describing Track Reference box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'tref' \n
 * Container: TrackBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - Undefined
 *   .
 */
TEMPLATE_SUPER_BOX(TrackReferenceBox, "tref", "Track Reference Box");

//! Class describing User Data box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'udta' \n
 * Container: MovieBox or TrackBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - Undefined
 *   .
 */
TEMPLATE_SUPER_BOX(UserDataBox, "udta", "User Data Box");

#undef TEMPLATE_SUPER_BOX

#endif // TEMPLATE_SUPER_BOX_H
