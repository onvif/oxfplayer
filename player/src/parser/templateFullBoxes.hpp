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

#ifndef TEMPLATE_FULL_BOX_H
#define TEMPLATE_FULL_BOX_H

#include "basic/fullBox.hpp"

//! Class describing Data Entry Url box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'url ' \n
 * Container: DataInformationBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class DataEntryUrlBox
        : public FullBox<QString>
{
public:
    typedef FullBox<QString> FullBoxType;

public:
    explicit DataEntryUrlBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns the location.
    QString getLocation()
    {
        return std::get<2>(FullBoxType::m_data);
    }

public:
    BOX_INFO("url ", "Data Entry Url Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(Location);
    }
};

//! Class describing Data Entry Urn box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'urn ' \n
 * Container: DataInformationBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class DataEntryUrnBox
        : public FullBox<QString, QString>
{
public:
    typedef FullBox<QString, QString> FullBoxType;

public:
    explicit DataEntryUrnBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns the name.
    QString getName()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns the location.
    QString getLocation()
    {
        return std::get<3>(FullBoxType::m_data);
    }

public:
    BOX_INFO("urn ", "Data Entry Urn Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(Name);
        BOX_PROPERTY(Location);
    }
};

//! Class describing Handler box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'hdlr' \n
 * Container: MediaBox or MetaBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class HandlerBox
        : public FullBox<uint32_t, FourCC, uint32_t, uint32_t, uint32_t, QString>
{
public:
    typedef FullBox<uint32_t, FourCC, uint32_t, uint32_t, uint32_t, QString> FullBoxType;

public:
    explicit HandlerBox(ChildrenMixin * parent = nullptr)
        : FullBox<uint32_t, FourCC, uint32_t, uint32_t, uint32_t, QString>(parent)
    {}

public:
    //! Returns the predefined data.
    uint32_t getPredefined()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns the handler type.
    FourCC getHandlerType()
    {
        return std::get<3>(FullBoxType::m_data);
    }

    //! Returns the name.
    QString getName()
    {
        return std::get<7>(FullBoxType::m_data);

    }

public:
    BOX_INFO("hdlr", "Handler Reference Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(Predefined);
        BOX_PROPERTY(HandlerType);
        BOX_PROPERTY(Name);
    }
};

//! Class describing Hint Media Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'hmhd' \n
 * Container: MediaInformationBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one specific media header (VideoMediaHeaderBox, SoundMediaHeaderBox, HintMediaHeaderBox or NullMediaHeaderBox) shall be present
 */
class HintMediaHeaderBox
        : public FullBox<uint16_t, uint16_t, uint32_t, uint32_t, uint32_t>
{
public:
    typedef FullBox<uint16_t, uint16_t, uint32_t, uint32_t, uint32_t> FullBoxType;

public:
    explicit HintMediaHeaderBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns maximum protocal data unit size.
    uint16_t getMaxProtocolDataUnitSize()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns average protocol data unit size.
    uint16_t getAverageProtocolDataUnitSize()
    {
        return std::get<3>(FullBoxType::m_data);
    }

    //! Returns maximum bitrate.
    uint32_t getMaxBitrate()
    {
        return std::get<4>(FullBoxType::m_data);
    }

    //! Returns average bitrate.
    uint32_t getAverageBitrate()
    {
        return std::get<5>(FullBoxType::m_data);
    }

public:
    BOX_INFO("hmhd", "Hint Media Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(MaxProtocolDataUnitSize);
        BOX_PROPERTY(AverageProtocolDataUnitSize);
        BOX_PROPERTY(MaxBitrate);
        BOX_PROPERTY(AverageBitrate);
    }
};

//! Class describing Movie Fragment Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'mfhd' \n
 * Container: MovieFragmentBox \n
 * Mandatory: No \n
 * Quantity: Exactly one
 */
class MovieFragmentHeaderBox
        : public FullBox<uint32_t>
{
public:
    typedef FullBox<uint32_t> FullBoxType;

public:
    explicit MovieFragmentHeaderBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns the sequence number.
    uint32_t getSequenceNumber()
    {
        return std::get<2>(FullBoxType::m_data);
    }

public:
    BOX_INFO("mfhd", "Movie Fragment Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(SequenceNumber);
    }
};

//! Class describing Movie Fragment Random Access Offset box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'mfro' \n
 * Container: MovieFragmentRandomAccessBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class MovieFragmentRandomAccessOffsetBox
        : public FullBox<uint32_t>
{
public:
    typedef FullBox<uint32_t> FullBoxType;

public:
    explicit MovieFragmentRandomAccessOffsetBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns size.
    uint32_t getSize()
    {
        return std::get<2>(FullBoxType::m_data);
    }

public:
    BOX_INFO("mfro", "Movie Fragment Random Access Offset Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(Size);
    }
};

//! Class describing Null Media Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'nmhd' \n
 * Container: MediaInformationBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one specific media header (VideoMediaHeaderBox, SoundMediaHeaderBox, HintMediaHeaderBox or NullMediaHeaderBox) shall be present
 */
class NullMediaHeaderBox
        : public FullBox<>
{
public:
    typedef FullBox<> FullBoxType;

public:
    explicit NullMediaHeaderBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    BOX_INFO("nmhd", "Null Media Header Box")
};

//! Class describing Scheme Type box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'schm' \n
 * Container: ProtectionSchemeInfoBox, RestrictedSchemeInfoBox or SRTPProcessBox \n
 * Mandatory: No \n
 * Quantity: Zero or one in ProtectionSchemeInfoBox, Exactly one in RestrictedSchemeInfoBox and STRPProcessBox
 */
class SchemeTypeBox
        : public FullBox<FourCC, uint32_t, QString>
{
public:
    typedef FullBox<FourCC, uint32_t, QString> FullBoxType;

public:
    explicit SchemeTypeBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns the scheme type FourCC.
    FourCC getSchemeType()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns the scheme version.
    uint32_t getSchemeVersion()
    {
        return std::get<3>(FullBoxType::m_data);
    }

    //! Returns the scheme Uri.
    QString getSchemeURI()
    {
        return std::get<4>(FullBoxType::m_data);
    }

public:
    BOX_INFO("schm", "Protection Scheme Information Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(SchemeType);
        BOX_PROPERTY(SchemeVersion);
        BOX_PROPERTY(SchemeURI);
    }
};

//! Class describing Sound Media Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'smhd' \n
 * Container: MediaInformationBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one specific media header (VideoMediaHeaderBox, SoundMediaHeaderBox, HintMediaHeaderBox or NullMediaHeaderBox) shall be present
 */
class SoundMediaHeaderBox
        : public FullBox<uint16_t, uint16_t>
{
public:
    typedef FullBox<uint16_t, uint16_t> FullBoxType;

public:
    explicit SoundMediaHeaderBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns the balance.
    uint16_t getBalance()
    {
        return std::get<2>(FullBoxType::m_data);
    }

public:
    BOX_INFO("smhd", "Sound Media Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(Balance);
    }
};

//! Class describing Track Extends box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'trex' \n
 * Container: MovieExtendsBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one for each track in the MovieBox
 */
class TrackExtendsBox
        : public FullBox<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t>
{
public:
    typedef FullBox<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t> FullBoxType;

public:
    explicit TrackExtendsBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Returns the track id.
    uint32_t getTrackID()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns the default sample description index.
    uint32_t getDefaultSampleDescriptionIndex()
    {
        return std::get<3>(FullBoxType::m_data);
    }

    //! Returns the default sample duration.
    uint32_t getDefaultSampleDuration()
    {
        return std::get<4>(FullBoxType::m_data);
    }

    //! Returns the default sample size.
    uint32_t getDefaultSampleSize()
    {
        return std::get<5>(FullBoxType::m_data);
    }

    //! Returns the default sample flags.
    uint32_t getDefaultSampleFlags()
    {
        return std::get<6>(FullBoxType::m_data);
    }

public:
    BOX_INFO("trex", "Track Extends Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(TrackID);
        BOX_PROPERTY(DefaultSampleDescriptionIndex);
        BOX_PROPERTY(DefaultSampleDuration);
        BOX_PROPERTY(DefaultSampleSize);
        BOX_PROPERTY(DefaultSampleFlags);
    }
};

//! Class describing Video Media Header box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'vmhd' \n
 * Container: MediaInformationBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one specific media header (VideoMediaHeaderBox, SoundMediaHeaderBox, HintMediaHeaderBox or NullMediaHeaderBox) shall be present
 */
class VideoMediaHeaderBox
        : public FullBox<uint16_t, uint16_t, uint16_t, uint16_t>
{
public:
    typedef FullBox<uint16_t, uint16_t, uint16_t, uint16_t> FullBoxType;

public:
    explicit VideoMediaHeaderBox(ChildrenMixin * parent)
        : FullBoxType(parent)
    {}

public:
    //! Returns graphics mode.
    uint16_t getGraphicsMode()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns red.
    uint16_t getRed()
    {
        return std::get<3>(FullBoxType::m_data);
    }

    //! Returns green.
    uint16_t getGreen()
    {
        return std::get<4>(FullBoxType::m_data);
    }

    //! Returns blue.
    uint16_t getBlue()
    {
        return std::get<5>(FullBoxType::m_data);
    }

public:
    BOX_INFO("vmhd", "Video Media Header Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(GraphicsMode);
        BOX_PROPERTY(Red);
        BOX_PROPERTY(Green);
        BOX_PROPERTY(Blue);
    }
};

//! Class describing Handler box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'tfdt' \n
 * Container: TrackFragmentBox \n
 * Mandatory: No \n
 * Quantity: Zero or one
 */
class TrackFragmentDecodeTimeBox
        : public FullBox<uint64_t>
{
public:
    typedef FullBox<uint64_t> FullBoxType;

public:
    explicit TrackFragmentDecodeTimeBox(ChildrenMixin * parent = nullptr)
        : FullBox<uint64_t>(parent)
    {}

public:
    //! Read this box to support v0 and v1
    virtual void initialize(LimitedStreamReader& stream) CC_CXX11_OVERRIDE
    {
        initializeHeaders(stream);
        if (getVersion() == 0) {
            uint32_t size2;
            stream.read(size2);
            std::get<2>(m_data) = size2;
        }
        else {
            stream.read(std::get<2>(m_data));
        }
    }
    //! Returns the time offset relative to scale.
    uint64_t getStartTime()
    {
        return std::get<2>(m_data);

    }

public:
    BOX_INFO("tfdt", "Track Fragment Decode Time")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(StartTime);
    }
};


#endif // TEMPLATE_FULL_BOX_H
