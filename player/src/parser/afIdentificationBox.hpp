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

#ifndef AF_IDENTIFICATION_BOX_H
#define AF_IDENTIFICATION_BOX_H

#include "basic/dataBox.hpp"

//! Class describing Surveillance AF box.
/*!
 * \brief Declared in Survieallance Application Format. \n
 * Type: 'sumi' \n
 * Container: MetaBox, file level \n
 * Mandatory: yes \n
 * Quantity: exactly one
 */
class AFIdentificationBox
        : public DataBox<QUuid, QUuid, QUuid, uint64_t, uint64_t, uint16_t, uint16_t>
{
public:
    typedef DataBox<QUuid, QUuid, QUuid, uint64_t, uint64_t, uint16_t, uint16_t> DataBoxType;

public:
    explicit AFIdentificationBox(ChildrenMixin * parent)
        : DataBoxType(parent)
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        DataBoxType::initialize(stream);
        readContents(stream);
    }

public:
    //! Returns the current fragment UUID.
    QUuid getFragmentUUID()
    {
        return std::get<0>(DataBoxType::m_data);
    }

    //! Returns the predecessor fragment UUID.
    QUuid getPredecessorUUID()
    {
        return std::get<1>(DataBoxType::m_data);
    }

    //! Returns the successor fragment UUID.
    QUuid getSuccessorUUID()
    {
        return std::get<2>(DataBoxType::m_data);
    }

    //! Returns fragment start time.
    QDateTime getStartTime()
    {
        return QDateTime(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC).addSecs(std::get<3>(DataBoxType::m_data) / 10000000);
    }

    //! Returns the fragment duration.
    uint64_t getDuration()
    {
        return std::get<4>(DataBoxType::m_data) / 10000000;
    }

    //! Returns predecessor URI size.
    uint16_t getPredecessorURISize()
    {
        return std::get<5>(DataBoxType::m_data);
    }

    //! Returns successor URI size.
    uint16_t getSuccessorURISize()
    {
        return std::get<6>(DataBoxType::m_data);
    }

    //! Returns predecessor URI.
    QString getPredecessorURI()
    {
        return m_predecessorURI;
    }

    //! Returns successor URI.
    QString getSuccessorURI()
    {
        return m_successorURI;
    }

public:
    BOX_INFO("sumi", "Surveillance AF Identification Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(FragmentUUID);
        BOX_PROPERTY(PredecessorUUID);
        BOX_PROPERTY(SuccessorUUID);
        BOX_PROPERTY(StartTime);
        BOX_PROPERTY(Duration);
        BOX_PROPERTY(PredecessorURISize);
        BOX_PROPERTY(SuccessorURISize);
        BOX_PROPERTY(PredecessorURI);
        BOX_PROPERTY(SuccessorURI);
    }

private:
    //! Reads the box predessor URI and successor URI from the stream.
    void readContents(LimitedStreamReader &stream)
    {
        if(getPredecessorURISize() > 0)
        {
            stream.read(m_predecessorURI);
        }
        if(getSuccessorURISize() > 0)
        {
            stream.read(m_successorURI);
        }
    }

private:
    //! Predecessor URI string.
    QString m_predecessorURI;
    //! Successor URI string.
    QString m_successorURI;
};

#endif // AF_IDENTIFICATION_BOX_H
