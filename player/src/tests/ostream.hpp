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

#ifndef OSTREAM_H
#define OSTREAM_H

#include "helpers/istream.hpp"

//! Class for reading input data from stream.
class StreamWriter final
{
public:
    StreamWriter(std::ostream & stream)
        : m_stream(stream)
    {}

    virtual ~StreamWriter()
    {}

    StreamWriter() = default;
    StreamWriter(const StreamWriter &) = default;
    StreamWriter(StreamWriter &&) = default;

public:
    StreamWriter & operator =(const StreamWriter &) = default;
    StreamWriter & operator =(StreamWriter &&) = default;

    //! Gets the current offset.
    inline uint64_t getOffset()
    {
        return m_stream.tellp();
    }

    //! Writes integer types with endianess conversion to the stream.
    template<typename T>
    inline typename std::enable_if< is_endianess_convertible<T>::value, StreamWriter & >::type write(const T & value)
    {
        T write_value = qToBigEndian(value);
        m_stream.write((std::ostream::char_type *)&write_value, sizeof(write_value));
        return *this;
    }

    //! Writes base types without endianess conversion to the stream.
    template<typename T>
    inline typename std::enable_if< !is_endianess_convertible<T>::value, StreamWriter & >::type write(const T & value)
    {
        m_stream.write((std::ostream::char_type *)&value, sizeof(value));
        return *this;
    }

    //! Writes UUID to the stream.
    inline StreamWriter & write(const QUuid & value)
    {
        return write(value.data1).write(value.data2).write(value.data3).write(value.data4, sizeof(value.data4));
    }

    //! Writes uint24_t to the stream.
    inline StreamWriter & write(const U_UInt24 & value)
    {
        U_UInt24 write_value;
        write_value.m_value = qToBigEndian(value.m_value);
        m_stream.write((std::ostream::char_type *)write_value.bytes_representation.m_bytes, sizeof(write_value.bytes_representation.m_bytes));
        return *this;
    }

    //! Writes null-terminated string to the stream.
    inline StreamWriter & write(const QString & value)
    {
        std::string buffer = value.toStdString();
        m_stream.write(&buffer[0], buffer.size());
        char zero = 0;
        m_stream.write(&zero, sizeof(zero));
        return *this;
    }

    //! Writes QByteArray to the stream.
    inline StreamWriter & write(const QByteArray & value)
    {
        m_stream.write(value.constData(), value.size());
        return *this;
    }

    //! Writes FourCC to the stream.
    inline StreamWriter & write(const FourCC & value)
    {
        uint32_t code = (uint32_t)value;
        return write(code);
    }

    //! Writes optional value to the stream.
    template<typename T>
    inline StreamWriter & write(const optional<T> & value)
    {
        if(value.hasValue())
        {
            T optional_value = value.value();
            write(optional_value);
        }
        return *this;
    }

    //! Writes fixed-size array of values to the stream.
    template<typename T, size_t size>
    inline StreamWriter & write(const std::array<T, size> & value)
    {
        for(const T & entry : value)
        {
            write(entry);
        }
        return *this;
    }

    //! Writes a tuple of values to the stream.
    template<typename... TArgs>
    inline StreamWriter & write(std::tuple<TArgs...> & value)
    {
        write_tuple(value);
        return *this;
    }

    //! Writes the value by pointer. Suits for arrays of plain types, without endianess conversion.
    template<typename T>
    inline StreamWriter & write(const T * value, std::streamsize size)
    {
        m_stream.write((std::ostream::char_type *)value, size);
        return *this;
    }

private:
    //! Helper function for writing tuple of values from the stream. Finalizes reading.
    template<std::size_t iterator = 0, typename... TupleArgs>
    inline typename std::enable_if< iterator == sizeof...(TupleArgs), void >::type write_tuple(std::tuple<TupleArgs...> &)
    {
        //does nothing
    }

    //! Helper function for writing tuple of values from the stream.
    template<std::size_t iterator = 0, typename... TupleArgs>
    inline typename std::enable_if< iterator != sizeof...(TupleArgs), void >::type write_tuple(std::tuple<TupleArgs...> & tuple)
    {
        write( std::get<iterator>(tuple) );
        write_tuple< iterator + 1, TupleArgs...>(tuple);
    }

protected:
    std::ostream & m_stream;
};

#endif // OSTREAM_H
