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

#ifndef HELPERS_ENDIAN_H
#define HELPERS_ENDIAN_H

#include <QtEndian>

#include <type_traits>

#include "crosscompilation_inttypes.h"

namespace detail
{
    //! Helper class for identifying endianess convertible types.
    template<typename T>
    struct is_endianess_convertible_helper
            : std::false_type
    {};

    //! Helper class for identifying endianess convertible types. Generalization for int16_t.
    template<>
    struct is_endianess_convertible_helper<int16_t>
            : std::true_type
    {};

    //! Helper class for identifying endianess convertible types. Generalization for uint16_t.
    template<>
    struct is_endianess_convertible_helper<uint16_t>
            : std::true_type
    {};

    //! Helper class for identifying endianess convertible types. Generalization for int32_t.
    template<>
    struct is_endianess_convertible_helper<int32_t>
            : std::true_type
    {};

    //! Helper class for identifying endianess convertible types. Generalization for uint32_t.
    template<>
    struct is_endianess_convertible_helper<uint32_t>
            : std::true_type
    {};

    //! Helper class for identifying endianess convertible types. Generalization for int64_t.
    template<>
    struct is_endianess_convertible_helper<int64_t>
            : std::true_type
    {};

    //! Helper class for identifying endianess convertible types. Generalization for uint64_t.
    template<>
    struct is_endianess_convertible_helper<uint64_t>
            : std::true_type
    {};
}

//! Class for identifying endianess convertible types.
template<typename T>
struct is_endianess_convertible
        : public std::integral_constant<bool, (detail::is_endianess_convertible_helper<typename std::remove_cv<T>::type>::value)>
{};

#endif // HELPERS_ENDIAN_H
