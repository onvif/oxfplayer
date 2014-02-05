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

#ifndef VALIDATOROXF_H
#define VALIDATOROXF_H

#include "crosscompilation_cxx11.h"

#include <QObject>
#include "basic/box.h"

//! This structure contains the information about a file validity according to Onvif export file standard.
struct OXFFileInformation
{
    OXFFileInformation()
        : m_surveillance_export_box_count(0)
        , m_signature_box_count(0)
    {

    }

    //! Checks if a file is treated as valid Onvif export file.
    bool isValid() const
    {
        return (m_surveillance_export_box_count == 1)
                && (m_signature_box_count > 0)
                && (m_certificate_box_count == m_signature_box_count);
    }

    //! 'suep' box count. Should be at exactly 1.
    ushort m_surveillance_export_box_count;
    //! 'sibo' box count. Should be at least 1.
    ushort m_signature_box_count;
    //! 'cert' box count. Should be the same as signature box count.
    ushort m_certificate_box_count;
};

//! This class checks the validity of the files according to Onvif export file standard.
class ValidatorOXF : public QObject
{
    Q_OBJECT
public:
    explicit ValidatorOXF(QObject *parent = 0);

public:
    //! Checks if the fileset consists of a single valid file.
    bool isValidFile();
    //! Checks if the fileset consists of valid files.
    bool isValidFileset() const;

signals:
    
public slots:
    //! This slot is called when the fileset information is cleared.
    void onContentsCleared();
    //! This slot is called when the file is being added to a fileset.
    void onFileOpened(QString path);
    //! This slot is called when the file parsing was finished.
    void onFileClosed();
    //! This slot is called when the box is created.
    void onBoxCreated(Box * box);

private:
    //! Fileset validity information.
    QMap< QString, OXFFileInformation > m_fileset_information;
    //! Current file being parsed.
    QString m_current_file;
};

#endif // VALIDATOROXF_H
