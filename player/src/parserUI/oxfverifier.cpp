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

#include "crosscompilation_cxx11.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include "oxfverifier.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

static const qint64 cMaxLen = 1024*8;

QString toDebug(const char * line, int size)
{
    QString s;
    uchar   c;

    for ( int i=0; i < size; i++)
    {
        c = *(line + i);
        s.append(QString("%1").arg(c, 2, 16, QChar('0')));
    }
    return s;
}

QString toDebug(const QByteArray & line)
{
    return toDebug((const char *)line.constData(), line.length());
}

// ctor
OXFVerifier::OXFVerifier(QObject *parent)
            : QThread()
{
    m_cancel_operation = false;
    m_parent = parent;
    m_sibo_box = nullptr;
}

void  OXFVerifier::initialize(const SigningInformation* pInf, const QString& file)
{
    m_cert.clear();

    QByteArray data = pInf->getCertificate();
    QList<QSslCertificate>  s = QSslCertificate::fromData(data, QSsl::Der);
    if (s.length() != 0)
       m_cert = s.at(0);
    else
       m_cert = QSslCertificate();

    m_fileName = file;
    m_sibo_box = pInf->getSignatureBox();
}

/* starts execution in thread*/
void OXFVerifier::start(void)
{
    m_cancel_operation = false;
    QThread::start();
    //run();
}

/*!  Calculate the cryptographic hash till the stop mark
 *   @param sh - cryptographic hash object
 *   @param inp - input file
 *   @param stop - where we need to stop calculation
 */
bool OXFVerifier::calculateCryptoHashRaw(QCryptographicHash& sh, QIODevice& inp, qint64 stop)
{
    qint64  actualRead = 1;
    qint64  bytesCalculated = 0;
    qint64  bytesToRead = 0;

    char*  buffer = new char[cMaxLen];
    while ((!m_cancel_operation) && (actualRead != 0))
    {
        // how many bytes we need to read
        bytesToRead = std::min<qint64>(cMaxLen, (stop - bytesCalculated));
        // what we actualy read
        actualRead = inp.read(buffer, bytesToRead);
        // count them
        bytesCalculated += actualRead;

        // add to hash
        sh.addData(buffer, actualRead);

        emit operationRunning();
        if (m_cancel_operation)
        {
            qDebug() << "Operation has been canceled";
            delete[] buffer;
            return false;
        }
    } // end of while (reading)

    delete[] buffer;
    return true;
}

/*!  Calculate the cryptographic hash for the meta box till the end of file
 *   @param sh - cryptographic hash object
 *   @param inp - input file
 *   @param start - where we need to start calculation
 *   @param size - how many bytes to calculate
 */
bool OXFVerifier::calculateCryptoHashLast(QCryptographicHash& sh, QIODevice& inp, qint64 start, qint64 size)
{
    qint64  actualRead = 1;
    qint64  bytesToRead = size;

    char*  buffer = new char[bytesToRead];

    while ((!m_cancel_operation) && (actualRead != 0))
    {
        // what we actualy read
        actualRead = inp.read(buffer, bytesToRead);

        // we need to zero the signature
        qint64 signatureOffset =  m_sibo_box->getBoxOffset() + 8;
        qint64  shift = signatureOffset - start;

        memset(buffer + shift, 0, m_sibo_box->getSignature().size());

        // add to hash
        sh.addData(buffer, actualRead);

        emit operationRunning();
        if (m_cancel_operation)
        {
            qDebug() << "Operation has been canceled";
            delete[] buffer;
            return false;
        }
    } // end of while (reading)

    delete[] buffer;
    return true;
}

/*!  Calculate the cryptographic hash for the meta box till the end of corresponding sinf box
 *   @param sh - cryptographic hash object
 *   @param inp - input file
 *   @param start - where we need to start calculation
 *   @param end - where to stop
 */
bool OXFVerifier::calculateCryptoHashOther(QCryptographicHash& sh, QIODevice& inp, qint64 start, qint64 end)
{
    qint64  actualRead = 1;
    qint64  bytesToRead = end - start;

    char*  buffer = new char[bytesToRead];

    // what we actualy read
    actualRead = inp.read(buffer, bytesToRead);

    // we need to zero the signature
    qint64 signatureOffset =  m_sibo_box->getBoxOffset() + 8;
    qint64  shift = signatureOffset - start;

    memset(buffer + shift, 0, m_sibo_box->getSignature().size());

    // and also we nned to adjust meta and ipro boxes
    Box* pIpro = m_sibo_box->getParent()->getParent()->getParent();
    qint64 iproOffset = pIpro->getBoxOffset();
    int newsize = end - iproOffset;
    newsize =qToBigEndian(newsize);

    shift = iproOffset - start;
    *(int*)((char*)(buffer + shift)) = newsize;

    Box* pMeta = pIpro->getParent();
    qint64 metaOffset = pMeta->getBoxOffset();
    newsize = end - metaOffset;
    newsize = qToBigEndian(newsize);

    shift = metaOffset - start;
    *(int*)((char*)(buffer + shift)) = newsize;

    // add to hash
    sh.addData(buffer, actualRead);

    emit operationRunning();
    if (m_cancel_operation)
    {
        qDebug() << "Operation has been canceled";
        delete[] buffer;
        return false;
    }

    delete[] buffer;
    return true;
}


// thread proc
void OXFVerifier::run()
{
    VerificationStatus  st = vsNA;
    QFile       inp(m_fileName);
    QFileInfo   iff(inp);

    if (!inp.open(QIODevice::ReadOnly))
    {
        emit operationCompleted(vsFailed);
        return;
    }

    qint64      fileSize = iff.size();
    int         steps = fileSize/cMaxLen + 15;

    emit operationStarted(steps);

    QCryptographicHash  sh(QCryptographicHash::Sha256);
    Box*    pMeta =  m_sibo_box->getParent()->getParent()->getParent()->getParent();
    qint64  firstStop = pMeta->getBoxOffset();

    //  calculate bytes till meta box first
    bool bRes = calculateCryptoHashRaw(sh, inp, firstStop);
    if (bRes)
    {
        // ok, the most interesting part
        auto total_size = m_sibo_box->getTotalSize();
        if (total_size < (firstStop + pMeta->getBoxSize()) )
        {
            bRes = calculateCryptoHashOther(sh, inp, firstStop, total_size);
        }
        else
        {
            bRes = calculateCryptoHashLast(sh, inp, firstStop, pMeta->getBoxSize());
        }
    }

    if (!bRes)
    {
        inp.close();
        emit operationCompleted(vsCanceled);
        return;
    }

    // ok we have got hash
    emit operationRunning();
    if ((!m_cancel_operation))
    {
        QByteArray data = sh.result();
        m_sign = m_sibo_box->getSignature();

        // and now we are redy to verify the signature
        bool res = verifySinature(data);
        st = res ? vsOK : vsFailed;

        emit operationCompleted(st);
        return;
    }
    emit operationCompleted(vsCanceled);
    return;
}

/*!  Perform signature validation. Certificate has to be loaded and valid at this point.
 *    public key will be taken from certificate.
 *   @param hashData - binary data of cryptographic hash
 */
bool OXFVerifier::verifySinature(const QByteArray& hashData)
{
    // first of all make sure the OpenSSL is initialized
    // some stuff is prepared in Qt, so may be we don't tneed all init here
    OpenSSL_add_all_algorithms();   // !! IMPORTANT
    OpenSSL_add_all_digests();
    SSL_library_init();

    // get the public key
    QSslKey    key = m_cert.publicKey();   // get key
    EVP_PKEY*  pkey = EVP_PKEY_new();

    if (key.algorithm() == QSsl::Rsa)
        EVP_PKEY_assign_RSA(pkey, (RSA *)key.handle());
    else
        EVP_PKEY_assign_DSA(pkey, (DSA *)key.handle());

    while ( 0 != ERR_get_error() ); // clean up error queue

    // and not let check signature
    // we have several variants on how to perform this
    // 1. decrypt data with public key and compare hashes
    return verifySignatureWithDecrypt(hashData, pkey);

    //  OID 1.2.840.113549.1.1.10 corresponds to RSASSA-PSS
    //  unfortunately it is not clear how it is implemented in openssl RSASSA-PKCS1-V1_5 or RSASSA-PSS
    //  The low-level functions RSA_padding_add_PKCS1_PSS and  RSA_verify_PKCS1_PSS should be used
    //   let try to verify with these functions
    //return verifySignatureWithPss(hashData, pkey);

    // verify via digest with public key
    //return verifySignatureWithDigest(hashData, pkey);
}


/*  experimental fuinction. Trying to check signature with PSS functionality of openSSL
*/
bool OXFVerifier::verifySignatureWithPss(const QByteArray& hashData, EVP_PKEY* pkey)
{
    RSA* pRsaKey = EVP_PKEY_get1_RSA(pkey);

    int keysize = RSA_size(pRsaKey);
    int rsa_inlen = m_sign.length();  // what we got from parser
    int status = 0;

    unsigned char * rsa_out = (unsigned char*)OPENSSL_malloc(keysize);

    /* now we will verify the signature
       Start by a RAW decrypt of the signature
    */
    int rsa_outlen = RSA_public_decrypt(rsa_inlen/*keysize*/, (unsigned char*)m_sign.data(), rsa_out, pRsaKey, RSA_NO_PADDING /*RSA_PKCS1_PADDING*/);
    int err;
    if (rsa_outlen <= 0)
    {
        while ( 0 != (err = ERR_get_error())  )
            qDebug() << "decrypt ERROR: " << ERR_error_string(ERR_get_error(),NULL);
    }
    else
    {
        qDebug() << "hash sum from signature in file : " << toDebug((const char *)rsa_out, rsa_outlen);
        qDebug() << "hash calculated                 : " << toDebug(hashData);

        //qDebug() << "signarure : " << toDebug(m_sign);

        /* verify the data */
        status = RSA_verify_PKCS1_PSS(pRsaKey, (const unsigned char*)hashData.data(), EVP_sha256(), rsa_out, -2 /* salt length recovered from signature*/);
        if (status == 1)
        {
           qDebug() << "Signature verification successfull!";
        }
        else
        {
           qDebug() << "RSA_verify_PKCS1_PSS failed with error " << ERR_error_string(ERR_get_error(), NULL);
        }
    }

    RSA_free(pRsaKey);
    if (rsa_out)
        OPENSSL_free(rsa_out);

    CRYPTO_cleanup_all_ex_data();
    EVP_cleanup();
    return (status == 1) ? true : false;
}


/*!  Perform signature extraction from a signing data. Compare the calcu;lated hash with decrypted.
 *   @param hashData - binary data of cryptographic hash
 *   @param pkey - public key
 */
bool OXFVerifier::verifySignatureWithDecrypt(const QByteArray& hashData, EVP_PKEY* pkey)
{
    // get the RSA key
    RSA* rsa = EVP_PKEY_get1_RSA(pkey);

    int keysize = RSA_size(rsa);
    int rsa_inlen = m_sign.length();  // what we got from parser

    unsigned char * rsa_out = (unsigned char*)OPENSSL_malloc(keysize);

    int rsa_outlen = RSA_public_decrypt(rsa_inlen/*keysize*/, (unsigned char*)m_sign.data(), rsa_out, rsa, RSA_PKCS1_PADDING);
    int err;
    if (rsa_outlen <= 0)
    {
        while ( 0 != (err = ERR_get_error())  )
            qDebug() << "decrypt ERROR: " << ERR_error_string(ERR_get_error(),NULL);
    }

#ifdef _DEBUG
    qDebug() << "hash sum from signature in file : " << toDebug((const char *)rsa_out, rsa_outlen);
    qDebug() << "hash calculated                 : " << toDebug(hashData);

    qDebug() << "signarure : " << toDebug(m_sign);
#endif
    int evpRes = 1;

    // hack
    if (rsa_outlen == 47)
    {
        evpRes = memcmp(rsa_out + 15, (const char*)hashData,  hashData.length());
    }
    else
    {
        if (rsa_outlen == hashData.length())
            evpRes = memcmp(rsa_out, (const char*)hashData,  rsa_outlen);
    }

    RSA_free(rsa);
    if (rsa_out)
        OPENSSL_free(rsa_out);

    return (evpRes == 0) ? true : false;
}

// one more experimental functionality
bool OXFVerifier::verifySignatureWithDigest(const QByteArray& hashData, EVP_PKEY* pkey)
{
    // EVP context initialization
    EVP_MD_CTX  ctx;
    EVP_MD_CTX_init(&ctx);

    int keysize = EVP_PKEY_size(pkey);
    int rsa_inlen = m_sign.length();  // what we got from parser
    if (keysize != rsa_inlen)
        qDebug() << "Size from EVP_PKEY_size is not equal to signature in file.";

    //EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new(pkey, nullptr);

    int evpRes = 0;
    int res1 = EVP_DigestVerifyInit(&ctx, /*&pctx*/nullptr, EVP_sha256(), nullptr, pkey);
    int res2 = EVP_DigestVerifyUpdate(&ctx, hashData.data(), hashData.length());
    if ( (1 == res1) && (1 == res2))
    {
        evpRes = EVP_DigestVerifyFinal(&ctx, (unsigned char*)m_sign.data(), rsa_inlen);
        if (1 != evpRes)
        {
            ERR_load_crypto_strings();
            qDebug() << "verify ERROR: " << ERR_error_string(ERR_get_error(),NULL);
        }
    }
    else
    {
        ERR_load_crypto_strings();
        qDebug() << "EVP_Digest ERROR: " << ERR_error_string(ERR_get_error(),NULL);
    }

    EVP_MD_CTX_cleanup(&ctx);
    return (evpRes != 0) ? true : false;
}

/*!  Return information about certificate issuer
 *   @return  issuer
 */
QString  OXFVerifier::getCertificateIssuer(void)
{
    if (m_cert.isNull())
        return tr("Certificate is not valid.");

    QStringList  res;

    res << m_cert.issuerInfo(QSslCertificate::CommonName).join(QLatin1Char(' '));
    res << m_cert.issuerInfo(QSslCertificate::Organization).join(QLatin1Char(' '));
    return res.join(' ');
}

/*!  load certificate from a file system
 *   @param  certName - certificate file name
 *   @return  result
 */
bool  OXFVerifier::loadCertificateFromFile(const QString& certName)
{
    QSsl::EncodingFormat  fmt = getSslFormatFromName(certName);

    QFile file(certName);
    if (!file.open(QIODevice::ReadOnly))
         return false;

    QByteArray  array = file.readAll();
    file.close();

    QList<QSslCertificate>  s = QSslCertificate::fromData(array, fmt);
    if (s.length() != 0)
       m_cert = s.at(0);
    else
       m_cert = QSslCertificate();

    return true;
}

/*!  Save existing certificate to file
 *   @param  certName - certificate file name to save
 *   @return  result
 */
bool  OXFVerifier::saveCertificateToFile(const QString& certName)
{
    QByteArray  data;
    QSsl::EncodingFormat inFmt = getSslFormatFromName(certName);
    if (inFmt == QSsl::Der)
        data = m_cert.toDer();
    else
        data = m_cert.toPem();

    QFile file(certName);
    if (!file.open(QIODevice::WriteOnly))
         return false;

    file.write(data);

    file.flush();
    file.close();
    return true;
}

/*!  Get binary certificate content
 */
QByteArray OXFVerifier::getBinaryCertificate()
{
    return m_cert.toDer();
}

/*!  Parse file name and get certificate format (binary or text with base64 encoding)
 *   @param  certName - certificate file name to save
 *   @return  format
 */
QSsl::EncodingFormat  OXFVerifier::getSslFormatFromName(const QString& certName)
{
    QSsl::EncodingFormat inFmt;

    QFileInfo  fi(certName);
    if ( fi.suffix().compare(QString("pem"), Qt::CaseInsensitive) == 0)
    {
        inFmt = QSsl::Pem;
    }
    else if ( fi.suffix().compare(QString("der"), Qt::CaseInsensitive) == 0 )
    {
        inFmt = QSsl::Der;
    }
    else
    {
        inFmt = QSsl::Pem;
        qDebug() << "Unknown file extention. trying PEM";
    }
    return inFmt;
}

/*!  Returns complete information from certificate
 *   @param  certInfo - this list will be populated with information
 *   @return  none
 */
void OXFVerifier::getCertificateInfo(QStringList&  certInfo)
{
    if (m_cert.isNull())
    {
        certInfo.append(tr("Not valid certificate."));
        return;
    }

    qDebug() << m_cert.toText();

    certInfo <<  tr("Certificate version: ") << m_cert.version();
    certInfo << tr("Organization: %1").arg(m_cert.subjectInfo(QSslCertificate::Organization).join(QLatin1Char(' ')))
            << tr("Subunit: %1").arg(m_cert.subjectInfo(QSslCertificate::OrganizationalUnitName).join(QLatin1Char(' ')))
            << tr("Country: %1").arg(m_cert.subjectInfo(QSslCertificate::CountryName).join(QLatin1Char(' ')))
            << tr("Locality: %1").arg(m_cert.subjectInfo(QSslCertificate::LocalityName).join(QLatin1Char(' ')))
            << tr("State/Province: %1").arg(m_cert.subjectInfo(QSslCertificate::StateOrProvinceName).join(QLatin1Char(' ')))
            << tr("Common Name: %1").arg(m_cert.subjectInfo(QSslCertificate::CommonName).join(QLatin1Char(' ')))
            << QString()
            << tr("Issuer Organization: %1").arg(m_cert.issuerInfo(QSslCertificate::Organization).join(QLatin1Char(' ')))
            << tr("Issuer Unit Name: %1").arg(m_cert.issuerInfo(QSslCertificate::OrganizationalUnitName).join(QLatin1Char(' ')))
            << tr("Issuer Country: %1").arg(m_cert.issuerInfo(QSslCertificate::CountryName).join(QLatin1Char(' ')))
            << tr("Issuer Locality: %1").arg(m_cert.issuerInfo(QSslCertificate::LocalityName).join(QLatin1Char(' ')))
            << tr("Issuer State/Province: %1").arg(m_cert.issuerInfo(QSslCertificate::StateOrProvinceName).join(QLatin1Char(' ')))
            << tr("Issuer Common Name: %1").arg(m_cert.issuerInfo(QSslCertificate::CommonName).join(QLatin1Char(' ')));

    QDateTime beginDate = m_cert.effectiveDate();
    QDateTime endDate = m_cert.expiryDate();

    certInfo << QString("Certeficate Start date: %1").arg(beginDate.toString());
    certInfo << QString("Certeficate End date: %1").arg(endDate.toString());

    QSslKey  signKey = m_cert.publicKey();
    if (QSsl::Rsa == signKey.algorithm())
        certInfo << tr("RSA key in certificate.");
    else
        certInfo << tr("DSA key in certificate.");

    if ( QSsl::PublicKey == signKey.type())
        certInfo << tr("Public key in certificate.");
    else
        certInfo << tr("Private key in certificate.");

    return;
}
