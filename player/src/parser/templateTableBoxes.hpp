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

#ifndef TEMPLATE_TABLE_BOX_H
#define TEMPLATE_TABLE_BOX_H

#include "basic/tableBox.hpp"

//! Template macro for describing boxes containing other boxes.
//! \param THE_CLASS class name for the box
//! \param TABLE_TYPE data type contained in the table
//! \param FOUR_CC FourCC for the box
//! \param DESCRIPTION description string for the box
#define TEMPLATE_TABLE_BOX(THE_CLASS, TABLE_TYPE, FOUR_CC, DESCRIPTION) \
    class THE_CLASS \
            : public TableBox<TABLE_TYPE> \
    { \
    public: \
        explicit THE_CLASS(ChildrenMixin * parent = nullptr) \
            : TableBox<TABLE_TYPE>(parent) \
        { } \
        virtual ~THE_CLASS() \
        { } \
    public: \
        BOX_INFO(FOUR_CC, DESCRIPTION); \
    }

typedef uint64_t ChunkLargeOffsetEntry;
typedef uint32_t ChunkOffsetEntry;
typedef std::tuple<uint32_t, uint32_t> CompositionOffsetEntry;
typedef std::tuple<int32_t, int32_t> ProgressiveDownloadInfoEntry;
typedef std::tuple<uint32_t, uint32_t, uint32_t> SampleToChunkEntry;
typedef uint32_t SyncSampleEntry;
typedef std::tuple<uint32_t, uint32_t> TimeToSampleEntry;

//! Class describing Chunk Large Offset box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'co64' \n
 * Container: SampleTableBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one variant (ChunkLargeOffsetBox or ChunkOffsetBox) must be present
 */
TEMPLATE_TABLE_BOX(ChunkLargeOffsetBox, ChunkLargeOffsetEntry, "co64", "Chunk Large Offset Box");

//! Class describing Chunk Offset box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'stco' \n
 * Container: SampleTableBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one variant (ChunkLargeOffsetBox or ChunkOffsetBox) must be present
 */
TEMPLATE_TABLE_BOX(ChunkOffsetBox, ChunkOffsetEntry, "stco", "Chunk Offset Box");

//! Class describing Composition Offset box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'ctts' \n
 * Container: SampleTableBox \n
 * Mandatory: No \n
 * Quantity: Zero or one
 */
TEMPLATE_TABLE_BOX(CompositionOffsetBox, CompositionOffsetEntry, "ctts", "Composition Offset Box");

//!Class describing Progressive Download Info Box
/*!
 * \brief Declared in ISO Base Media Format.
 * Type: 'pdin'
 * Container: FileBox
 * Mandatory: No
 * Quantity: Zero or one
 */
TEMPLATE_TABLE_BOX(ProgressiveDownloadInfoBox, ProgressiveDownloadInfoEntry, "pdin", "Progressive Download Info Box");

//! Class describing Sample To Chunk box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'stsc' \n
 * Container: SampleTableBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
TEMPLATE_TABLE_BOX(SampleToChunkBox, SampleToChunkEntry, "stsc", "Sample To Chunk Box");

//! Class describing Sync Sample box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'stss' \n
 * Container: SampleTableBox \n
 * Mandatory: No \n
 * Quantity: Zero or one
 */
TEMPLATE_TABLE_BOX(SyncSampleBox, SyncSampleEntry, "stss", "Sync Sample Box");

//! Class describing Time To Sample box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'stts' \n
 * Container: SampleTableBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
TEMPLATE_TABLE_BOX(TimeToSampleBox, TimeToSampleEntry, "stts", "Time To Sample Box");

#undef TEMPLATE_TABLE_BOX

#endif // TEMPLATE_TABLE_BOX_H
