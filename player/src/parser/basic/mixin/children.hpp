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

#ifndef BASIC_MIXIN_CHILDREN_H
#define BASIC_MIXIN_CHILDREN_H

#include "basic/box.h"
#include "boxFactory.h"

typedef QList<Box *> BoxPtrList;

//! Class that describes a box mix-in for containing other boxes.
/*!
 * \brief This class is responsible for holding all boxes that are members of other boxes.
 */
class ChildrenMixin
{
public:
    //! Destroys all child boxes, when the class object is destroyed.
    ~ChildrenMixin()
    {
        for(auto it = m_children.begin(), end = m_children.end(); it != end; ++it)
        {
			Box * child = *it;
            delete child;
        }
    }

public:
    //! Reads the boxes from the input stream.
    /*!
     * \param stream input stream
     * \param final_offset the offset at which reading should be stopped
     */
    inline void initialize(LimitedStreamReader & stream)
    {
        while(stream.getCurrentOffset() < stream.getFinalOffset())
        {
            BoxFactory::instance().parseBox(stream, this);
        }
    }

    //! Adds a child to a children list.
    inline void addChild(Box * child)
    {
        m_children.append(child);
    }

    //! Returns children list.
    inline BoxPtrList & getChildren()
    {
        return m_children;
    }

public:
    //! Returns a pointer to this box. Must be CC_CXX11_OVERRIDEn in children boxes.
    virtual Box * getThisBox() = 0;
    //! Returns a const pointer to this box.
    virtual const Box * getThisBox() const = 0;

protected:
    //! Children box list.
    BoxPtrList m_children;
};

#endif // BASIC_MIXIN_CHILDREN_H
