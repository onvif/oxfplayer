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

#ifndef SURVEILLANCE_METADATA_SAMPLE_CONFIG_BOX_H
#define SURVEILLANCE_METADATA_SAMPLE_CONFIG_BOX_H

#include "basic/fullBox.hpp"

//! Class describing Surveillance Metadata Sample Config box.
/*!
 * \brief Declared in Surveillance Application Format. \n
 * Type: 'vcmC' \n
 * Container: SurveillanceMetadataSampleEntryBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one (per instance)
 */
class SurveillanceMetadataSampleConfigBox
        : public FullBox<uint8_t>
{
public:
    typedef FullBox<uint8_t> FullBoxType;

public:
    explicit SurveillanceMetadataSampleConfigBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns the Surveillance Metadata Sample Config version.
    uint8_t getSMSCVersion()
    {
        return std::get<2>(FullBoxType::m_data);
    }

public:
    BOX_INFO("vcmC", "Surveillance Metadata Sample Config Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(SMSCVersion);
    }
};

#endif // SURVEILLANCE_METADATA_SAMPLE_CONFIG_BOX_H
