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

#include "validatorOXF.h"

#include "certificateBox.hpp"
#include "signatureBox.hpp"
#include "surveillanceExportBox.hpp"

ValidatorOXF::ValidatorOXF(QObject *parent) :
    QObject(parent)
{
}

bool ValidatorOXF::isValidFile()
{
    return ((m_fileset_information.size() == 1) && (m_fileset_information.begin()->isValid()));
}

bool ValidatorOXF::isValidFileset() const
{
    for(auto it = m_fileset_information.begin(), end = m_fileset_information.end(); it != end; ++it)
    {
		const OXFFileInformation & info = *it;
        if(info.isValid() == false)
            return false;
    }
    return (m_fileset_information.isEmpty() == false);
}

void ValidatorOXF::onContentsCleared()
{
    m_fileset_information.clear();
}

void ValidatorOXF::onFileOpened(QString path)
{
    m_fileset_information.insert(path, OXFFileInformation());
    m_current_file = path;
}

void ValidatorOXF::onFileClosed()
{
    m_current_file.clear();
}

void ValidatorOXF::onBoxCreated(Box *box)
{
    static const FourCC sc_sibo_four_cc(SignatureBox::getFourCC())
            , sc_cert_four_cc(CertificateBox::getFourCC())
            , sc_suep_four_cc(SurveillanceExportBox::getFourCC());

    FourCC four_cc = box->getBoxFourCC();

    if(!m_current_file.isEmpty())
    {
        if(four_cc == sc_sibo_four_cc)
        {
            m_fileset_information[m_current_file].m_signature_box_count++;
        }
        else if(four_cc == sc_cert_four_cc)
        {
            m_fileset_information[m_current_file].m_certificate_box_count++;
        }
        else if(four_cc == sc_suep_four_cc)
        {
            m_fileset_information[m_current_file].m_surveillance_export_box_count++;
        }
    }
}
