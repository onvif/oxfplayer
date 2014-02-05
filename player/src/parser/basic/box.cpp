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

#include "box.h"
#include "mixin/children.hpp"

Box::Box(ChildrenMixin * parent /*= nullptr*/)
    : m_size_error(SizeOk)
    , m_consistency_error(Consistent)
    , m_parent(parent)
{
    if(nullptr != m_parent)
    {
        m_parent->addChild(this);
    }
}

void Box::initialize(LimitedStreamReader &stream)
{
    Q_UNUSED(stream);
    m_offset = stream.getInitialOffset();
    m_size = stream.getShortSize();
    m_large_size = stream.getLongSize();
    registerProperties();
}

Box * Box::getParent() const
{
    if(m_parent != nullptr)
        return m_parent->getThisBox();
    return nullptr;
}

void Box::registerProperties()
{
    BOX_PROPERTY(BoxSize);
    BOX_PROPERTY(BoxOffset);
}

void Box::registerProperty(QString property, PropertyGetter getter)
{
    m_known_properties.append(property);
    m_property_map[property.toLower()] = getter;
}
