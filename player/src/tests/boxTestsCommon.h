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

#ifndef BOXTESTSCOMMON_H
#define BOXTESTSCOMMON_H

#include <QtTest>

#include <QString>
#include <QDateTime>
#include <QList>
#include <QBitArray>

#include <sstream>

#include "basic/fileBox.hpp"
#include "boxFactory.h"
#include "fourcc.h"

#include "ostream.hpp"

Q_DECLARE_METATYPE(FourCC)
Q_DECLARE_METATYPE(U_UInt24)

class BoxSize
{
public:
    BoxSize(uint32_t data_size)
        : m_data_size(data_size)
    {}

public:
    static uint32_t empty()
    {
        return 1;
    }

    static uint64_t large_empty()
    {
        return 0;
    }

    uint32_t size()
    {
        return 2 * sizeof(uint32_t) + m_data_size;
    }

    uint64_t large_size()
    {
        return size() + sizeof(uint64_t);
    }

    uint32_t fullbox_size()
    {
        return size() + sizeof(uint32_t);
    }

    uint64_t fullbox_large_size()
    {
        return large_size() + sizeof(uint32_t);
    }

private:
    uint32_t m_data_size;
};

template<typename TFlags>
class FlagStateGenerator
{
public:
    FlagStateGenerator()
        : m_end(begin())
    {

    }

public:
    FlagStateGenerator & add(TFlags flag)
    {
        if(!m_flags.contains(flag))
        {
            m_end = (TFlags)(m_end | flag);
            m_flags.append(flag);
        }
        return *this;
    }

    TFlags begin()
    {
        m_state.clear();
        m_state.append(0);
        return TFlags(0);
    }

    TFlags end()
    {
        return m_end;
    }

    TFlags next()
    {
        TFlags result(TFlags(0));
        if(m_state.isEmpty())
        {
            return TFlags(m_end + 1);
        }
        for(int i = 0; i < m_flags.size(); ++i)
        {
            if( m_state.contains(i) )
                result = TFlags(result | m_flags[i]);
        }
        int last = m_state.back();
        if(m_state.size() == (last + 1))
        {
            m_state.clear();
            m_state.append(last + 1);
        }
        else
        {
            m_state.push_front(last - m_state.size());
        }
        if(result == m_end)
        {
            m_state.clear();
        }
        return result;
    }

private:
    QList<TFlags> m_flags;
    QVector<int> m_state;
    TFlags m_end;
};

inline uint32_t uuid_size()
{
    return sizeof(uint32_t) + 2 * sizeof(uint16_t) + 8 * sizeof(uint8_t);
}

inline QDateTime origin()
{
    QDateTime origin_value = QDateTime(QDate(1904, 1, 1), QTime(0,0), Qt::UTC);
    return origin_value;
}

inline uint64_t datetime_write()
{
    return origin().secsTo( QDateTime::currentDateTime() );
}

inline QDateTime datetime_read(uint64_t time)
{
    return origin().addSecs(time);
}

inline QBitArray flag_to_bits(U_UInt24 flag)
{
    QBitArray result(24);
    for(int i = 0; i < 24; ++i)
    {
        result[i] = flag & (1 << i);
    }
    return result;
}

class FBHeader : BoxSize
{
public:
    FBHeader(uint32_t data_size) : BoxSize(data_size + sizeof(uint8_t) + sizeof(U_UInt24)) {}

};


#endif // BOXTESTSCOMMON_H
