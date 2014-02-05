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

#ifndef MOVIE_EXTENDS_HEADER_BOX_H
#define MOVIE_EXTENDS_HEADER_BOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"

//! Class describing Movie Extends Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'mehd' \n
 * Container: MovieExtendsBox \n
 * Mandatory: No \n
 * Quantity: Zero or one
 */
class MovieExtendsHeaderBox
        : public Box
        , public FullBoxMixin<uint64_t>
{
public:
    typedef FullBoxMixin<uint64_t> FullBoxType;

public:
    explicit MovieExtendsHeaderBox(ChildrenMixin * parent = nullptr)
        : Box(parent)
        , FullBoxType()
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        readFullBox(stream);
    }

public:
    //! Returns the fragment duration.
    uint64_t getFragmentDuration()
    {
        return std::get<2>(FullBoxType::m_data);
    }

public:
    BOX_INFO("mehd", "Movie Extends Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(FragmentDuration);
    }

private:
    //! Reads the box contents from the stream depending on the box version.
    void readFullBox(LimitedStreamReader &stream)
    {
        unsigned char version;
        U_UInt24 flags;
        stream.read(version).read(flags);
        if(1 == version)
        {
            uint64_t fragment_duration;
            stream.read(fragment_duration);
            FullBoxType::DataType data(version, flags, fragment_duration);
            FullBoxType::m_data.swap(data);
        }
        else
        {
            uint32_t fragment_duration;
            stream.read(fragment_duration);
            FullBoxType::DataType data(version, flags, fragment_duration);
            FullBoxType::m_data.swap(data);
        }
    }
};

#endif // MOVIE_EXTENDS_HEADER_BOX_H
