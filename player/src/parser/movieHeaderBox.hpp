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

#ifndef MOVIE_HEADER_BOX_H
#define MOVIE_HEADER_BOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"

typedef std::array<int32_t, 9> MovieHeaderMatrix;
typedef std::array<unsigned char, 24> MovieHeaderPreDefined;

//! Class describing Movie Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'mvhd' \n
 * Container: MovieBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class MovieHeaderBox
        : public Box
        , public FullBoxMixin<>
{
public:
    typedef FullBoxMixin<> FullBoxType;

public:
    explicit MovieHeaderBox(ChildrenMixin * parent = nullptr)
        : Box(parent)
        , FullBoxType()
        , m_creation_time(QDate(1904, 1, 1), QTime(0, 0), Qt::UTC)
        , m_modification_time(QDate(1904, 1, 1), QTime(0, 0), Qt::UTC)
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        FullBoxType::initialize(stream);
        readContents(stream);
    }

public:
    //! Returns the creation time.
    QDateTime getCreationTime()
    {
        return m_creation_time;
    }

    //! Returns the modification time.
    QDateTime getModificationTime()
    {
        return m_modification_time;
    }

    //! Returns the time scale.
    uint32_t getTimeScale()
    {
        return m_time_scale;
    }

    //! Returns the duration.
    uint64_t getDuration()
    {
        return m_duration;
    }

    //! Returns the rate.
    int32_t getRate()
    {
        return (int32_t)(1.0 * m_rate / (USHRT_MAX + 1));
    }

    //! Returns the volume.
    int16_t getVolume()
    {
        return (int16_t)(1.0 * m_volume / (UCHAR_MAX + 1));
    }

    //! Returns the matrix.
    MovieHeaderMatrix & getMatrix()
    {
        return m_matrix;
    }

    //! Returns the predefined data.
    MovieHeaderPreDefined getPredefined()
    {
        return m_predefined;
    }

    //! Returns the next track id.
    uint32_t getNextTrackID()
    {
        return m_next_track_id;
    }

public:
    BOX_INFO("mvhd", "Movie Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(CreationTime);
        BOX_PROPERTY(ModificationTime);
        BOX_PROPERTY(TimeScale);
        BOX_PROPERTY(Duration);
        BOX_PROPERTY(Rate);
        BOX_PROPERTY(Volume);
        BOX_PROPERTY(Matrix);
        BOX_PROPERTY(Predefined);
        BOX_PROPERTY(NextTrackID);
    }

private:
    //! Reads the contents of the box from the stream depending on the box version.
    void readContents(LimitedStreamReader &stream)
    {
        std::array<unsigned char, 2 + (2 * 4)> reserved;
        if(1 == getVersion())
        {
            uint64_t creation_time, modification_time;
            stream.read(creation_time)
                    .read(modification_time)
                    .read(m_time_scale)
                    .read(m_duration);
            m_creation_time = m_creation_time.addSecs(creation_time);
            m_modification_time = m_modification_time.addSecs(modification_time);
        }
        else
        {
            uint32_t creation_time, modification_time, duration;
            stream.read(creation_time)
                    .read(modification_time)
                    .read(m_time_scale)
                    .read(duration);
            m_creation_time = m_creation_time.addSecs(creation_time);
            m_modification_time = m_modification_time.addSecs(modification_time);
            m_duration = (duration == UINT_MAX ? ULONG_MAX : duration);
        }
        stream.read(m_rate)
                .read(m_volume)
                .read(reserved)
                .read(m_matrix)
                .read(m_predefined)
                .read(m_next_track_id);
    }

private:
    //! Creation time.
    QDateTime m_creation_time;
    //! Modification time.
    QDateTime m_modification_time;
    //! Duration.
    uint64_t m_duration;
    //! Time scale.
    uint32_t m_time_scale;
    //! Next track id.
    uint32_t m_next_track_id;
    //! Rate.
    int32_t m_rate;
    //! Volume.
    int16_t m_volume;
    //! Matrix.
    MovieHeaderMatrix m_matrix;
    //! Predefined data.
    MovieHeaderPreDefined m_predefined;
};

#endif // MOVIE_HEADER_BOX_H
