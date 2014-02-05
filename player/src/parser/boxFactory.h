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

#ifndef BOX_FACTORY_H
#define BOX_FACTORY_H

#include "crosscompilation_cxx11.h"

#include <QMap>
#include <functional>
#include <type_traits>

#include "basic/box.h"

//! Helper class that allows to detect, if a class has getFourCC function.
template<typename BoxType>
struct has_four_cc {

    template<typename CheckType> static uint32_t check(CheckType*, decltype(CheckType::getFourCC())* = 0);
    static uint8_t check(...);

    static const bool value = (sizeof(check((BoxType*)nullptr)) == sizeof(uint32_t));
};

//! Singleton class, that performs parsing of the boxes from the input stream.
class BoxFactory CC_CXX11_FINAL
        : public QObject
{
    Q_OBJECT

private:
    typedef std::function<Box * (ChildrenMixin * parent)> BoxCreator;
    typedef QMap<FourCC, BoxCreator> CreatorMap;

private:
    BoxFactory();

	BoxFactory(const BoxFactory &);
	BoxFactory & operator =(const BoxFactory &);
	BoxFactory & operator =(BoxFactory &&);

public:
    //! Gets the instance of a class.
    static BoxFactory & instance();

public:
    //! Performs parsing of a box from a stream.
    /*!
     * \param stream input stream
     * \param parent parent container, if any
     * \return if the stream contains any further data
     */
    bool parseBox(LimitedStreamReader & stream, ChildrenMixin * parent = nullptr);

public slots:
    //! This slot is called, if a mandatory box defined as 'exactly one' or 'one or more' is missing.
    /*!
     * \param source parent box
     * \param four_cc FourCC code of a missing box
     */
    void onMandatoryBoxIsMissing(Box * source, FourCC four_cc);
    //! This slot is called, if no mandatory boxes defined as 'exactly one of' are missing.
    /*!
     * \param source parent box
     * \param four_cc_codes list of FourCC code of missing boxes
     */
    void onMandatoryBoxesAreMissing(Box * source, QList<FourCC> four_cc_codes);
    //! This slot is called, if the count of boxes defined as 'zero or one', 'exactly one' or 'exactly one of' is exceeding.
    /*!
     * \param source parent box
     * \param boxes list of boxes which are conflicting
     */
    void onBoxCountIsExceeding(Box * source, QList<Box *> boxes);
    //! This slot is called, if unexpected boxes are met in a source box.
    /*!
     * \param source parent box
     * \param boxes list of boxes which are unexpected
     */
    void onUnexpectedBoxesMet(Box * source, QList<Box *> boxes);

signals:
    //! This signal is sent, after the box was created.
    void boxCreated(Box * box);

private:
    //! Registers a creator functions for a box, that can be identified either by FourCC code.
    template<typename TBoxType>
    inline typename std::enable_if< has_four_cc<TBoxType>::value, void >::type registerCreator()
    {
        FourCC four_cc = TBoxType::getFourCC();
        if(m_creators.contains(four_cc))
        {
            qCritical() << "A box type with FourCC =" << four_cc << "is already registered in the factory.";
        }
        m_creators.insert(four_cc, [] (ChildrenMixin * parent) -> Box * { return new TBoxType(parent); } );
    }

    //! Creates the box, identified by the FourCC code.
    /*!
     * \param four_cc FourCC code, identifying the box.
     * \param parent parent container, if any
     * \return created box, if the FourCC code corresponds any registered creator function, or UnknownBox, if not
     */
    Box * createBox(const FourCC & four_cc, ChildrenMixin * parent = nullptr);

private:
    //! Creator functions mapping for the boxes identified by FourCC codes
    CreatorMap m_creators;
    //!
    size_t m_debug_tab_count;
};

#endif // BOX_FACTORY_H
