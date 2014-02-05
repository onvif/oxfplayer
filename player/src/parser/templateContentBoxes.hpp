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

#ifndef TEMPLATE_CONTENT_BOX_H
#define TEMPLATE_CONTENT_BOX_H

#include "basic/contentBox.hpp"

//! Template macro for describing Content boxes.
//! \param THE_CLASS class name for the box
//! \param FOUR_CC FourCC for the box
//! \param DESCRIPTION description string for the box
#define TEMPLATE_CONTENT_BOX(THE_CLASS, FOUR_CC, DESCRIPTION) \
    class THE_CLASS : public ContentBox \
    { \
        public: \
            explicit THE_CLASS(ChildrenMixin * parent = nullptr) \
                : ContentBox(parent) \
            { } \
        public: \
            BOX_INFO(FOUR_CC, DESCRIPTION) \
    }

//! Class describing Free Space box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'free' \n
 * Container: FileBox or other box \n
 * Mandatory: No \n
 * Quantity: Zero or more
 */
TEMPLATE_CONTENT_BOX(FreeSpaceBox, "free", "Free Space Box");
//! Class describing Movie Data box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'mdat' \n
 * Container: FileBox \n
 * Mandatory: No \n
 * Quantity: Zero or more
 */
TEMPLATE_CONTENT_BOX(MovieDataBox, "mdat", "Movie Data Box");
//! Class describing Skip box.
/*!
 * \brief Declared in ISO Base Media Format. \n
 * Type: 'skip' \n
 * Container: FileBox or other box \n
 * Mandatory: No \n
 * Quantity: Zero or more
 */
TEMPLATE_CONTENT_BOX(SkipBox, "skip", "Skip Box");

#undef TEMPLATE_CONTENT_BOX

//! Template macro for describing Content full boxes.
//! \param THE_CLASS class name for the box
//! \param FOUR_CC FourCC for the box
//! \param DESCRIPTION description string for the box
#define TEMPLATE_CONTENT_FULL_BOX(THE_CLASS, FOUR_CC, DESCRIPTION) \
    class THE_CLASS : public ContentFullBox \
    { \
        public: \
            explicit THE_CLASS(ChildrenMixin * parent = nullptr) \
                : ContentFullBox(parent) \
            { } \
        public: \
            BOX_INFO(FOUR_CC, DESCRIPTION) \
    }

//! Class describing Initial Object Descriptor Box.
/*!
 * \brief Declared in ISO MP4 file format (ISO/IEC 14496-14). \n
 * Type: 'iods' \n
 * Container: MovieBox \n
 * Mandatory: No \n
 * Quantity: Zero or one \n
 */
TEMPLATE_CONTENT_FULL_BOX(InitialObjectDescriptorBox, "iods", "Initial Object Descriptor Box");

#undef TEMPLATE_CONTENT_FULL_BOX

#endif // TEMPLATE_CONTENT_BOX_H
