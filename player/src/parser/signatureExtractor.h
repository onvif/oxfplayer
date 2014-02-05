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

#ifndef SIGNATUREEXTRACTOR_H
#define SIGNATUREEXTRACTOR_H

#include "crosscompilation_cxx11.h"

#include <QObject>

#include "basic/box.h"
#include "../common/ONVIFSignInfo.h"
#include "signatureBox.hpp"

//! This structure contains the information about a file signature according to OXF standard.
struct SignatureInfo
{
    SignatureInfo()
        : signature_box(nullptr)
    {

    }

    //! Checks if a signature is valid. Signature is treated as valid, if a SignatureBox is present.
    inline bool isValid()
    {
        return (signature_box != nullptr) && !certificate.isEmpty();
    }

    //! Converts a SignatureInfo into SigningInformation to pass it to a signature verifier.
    operator SigningInformation() const
    {
        return SigningInformation(certificate, algorithm_id, user_information, signature_box);
    }

    //! Certificate of a signging instance, mandatory.
    QByteArray certificate;
    //! Pointer to a signature box, mandatory.
    SignatureBox* signature_box;
    //! Algorithm identifier, optional.
    QByteArray algorithm_id;
    //! User information, optional.
    QString user_information;
};

//! This class extracts the information about certificates and signatures from a fileset.
class SignatureExtractor : public QObject
{
    Q_OBJECT
public:
    explicit SignatureExtractor(QObject *parent = 0);
    
public:
    //! Returns the signature list.
    SigningInformationMap getSignaturesMap() const;

signals:

public slots:
    //! This slot is called when the fileset information is cleared.
    void onContentsCleared();
    //! This slot is called when the file is being added to a fileset.
    void onFileAdded(QString path);
    //! This slot is called when the file parsing was finished.
    void onFileClosed();
    //! This slot is called when the box is created.
    void onBoxCreated(Box *box);

private:
    //! Appends current signature to a list of signatures for a current file.
    void appendSignature();

private:
    //! Signature list of a fileset.
    SigningInformationMap m_signatures_map;
    //! Information about signature being parsed.
    SignatureInfo m_current_signature;
    //! File currently being parsed.
    QString m_current_path;
};

#endif // SIGNATUREEXTRACTOR_H
