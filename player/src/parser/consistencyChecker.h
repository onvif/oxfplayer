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

#ifndef CONSISTENCYCHECKER_H
#define CONSISTENCYCHECKER_H

#include "crosscompilation_cxx11.h"

#include <QObject>

#include <functional>

#include "basic/box.h"
#include "basic/fileBox.hpp"
#include "basic/mixin/children.hpp"

/*!
 * This class performs a consistency checking of created boxes.
 * It verifies, if a box contains all mandatory boxes and does not contain unexpected boxes.
 */
class ConsistencyChecker CC_CXX11_FINAL
        : public QObject
{
    Q_OBJECT

private:
    typedef QMap<FourCC, uint32_t> CounterInfo;
    typedef std::function<void ()> CheckerFunction;
    typedef QMap<FourCC, CheckerFunction> CheckersMap;

public:
    ConsistencyChecker();

public:
    //! Performs a check for a file itself.
    void checkFileBox(FileBox * box);

public slots:
    //! This slot is called, when a box is created and needs to be verified.
    void onBoxCreated(Box * box);

signals:
    //! This signal is sent, if a mandatory box defined as 'exactly one' or 'one or more' is missing.
    /*!
     * \param source parent box
     * \param four_cc FourCC code of a missing box
     */
    void mandatoryBoxIsMissing(Box * source, FourCC four_cc);
    //! This signal is sent, if no mandatory boxes defined as 'exactly one of' are missing.
    /*!
     * \param source parent box
     * \param four_cc_codes list of FourCC code of missing boxes
     */
    void mandatoryBoxesAreMissing(Box * source, QList<FourCC> four_cc_codes);
    //! This signal is sent, if the count of boxes defined as 'zero or one', 'exactly one' or 'exactly one of' is exceeding.
    /*!
     * \param source parent box
     * \param boxes list of boxes which are conflicting
     */
    void boxCountIsExceeding(Box * source, QList<Box *> boxes);
    //! This signal is sent, if unexpected boxes are met in a source box.
    /*!
     * \param source parent box
     * \param boxes list of boxes which are unexpected
     */
    void unexpectedBoxesMet(Box * source, QList<Box *> boxes);

private:
    //! Counts all child boxes of a source box.
    static CounterInfo countBoxes(ChildrenMixin * box);
    //! Retrieves the list of boxes of a specific type.
    static QList<Box *> extractBoxes(ChildrenMixin * box, FourCC four_cc);
    //! Retrieves the list of boxes of specified types.
    static QList<Box *> extractBoxes(ChildrenMixin * box, QList<FourCC> four_cc_codes);

    //! Registers a checker function for a box type.
    template<typename TBoxType>
    inline void registerChecker(CheckerFunction checker)
    {
        m_checkers_map[TBoxType::getFourCC()] = checker;
    }

    //! Performs no checks, allowing any count of specified box type to be present. Describes 'zero or more' clause.
    template<typename TBoxType>
    inline void anyCountCheck()
    {
        m_counter_info.remove(TBoxType::getFourCC());
    }

    //! Performs check, allowing only one box of specified box type to be present. Describes 'exactly one' clause.
    template<typename TBoxType>
    inline void exactlyOneCheck()
    {
        FourCC four_cc = TBoxType::getFourCC();
        uint32_t count = m_counter_info[four_cc];
        if(count == 0)
        {
            emit mandatoryBoxIsMissing(m_current_box, four_cc);
        }
        else if(count > 1)
        {
            emit boxCountIsExceeding(m_current_box, extractBoxes(dynamic_cast<ChildrenMixin*>(m_current_box), four_cc));
        }
        m_counter_info.remove(four_cc);
    }

    //! Performs check, allowing one or more boxes of specified box type to be present. Describes 'one or more' clause.
    template<typename TBoxType>
    inline void oneOrMoreCheck()
    {
        FourCC four_cc = TBoxType::getFourCC();
        uint32_t count = m_counter_info[four_cc];
        if(count == 0)
        {
            emit mandatoryBoxIsMissing(m_current_box, four_cc);
        }
        m_counter_info.remove(four_cc);
    }

    //! Performs check, allowing zero or one box of specified box type to be present. Describes 'zero or one' clause.
    template<typename TBoxType>
    inline void zeroOrOneCheck()
    {
        FourCC four_cc = TBoxType::getFourCC();
        uint32_t count = m_counter_info[four_cc];
        if(count > 1)
        {
            emit boxCountIsExceeding(m_current_box, extractBoxes(dynamic_cast<ChildrenMixin*>(m_current_box), four_cc));
        }
        m_counter_info.remove(four_cc);
    }

    //! Performs check, allowing only one box of specified box type list to be present. Describes 'exactly one of' clause.
    void exactlyOneOfCheck(QList<FourCC> four_cc_codes)
    {
        uint32_t count = 0;
        for(auto it = four_cc_codes.begin(), end = four_cc_codes.end(); it != end; ++it)
        {
			auto & four_cc = *it;
            count += m_counter_info[four_cc];
        }
        if(count == 0)
        {
            emit mandatoryBoxesAreMissing(m_current_box, four_cc_codes);
        }
        else if(count > 1)
        {
            emit boxCountIsExceeding(m_current_box, extractBoxes(dynamic_cast<ChildrenMixin*>(m_current_box), four_cc_codes));
        }
        for(auto it = four_cc_codes.begin(), end = four_cc_codes.end(); it != end; ++it)
        {
			auto & four_cc = *it;
            m_counter_info.remove(four_cc);
        }
    }

    //! Performs no checks, allowing any count of FreeSpaceBox or SkipBox to be present.
    void freeBoxesCheck();
    //! Performs no checks, allowing to store any boxes with any hierarchy within UserDataBox boxes. Returns true if a box is contained in UserDataBox.
    bool udtaBoxesCheck();
    //! Performs a check listing all boxes, that were not checked by previous checks to be treated as unexpected.
    void markUnexpectedBoxes();

private:
    //! Map of a checker functions.
    CheckersMap m_checkers_map;
    //! Count of all box types within current box.
    CounterInfo m_counter_info;
    //! Current box, being checked.
    Box * m_current_box;
};

#endif // CONSISTENCYCHECKER_H
