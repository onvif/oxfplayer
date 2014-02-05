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

#ifndef CROSSCOMPILATION_CXX11_H
#define CROSSCOMPILATION_CXX11_H

#ifdef _MSC_VER
#   if (_MSC_VER <= 1600)
#       define CC_CXX11_EXPLICIT_CONVERSION
#		define CC_CXX11_FINAL
#		define CC_CXX11_OVERRIDE
#		define CC_MSSPEC_TUPLE_NIL std::tr1::_Nil
#       define _VARIADIC_MAX 10
#       include <tuple>
#   elif (_MSC_VER <= 1700)
#       define CC_CXX11_EXPLICIT_CONVERSION
#		define CC_CXX11_FINAL final
#		define CC_CXX11_OVERRIDE override
#		define CC_MSSPEC_TUPLE_NIL	std::_Nil
#       define _VARIADIC_MAX 10
#       include <tuple>
#   else
#       define CC_CXX11_EXPLICIT_CONVERSION explicit
#		define CC_CXX11_FINAL final
#		define CC_CXX11_OVERRIDE override
#       define CC_CXX11_SUPPORT_VARIADIC_TEMPLATES
#      include <tuple>
#   endif
#else
#   define CC_CXX11_EXPLICIT_CONVERSION explicit
#	define CC_CXX11_FINAL final
#	define CC_CXX11_OVERRIDE override
#   define CC_CXX11_SUPPORT_VARIADIC_TEMPLATES
#   include <tuple>
#endif

#endif // CROSSCOMPILATION_CXX11_H
