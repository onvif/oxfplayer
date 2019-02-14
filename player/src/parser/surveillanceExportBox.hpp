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

#ifndef SURVEILLANCE_EXPORT_BOX_H
#define SURVEILLANCE_EXPORT_BOX_H

#include "basic/fullBox.hpp"

typedef std::tuple<QUuid, QString, QString, QString, QString> SurveillanceVideoEntry;
typedef std::tuple<QUuid, QString, QString, QString> SurveillanceAudioEntry;
typedef std::tuple<uint32_t, QString, QString, QString, QString> SurveillanceEntry;

typedef QList<SurveillanceVideoEntry> SurveillanceVideoTable;
typedef QList<SurveillanceAudioEntry> SurveillanceAudioTable;
typedef QList<SurveillanceEntry> SurveillanceTable;

//! Class describing Surveillance Export box.
/*!
 * \brief Declared in Onvif Export Format. \n
 * Type: 'suep' \n
 * Container: MetaBox, file level \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class SurveillanceExportBox
        : public FullBox<QString, QString, QString, uint64_t, QString>
{
public:
    typedef FullBox<QString, QString, QString, uint64_t, QString> FullBoxType;

public:
    explicit SurveillanceExportBox(ChildrenMixin * parent = nullptr)
        : FullBoxType(parent)
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream)
    {
        FullBoxType::initialize(stream);
        readContents(stream);
    }

public:
    //! Return the export unit name.
    QString getExportUnitName()
    {
        return std::get<2>(FullBoxType::m_data);
    }

    //! Returns the export unit url.
    QString getExportUnitURL()
    {
        return std::get<3>(FullBoxType::m_data);
    }

    //! Returns the export unit mac address.
    QString getExportUnitMAC()
    {
        return std::get<4>(FullBoxType::m_data);
    }

    //! Returns the export time.
    QDateTime getExportUnitTime()
    {
        return QDateTime(QDate(1904, 1, 1), QTime(0, 0), Qt::UTC).addSecs( std::get<5>(FullBoxType::m_data) );
    }

    //! Returns the export unit OID.
    QString getExportOperator()
    {
        return std::get<6>(FullBoxType::m_data);
    }

    //! Returns the list of video entries.
    SurveillanceTable & getEntries()
    {
        return m_entries;
    }
    //! Returns the list of video entries.
    SurveillanceVideoTable & getVideoEntries()
    {
        return m_video_entries;
    }

    //! Returns the list of audio entries.
    SurveillanceAudioTable & getAudioEntries()
    {
        return m_audio_entries;
    }

public:
    BOX_INFO("suep", "Surveillance Export Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        FullBoxType::registerProperties();
        BOX_PROPERTY(ExportUnitName);
        BOX_PROPERTY(ExportUnitURL);
        BOX_PROPERTY(ExportUnitMAC);
        BOX_PROPERTY(ExportUnitTime);
        BOX_PROPERTY(ExportOperator);
        BOX_PROPERTY(Entries);
        BOX_PROPERTY(VideoEntries);
        BOX_PROPERTY(AudioEntries);
    }

private:
    //! Reads the lists of video and audio entries from the stream.
    void readContents(LimitedStreamReader &stream)
    {
		int version = getVersion();
	if (version == 1) {
			int entries;
			stream.read(entries);
			while (--entries >= 0) {
				SurveillanceEntry value;
				stream.read(value);
				m_entries.append(value);
			}
		}
		else if (version == 0) {
			uint8_t table_size;
			stream.read(table_size);
			m_video_entries.reserve(table_size);
			while(table_size--)
			{
				SurveillanceVideoEntry value;
				stream.read(value);
				m_video_entries.append(value);
			}
			stream.read(table_size);
			m_audio_entries.reserve(table_size);
			while(table_size--)
			{
				SurveillanceAudioEntry value;
				stream.read(value);
				m_audio_entries.append(value);
			}
		}
    }

private:
    //! List of video entries.
    SurveillanceVideoTable m_video_entries;
    //! List of audio entries.
    SurveillanceAudioTable m_audio_entries;
    SurveillanceTable m_entries;
};

#endif // SURVIELANCE_EXPORT_BOX_H
