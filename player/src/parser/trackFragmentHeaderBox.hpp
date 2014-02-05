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

#ifndef TRACK_REFERENCE_HEADER_BOX_H
#define TRACK_REFERENCE_HEADER_BOX_H


#include "basic/box.h"
#include "basic/mixin/data.hpp"

enum TrackFragmentFlags : uint32_t
{
    BaseDataOffsetPresent           = 0x01,
    SampleDescriptionIndexPresent  = 0x02,
    DefaultSampleDurationPresent    = 0x08,
    DefaultSampleSizePresent        = 0x10,
    DefaultSampleFlagsPresent       = 0x20,
};

//! Class describing Track Fragment Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'tfhd' \n
 * Container: TrackFragmentBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class TrackFragmentHeaderBox
        : public Box
        , public FullBoxMixin<uint32_t>
{
public:
    typedef FullBoxMixin<uint32_t> FullBoxType;

public:
    explicit TrackFragmentHeaderBox(ChildrenMixin * parent = nullptr)
        : Box(parent)
        , FullBoxType()
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxType::initialize(stream);
        readContents(stream);
    }

public:
    //! Returns track fragment size.
    TrackFragmentFlags getTrackFragmentFlags()
    {
        return (TrackFragmentFlags)std::get<1>(FullBoxType::m_data).m_value;
    }

    //! Returns track id.
    uint32_t getTrackID()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns base data offset.
    optional<uint64_t> & getBaseDataOffset()
    {
        return m_base_data_offset;
    }

    //! Returns sample description index.
    optional<uint32_t> & getSampleDescriptionIndex()
    {
        return m_sample_description_index;
    }

    //! Returns default sample duration.
    optional<uint32_t> & getDefaultSampleDuration()
    {
        return m_default_sample_duration;
    }

    //! Returns default sample size.
    optional<uint32_t> & getDefaultSampleSize()
    {
        return m_default_sample_size;
    }

    //! Returns default sample flags.
    optional<uint32_t> & getDefaultSampleFlags()
    {
        return m_default_sample_flags;
    }

public:
    BOX_INFO("tfhd", "Track Fragment Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(TrackID);
        BOX_PROPERTY(BaseDataOffset);
        BOX_PROPERTY(SampleDescriptionIndex);
        BOX_PROPERTY(DefaultSampleDuration);
        BOX_PROPERTY(DefaultSampleSize);
        BOX_PROPERTY(DefaultSampleFlags);
    }

private:
    //! Reads the box information from the input stream, depending on the flags.
    void readContents(LimitedStreamReader &stream)
    {
        TrackFragmentFlags flags = getTrackFragmentFlags();

        if(flags & BaseDataOffsetPresent)
        {
            stream.read(m_base_data_offset);
        }
        if(flags & SampleDescriptionIndexPresent)
        {
            stream.read(m_sample_description_index);
        }
        if(flags & DefaultSampleDurationPresent)
        {
            stream.read(m_default_sample_duration);
        }
        if(flags & DefaultSampleSizePresent)
        {
            stream.read(m_default_sample_size);
        }
        if(flags & DefaultSampleFlagsPresent)
        {
            stream.read(m_default_sample_flags);
        }
    }

private:
    //! Base data offset.
    optional<uint64_t> m_base_data_offset;
    //! Sample description index.
    optional<uint32_t> m_sample_description_index;
    //! Default sample duration.
    optional<uint32_t> m_default_sample_duration;
    //! Default sample size.
    optional<uint32_t> m_default_sample_size;
    //! Default sample flags.
    optional<uint32_t> m_default_sample_flags;
};

#endif // TRACK_REFERENCE_HEADER_BOX_H
