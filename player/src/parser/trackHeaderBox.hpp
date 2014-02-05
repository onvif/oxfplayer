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

#ifndef TRACK_HEADER_BOX_H
#define TRACK_HEADER_BOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"

typedef std::array<int32_t, 9> TrackHeaderMatrix;

//! Class describing Track Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'tkhd' \n
 * Container: TrackBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class TrackHeaderBox
        : public Box
        , public FullBoxMixin<>
{
public:
    typedef FullBoxMixin<> FullBoxType;

public:
    explicit TrackHeaderBox(ChildrenMixin * parent = nullptr)
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
    //! Returns creation time.
    QDateTime getCreationTime()
    {
        return m_creation_time;
    }

    //! Returns modification time.
    QDateTime getModificationTime()
    {
        return m_modification_time;
    }

    //! Returns track id.
    uint32_t getTrackID()
    {
        return m_track_id;
    }

    //! Returns duration.
    uint64_t getDuration()
    {
        return m_duration;
    }

    //! Returns layer.
    int16_t getLayer()
    {
        return m_layer;
    }

    //! Returns alternate group.
    int16_t getAlternateGroup()
    {
        return m_alternate_group;
    }

    //! Returns volume.
    int16_t getVolume()
    {
        return m_volume;
    }

    //! Returns matrix.
    TrackHeaderMatrix & getMatrix()
    {
        return m_matrix;
    }

    //! Returns width.
    uint32_t getWidth()
    {
        return m_width;
    }

    //! Returns height.
    uint32_t getHeight()
    {
        return m_height;
    }


public:
    BOX_INFO("tkhd", "Track Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Version);
        BOX_PROPERTY(Flags);
        BOX_PROPERTY(CreationTime);
        BOX_PROPERTY(ModificationTime);
        BOX_PROPERTY(TrackID);
        BOX_PROPERTY(Duration);
        BOX_PROPERTY(Layer);
        BOX_PROPERTY(AlternateGroup);
        BOX_PROPERTY(Volume);
        BOX_PROPERTY(Matrix);
        BOX_PROPERTY(Width);
        BOX_PROPERTY(Height);
    }

private:
    //! Reads the box data from the stream, depending on the box version.
    void readContents(LimitedStreamReader &stream)
    {
        uint32_t reserved1, reserved2, reserved3;
        uint16_t reserved4;
        if(1 == getVersion())
        {
            uint64_t creation_time, modification_time;
            stream.read(creation_time).read(modification_time).read(m_track_id).read(reserved1).read(m_duration);
            m_creation_time = m_creation_time.addSecs(creation_time);
            m_modification_time = m_modification_time.addSecs(modification_time);
        }
        else
        {
            uint32_t creation_time, modification_time, duration;
            stream.read(creation_time).read(modification_time).read(m_track_id).read(reserved1).read(duration);
            m_creation_time = m_creation_time.addSecs(creation_time);
            m_modification_time = m_modification_time.addSecs(modification_time);
            m_duration = (duration == UINT_MAX ? ULONG_MAX : duration);
        }
        stream.read(reserved2)
                .read(reserved3)
                .read(m_layer)
                .read(m_alternate_group)
                .read(m_volume)
                .read(reserved4)
                .read(m_matrix)
                .read(m_width)
                .read(m_height);
    }

private:
    //! Creation time.
    QDateTime m_creation_time;
    //! Modification time.
    QDateTime m_modification_time;
    //! Duration.
    uint64_t m_duration;
    //! Track ID.
    uint32_t m_track_id;
    //! Width.
    uint32_t m_width;
    //! Height.
    uint32_t m_height;
    //! Layer.
    int16_t m_layer;
    //! Alternate group.
    int16_t m_alternate_group;
    //! Volume.
    int16_t m_volume;
    //! Matrix.
    TrackHeaderMatrix m_matrix;
};

#endif // TRACK_HEADER_BOX_H
