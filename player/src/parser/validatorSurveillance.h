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

#ifndef VALIDATORSURVEILLANCE_H
#define VALIDATORSURVEILLANCE_H

#include "crosscompilation_cxx11.h"

#include <QObject>
#include "basic/box.h"

//! Surviellance conformance type
enum SurveillanceConformanceType
{
    SurveillanceConformant,
    IsNotSurveillanceFormat,
    IsNotSurveillanceFileset
};


//! Contains the information about a file validity according to surveillance application standard.
struct SurviellanceFileInformation
{
    //! Surveillance fragment type
    enum FragmentType
    {
        Undefined = 0x00,
        HasPredecessor = 0x01,
        HasSuccessor = 0x02,
        IsStartFragment = 0x10,
        IsFinalFragment = 0x20,
        IsSurveillance = 0x100,
        StartFragment = IsSurveillance | IsStartFragment | HasSuccessor,
        FinalFragment = IsSurveillance | IsFinalFragment | HasPredecessor,
        MiddleFragment = IsSurveillance | HasSuccessor | HasPredecessor,
        StandaloneFragment = IsSurveillance | IsStartFragment | IsFinalFragment,
    };

    SurviellanceFileInformation()
        : m_af_identification_box_count(0)
        , m_fragment_type(Undefined)
    {

    }

    //! Checks if a file is treated as valid surviellance application file.
    bool isValid() const
    {
        return (m_af_identification_box_count == 1) && (m_fragment_type != Undefined);
    }

    //! 'sumi' box count. Should be exactly 1.
    ushort m_af_identification_box_count;
    /*!
     * Fragment type.
     * Should be either StartFragment or FinalFragment or MiddleFragment or StandaloneFragment.
     * The sequence should contain at least one StartFragment and one FinalFragment.
     * One file should be StandaloneFragment.
     */
    FragmentType m_fragment_type;
    //! UUID of predecessor fragment.
    QUuid m_predecessor_UUID;
    //! UUID of current fragment.
    QUuid m_fragment_UUID;
    //! UUID of successor fragment.
    QUuid m_successor_UUID;
};

//! This class checks the validity of the files according to surviellance application standard.
class ValidatorSurveillance : public QObject
{
    Q_OBJECT
public:
    explicit ValidatorSurveillance(QObject *parent = 0);

public:
    //! Checks if the fileset consists of a single valid file.
    SurveillanceConformanceType isValidFile();
    //! Checks if the fileset consists of valid files.
    SurveillanceConformanceType isValidFileset();

signals:

public slots:
    //! This slot is called when the fileset information is cleared.
    void onContentsCleared();
    //! This slot is called when the file is being added to a fileset.
    void onFileOpened(QString path);
    //! This slot is called when the file parsing was finished.
    void onFileClosed();
    //! This slot is called when the box is created.
    void onBoxCreated(Box *box);


private:
    //! Fileset validity information.
    QMap< QString, SurviellanceFileInformation > m_fileset_information;
    //! Current file being parsed.
    QString m_current_file;
};

#endif // VALIDATORSURVIELLANCE_H
