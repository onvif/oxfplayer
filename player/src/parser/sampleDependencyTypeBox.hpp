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

#ifndef SAMPLE_DEPENDENCY_TYPE_BOX_H
#define SAMPLE_DEPENDENCY_TYPE_BOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"
#include "basic/mixin/table.hpp"

typedef unsigned char SampleDependencyEntry;

//! Class describing Sample Dependency Type box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'sdtp' \n
 * Container: SampleTableBox \n
 * Mandatory: No \n
 * Quantity: Zero or one
 */
class SampleDependencyTypeBox
        : public Box
        , public FullBoxMixin<>
        , public TableMixin<SampleDependencyEntry>
{
public:
    explicit SampleDependencyTypeBox(ChildrenMixin *parent)
        : Box(parent)
        , FullBoxMixin<>()
        , TableMixin<SampleDependencyEntry>()
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxMixin<>::initialize(stream);
		readTable(stream, (stream.getFinalOffset() - stream.getCurrentOffset())/sizeof(SampleDependencyEntry));
    }

public:
    BOX_INFO("sdtp", "Sample Dependency Type Box")

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
    inline void readTable(LimitedStreamReader &stream, uint64_t table_size)
    {
        m_table.reserve(table_size);

        unsigned char* values = new unsigned char[table_size];
        stream.read(values, table_size * sizeof(unsigned char));
        for(unsigned char* it = values; it != values + table_size; ++it)
        {
            m_table.append( *it );
        }
    }
};

#endif // SAMPLE_DEPENDENCY_TYPE_BOX_H
