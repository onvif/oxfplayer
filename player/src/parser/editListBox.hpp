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

#ifndef EDIT_LIST_BOX_H
#define EDIT_LIST_BOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"
#include "basic/mixin/table.hpp"

typedef std::tuple<uint64_t, int64_t, int16_t, int16_t> EditListEntry;

#pragma pack(push, 1)

struct EditListTableReaderV1
{
    uint64_t segment_duration;
    int64_t media_time;
    int16_t media_rate_integer;
    int16_t media_rate_fraction;
};

struct EditListTableReaderV0
{
    uint32_t segment_duration;
    int32_t media_time;
    int16_t media_rate_integer;
    int16_t media_rate_fraction;
};

#pragma pack(pop)

//! Class describing Edit List box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'elst' \n
 * Container: EditBox \n
 * Mandatory: No \n
 * Quantity: Zero or one
 */
class EditListBox
        : public Box
        , public FullBoxMixin<>
        , public TableMixin<EditListEntry>
{
public:
    explicit EditListBox(ChildrenMixin *parent)
        : Box(parent)
        , FullBoxMixin<>()
        , TableMixin<EditListEntry>()
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxMixin<>::initialize(stream);

        if(getVersion() == 1)
        {
            readTable<EditListTableReaderV1>(stream);
        }
        else
        {
            readTable<EditListTableReaderV0>(stream);
        }
    }

public:
    BOX_INFO("elst", "Edit List Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(Table);
    }

private:
    //! Reads the table of data from the stream.
    template <typename ReaderType>
    inline void readTable(LimitedStreamReader &stream)
    {
        uint32_t table_size;
        stream.read(table_size);
        m_table.reserve(table_size);

        ReaderType* values = new ReaderType[table_size];
        stream.read(values, table_size * sizeof(ReaderType));
        for(ReaderType* it = values; it != values + table_size; ++it)
        {
            m_table.append(
                        EditListEntry( qFromBigEndian( it->segment_duration )
                                     , qFromBigEndian( it->media_time )
                                     , qFromBigEndian( it->media_rate_integer )
                                     , qFromBigEndian( it->media_rate_fraction ) ) );
        }
    }
};

#endif // EDIT_LIST_BOX_H
