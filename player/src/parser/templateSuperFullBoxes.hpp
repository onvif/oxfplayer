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

#ifndef TEMPLATE_SUPER_FULL_BOX_H
#define TEMPLATE_SUPER_FULL_BOX_H

#include "basic/superFullBox.hpp"

//! Class describing Data Reference box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'dref' \n
 * Container: DataInformationBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one \n
 * Contains:
 *   - exactly 'Size' of either DataEntryUrlBoxes or DataEntryUrnBoxes
 *   .
 */
class DataReferenceBox
        : public SuperFullBox<uint32_t>
{
public:
    typedef SuperFullBox<uint32_t> SuperFullBoxType;

public:
    explicit DataReferenceBox(ChildrenMixin * parent = nullptr)
        : SuperFullBoxType(parent)
    {}

public:
    //! Returns size.
    uint32_t getSize()
    {
        return std::get<2>(SuperFullBoxType::m_data);
    }

public:
    BOX_INFO("dref", "Data Reference Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        SuperFullBoxType::registerProperties();
        BOX_PROPERTY(Size);
    }
};

//! Class describing File Delivery Item Information box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'fiin' \n
 * Container: MetaBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - unknown count of PartitionEntryBox
 *   .
 */
class FileDeliveryItemInformationBox
        : public SuperFullBox<uint16_t>
{
public:
    typedef SuperFullBox<uint16_t> SuperFullBoxType;

public:
    explicit FileDeliveryItemInformationBox(ChildrenMixin * parent = nullptr)
        : SuperFullBoxType(parent)
    {}

public:
    //! Returns size.
    uint16_t getSize()
    {
        return std::get<0>(SuperFullBoxType::m_data);
    }

public:
    BOX_INFO("fiin", "File Delivery Item Information Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        SuperFullBoxType::registerProperties();
        BOX_PROPERTY(Size);
    }
};

//! Class describing Item Protection box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'ipro' \n
 * Container: MetaBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 * Contains:
 *   - one or more ProtectionSchemeInfoBox
 *   .
 */
class ItemProtectionBox
        : public SuperFullBox<uint16_t>
{
public:
    typedef SuperFullBox<uint16_t> SuperFullBoxType;

public:
    explicit ItemProtectionBox(ChildrenMixin * parent = nullptr)
        : SuperFullBoxType(parent)
    {}

public:
    //! Returns protection count.
    uint16_t getProtectionCount()
    {
        return std::get<2>(SuperFullBoxType::m_data);
    }

public:
    BOX_INFO("ipro", "Item Protection Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        SuperFullBoxType::registerProperties();
        BOX_PROPERTY(ProtectionCount);
    }
};

//! Class describing Meta box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'meta' \n
 * Container: File, MovieBox, TrackBox or AdditionalMetadataContainerBox \n
 * Mandatory: No \n
 * Quantity: Zero or one in FileBox, MovieBox and TrackBox, one or more in AdditionalMetadataContainerBox
 * FileBox level MetaBox contains:
 *   - zero or one DataInformationBox
 *   - zero or one FileDeliveryItemInformationBox
 *   - zero or one ItemProtectionBox
 *   - [SAF extension] exactly one AfIdentificationBox
 *   - [OXF extension] exactly one SurveillanceExportBox
 *   .
 * MovieBox level MetaBox contains:
 *   - zero or one DataInformationBox
 *   .
 * TrackBox level MetaBox contains:
 *   - zero or one DataInformationBox
 *   - [SAF extension] exactly one CameraMicrophoneIdentificationBox
 *   .
 */
class MetaBox
        : public SuperFullBox<>
{
public:
    typedef SuperFullBox<> SuperFullBoxType;

public:
    explicit MetaBox(ChildrenMixin * parent = nullptr)
        : SuperFullBoxType(parent)
    {}

public:
    BOX_INFO("meta", "Meta Box")
};

//! Class describing Sample Description box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'stsd' \n
 * Container: SambleTableBox \n
 * Mandatory: Yes \n
 * Quantity: Exactly one \n
 * Contains:
 *   - exactly 'Size' of track and codec specific SampleEntries, including SurveillanceMetadataSampleEntryBox [SAF extension]
 *   .
 */
class SampleDescriptionBox
        : public SuperFullBox<uint32_t>
{
public:
    typedef SuperFullBox<uint32_t> SuperFullBoxType;

public:
    explicit SampleDescriptionBox(ChildrenMixin * parent = nullptr)
        : SuperFullBoxType(parent)
    {}

public:
    //! Returns size.
    uint32_t getSize()
    {
        return std::get<2>(SuperFullBoxType::m_data);
    }

public:
    BOX_INFO("stsd", "Sample Description Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        SuperFullBoxType::registerProperties();
        BOX_PROPERTY(Size);
    }
};

#endif // TEMPLATE_SUPER_FULL_BOX_H
