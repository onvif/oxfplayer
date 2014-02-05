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

#ifndef SIGNATURE_CONFIGURATION_BOX_H
#define SIGNATURE_CONFIGURATION_BOX_H

#include "basic/box.h"

//! Class describing Signature Configuration box.
/*!
 * \brief Declared in Onvif Export Format. \n
 * Type: 'sigC' \n
 * Container: SchemeInformationBox \n
 * Mandatory: No \n
 * Quantity: Zero or one
 */
class SignatureConfigurationBox
        : public Box
{
public:
    explicit SignatureConfigurationBox(ChildrenMixin * parent = nullptr)
        : Box(parent)
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        size_t stream_size = stream.getFinalOffset() - stream.getCurrentOffset();
        if(stream_size > 0)
        {
            m_algorithm_identifier.resize(stream_size);
            stream.read(m_algorithm_identifier.data(), stream_size);
        }
    }

public:
    //! Returns the algorithm identifier data.
    QByteArray & getAlgorithmIdentifier()
    {
       return m_algorithm_identifier;
    }

public:
    BOX_INFO("sigC", "Signature Configuration Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(AlgorithmIdentifier);
    }

private:
    //! Algorithm identifier data.
    QByteArray m_algorithm_identifier;
};

#endif // SIGNATURE_CONFIGURATION_BOX_H
