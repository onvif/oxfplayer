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

#include "signatureExtractor.h"

#include "templateSuperBoxes.hpp"
#include "certificateBox.hpp"
#include "signatureConfigurationBox.hpp"
#include "additionalUserInformation.hpp"

SignatureExtractor::SignatureExtractor(QObject *parent) :
    QObject(parent)
{
}

SigningInformationMap SignatureExtractor::getSignaturesMap() const
{
    return m_signatures_map;
}

void SignatureExtractor::onContentsCleared()
{
    m_signatures_map.clear();
}

void SignatureExtractor::onFileAdded(QString path)
{
    m_signatures_map.insert(path, ONVIFSigningInformation());
    m_current_path = path;
}

void SignatureExtractor::onFileClosed()
{
    if(!m_current_path.isEmpty() && m_current_signature.isValid())
    {
        appendSignature();
    }
    m_current_path.clear();
}

void SignatureExtractor::onBoxCreated(Box *box)
{
    static const FourCC sc_sinf_four_cc(ProtectionSchemeInfoBox::getFourCC()),
            sc_cert_four_cc(CertificateBox::getFourCC()),
            sc_sibo_four_cc(SignatureBox::getFourCC()),
            sc_sigC_four_cc(SignatureConfigurationBox::getFourCC()),
            sc_auib_four_cc(AdditionalUserInformationBox::getFourCC());

    FourCC four_cc = box->getBoxFourCC();

    if(four_cc == sc_sinf_four_cc)
    {
        if(m_current_signature.isValid())
        {
            appendSignature();
        }
    }
    else if(four_cc == sc_cert_four_cc)
    {
        CertificateBox * certificate_box = dynamic_cast<CertificateBox*>(box);
        m_current_signature.certificate = certificate_box->getCertificateData();
    }
    else if(four_cc == sc_sibo_four_cc)
    {
        m_current_signature.signature_box = dynamic_cast<SignatureBox*>(box);
    }
    else if(four_cc == sc_sigC_four_cc)
    {
        SignatureConfigurationBox * signature_configuration_box = dynamic_cast<SignatureConfigurationBox*>(box);
        m_current_signature.algorithm_id = signature_configuration_box->getAlgorithmIdentifier();
    }
    else if(four_cc == sc_auib_four_cc)
    {
        AdditionalUserInformationBox * additional_user_information_box = dynamic_cast<AdditionalUserInformationBox*>(box);
        m_current_signature.user_information = additional_user_information_box->getUserInformation();
    }
}

void SignatureExtractor::appendSignature()
{
    m_signatures_map[m_current_path].addSignature((SigningInformation)m_current_signature);
    m_current_signature = SignatureInfo();
}
