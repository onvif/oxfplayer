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

#ifndef SIGNINGINFORMATION_H
#define SIGNINGINFORMATION_H

#include <QString>
#include <QByteArray>

#include "signatureBox.hpp"

//! Class describing one particular sign.
/*!
 * \brief Contains oinformation about one signing. \n
 */
class SigningInformation
{
public:
    SigningInformation(QByteArray certificate, QByteArray algorithm_id, QString user_information, SignatureBox * signature_box)
        : m_certificate(certificate)
        , m_algorithm_id(algorithm_id)
        , m_user_information(user_information)
        , m_signature_box(signature_box)
    {}


public:
    SignatureBox* getSignatureBox() const        { return m_signature_box; }
    QByteArray    getCertificate(void) const     { return m_certificate; }
    QByteArray    getSignData(void) const        { return m_signature_box->getSignature(); }
    QByteArray    getAlgorithmId(void) const     { return m_algorithm_id; }
    QString       getUserInformation(void) const { return m_user_information; }

    //Size information getters and setters
    int64_t     getTotalSize(void) const         { return m_signature_box->getTotalSize(); }

    int64_t     getOffsetToSignBox(void) const   { return m_signature_box->getBoxOffset(); }

    int64_t     getSignatureSize(void) const     { return m_signature_box->getSignature().size(); }

private:
    QByteArray    m_certificate;
    QByteArray    m_algorithm_id;
    QString       m_user_information;
    SignatureBox* m_signature_box;
};

#endif // SIGNINGINFORMATION_H
