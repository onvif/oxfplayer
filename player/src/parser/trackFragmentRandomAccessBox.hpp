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

#ifndef TRACKFRAGMENTRANDOMACCESSBOX_H
#define TRACKFRAGMENTRANDOMACCESSBOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"
#include "basic/mixin/table.hpp"

typedef std::tuple<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t> TrackFragmentEntry;

//! Class describing Track Fragment Random Access box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'tfra' \n
 * Container: MovieFragmentRandomAccessBox \n
 * Mandatory: No \n
 * Quantity: Zero or one per track
 */
class TrackFragmentRandomAccessBox
        : public Box
        , public FullBoxMixin<uint32_t, U_UInt24, unsigned char>
        , public TableMixin<TrackFragmentEntry>
{
public:
    typedef FullBoxMixin<uint32_t, U_UInt24, unsigned char> FullBoxType;

public:
    explicit TrackFragmentRandomAccessBox(ChildrenMixin * parent = nullptr)
        : Box(parent)
        , FullBoxType()
        , TableMixin<TrackFragmentEntry>()
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxType::initialize(stream);
        readTable(stream);
    }

public:
    //! Returns track id.
    uint32_t getTrackID()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns traf number size.
    unsigned char getTrafNumberSize()
    {
        return (std::get<4>(FullBoxType::m_data) & 0x30) >> 4;
    }

    //! Return trun number size.
    unsigned char getTrunNumberSize()
    {
        return (std::get<4>(FullBoxType::m_data) & 0x0C) >> 2;
    }

    //! Returns sample number size.
    unsigned char getSampleNumberSize()
    {
        return (std::get<4>(FullBoxType::m_data) & 0x03);
    }

public:
    BOX_INFO("tfra", "Track Fragment Random Access Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(TrackID);
        BOX_PROPERTY(TrafNumberSize);
        BOX_PROPERTY(TrunNumberSize);
        BOX_PROPERTY(SampleNumberSize);
        BOX_PROPERTY(Table);
    }

private:
    //! Provides the readTableImpl with Version information, determining the type to use, while reading time and moof_offset.
    //! Call stack will look like following:
    //!   readTable      ->      readTableImpl1      ->       readTableImpl2      ->       readTableImpl3       ->       readTableImpl
    //!  <         TVersionedType      ,       TTrafNumberType      ,       TTrunNumberType      ,       SampleNumberType             >
    void readTable(LimitedStreamReader &stream)
    {
        switch( getVersion() )
        {
        case 0:
            readTableImpl1<uint32_t>(stream);
            break;
        default:
            readTableImpl1<uint64_t>(stream);
        }
    }

    //! Provides the readTableImpl with TrafNumberSize information, determining the type to use, while reading traf_number.
    //! Call stack will look like following:
    //!   readTable      ->      readTableImpl1      ->       readTableImpl2      ->       readTableImpl3       ->       readTableImpl
    //!  <         TVersionedType      ,       TTrafNumberType      ,       TTrunNumberType      ,       SampleNumberType             >
    template<typename TVersionedType>
    void readTableImpl1(LimitedStreamReader &stream)
    {
        switch( getTrafNumberSize() )
        {
        case 0:
            readTableImpl2<TVersionedType, uint8_t>(stream);
            break;
        case 1:
            readTableImpl2<TVersionedType, uint16_t>(stream);
            break;
        case 2:
            readTableImpl2<TVersionedType, U_UInt24>(stream);
            break;
        default:
            readTableImpl2<TVersionedType, uint32_t>(stream);
        }
    }

    //! Provides the readTableImpl with TrunNumberSize information, determining the type to use, while reading trun_number.
    //! Call stack will look like following:
    //!   readTable      ->      readTableImpl1      ->       readTableImpl2      ->       readTableImpl3       ->       readTableImpl
    //!  <         TVersionedType      ,       TTrafNumberType      ,       TTrunNumberType      ,       SampleNumberType             >
    template<typename TVersionedType, typename TTrafNumberType>
    void readTableImpl2(LimitedStreamReader &stream)
    {
        switch( getTrunNumberSize() )
        {
        case 0:
            readTableImpl3<TVersionedType, TTrafNumberType, uint8_t>(stream);
            break;
        case 1:
            readTableImpl3<TVersionedType, TTrafNumberType, uint16_t>(stream);
            break;
        case 2:
            readTableImpl3<TVersionedType, TTrafNumberType, U_UInt24>(stream);
            break;
        default:
            readTableImpl3<TVersionedType, TTrafNumberType, uint32_t>(stream);
        }
    }

    //! Provides the readTableImpl with SampleNumberSize information, determining the type to use, while reading sample_number.
    //! Call stack will look like following:
    //!   readTable      ->      readTableImpl1      ->       readTableImpl2      ->       readTableImpl3       ->       readTableImpl
    //!  <         TVersionedType      ,       TTrafNumberType      ,       TTrunNumberType      ,       SampleNumberType             >
    template<typename TVersionedType, typename TTrafNumberType, typename TTrunNumberType>
    void readTableImpl3(LimitedStreamReader &stream)
    {
        switch( getSampleNumberSize() )
        {
        case 0:
            readTableImpl<TVersionedType, TTrafNumberType, TTrunNumberType, uint8_t>(stream);
            break;
        case 1:
            readTableImpl<TVersionedType, TTrafNumberType, TTrunNumberType, uint16_t>(stream);
            break;
        case 2:
            readTableImpl<TVersionedType, TTrafNumberType, TTrunNumberType, U_UInt24>(stream);
            break;
        default:
            readTableImpl<TVersionedType, TTrafNumberType, TTrunNumberType, uint32_t>(stream);
        }
    }

    //! Reads the data from the stream depending on the box version. Reads the table of data from the stream, depending on the record size.
    //! Call stack will look like following:
    //!   readTable      ->      readTableImpl1      ->       readTableImpl2      ->       readTableImpl3       ->       readTableImpl
    //!  <         TVersionedType      ,       TTrafNumberType      ,       TTrunNumberType      ,       SampleNumberType             >
    template<typename TVersionedType, typename TTrafNumberType, typename TTrunNumberType, typename TSampleNumberType>
    void readTableImpl(LimitedStreamReader &stream)
    {
        uint32_t table_size;
        stream.read(table_size);
        m_table.reserve(table_size);
        while(table_size--)
        {
            TVersionedType time, moof_offset;
            TTrafNumberType traf_number;
            TTrunNumberType trun_number;
            TSampleNumberType sample_number;
            stream.read(time).read(moof_offset).read(traf_number).read(trun_number).read(sample_number);
            m_table.append(TrackFragmentEntry(time, moof_offset, (uint32_t)traf_number, (uint32_t)trun_number, (uint32_t)sample_number));
        }
    }
};

#endif // TRACKFRAGMENTRANDOMACCESSBOX_H
