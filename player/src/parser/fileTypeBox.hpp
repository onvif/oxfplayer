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

#ifndef FILE_TYPE_BOX_H
#define FILE_TYPE_BOX_H

#include "basic/box.h"
#include "basic/mixin/data.hpp"
#include "basic/mixin/table.hpp"

typedef FourCC FileTypeBoxEntry;

//! Class describing File Type box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'ftyp' \n
 * Container: FileBox \n
 * Mandatory: Yes* \n
 * Quantity: Exactly one* \n
 * * For compatibility with an earlier version of the specification, files may be conformant to the specification and not contain a file-type box. \n
 * Files with no file-type box should be read as if they contained an FTYP box with Major_brand='mp41', minor_version=0, and the single compatible brand 'mp41'.
 */
class FileTypeBox
        : public Box
        , public DataMixin<FourCC, uint32_t>
        , public TableMixin<FileTypeBoxEntry>
{
public:
    typedef DataMixin<FourCC, uint32_t> DataBoxType;

public:
    explicit FileTypeBox(ChildrenMixin * parent = nullptr)
        : Box(parent)
    {}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream) CC_CXX11_OVERRIDE
    {
        Box::initialize(stream);
        DataBoxType::initialize(stream);
		readTable(stream, (stream.getFinalOffset() - stream.getCurrentOffset()) / sizeof(uint32_t));
    }

public:
    //! Returns the major brand.
    FourCC getMajorBrand()
    {
        return std::get<0>(DataBoxType::m_data);
    }

    //! Returns the minor version.
    uint32_t getMinorVersion()
    {
        return std::get<1>(DataBoxType::m_data);
    }

public:
    BOX_INFO("ftyp", "File Type Box")

protected:
    //! Registers properties for a box type.
    virtual void registerProperties() CC_CXX11_OVERRIDE
    {
        Box::registerProperties();
        BOX_PROPERTY(MajorBrand);
        BOX_PROPERTY(MinorVersion);
        BOX_PROPERTY(Table);
    }

private:
    //! Reads the table of FourCC identifiers of codec specific box types.
    void readTable(LimitedStreamReader &stream, size_t table_size)
    {
        m_table.reserve(table_size);

        uint32_t* values = new uint32_t[table_size];
        stream.read(values, table_size * sizeof(uint32_t));
        for(uint32_t* it = values; it != values + table_size; ++it)
        {
            m_table.append( FourCC( qFromBigEndian( *it ) ) );
        }
    }
};

#endif // FILE_TYPE_BOX_H
