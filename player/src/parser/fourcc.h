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

#ifndef FOURCC_H
#define FOURCC_H

#include "crosscompilation_cxx11.h"
#include "crosscompilation_inttypes.h"

#include <QUuid>
#include <QtDebug>

//! Class for working with FourCC codes.
class FourCC CC_CXX11_FINAL
{
public:
    explicit FourCC(uint32_t code = 0);
    explicit FourCC(const char (& code)[5]);

public:
    bool isUuid() const;

public:
    static FourCC fromUuid(const QUuid & uuid);

public:
    bool operator == (const FourCC & right) const;
    bool operator != (const FourCC & right) const;
    bool operator < (const FourCC & right) const;
    //! Converts the FourCC code to uint32_t.
    CC_CXX11_EXPLICIT_CONVERSION operator uint32_t() const;
    //! Converts the FourCC code to the UUID.
    CC_CXX11_EXPLICIT_CONVERSION operator QUuid() const;
    //! Converts the FourCC code to the string.
    CC_CXX11_EXPLICIT_CONVERSION operator QString() const;

private:
    //! FourCC code.
    uint32_t m_code;

private:
    //! FourCC code for the "uuid" FourCC.
    static const uint32_t sc_uuid = 0x75756964;
};

inline QDebug operator <<(QDebug debug, const FourCC & four_cc)
{
    return debug << (QString)four_cc;
}

#endif // FOURCC_H
