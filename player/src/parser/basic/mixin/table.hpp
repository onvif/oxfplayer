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

#ifndef BASIC_MIXIN_TABLE_H
#define BASIC_MIXIN_TABLE_H

#include <QList>

#include "helpers/istream.hpp"

//! Class that describes a box mix-in for containing tables of data.
/*!
 * \brief This class is responsible for holding table of data for a box.
 * \param TContentType the data type of a table contents.
 */
template<typename TContentType>
class TableMixin
{
public:
    typedef TContentType ContentType;
    typedef QList<ContentType> TableType;

protected:
    TableMixin()
	{}

public:
    //! Reads the table from the input stream.
    void initialize(LimitedStreamReader & stream)
    {
        read_table<ContentType>(stream);
    }

    //! Returns the table.
    inline TableType getTable()
    {
        return m_table;
    }

protected:
    //! Reads the table of simple big endian integer types in a fast way
    template<typename T>
    inline typename std::enable_if< is_endianess_convertible<T>::value, void >::type read_table(LimitedStreamReader & stream)
    {
        uint32_t table_size;
        stream.read(table_size);
        m_table.reserve(table_size);

        T* values = new T[table_size];
        stream.read(values, table_size * sizeof(T));
        for(T* it = values; it != values + table_size; ++it)
        {
            m_table.append( qFromBigEndian(*it) );
        }
    }

    //! Reads the table of complex or non-endian types in a standard slow way
    template<typename T>
    inline typename std::enable_if< !is_endianess_convertible<T>::value, void >::type read_table(LimitedStreamReader & stream)
    {
        uint32_t table_size;
        stream.read(table_size);
        m_table.reserve(table_size);

        while(table_size--)
        {
            ContentType value;
            stream.read(value);
            m_table.append(value);
        }
    }

protected:
    TableType m_table;
};

#endif // BASIC_MIXIN_TABLE_H
