#ifndef CERTIFICATESSLTEST_H
#define CERTIFICATESSLTEST_H

#include <QObject>


class CertificateSSLTest : public QObject
{
    Q_OBJECT

public:
    CertificateSSLTest();

private Q_SLOTS:
    void testCertToPem();
    void testCertProperties();

private:

};


#endif // CERTIFICATESSLTEST_H
