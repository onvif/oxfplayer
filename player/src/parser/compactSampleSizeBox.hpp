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

#ifndef COMPACT_SAMPLE_SIZE_BOX_H
#define COMPACT_SAMPLE_SIZE_BOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"
#include "basic/mixin/table.hpp"

typedef uint16_t CompactSampleSizeEntry;

//! Class describing Compact Sample Size box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'stz2' \n
 * Container: SampleTableBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one variant (CompactSampleSizeBox or SampleSizeBox) must be present
 */
class CompactSampleSizeBox
        : public Box
        , public FullBoxMixin<U_UInt24, unsigned char>
        , public TableMixin<CompactSampleSizeEntry>
{
public:
    typedef FullBoxMixin<U_UInt24, unsigned char> FullBoxType;

public:
    explicit CompactSampleSizeBox(ChildrenMixin *parent)
        : Box(parent)
        , FullBoxType()
        , TableMixin<CompactSampleSizeEntry>()
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxType::initialize(stream);
        switch(getFieldSize())
        {
        case 4:
            readTable4(stream);
            break;
        case 8:
            readTable<unsigned char>(stream);
            break;
        case 16:
            readTable<uint16_t>(stream);
            break;
        }
    }

public:
    //! Returns the field size.
    inline uint32_t getFieldSize()
    {
        return std::get<3>(FullBoxMixin<U_UInt24, unsigned char>::m_data);
    }

public:
    BOX_INFO("stz2", "Compact Sample Size Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(FieldSize);
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(Table);
    }

private:
    //! Reads the table of sample sizes from the stream for the case the field size equals to 4 bits.
    inline void readTable4(LimitedStreamReader &stream)
    {
        uint32_t table_size;
        stream.read(table_size);
        m_table.reserve(table_size);
        bool is_padded = (table_size % 2 != 0);
        if(is_padded)
        {
            table_size++;
        }

        unsigned char* values = new unsigned char[table_size / 2];
        stream.read(values, (table_size / 2)* sizeof(unsigned char));
        for(unsigned char* it = values; it != values + (table_size / 2); ++it)
        {
            m_table.append( ((*it) & 0xF0) >> 4 );
            m_table.append( ((*it) & 0x0F) );
        }

        if(is_padded)
        {
            m_table.pop_back();
        }
    }

    //! Reads the table of sample sizes from the stream for the case the field size equals to 8 or 16 bits.
    template<typename ContentType>
    inline void readTable(LimitedStreamReader &stream)
    {
        uint32_t table_size;
        stream.read(table_size);
        m_table.reserve(table_size);

        ContentType* values = new ContentType[table_size];
        stream.read(values, table_size * sizeof(ContentType));
        for(ContentType* it = values; it != values + table_size; ++it)
        {
            m_table.append( qFromBigEndian( *it ) );
        }
    }
};

#endif // COMPACT_SAMPLE_SIZE_BOX_H
