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

#ifndef BASIC_FULL_BOX_H
#define BASIC_FULL_BOX_H

#include "box.h"
#include "mixin/data.hpp"

//! Base box class for full boxes containing data.
/*!
 * \param TData... the list of data types that will be contained in a box.
 */
#ifdef CC_CXX11_SUPPORT_VARIADIC_TEMPLATES
template<typename... TData>
class FullBox
        : public Box
        , public FullBoxMixin<TData...>
		
{
protected:
    FullBox(ChildrenMixin *parent = nullptr)
        : Box(parent)
        , FullBoxMixin<TData...>()
    {}

public:
    //! Reads the box and data from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxMixin<TData...>::initialize(stream);
    }

protected:
    //! Registeres properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
    }

    //! Read headers only to allow version dependent reading
    void initializeHeaders(LimitedStreamReader& stream)
    {
        Box::initialize(stream);
        stream.read(std::get<0>(m_data));
        stream.read(std::get<1>(m_data));
    }
};

#else

template<typename T0 = CC_MSSPEC_TUPLE_NIL,
	typename T1 = CC_MSSPEC_TUPLE_NIL,
	typename T2 = CC_MSSPEC_TUPLE_NIL,
	typename T3 = CC_MSSPEC_TUPLE_NIL,
	typename T4 = CC_MSSPEC_TUPLE_NIL,
	typename T5 = CC_MSSPEC_TUPLE_NIL,
	typename T6 = CC_MSSPEC_TUPLE_NIL,
	typename T7 = CC_MSSPEC_TUPLE_NIL>
class FullBox
		: public Box
		, public FullBoxMixin<T0, T1, T2, T3, T4, T5, T6, T7>
		
{
protected:
    FullBox(ChildrenMixin *parent = nullptr)
        : Box(parent)
        , FullBoxMixin<T0, T1, T2, T3, T4, T5, T6, T7>()
    {}

public:
    //! Reads the box and data from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxMixin<T0, T1, T2, T3, T4, T5, T6, T7>::initialize(stream);
    }

protected:
    //! Registeres properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
    }
};

#endif

#endif // BASIC_FULL_BOX_H
