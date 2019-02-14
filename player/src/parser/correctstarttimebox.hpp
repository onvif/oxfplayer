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

#ifndef CORRECTSTARTTIME_BOX_H
#define CORRECTSTARTTIME_BOX_H

#include "basic/fullBox.hpp"

typedef std::tuple<uint32_t, QDateTime> CorrectionEntry;

typedef QList<CorrectionEntry> CorrectionTable;

//! Class describing Correct Start Time box.
/*!
 * \brief Declared in Onvif Export Format. \n
 * Type: 'suep' \n
 * Container: Schi Box, file level \n
 * Mandatory: Yes \n
 * Quantity: Exactly one
 */
class CorrectStartTimeBox
        : public Box
{
public:
    explicit CorrectStartTimeBox(ChildrenMixin * parent = nullptr)
        : Box(parent)
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream)
    {
        Box::initialize(stream);
		int entries;
		stream.read(entries);
			while (--entries >= 0) {
				int id;
				int64_t time;
				stream.read(id);
				stream.read(time);
				CorrectionEntry value(id, QDateTime(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC).addMSecs(time / 10000) );
				m_entries.append(value);
			}
    }

public:

    //! Returns the list of video entries.
    CorrectionTable & getEntries()
    {
        return m_entries;
    }

	//! Returns the start time of the first entry
    QDateTime getStartTime() const
	{
		if (m_entries.count() == 0) return QDateTime();
		return std::get<1>(m_entries.first());
	}

public:
    BOX_INFO("cstb", "Correction Start Time Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(Entries);
    }

private:
    CorrectionTable m_entries;
};

#endif // SURVIELANCE_EXPORT_BOX_H
