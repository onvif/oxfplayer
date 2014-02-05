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

#ifndef BASIC_MANDATORYBOX_H
#define BASIC_MANDATORYBOX_H

#include "contentBox.hpp"
#include "helpers/optional.hpp"

//! Box class for mandatory boxes. This box is a stub, created when a mandatory 'exactly one' or 'one or more' box is missing.
class MandatoryBox CC_CXX11_FINAL
        : public Box
{
public:
    explicit MandatoryBox(const FourCC & fourCC, ChildrenMixin * parent = nullptr);

public:
    //! Returns the FourCC code of a missing mandatory box.
    virtual FourCC getBoxFourCC() CC_CXX11_OVERRIDE;
    //! Returns the box description.
    virtual QString getBoxDescription() CC_CXX11_OVERRIDE;

private:
    //! FourCC code for the box.
    FourCC m_four_cc;
};

//! Box class for mandatory boxes. This box is a stub, created when a mandatory 'exactly one of' boxes are missing.
class MandatoryBoxes CC_CXX11_FINAL
        : public Box
{
public:
    explicit MandatoryBoxes(const QList<FourCC> & four_cc_codes, ChildrenMixin * parent = nullptr);

public:
    //! Returns the first FourCC code from the list of missing mandatory boxes.
    virtual FourCC getBoxFourCC() CC_CXX11_OVERRIDE;
    //! Returns the box description.
    virtual QString getBoxDescription() CC_CXX11_OVERRIDE;
    //! Returns the list of FourCC codes of missing mandatory boxes.
    QList<FourCC> getMandatoryFourCC();

private:
    //! FourCC code for the box.
    QList<FourCC> m_four_cc_codes;
};

#endif // BASIC_MANDATORYBOX_H
