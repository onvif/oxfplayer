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
#include <qinputdialog.h>

#include "defines.h"

#include <openssl/pkcs12.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <map>
#include <string>

CertificateStorage::CertificateStorage(const char *folder)
{
    m_folder = folder;
    update();
}

CertificateStorage::~CertificateStorage()
{

}

QString CertificateStorage::getCertificateFolder()
{
    QString certificates_folder;
#ifdef WIN32
    certificates_folder = QDir::homePath() + WINP_APP_DATA_ROAMING + COMPANY_NAME + "/" + PRODUCT_NAME + "/" + m_folder;
#endif //WIN32
#ifdef UNIX
    certificates_folder = QDir::homePath() + "/." + PRODUCT_NAME + "/" + m_folder;
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
    auto ext = QString(CERT_FILE_EXTENSIONS).split(';');
    m_files = certificate_dir.entryInfoList(ext, QDir::Files | QDir::Readable, QDir::Name);
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

HexArray CertificateStorage::decryptKey(const HexArray& thumbPrint, const HexArray& encryptedKey)
{
    HexArray resp;
    //
    // Lookup cached hashes
    // Note: will fail when a file is replaced and encrypted by different key
    //
    std::map<std::string, bool> files;
    auto hexthumb = QByteArray(thumbPrint).toHex().toStdString().c_str();
    auto hashes = getCertificateFolder() + "/hashes.txt";
    std::string fname;
    if (FILE* fd = fopen(hashes.toStdString().c_str(), "r")) {
        char line[1024] = {};
        while (fgets(line, sizeof(line), fd)) {
            auto del = strchr(line, ' ');
            size_t hlen = del - line, len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') line[len - 1] = 0;    // remove trailing newline
            files[del + 1] = true;
            if (hlen < sizeof(line) && hlen == thumbPrint.size() * 2 && !memcmp(line, hexthumb, hlen)) {
                fname = del + 1;
            }
        }
        fclose(fd);
    }

    EVP_PKEY* pkey = 0;
    PKCS12* p12 = 0;
    if (fname.empty()) {        // update hashes if not found
        //
        // If unkown cert hash search for a new pkcs12 file
        //
        if (FILE* fd = fopen(hashes.toStdString().c_str(), "a+")) {
            for (auto cIter = m_files.constBegin(); cIter != m_files.constEnd(); ++cIter)
            {
                if (files.find(cIter->fileName().toStdString()) == files.end()) {
                    if (auto p12_file = fopen(cIter->absoluteFilePath().toStdString().c_str(), "rb"))
                    {
                        if (p12) PKCS12_free(p12);
                        p12 = d2i_PKCS12_fp(p12_file, 0);
                        fclose(p12_file);

                        bool ok{};
                        QString text = QInputDialog::getText(0, "Password for " + cIter->fileName(),
                            "Password:", QLineEdit::Normal, "", &ok);

                        EVP_PKEY* pk = 0;
                        X509* cert = 0;
                        if (!PKCS12_parse(p12, text.toStdString().c_str(), &pk, &cert, 0)) {
                            char buf[256];
                            fprintf(stdout, "Error parsing PKCS#12 file: %s\n", ERR_error_string(ERR_get_error(), buf));
                        }
                        else {
                            if (cert) {
                                const EVP_MD* md = EVP_sha256();
                                uint8_t sig[64] = {};
                                uint32_t siglen = sizeof(sig);
                                if (X509_digest(cert, md, sig, &siglen)) {
                                    const char* thumb = QByteArray((char*)sig, siglen).toHex().toStdString().c_str();
                                    fprintf(fd, "%s %s\n", thumb, cIter->fileName().toStdString().c_str());
                                    if (siglen == thumbPrint.size() && !memcmp(thumb, thumbPrint.data(), siglen)) {
                                        fname = cIter->fileName().toStdString();
                                        pkey = pk;      // keep key for decryption
                                        pk = 0;
                                    }
                                }
                                X509_free(cert);
                            }
                            if (pk) EVP_PKEY_free(pk);
                        }
                        fclose(p12_file);
                    }
                }
            }
            fclose(fd);
        }
    }

    if (pkey == 0) {
        auto pfx = (getCertificateFolder() + QString("/") + QString(fname.c_str())).toStdString();
        if (auto p12_file = fopen(pfx.c_str(), "rb"))
        {
            auto p12 = d2i_PKCS12_fp(p12_file, 0);

            bool ok{};
            QString text = QInputDialog::getText(0, "Password for " + QString(fname.c_str()),
                "Password:", QLineEdit::Normal, "", &ok);
            X509* cert = 0;
            if (!PKCS12_parse(p12, text.toStdString().c_str(), &pkey, &cert, 0) || pkey == 0) {
                char buf[256];
                fprintf(stdout, "Error parsing PKCS#12 file: %s\n", ERR_error_string(ERR_get_error(), buf));
                return resp;
            }
            fclose(p12_file);
        }
    }

    char buf[256];
    auto ctx = EVP_PKEY_CTX_new(pkey, NULL);
    EVP_PKEY_decrypt_init(ctx);
    EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    if (EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256()) <= 0)
        fprintf(stdout, "Error parsing PKCS#12 file: %s\n", ERR_error_string(ERR_get_error(), buf));
    
    uint8_t buffer[1024] = {};
    size_t bufsiz = sizeof(buffer);
    EVP_PKEY_decrypt(ctx, NULL, &bufsiz, encryptedKey.data(), encryptedKey.size());

    if (bufsiz <= sizeof(buffer) && EVP_PKEY_decrypt(ctx, buffer, &bufsiz, encryptedKey.data(), encryptedKey.size())) {
        resp = HexArray(buffer, bufsiz);
    }
    else {
        fprintf(stdout, "Error parsing PKCS#12 file: %s\n", ERR_error_string(ERR_get_error(), buf));
    }
    if (p12) PKCS12_free(p12);
    EVP_PKEY_CTX_free(ctx);
    if (pkey) EVP_PKEY_free(pkey);

    return resp;
}
