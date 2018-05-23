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

#ifndef HELPERS_ISTREAM_H
#define HELPERS_ISTREAM_H

#include <QStack>
#include <QString>
#include <QUuid>

#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

#include "endian.hpp"
#include "uint24.hpp"
#include "optional.hpp"
#include "fourcc.h"

//! Stream size state.
enum StreamState
{
    SS_Ok,          //!< Size of a stream is ok
    SS_Undersized,  //!< Count of symbols read is less than expected
    SS_Oversized    //!< Count of symbols read is more than expected
};

//! Stream wrapper class providing reading of a stream within specified boundaries.
class StreamWrapper CC_CXX11_FINAL
{
public:
    //! Shared stream pointer type
    typedef std::shared_ptr<std::istream> pointer;

public:
    //! Constructor
    /*!
     * \param stream_ptr Stream pointer shared among all of the stream wrappers reading this stream.
     */
    StreamWrapper(pointer stream_ptr)
        : m_parent(nullptr)
        , m_stream_ptr(stream_ptr)
        , m_stream_state(SS_Undersized)
    {
        m_start_position = m_stream_ptr->tellg();
        m_stream_ptr->seekg(0, std::ios_base::end);
        m_final_position = m_stream_ptr->tellg();
        restart();
    }

private:
    //! Private constructor, creating a substream.
    /*!
     * \param parent Parent stream wrapper
     * \param start_position Left boundary within stream
     * \param final_position Right boundary within stream
     */
    StreamWrapper(StreamWrapper * parent, std::streampos start_position, std::streampos final_position)
        : m_parent(parent)
        , m_stream_ptr(parent->m_stream_ptr)
        , m_start_position(start_position)
        , m_final_position(final_position)
        , m_stream_state(start_position < final_position ? SS_Undersized : ( start_position > final_position ? SS_Oversized : SS_Ok ))
    {
    }

public:
    //! Boolean operator saying if a stream state is ok. Has to be non-const, due to some strange implementation decisions in STL streams.
    operator bool()
    {
        m_stream_ptr->get();
        bool result = (bool)(*m_stream_ptr);
        m_stream_ptr->unget();
        return result;
    }

public:
	//! Goto defined position
	void seek(uint64_t pos) 
	{
        m_stream_ptr->seekg(pos);
	}
    //! Rewinds a stream read pointer to the left boundary and updates the stream state.
    void restart()
    {
        rewindToStart();
        onAfterMove();
    }

    //! Rewinds a stream read pointer to the left boundary and does not update the stream state.
    void rewindToStart()
    {
        m_stream_ptr->seekg(m_start_position);
    }

    //! Rewinds a stream read pointer to the right boundary and does not update the stream state.
    void rewindToFinish()
    {
        m_stream_ptr->seekg(m_final_position);
    }

    //! Rewinds a stream read pointer to the right boundary and updates the stream state.
    void moveToFinish()
    {
        rewindToFinish();
        onAfterMove();
    }

    //! Returns the left boundary value
    uint64_t getStartPosition()
    {
        return m_start_position;
    }

    //! Returns the right boundary value.
    uint64_t getFinishPosition()
    {
        return m_final_position;
    }

    //! Returns the current position.
    uint64_t getPosition()
    {
        return m_stream_ptr->tellg();
    }

    //! Returns the current stream state.
    StreamState getStreamState()
    {
        return m_stream_state;
    }

    //! Creates a new child stream wrapper and sets its boundaries to values passed by user.
    /*!
     * \param start_position Left boundary of a new child stream wrapper
     * \param final_position Right boundary of a new child stream wrapper
     * \return New child stream wrapper
     */
    StreamWrapper makeNew(std::streampos start_position, std::streampos final_position)
    {
        return StreamWrapper(this, start_position, final_position);
    }

    //! Creates a new child stream wrapper and sets its left boundary to a current position and right boundary to a value passed by user.
    /*!
     * \param final_position Right boundary of a new child stream wrapper
     * \return New child stream wrapper
     */
    StreamWrapper makeNew(std::streampos final_position)
    {
        return StreamWrapper(this, getPosition(), final_position);
    }

    //! Reads a value of a specified size from a stream. Stops at the right boundary, if the requested size was more than allowed.
    /*!
     * \param value_pointer Pointer to a value
     * \param size Value size
     * \return True, if operation was successful and the stream state allowed to read a value, false otherwise.
     */
    bool read(std::istream::char_type * value_pointer, size_t size)
    {
        if(m_parent != nullptr)
            m_parent->onBeforeMove(size);

        if(m_stream_state != SS_Oversized)
        {
            if( getPosition() + size <= m_final_position )
            {
                m_stream_ptr->read(value_pointer, size);
                m_stream_state = (getPosition() < m_final_position ? SS_Undersized : SS_Ok);
                return true;
            }

            qDebug() << "Attempt to read block of data" << size << "bytes size at" << getPosition() << "leaded to oversize of a box.";
            rewindToFinish();
            m_stream_state = SS_Oversized;
        }
        return false;
    }

    //! Reads a null-terminated string from a stream. Stops at the right boundary, if the requested size was more than allowed.
    /*!
     * \param string String value reference
     * \return True, if operation was successful and the stream state allowed to read a value, false otherwise.
     */
    bool readString(std::string & string)
    {
        if( m_stream_state == SS_Undersized )
        {
            uint64_t start = getPosition();
            std::getline(*m_stream_ptr, string, '\0');

            if(m_parent != nullptr)
                onAfterMove();

            if( start + string.size() <= m_final_position )
            {
                m_stream_state = (getPosition() < m_final_position ? SS_Undersized : SS_Ok);
                return true;
            }

            qDebug() << "Attempt to string" << string.size() << "bytes size at" << getPosition() << "leaded to oversize of a box.";
            rewindToFinish();
            m_stream_state = SS_Oversized;
        }
        return false;
    }

private:
    //! Updates the stream state of a current stream and all its parents after a move was performed.
    void onAfterMove()
    {
        if(m_parent != nullptr)
            m_parent->onAfterMove();

        uint64_t position = getPosition();
        m_stream_state = ( position < m_final_position ? SS_Undersized : ( position > m_final_position ? SS_Oversized : SS_Ok ) );
    }

    //! Updates the stream state of a current stream and all its parents before a move will be performed.
    /*!
     * \param size Size of a value to be read
     */
    void onBeforeMove(size_t size)
    {
        if(m_parent != nullptr)
            m_parent->onBeforeMove(size);

        if(m_stream_state != SS_Oversized)
        {
            uint64_t position = getPosition();
            if( position + size < m_final_position)
                m_stream_state = SS_Undersized;
            else if( position + size > m_final_position )
                m_stream_state = SS_Oversized;
            else
                m_stream_state = SS_Ok;
        }
    }

private:
    StreamWrapper* m_parent;
    pointer m_stream_ptr;
    uint64_t m_start_position;
    uint64_t m_final_position;
    StreamState m_stream_state;
};

//! Class for reading input data from stream. Knows its limitations, like start and finish position. Used to track size correctness.
class LimitedStreamReader CC_CXX11_FINAL
{
public:
    LimitedStreamReader(std::shared_ptr<std::istream> stream)
        : m_stream( stream )
        , m_initial_offset(0)
    {
        uint64_t final_position = m_stream.getFinishPosition();
        if( final_position > UINT_MAX )
        {
            m_short_size = 1;
            m_large_size = final_position;
        }
        else
        {
            m_short_size = (uint32_t)final_position;
            m_large_size = 0;
        }
    }

private:
    LimitedStreamReader(LimitedStreamReader * stream, uint64_t initial_offset, uint32_t size, uint64_t large_size)
        : m_stream( stream->m_stream.makeNew( initial_offset, initial_offset + ( size == 1 ? large_size : size ) ) )
        , m_initial_offset(initial_offset)
        , m_short_size(size)
        , m_large_size(large_size)
    {}

public:
    //! Creates a child stream reader, passing its known limitations.
    inline LimitedStreamReader makeNew(uint64_t initial_offset, uint32_t size, uint64_t large_size)
    {
        return LimitedStreamReader( this, initial_offset, size, large_size );
    }

    //! Returns the stream initial offset.
    inline uint64_t getInitialOffset()
    {
        return m_initial_offset;
    }

    //! Returns the stream final offset.
    inline uint64_t getFinalOffset()
    {
        return m_initial_offset + getSize();
    }

    //! Returns the stream current offset.
    inline uint64_t getCurrentOffset()
    {
        return m_stream.getPosition();
    }

    //! Returns the stream width-independent size.
    inline uint64_t getSize()
    {
        return (m_short_size == 1 ? m_large_size : m_short_size);
    }

    //! Returns the stream short size.
    inline uint32_t getShortSize()
    {
        return m_short_size;
    }

    //! Returns the stream long size.
    inline uint64_t getLongSize()
    {
        return m_large_size;
    }

    //! Conversion to boolean. Returns true if undrelying stream is valid.
    inline operator bool()
    {
        return (bool)m_stream;
    }

    //! Returns the stream state of an underlying stream wrapper.
    StreamState getStreamState()
    {
        return m_stream.getStreamState();
    }

	//! Goto defined position
	void seek(uint64_t pos) 
	{
        m_stream.seek(pos);
	}
	
	//! Rewinds stream read position to a left boundary and does not updates the stream state.
    void rewindToStart()
    {
        m_stream.rewindToStart();
    }

    //! Rewinds stream read position to a right boundary and does not updates the stream state.
    void rewindToFinish()
    {
        m_stream.rewindToFinish();
    }

    //! Rewinds stream read position to a right boundary and updates the stream state.
    void moveToFinish()
    {
        m_stream.moveToFinish();
    }

    //! Rewinds stream read position to a left boundary and updates the stream state.
    void restart()
    {
        m_stream.restart();
    }

    //! Reads integer types with endianess conversion from the stream.
    template<typename T>
    inline typename std::enable_if< is_endianess_convertible<T>::value, LimitedStreamReader & >::type read(T & value)
    {
        if(m_stream.read((std::istream::char_type *)&value, sizeof(value)))
            value = qFromBigEndian(value);
        else
            qDebug() << "Attempt to read integer value failed.";
        return *this;
    }

    //! Reads base types without endianess conversion from the stream.
    template<typename T>
    inline typename std::enable_if< !is_endianess_convertible<T>::value, LimitedStreamReader & >::type read(T & value)
    {
        if( !m_stream.read((std::istream::char_type *)&value, sizeof(value)) )
            qDebug() << "Attempt to read non-integer value failed.";
        return *this;
    }

    //! Reads UUID from the stream.
    inline LimitedStreamReader & read(QUuid & value)
    {
        return read(value.data1).read(value.data2).read(value.data3).read(value.data4, sizeof(value.data4));
    }

    //! Reads uint24_t from the stream.
    inline LimitedStreamReader & read(U_UInt24 & value)
    {
        value.m_value = 0;
        if( m_stream.read((std::istream::char_type *)value.bytes_representation.m_bytes, sizeof(value.bytes_representation.m_bytes)) )
            value.m_value = qFromBigEndian(value.m_value);
        else
            qDebug() << "Attempt to read UInt24 value failed.";
        return *this;
    }

    //! Reads null-terminated string from the stream.
    inline LimitedStreamReader & read(QString & value)
    {
        std::string buffer;
        if( m_stream.readString(buffer) )
            value = QString::fromUtf8(buffer.c_str(), (int)buffer.size());
        else
            qDebug() << "Attempt to read string value failed.";
        return *this;
    }

    //! Reads FourCC from the stream.
    inline LimitedStreamReader & read(FourCC & value)
    {
        uint32_t four_cc;
        read(four_cc);
        value = FourCC(four_cc);
        return *this;
    }

    //! Reads optional value from the stream.
    template<typename T>
    inline LimitedStreamReader & read(optional<T> & value)
    {
        T optional_value;
        read(optional_value);
        value = optional_value;
        return *this;
    }

    //! Reads fixed-size array of values from the stream.
    template<typename T, size_t size>
    inline LimitedStreamReader & read(std::array<T, size> & value)
    {
        for(auto it = value.begin(), end = value.end(); it != end; ++it)
        {
            T & entry = *it;
            read(entry);
        }
        return *this;
    }

    //! Reads the value by pointer. Suits for arrays of plain types, without endianess conversion.
    template<typename T>
    inline LimitedStreamReader & read(T * value, std::streamsize size)
    {
        if( !m_stream.read((std::istream::char_type *)value, size) )
            qDebug() << "Attempt to read plain data failed.";
        return *this;
    }

#ifdef CC_CXX11_SUPPORT_VARIADIC_TEMPLATES
    //! Reads a tuple of values from the stream.
    template<typename... TArgs>
    inline LimitedStreamReader & read(std::tuple<TArgs...> & value)
    {
        read_tuple(value);
        return *this;
    }

private:
    //! Helper function for reading tuple of values from the stream. Finalizes reading.
    template<std::size_t iterator = 0, typename... TupleArgs>
    inline typename std::enable_if< iterator == sizeof...(TupleArgs), void >::type read_tuple(std::tuple<TupleArgs...> &)
    {
        //does nothing
    }

    //! Helper function for reading tuple of values from the stream.
    template<std::size_t iterator = 0, typename... TupleArgs>
    inline typename std::enable_if< iterator != sizeof...(TupleArgs), void >::type read_tuple(std::tuple<TupleArgs...> & tuple)
    {
        read( std::get<iterator>(tuple) );
        read_tuple< iterator + 1, TupleArgs...>(tuple);
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
    inline LimitedStreamReader & read(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        read_tuple(std::get<2>(value));
        read_tuple(std::get<3>(value));
        read_tuple(std::get<4>(value));
        read_tuple(std::get<5>(value));
        read_tuple(std::get<6>(value));
        read_tuple(std::get<7>(value));
        read_tuple(std::get<8>(value));
        read_tuple(std::get<9>(value));
        return *this;
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
    inline LimitedStreamReader & read(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        read_tuple(std::get<2>(value));
        read_tuple(std::get<3>(value));
        read_tuple(std::get<4>(value));
        read_tuple(std::get<5>(value));
        read_tuple(std::get<6>(value));
        read_tuple(std::get<7>(value));
        read_tuple(std::get<8>(value));
        return *this;
    }

    template<typename T0,
        typename T1,
        typename T2,
        typename T3,
        typename T4,
        typename T5,
        typename T6,
        typename T7>
    inline LimitedStreamReader & read(std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        read_tuple(std::get<2>(value));
        read_tuple(std::get<3>(value));
        read_tuple(std::get<4>(value));
        read_tuple(std::get<5>(value));
        read_tuple(std::get<6>(value));
        read_tuple(std::get<7>(value));
        return *this;
    }

    template<typename T0,
        typename T1,
        typename T2,
        typename T3,
        typename T4,
        typename T5,
        typename T6>
    inline LimitedStreamReader & read(std::tuple<T0, T1, T2, T3, T4, T5, T6> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        read_tuple(std::get<2>(value));
        read_tuple(std::get<3>(value));
        read_tuple(std::get<4>(value));
        read_tuple(std::get<5>(value));
        read_tuple(std::get<6>(value));
        return *this;
    }

    template<typename T0,
        typename T1,
        typename T2,
        typename T3,
        typename T4,
        typename T5>
    inline LimitedStreamReader & read(std::tuple<T0, T1, T2, T3, T4, T5> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        read_tuple(std::get<2>(value));
        read_tuple(std::get<3>(value));
        read_tuple(std::get<4>(value));
        read_tuple(std::get<5>(value));
        return *this;
    }

    template<typename T0,
        typename T1,
        typename T2,
        typename T3,
        typename T4>
    inline LimitedStreamReader & read(std::tuple<T0, T1, T2, T3, T4> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        read_tuple(std::get<2>(value));
        read_tuple(std::get<3>(value));
        read_tuple(std::get<4>(value));
        return *this;
    }

    template<typename T0,
        typename T1,
        typename T2,
        typename T3>
    inline LimitedStreamReader & read(std::tuple<T0, T1, T2, T3> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        read_tuple(std::get<2>(value));
        read_tuple(std::get<3>(value));
        return *this;
    }

    template<typename T0,
        typename T1,
        typename T2>
    inline LimitedStreamReader & read(std::tuple<T0, T1, T2> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        read_tuple(std::get<2>(value));
        return *this;
    }

    template<typename T0,
        typename T1>
    inline LimitedStreamReader & read(std::tuple<T0, T1> & value)
    {
        read_tuple(std::get<0>(value));
        read_tuple(std::get<1>(value));
        return *this;
    }

    template<typename T0>
    inline LimitedStreamReader & read(std::tuple<T0> & value)
    {
        read_tuple(std::get<0>(value));
        return *this;
    }

    inline LimitedStreamReader & read(std::tuple<> &)
    {
        return *this;
    }

private:
    template<typename T>
    inline void read_tuple(T& value)
    {
        read(value);
    }

    inline void read_tuple(CC_MSSPEC_TUPLE_NIL &)
    {
    }

#endif

private:
    //! Underlying stream object.
    StreamWrapper m_stream;
    //! Initial offset.
    uint64_t m_initial_offset;
    //! Stream size. Equal to box size.
    uint32_t m_short_size;
    //! Stream long size. Equal to box long size, if it is present.
    uint64_t m_large_size;
};

#endif // HELPERS_ISTREAM_H
