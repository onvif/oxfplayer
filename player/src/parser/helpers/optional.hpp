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

#ifndef HELPERS_OPTIONAL_H
#define HELPERS_OPTIONAL_H

#include <memory>

//! Class, containing optional values.
template<typename T>
class optional CC_CXX11_FINAL
{
public:
    optional()
        : m_value(nullptr)
    {}

    optional(const T & value)
        : m_value(new T(value))
    {}

    optional(const optional & right)
        : m_value(right.hasValue() ? new T(right.value()) : nullptr)
    {}

public:
    optional & operator =(const optional & right)
    {
        m_value.reset(right.hasValue() ? new T(right.value()) : nullptr);
        return *this;
    }

    T & operator *()
    {
        return *m_value;
    }

    const T & operator *() const
    {
        return *m_value;
    }

    T * operator ->()
    {
        return m_value.operator ->();
    }

    const T * operator ->() const
    {
        return m_value.operator ->();
    }

public:
    //! Clears the value of the object, if it is present;
    void reset()
    {
        m_value.reset();
    }

    //! Checks if the object has its value.
    bool hasValue() const
    {
        return m_value != nullptr;
    }

    //! Returns reference to the contained value.
    T & value()
    {
        return *m_value;
    }

    //! Returns const reference to the contained value.
    const T & value() const
    {
        return *m_value;
    }

private:
    std::unique_ptr<T> m_value;
};

#endif // HELPERS_OPTIONAL_H
