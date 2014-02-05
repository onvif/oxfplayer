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

#ifndef BASIC_BOX_H
#define BASIC_BOX_H

#include "crosscompilation_cxx11.h"

#include <QList>
#include <QHash>

#include <functional>

#include "helpers/istream.hpp"
#include "helpers/property.hpp"
#include "helpers/is_a.hpp"
#include "fourcc.h"

class ChildrenMixin;

//! Base class for all box types.
class Box
{
public:
    typedef std::function<Property ()> PropertyGetter;

    //! Box size state
    enum SizeError
    {
        SizeOk,             //!< Size of a box was ok
        SizeInsufficient,   //!< Size of a box was less, than expected
        SizeExceeding       //!< Size of a box was more, than expected
    };

    //! Box consistency state flags
    enum ConsistencyError
    {
        Consistent              = 0x00,     //!< Box is consistent
        HasNotEnoughBoxes       = 0x01,     //!< Box does not contain expected mandatory boxes or boxes that have count defined 'the same as'
        HasTooManyBoxes         = 0x02,     //!< Box contains too many boxes which count was defined as 'zero or one', 'exactly one', 'the same as'
        HasConflictingBoxes     = 0x04,     //!< Box contains children boxes which have count defined as 'one of specified types', but different boxes of these types are present
        HasUnexpectedBoxes      = 0x08,     //!< Box contains other boxes, which present was not defined by known standards
        IsMandatoryBox          = 0x10,     //!< Box is a mandatory box stub, which was created to signal a parent box, which mandatory boxes are missing
        IsBoxWithExceedingCount = 0x20,     //!< Box is a box which count was defined as 'zero or one', 'exactly one' or 'the same as', but its count is exceeding
        IsConflictingBox        = 0x40,     //!< Box is a box which count was defined as 'one of specific types', but different boxes of these types are present in a parent box
        IsUnexpectedBox         = 0x80      //!< Box presence is unexpected in a parent box
    };

protected:
    Box(ChildrenMixin * parent = nullptr);

public:
	virtual ~Box()
	{}

public:
    //! Reads the box from the input stream.
    virtual void initialize(LimitedStreamReader &stream);
    //! Returns the box FourCC code.
    virtual FourCC getBoxFourCC() = 0;
    //! Returns the box description.
    virtual QString getBoxDescription() = 0;

public:
    //! Returns the parent of the box.
    Box * getParent() const;

    //! Sets size error for the box.
    void setSizeError(SizeError error)
    {
        m_size_error = error;
    }

    //! Gets size error of the box.
    SizeError getSizeError() const
    {
        return m_size_error;
    }

    //! Sets size error for the box.
    void setConsistencyError(ConsistencyError error)
    {
        m_consistency_error = ConsistencyError(m_consistency_error | error);
    }

    //! Gets size error of the box.
    ConsistencyError getConsistencyError() const
    {
        return m_consistency_error;
    }

    //! Returns the size of box 'size' variable
    inline uint64_t getBoxSizeDimension()
    {
        return (m_size == 1 ? sizeof(m_large_size) : sizeof(m_size));
    }

    //! Returns the box size.
    inline uint64_t getBoxSize()
    {
        return (m_size == 1 ? m_large_size : m_size);
    }

    //! Returns the box initial offset in an input stream.
    inline uint64_t getBoxOffset()
    {
        return m_offset;
    }

    //! Returns the names of the box registered properties.
    inline QList<QString> getProperties()
    {
        return m_known_properties;
    }

    //! Returns the property by name. Returns empty QVariant() if a name is unknown.
    inline Property getProperty(QString name)
    {
        auto it = m_property_map.find(name.toLower());
        if(it != m_property_map.end())
        {
            return (*it)();
        }
        return Property();
    }

protected:
    //! Registeres properties for a box type.
    virtual void registerProperties();
    //! Registers one property.
    /*!
     * \param property property name
     * \param getter getter function for a property
     */
    void registerProperty(QString property, PropertyGetter getter);

protected:
    //! Box size error type.
    SizeError m_size_error;
    //! Consistency error type.
    ConsistencyError m_consistency_error;
    //! Box initial offset in an input stream.
    uint64_t m_offset;
    //! Box size.
    uint32_t m_size;
    //! Box size, if size exceeds uint32 maximum value.
    uint64_t m_large_size;
    //! Pointer to a parent box.
    ChildrenMixin * m_parent;
    //! List of registered property names.
    QList<QString> m_known_properties;
    //! Map of registered properties with getters.
    QHash<QString, PropertyGetter> m_property_map;
};

//! Defines FourCC for a box and getter functions.
#define BOX_FOUR_CC(FOUR_CC) \
    static FourCC getFourCC() \
    { \
        static const FourCC fourCC(FOUR_CC); \
        return fourCC; \
    } \
    inline virtual FourCC getBoxFourCC() CC_CXX11_OVERRIDE \
    { \
        return getFourCC(); \
    }

//! Defines box desription and getter functions.
#define BOX_DESCRIPTION(DESCRIPTION) \
    static QString getDescription() \
    { \
        static const QString description(DESCRIPTION); \
        return description; \
    } \
    inline virtual QString getBoxDescription() CC_CXX11_OVERRIDE \
    { \
        return getDescription(); \
    }

//! Defines box FourCC and description.
#define BOX_INFO(FOUR_CC, DESCRIPTION) \
    BOX_FOUR_CC(FOUR_CC) \
    BOX_DESCRIPTION(DESCRIPTION)

//! Syntaxic sugar for registering properties.
/*!
 * \param NAME Name of a property
 * \param FUNCTION Function to call in the box class
 */
#define BOX_PROPERTY2(NAME, FUNCTION) \
    registerProperty( #NAME, [this]() -> Property { return Property( this->FUNCTION() ); } )

//! Syntaxic sugar for registering properties.
/*!
 * \param NAME Name of a property. The function will be composed as get + property name
 */
#define BOX_PROPERTY(NAME) \
    BOX_PROPERTY2(NAME, get##NAME)

#endif // BASIC_BOX_H
