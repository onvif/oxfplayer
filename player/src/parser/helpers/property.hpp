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

#ifndef HELPERS_PROPERTY_H
#define HELPERS_PROPERTY_H

#include "fourcc.h"

#include <QBitArray>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QUuid>

#include <array>
#include <iostream>
#include <memory>
#include <string>

#include "defines.h"
#include "endian.hpp"
#include "uint24.hpp"
#include "optional.hpp"

//! Helper class, allowing to build trees of string values. Used for exporting box properties in human readable format. Uses curiosly recurring template pattern.
class Property CC_CXX11_FINAL
        : public QList<Property>
{
public:
	Property()
	{
	}

    template<typename T>
    Property(T value)
    {
        convert(value);
    }

public:
    //! Checks if a property contains other ones.
    bool isList() const
    {
        return size() > 0;
    }

    //! Returns a property value.
    QString getString() const
    {
        return m_string;
    }

private:
    //! Converts integer and floating values to strings.
    template<typename T>
    inline typename std::enable_if< std::is_integral<T>::value || std::is_floating_point<T>::value, void >::type convert(T value)
    {
        m_string = QString::number(value);
    }

    //! Converts boolean values to strings.
    inline void convert(bool value)
    {
        m_string = value ? "True" : "False";
    }

    //! Converts UUID values to strings.
    inline void convert(QUuid value)
    {
        m_string = value.toString();
    }

    //! Converts uint24_t values to strings.
    inline void convert(U_UInt24 value)
    {
        m_string = QString::number((uint32_t)value);
    }

    //! Stores string values.
    inline void convert(QString value)
    {
        m_string = value;
    }

    //! Converts FourCC codes to strings.
    inline void convert(FourCC value)
    {
        m_string = (QString)value;
    }

    //! Converts DateTime values to strings.
    inline void convert(QDateTime value)
    {
        m_string = value.toString(DATETIME_CONVERSION_FORMAT);
    }

    //! Converts bit-arrays to lists of properties.
    inline void convert(QBitArray value)
    {
        m_string = QString("Bitset of %1 records").arg(value.size());
        for(int i = 0; i < value.size(); ++i)
        {
            push_back( Property(value.at(i)) );
        }
    }

    //! Converts lists of values to lists of properties.
    template<typename T>
    inline void convert(QList<T> value)
    {
        m_string = QString("List of %1 records").arg(value.size());
        for(auto it = value.begin(), end = value.end(); it != end; ++it)
        {
			T entry = *it;
            push_back( Property(entry) );
        }
    }

    //! Converts vectors to lists of properties.
    template<typename T>
    inline void convert(QVector<T> value)
    {
        m_string = QString("Vector of %1 records").arg(value.size());
        for(auto it = value.begin(), end = value.end(); it != end; ++it)
        {
			T entry = *it;
            push_back( Property(entry) );
        }
    }

    //! Converts byte-arrays to lists of properties.
    inline void convert(QByteArray value)
    {
        m_string = QString("Byte array of %1 records").arg(value.size());
        for(auto it = value.begin(), end = value.end(); it != end; ++it)
        {
			uint8_t entry = *it;
            push_back( Property(entry) );
        }
    }

    //! Converts optional values to strings.
    template<typename T>
    inline void convert(optional<T> value)
    {
        if(value.hasValue())
            convert(value.value());
        else
            m_string = "<Not present>";
    }

    //! Converts fixed sized array from a standard library to lists of properties.
    template<typename T, size_t size>
    inline void convert(std::array<T, size> value)
    {
        m_string = QString("Array of %1 records").arg(size);
		for(auto it = value.begin(), end = value.end(); it != end; ++it)
        {
			T entry = *it;
            push_back( Property(entry) );
        }
    }

#ifdef CC_CXX11_SUPPORT_VARIADIC_TEMPLATES
    //! Converts tuples to lists of properties.
    template<typename... TArgs>
    inline void convert(const std::tuple<TArgs...> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<TArgs...> >::value );
        convert_tuple(value);
    }

private:
    //! Helper function for coverting tuple of values to a list of properties. Finalizes conversion.
    template<std::size_t iterator = 0, typename... TupleArgs>
    inline typename std::enable_if< iterator == sizeof...(TupleArgs), void >::type convert_tuple(const std::tuple<TupleArgs...> &)
    {
        //does nothing
    }

    //! Helper function for coverting tuple of values to a list of properties.
    template<std::size_t iterator = 0, typename... TupleArgs>
    inline typename std::enable_if< iterator != sizeof...(TupleArgs), void >::type convert_tuple(const std::tuple<TupleArgs...> & tuple)
    {
        push_back( Property( std::get<iterator>(tuple) ) );
        convert_tuple< iterator + 1, TupleArgs...>(tuple);
    }
#else
	template<typename T0,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
	inline void convert(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
		convert_tuple(std::get<2>(value));
		convert_tuple(std::get<3>(value));
		convert_tuple(std::get<4>(value));
		convert_tuple(std::get<5>(value));
		convert_tuple(std::get<6>(value));
		convert_tuple(std::get<7>(value));
		convert_tuple(std::get<8>(value));
		convert_tuple(std::get<9>(value));
    }

	template<typename T0,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
	inline void convert(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
		convert_tuple(std::get<2>(value));
		convert_tuple(std::get<3>(value));
		convert_tuple(std::get<4>(value));
		convert_tuple(std::get<5>(value));
		convert_tuple(std::get<6>(value));
		convert_tuple(std::get<7>(value));
		convert_tuple(std::get<8>(value));
    }

	template<typename T0,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
	inline void convert(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
		convert_tuple(std::get<2>(value));
		convert_tuple(std::get<3>(value));
		convert_tuple(std::get<4>(value));
		convert_tuple(std::get<5>(value));
		convert_tuple(std::get<6>(value));
		convert_tuple(std::get<7>(value));
    }

	template<typename T0,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
	inline void convert(std::tuple<T0, T1, T2, T3, T4, T5, T6> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1, T2, T3, T4, T5, T6> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
		convert_tuple(std::get<2>(value));
		convert_tuple(std::get<3>(value));
		convert_tuple(std::get<4>(value));
		convert_tuple(std::get<5>(value));
		convert_tuple(std::get<6>(value));
    }

	template<typename T0,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
	inline void convert(std::tuple<T0, T1, T2, T3, T4, T5> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1, T2, T3, T4, T5> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
		convert_tuple(std::get<2>(value));
		convert_tuple(std::get<3>(value));
		convert_tuple(std::get<4>(value));
		convert_tuple(std::get<5>(value));
    }

	template<typename T0,
		typename T1,
		typename T2,
		typename T3,
		typename T4>
	inline void convert(std::tuple<T0, T1, T2, T3, T4> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1, T2, T3, T4> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
		convert_tuple(std::get<2>(value));
		convert_tuple(std::get<3>(value));
		convert_tuple(std::get<4>(value));
    }

	template<typename T0,
		typename T1,
		typename T2,
		typename T3>
	inline void convert(std::tuple<T0, T1, T2, T3> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1, T2, T3> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
		convert_tuple(std::get<2>(value));
		convert_tuple(std::get<3>(value));
    }

	template<typename T0,
		typename T1,
		typename T2>
	inline void convert(std::tuple<T0, T1, T2> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1, T2> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
		convert_tuple(std::get<2>(value));
    }

	template<typename T0,
		typename T1>
	inline void convert(std::tuple<T0, T1> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0, T1> >::value );
        convert_tuple(std::get<0>(value));
		convert_tuple(std::get<1>(value));
    }

	template<typename T0>
	inline void convert(std::tuple<T0> & value)
    {
        m_string = QString("Tuple of %1 records").arg( std::tuple_size< std::tuple<T0> >::value );
        convert_tuple(std::get<0>(value));
    }

	inline void convert(std::tuple<> &)
    {
    }

private:
	template<typename T>
	inline void convert_tuple(T& value)
	{
		push_back(Property(value));
	}

	inline void convert_tuple(CC_MSSPEC_TUPLE_NIL &)
	{
	}

#endif

private:
    //! String container.
    QString m_string;
};

#endif // PROPERTY_H
