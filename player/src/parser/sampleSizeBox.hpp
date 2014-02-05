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

#ifndef SAMPLE_SIZE_BOX_H
#define SAMPLE_SIZE_BOX_H


#include "basic/box.h"
#include "basic/mixin/data.hpp"
#include "basic/mixin/table.hpp"

typedef uint32_t SampleSizeEntry;

//! Class describing Sample Size box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'stsz' \n
 * Container: SampleTableBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one variant (CompactSampleSizeBox or SampleSizeBox) must be present
 */
class SampleSizeBox
        : public Box
        , public FullBoxMixin<uint32_t>
        , public TableMixin<SampleSizeEntry>
{
public:
    typedef FullBoxMixin<uint32_t> FullBoxType;

public:
    explicit SampleSizeBox(ChildrenMixin *parent)
        : Box(parent)
        , FullBoxType()
        , TableMixin<SampleSizeEntry>()
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxType::initialize(stream);
        if(getSampleSize() == 0)
        {
            TableMixin<SampleSizeEntry>::initialize(stream);
        }
        else
        {
            uint32_t tableSize;
            stream.read(tableSize);
        }
    }

public:
    //! Returns the sample size.
    uint32_t getSampleSize()
    {
        return std::get<2>(FullBoxType::m_data);
    }

public:
    BOX_INFO("stsz", "Sample Size Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(SampleSize);
        BOX_PROPERTY(Table);
    }
};

#endif // SAMPLE_SIZE_BOX_H
