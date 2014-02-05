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

#ifndef SURVEILLANCE_METADATA_SAMPLE_ENTRY_BOX_H
#define SURVEILLANCE_METADATA_SAMPLE_ENTRY_BOX_H

#include <array>

#include "basic/dataBox.hpp"
#include "basic/mixin/children.hpp"

//! Class describing Surveillance Metadata Sample Entry box.
/*!
 * \brief Declared in Surveillance Application Format. \n
 * Type: 'vcmM' \n
 * Container: SampleDescriptionBox \n
 * Mandatory: Unspecified \n
 * Quantity: Unspecified
 */
class SurveillanceMetadataSampleEntryBox
        : public DataBox<uint32_t, uint16_t, uint16_t>
        , public ChildrenMixin
{
public:
    typedef DataBox<uint32_t, uint16_t, uint16_t> DataBoxType;

public:
    explicit SurveillanceMetadataSampleEntryBox(ChildrenMixin * parent = nullptr)
        : DataBoxType(parent)
        , ChildrenMixin()
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        DataBoxType::initialize(stream);

        LimitedStreamReader calculation_helper_stream(stream.makeNew(stream.getCurrentOffset(), sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint64_t), 0));
        uint32_t size = 1, dummy;
        uint64_t large_size = 0;
        calculation_helper_stream.read(size).read(dummy);
        if(1 == size)
        {
            calculation_helper_stream.read(large_size);
        }
        calculation_helper_stream.restart();
        LimitedStreamReader child_stream(stream.makeNew(stream.getCurrentOffset(), size, large_size));
        ChildrenMixin::initialize(child_stream);

        uint64_t timestampSize = (stream.getFinalOffset() - stream.getCurrentOffset()) / sizeof(uint64_t);
        m_timestamp.reserve(timestampSize);
        while(timestampSize--)
        {
            uint64_t value;
            stream.read(value);
            m_timestamp.append(value);
        }
    }

    virtual Box * getThisBox() CC_CXX11_OVERRIDE
    {
        return this;
    }

    virtual const Box * getThisBox() const CC_CXX11_OVERRIDE
    {
        return this;
    }

public:
    //! Returns data reference index.
    uint16_t getDataReferenceIndex()
    {
        return std::get<2>(DataBoxType::m_data);
    }

    //! Returns the timestamp.
    QVector<uint64_t> & getTimestamp()
    {
        return m_timestamp;
    }

public:
    BOX_INFO("vcmM", "Surveillance Metadata Sample Entry")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(DataReferenceIndex);
        BOX_PROPERTY(Timestamp);
    }

private:
    QVector<uint64_t> m_timestamp;
};

#endif // SURVEILLANCE_METADATA_SAMPLE_ENTRY_BOX_H
