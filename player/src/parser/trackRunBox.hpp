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

#ifndef TRACK_RUN_BOX_H
#define TRACK_RUN_BOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"
#include "basic/mixin/table.hpp"

enum TrackRunFlags : uint32_t
{
    DataOffsetPresent                   = 0x001,
    FirstSampleFlagsPresent             = 0x004,
    SampleDurationPresent               = 0x100,
    SampleSizePresent                   = 0x200,
    SampleFlagsPresent                  = 0x400,
    SampleCompositionTimeOffsetPresent  = 0x800,
};

typedef std::tuple<optional<uint32_t>, optional<uint32_t>, optional<uint32_t>, optional<uint32_t>> TrackRunEntry;

//! Class describing Track Run box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'trun' \n
 * Container: TrackFragmentBox \n
 * Mandatory: No \n
 * Quantity: Zero or more
 */
class TrackRunBox
        : public Box
        , public FullBoxMixin<>
        , public TableMixin<TrackRunEntry>
{
public:
    typedef FullBoxMixin<> FullBoxType;

public:
    explicit TrackRunBox(ChildrenMixin * parent = nullptr)
        : Box(parent)
        , FullBoxType()
        , TableMixin<TrackRunEntry>()
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
    //! Returns track run flags.
    TrackRunFlags getTrackRunFlags()
    {
        return (TrackRunFlags)std::get<1>(FullBoxType::m_data).m_value;
    }

    //! Returns data offset.
    optional<int32_t> & getDataOffset()
    {
        return m_data_offset;
    }

    //! Returns first sample flags.
    optional<uint32_t> & getFirstSampleFlags()
    {
        return m_first_sample_flags;
    }

public:
    BOX_INFO("trun", "Track Run Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(TrackRunFlags);
        BOX_PROPERTY(DataOffset);
        BOX_PROPERTY(FirstSampleFlags);
        BOX_PROPERTY(Table);
    }

private:
    //! Reads the data and the table of data, depending on the flags.
    void readContents(LimitedStreamReader &stream)
    {
        TrackRunFlags flags = getTrackRunFlags();

        uint32_t table_size;
        stream.read(table_size);
        m_table.reserve(table_size);

        if(flags & DataOffsetPresent)
        {
            stream.read(m_data_offset);
        }
        if(flags & FirstSampleFlagsPresent)
        {
            stream.read(m_first_sample_flags);
        }
        while(table_size--)
        {
            optional<uint32_t> sample_duration, sample_size, sample_flags, sample_composition_time_offset;
            if(flags & SampleDurationPresent)
            {
                stream.read(sample_duration);
            }
            if(flags & SampleSizePresent)
            {
                stream.read(sample_size);
            }
            if(flags & SampleFlagsPresent)
            {
                stream.read(sample_flags);
            }
            if(flags & SampleCompositionTimeOffsetPresent)
            {
                stream.read(sample_composition_time_offset);
            }
            m_table.append(TrackRunEntry(sample_duration, sample_size, sample_flags, sample_composition_time_offset));
        }
    }

private:
    optional<int32_t> m_data_offset;
    optional<uint32_t> m_first_sample_flags;
};

#endif // TRACK_RUN_BOX_H
