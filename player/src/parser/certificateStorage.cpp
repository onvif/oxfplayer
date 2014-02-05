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

#include "certificateStorage.h"

#include <QDir>

#include "defines.h"

CertificateStorage::CertificateStorage()
{
    update();
}

CertificateStorage::~CertificateStorage()
{

}

QString CertificateStorage::getCertificateFolder()
{
    QString certificates_folder;
#ifdef WIN32
    certificates_folder = QDir::homePath() + WINP_APP_DATA_ROAMING + COMPANY_NAME + "/" + PRODUCT_NAME + "/" + CERTIFICATES_FOLDER;
#endif //WIN32
#ifdef UNIX
    certificates_folder = QDir::homePath() + "/." + PRODUCT_NAME + "/" + CERTIFICATES_FOLDER;
#endif //UNIX

    //create it if needed
    if(!QDir().exists(certificates_folder))
        QDir().mkpath(certificates_folder);

    return certificates_folder;
}

void CertificateStorage::update()
{
    m_files.clear();
    QDir certificate_dir(getCertificateFolder());
    m_files = certificate_dir.entryInfoList(QStringList(BINARY_FILTER), QDir::Files | QDir::Readable, QDir::Name);
}

void CertificateStorage::removeCertificate(int index)
{
    if(index < 0 ||
       index >= m_files.size())
        return;
    QFileInfo file_info = m_files.takeAt(index);
    QFile::remove(file_info.absoluteFilePath());
}

bool CertificateStorage::isCertificateKnown(const QByteArray& binary_certificate)
{
    for(QFileInfoList::const_iterator cIter = m_files.constBegin(); cIter != m_files.constEnd(); ++cIter)
    {
        if(cIter->size() == binary_certificate.size())
        {
            QFile file(cIter->absoluteFilePath());
            if(file.open(QIODevice::ReadOnly))
            {
                if(file.readAll() == binary_certificate)
                {
                    file.close();
                    return true;
                }
                file.close();
            }
        }
    }
    return false;
}
