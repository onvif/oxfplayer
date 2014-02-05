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

#ifndef OXFVERIFIER_H
#define OXFVERIFIER_H

#include "crosscompilation_cxx11.h"

#include <QThread>
#include <QIODevice>
#include <QSslCertificate>
#include <QSslKey>
#include <QString>

#include "ONVIFSignInfo.h"

struct evp_pkey_st;

typedef enum _VerificationStatus
{
    vsNA = 0,
    vsFailed,
    vsOK,
    vsCanceled
} VerificationStatus;


/*!  This class performing signature verification for the file
 *   It calculates the hash sum and then verify this hash against signed has from a sibo box from file
 *
 */
class OXFVerifier : public QThread
{
    Q_OBJECT
public:
    explicit OXFVerifier(QObject *parent = 0);

    /*!
     *  Empty Destructor
     */
    virtual ~OXFVerifier() {}
    
    /*!
     * @brief Start operation in separate thread
     */
    void start(void);

    //! prepare for verification
    //!  @param  sInf - signing information obtained from file
    //!  @param  file - file with data
    void        initialize(const SigningInformation* pInf, const QString& file);

    //!  get all information available in certificate
    void        getCertificateInfo(QStringList&  certInfo);

    //!  get just Issuer name
    QString     getCertificateIssuer(void);

    //!  certificate loading from external resource
    bool        loadCertificateFromFile(const QString& certName);
    //!  save certificate from file for late usage
    bool        saveCertificateToFile(const QString& certName);

    //!  get binary certificate content
    QByteArray  getBinaryCertificate();

    void  setCancelOperation(bool val)  { m_cancel_operation = val; }

signals:

    //!  signal would emit right before verification is started, providing estimated number of steps to complete
    void operationStarted(int  steps);

    //!  signal would emit after verification is finished
    void operationCompleted(VerificationStatus);

    //! signal would emit during operation
    void operationRunning(void);
    //void operationRunning(bool* bContinue);

public slots:


private:
    /*!
     * @brief reimplemented function from base class for multiprocessing
     *
     */
    virtual void run (void);

    /*!
     * @brief perform signature verification
     */    
    bool    verifySinature(const QByteArray& hashData);

    //  one of the following method will be used for actual verification
    //  According to ExportFile definition verifySignatureWithPss  will be used in first version
    bool    verifySignatureWithDecrypt(const QByteArray& hashData, evp_pkey_st* pkey);
    bool    verifySignatureWithPss(const QByteArray& hashData, evp_pkey_st* pkey);
    bool    verifySignatureWithDigest(const QByteArray& hashData, evp_pkey_st* pkey);

    //  hash calculation helpers
    bool    calculateCryptoHashRaw(QCryptographicHash&, QIODevice& , qint64 );
    bool    calculateCryptoHashLast(QCryptographicHash& , QIODevice& , qint64 , qint64 );
    bool    calculateCryptoHashOther(QCryptographicHash& , QIODevice& , qint64 , qint64 );

    QSsl::EncodingFormat  getSslFormatFromName(const QString& certName);

private:
    QObject*            m_parent;   //! keep parent for future notification

    QSslCertificate     m_cert;     //! certificate from loaded file
    QSslKey             m_key;      //! public key from certificate

    SignatureBox*       m_sibo_box;  //! point to the box with signature
    QByteArray          m_sign;      //! signature from file

    QString             m_fileName;   //!  file name of vide to check
    bool                m_cancel_operation;  //!  identify that current operation needs to be canceled
};

#endif // OXFVERIFIER_H
