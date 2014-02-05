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

#ifndef BASIC_MIXIN_DATA_H
#define BASIC_MIXIN_DATA_H

#include <QBitArray>

#include "helpers/istream.hpp"

#ifdef CC_CXX11_SUPPORT_VARIADIC_TEMPLATES

//! Class that describes a box mix-in for containing data.
/*!
 * \brief This class is responsible for holding data for a box.
 * \param TData... the list of data types that will be contained in a box.
 */
template<typename... TData>
class DataMixin
{
public:
    //! The data is contained in a tuple.
    typedef std::tuple<TData...> DataType;

protected:
    DataMixin()
    {
    }

public:
    //! Reads the data from the input stream.
    inline void initialize(LimitedStreamReader &stream)
    {
        stream.read(m_data);
    }

    //! Returns the data.
    inline DataType & getData()
    {
        return m_data;
    }

protected:
    DataType m_data;
};

#else

//! Class that describes a box mix-in for containing data.
/*!
 * \brief This class is responsible for holding data for a box.
 * \param TData... the list of data types that will be contained in a box.
 */
template<typename T0 = CC_MSSPEC_TUPLE_NIL,
	typename T1 = CC_MSSPEC_TUPLE_NIL,
	typename T2 = CC_MSSPEC_TUPLE_NIL,
	typename T3 = CC_MSSPEC_TUPLE_NIL,
	typename T4 = CC_MSSPEC_TUPLE_NIL,
	typename T5 = CC_MSSPEC_TUPLE_NIL,
	typename T6 = CC_MSSPEC_TUPLE_NIL,
	typename T7 = CC_MSSPEC_TUPLE_NIL,
	typename T8 = CC_MSSPEC_TUPLE_NIL,
	typename T9 = CC_MSSPEC_TUPLE_NIL>
class DataMixin
{
public:
    //! The data is contained in a tuple.
typedef std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> DataType;

protected:
    DataMixin()
    {
    }

public:
    //! Reads the data from the input stream.
    inline void initialize(LimitedStreamReader &stream)
    {
        stream.read(m_data);
    }

    //! Returns the data.
    inline DataType & getData()
    {
        return m_data;
    }

protected:
    DataType m_data;
};

#endif

#ifdef CC_CXX11_SUPPORT_VARIADIC_TEMPLATES

//! Class that describes a box mix-in for full box.
/*!
 * \brief This class is responsible for holding data for a full box.
 * \param TData... the list of data types that will be contained in a full box (can be empty).
 */
template<typename... TData>
class FullBoxMixin
        : public DataMixin<unsigned char, U_UInt24, TData...>
{
public:
    typedef DataMixin<unsigned char, U_UInt24, TData...> DataMixinType;

protected:
    FullBoxMixin()
    {}

public:
    //! Returns the version of a full box.
    inline uint32_t getVersion()
    {
        return std::get<0>(DataMixinType::m_data);
    }

    //! Returns the flag set of a full box.
    inline QBitArray getFlags()
    {
        QBitArray result(24);
        uint32_t value = std::get<1>(DataMixinType::m_data).m_value;
        for(int bit = 0; bit < result.size(); ++bit)
        {
            result.setBit(bit, value & (1 << bit));
        }
        return result;
    }
};

#else

//! Class that describes a box mix-in for full box.
/*!
 * \brief This class is responsible for holding data for a full box.
 * \param TData... the list of data types that will be contained in a full box (can be empty).
 */
template<typename T0 = CC_MSSPEC_TUPLE_NIL,
	typename T1 = CC_MSSPEC_TUPLE_NIL,
	typename T2 = CC_MSSPEC_TUPLE_NIL,
	typename T3 = CC_MSSPEC_TUPLE_NIL,
	typename T4 = CC_MSSPEC_TUPLE_NIL,
	typename T5 = CC_MSSPEC_TUPLE_NIL,
	typename T6 = CC_MSSPEC_TUPLE_NIL,
	typename T7 = CC_MSSPEC_TUPLE_NIL>
class FullBoxMixin
        : public DataMixin<unsigned char, U_UInt24, T0, T1, T2, T3, T4, T5, T6, T7>
{
public:
    typedef DataMixin<unsigned char, U_UInt24, T0, T1, T2, T3, T4, T5, T6, T7> DataMixinType;

protected:
    FullBoxMixin()
    {}

public:
    //! Returns the version of a full box.
    inline uint32_t getVersion()
    {
        return std::get<0>(DataMixinType::m_data);
    }

    //! Returns the flag set of a full box.
    inline QBitArray getFlags()
    {
        QBitArray result(24);
        uint32_t value = std::get<1>(DataMixinType::m_data).m_value;
        for(int bit = 0; bit < result.size(); ++bit)
        {
            result.setBit(bit, value & (1 << bit));
        }
        return result;
    }
};

#endif

#endif // BASIC_MIXIN_DATA_H
