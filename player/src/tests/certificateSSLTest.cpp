#include <QtTest>

#include <QString>
#include <QDateTime>
#include <QList>
#include <QBitArray>
#include <QSslCertificate>

#include "certificateSSLTest.h"


CertificateSSLTest::CertificateSSLTest() {}

void CertificateSSLTest::testCertToPem()
{
    // a certificate whose PEM encoding's length is a multiple of 64
    const char *mycert = "-----BEGIN CERTIFICATE-----\n" \
        "MIIEGjCCAwKgAwIBAgIESikYSjANBgkqhkiG9w0BAQUFADBbMQswCQYDVQQGEwJF\n" \
        "RTEiMCAGA1UEChMZQVMgU2VydGlmaXRzZWVyaW1pc2tlc2t1czEPMA0GA1UECxMG\n" \
        "RVNURUlEMRcwFQYDVQQDEw5FU1RFSUQtU0sgMjAwNzAeFw0wOTA2MDUxMzA2MTha\n" \
        "Fw0xNDA2MDkyMTAwMDBaMIGRMQswCQYDVQQGEwJFRTEPMA0GA1UEChMGRVNURUlE\n" \
        "MRcwFQYDVQQLEw5hdXRoZW50aWNhdGlvbjEhMB8GA1UEAxMYSEVJQkVSRyxTVkVO\n" \
        "LDM3NzA5MjcwMjg1MRAwDgYDVQQEEwdIRUlCRVJHMQ0wCwYDVQQqEwRTVkVOMRQw\n" \
        "EgYDVQQFEwszNzcwOTI3MDI4NTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA\n" \
        "k2Euwhm34vu1jOFp02J5fQRx9LW2C7x78CbJ7yInoAKn7QR8UdxTU7mJk90Opejo\n" \
        "71RUi2/aYl4jCr9gr99v2YoLufMRwAuqdmwmwqH1WAHRUtIcD0oPdKyelmmn9ig0\n" \
        "RV+yJLNT3dnyrwPw+uuzDe3DeKepGKE4lxexliCaAx0CAyCMW6OCATEwggEtMA4G\n" \
        "A1UdDwEB/wQEAwIEsDAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwQwPAYD\n" \
        "VR0fBDUwMzAxoC+gLYYraHR0cDovL3d3dy5zay5lZS9jcmxzL2VzdGVpZC9lc3Rl\n" \
        "aWQyMDA3LmNybDAgBgNVHREEGTAXgRVzdmVuLmhlaWJlcmdAZWVzdGkuZWUwUQYD\n" \
        "VR0gBEowSDBGBgsrBgEEAc4fAQEBATA3MBIGCCsGAQUFBwICMAYaBG5vbmUwIQYI\n" \
        "KwYBBQUHAgEWFWh0dHA6Ly93d3cuc2suZWUvY3BzLzAfBgNVHSMEGDAWgBRIBt6+\n" \
        "jIdXlYB4Y/qcIysroDoYdTAdBgNVHQ4EFgQUKCjpDf+LcvL6AH0QOiW6rMTtB/0w\n" \
        "CQYDVR0TBAIwADANBgkqhkiG9w0BAQUFAAOCAQEABRyRuUm9zt8V27WuNeXtCDmU\n" \
        "MGzA6g4QXNAd2nxFzT3k+kNzzQTOcgRdmjiEPuK49On+GWnBr/5MSBNhbCJVPWr/\n" \
        "yym1UYTBisaqhRt/N/kwZqd0bHeLJk+ZxSePXRyqkp9H8KPWqz7H+O/FxRS4ffxo\n" \
        "Q9Clem+e0bcjNlL5xXiRGycBeZq8cKj+0+A/UuattznQlvHdlCEsSeu1fPOORqFV\n" \
        "fZur4HC31lQD7xVvETLiL83CtOQC78+29XPD6Zlrrc5OF2yibSVParY19b8Zh6yu\n" \
        "p1dNvN8pBgXGrsyxRonwHooV2ghGNmGILkpdvlQfnxeCUg4erfHjDdSY9vmT7w==\n" \
        "-----END CERTIFICATE-----\n";

    QByteArray pem1(mycert);
    QSslCertificate cert1(pem1);
    QVERIFY(!cert1.isNull());
    QByteArray pem2(cert1.toPem());
    QSslCertificate cert2(pem2);
    QVERIFY(!cert2.isNull());
    QCOMPARE(pem1, pem2);
}


void CertificateSSLTest::testCertProperties()
{
    static const char pem[] =
                    "-----BEGIN CERTIFICATE-----\n"
                    "MIIB8zCCAVwCAREwDQYJKoZIhvcNAQEFBQAwWzELMAkGA1UEBhMCQVUxEzARBgNV\n"
                    "BAgTClF1ZWVuc2xhbmQxGjAYBgNVBAoTEUNyeXB0U29mdCBQdHkgTHRkMRswGQYD\n"
                    "VQQDExJUZXN0IENBICgxMDI0IGJpdCkwHhcNMDcwNDE3MDc0MDI2WhcNMDcwNTE3\n"
                    "MDc0MDI2WjApMRowGAYDVQQDExFuYW1lL3dpdGgvc2xhc2hlczELMAkGA1UEBhMC\n"
                    "Tk8wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAOud6QOsME+pWANExxgmL0iT\n"
                    "1ayg++hTxHsqAYnm/FoMxfUh+NdKkgJn2/GfNppinfPOSI667VqonU+7JBZDTLV5\n"
                    "CPbZIo9fFQpDJQN6naev4yaxU1VeYFfI7S8c8zYKeGSR+RenNNeLvfH80YxPpZZ1\n"
                    "snv8IfDH2V8MVxiyr7lLAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAleaU4pgzV6KY\n"
                    "+q9QuXapUYMsC2GiNtDmkG3k+MTHUO8XlE4hqPrIM6rRf7zKQdZ950R2wL9FSnYl\n"
                    "Qm1Tdv38dCka6ivMBqvRuOt9axH3m0G7nzHL7U3zaCbtEx3yVln+b3yYtiVpTuq0\n"
                    "3MLrt7tQGAW6ra8ISf6YY1W65/uVXZE=\n"
                    "-----END CERTIFICATE-----\n";

    static const char der[] = // hex encoded
                    "30:82:01:f3:30:82:01:5c:02:01:11:30:0d:06:09:2a"
                    "86:48:86:f7:0d:01:01:05:05:00:30:5b:31:0b:30:09"
                    "06:03:55:04:06:13:02:41:55:31:13:30:11:06:03:55"
                    "04:08:13:0a:51:75:65:65:6e:73:6c:61:6e:64:31:1a"
                    "30:18:06:03:55:04:0a:13:11:43:72:79:70:74:53:6f"
                    "66:74:20:50:74:79:20:4c:74:64:31:1b:30:19:06:03"
                    "55:04:03:13:12:54:65:73:74:20:43:41:20:28:31:30"
                    "32:34:20:62:69:74:29:30:1e:17:0d:30:37:30:34:31"
                    "37:30:37:34:30:32:36:5a:17:0d:30:37:30:35:31:37"
                    "30:37:34:30:32:36:5a:30:29:31:1a:30:18:06:03:55"
                    "04:03:13:11:6e:61:6d:65:2f:77:69:74:68:2f:73:6c"
                    "61:73:68:65:73:31:0b:30:09:06:03:55:04:06:13:02"
                    "4e:4f:30:81:9f:30:0d:06:09:2a:86:48:86:f7:0d:01"
                    "01:01:05:00:03:81:8d:00:30:81:89:02:81:81:00:eb"
                    "9d:e9:03:ac:30:4f:a9:58:03:44:c7:18:26:2f:48:93"
                    "d5:ac:a0:fb:e8:53:c4:7b:2a:01:89:e6:fc:5a:0c:c5"
                    "f5:21:f8:d7:4a:92:02:67:db:f1:9f:36:9a:62:9d:f3"
                    "ce:48:8e:ba:ed:5a:a8:9d:4f:bb:24:16:43:4c:b5:79"
                    "08:f6:d9:22:8f:5f:15:0a:43:25:03:7a:9d:a7:af:e3"
                    "26:b1:53:55:5e:60:57:c8:ed:2f:1c:f3:36:0a:78:64"
                    "91:f9:17:a7:34:d7:8b:bd:f1:fc:d1:8c:4f:a5:96:75"
                    "b2:7b:fc:21:f0:c7:d9:5f:0c:57:18:b2:af:b9:4b:02"
                    "03:01:00:01:30:0d:06:09:2a:86:48:86:f7:0d:01:01"
                    "05:05:00:03:81:81:00:95:e6:94:e2:98:33:57:a2:98"
                    "fa:af:50:b9:76:a9:51:83:2c:0b:61:a2:36:d0:e6:90"
                    "6d:e4:f8:c4:c7:50:ef:17:94:4e:21:a8:fa:c8:33:aa"
                    "d1:7f:bc:ca:41:d6:7d:e7:44:76:c0:bf:45:4a:76:25"
                    "42:6d:53:76:fd:fc:74:29:1a:ea:2b:cc:06:ab:d1:b8"
                    "eb:7d:6b:11:f7:9b:41:bb:9f:31:cb:ed:4d:f3:68:26"
                    "ed:13:1d:f2:56:59:fe:6f:7c:98:b6:25:69:4e:ea:b4"
                    "dc:c2:eb:b7:bb:50:18:05:ba:ad:af:08:49:fe:98:63"
                    "55:ba:e7:fb:95:5d:91";

    QByteArray certData(pem);
    QSslCertificate cert(certData);
    QVERIFY(!cert.isNull());

    QCOMPARE(cert.issuerInfo(QSslCertificate::Organization).at(0), QString("CryptSoft Pty Ltd"));
    QCOMPARE(cert.issuerInfo(QSslCertificate::CommonName).at(0), QString("Test CA (1024 bit)"));

    QCOMPARE(cert.issuerInfo(QSslCertificate::LocalityName).size(), 0);
    QCOMPARE(cert.issuerInfo(QSslCertificate::OrganizationalUnitName).size(), 0);
    QCOMPARE(cert.issuerInfo(QSslCertificate::CountryName).at(0), QString("AU"));
    QCOMPARE(cert.issuerInfo(QSslCertificate::StateOrProvinceName).at(0), QString("Queensland"));

    QCOMPARE(cert.issuerInfo("O").at(0), QString("CryptSoft Pty Ltd"));
    QCOMPARE(cert.issuerInfo("CN").at(0), QString("Test CA (1024 bit)"));
    QCOMPARE(cert.issuerInfo("L").size(), 0);
    QCOMPARE(cert.issuerInfo("OU").size(), 0);
    QCOMPARE(cert.issuerInfo("C").at(0), QString("AU"));
    QCOMPARE(cert.issuerInfo("ST").at(0), QString("Queensland"));

    QCOMPARE(cert.subjectInfo(QSslCertificate::Organization).size(), 0);
    QCOMPARE(cert.subjectInfo(QSslCertificate::CommonName).at(0), QString("name/with/slashes"));
    QCOMPARE(cert.subjectInfo(QSslCertificate::LocalityName).size(), 0);
    QCOMPARE(cert.subjectInfo(QSslCertificate::OrganizationalUnitName).size(), 0);
    QCOMPARE(cert.subjectInfo(QSslCertificate::CountryName).at(0), QString("NO"));
    QCOMPARE(cert.subjectInfo(QSslCertificate::StateOrProvinceName).size(), 0);

    QCOMPARE(cert.subjectInfo("O").size(), 0);
    QCOMPARE(cert.subjectInfo("CN").at(0), QString("name/with/slashes"));
    QCOMPARE(cert.subjectInfo("L").size(), 0);
    QCOMPARE(cert.subjectInfo("OU").size(), 0);
    QCOMPARE(cert.subjectInfo("C").at(0), QString("NO"));
    QCOMPARE(cert.subjectInfo("ST").size(), 0);

    QCOMPARE(cert.version(), QByteArray(1, '1'));
    QCOMPARE(cert.toPem().constData(), (const char*)pem);
    QCOMPARE(cert.toDer(), QByteArray::fromHex(der));

    QCOMPARE(cert.digest(QCryptographicHash::Md5), QByteArray::fromHex("B6:CF:57:34:DA:A9:73:21:82:F7:CF:4D:3D:85:31:88"));
    QCOMPARE(cert.digest(QCryptographicHash::Sha1),QByteArray::fromHex("B6:D1:51:82:E0:29:CA:59:96:38:BD:B6:F9:40:05:91:6D:49:09:60"));

    QCOMPARE(cert.effectiveDate().toUTC(), QDateTime(QDate(2007, 4, 17), QTime(7,40,26), Qt::UTC));
    QCOMPARE(cert.expiryDate().toUTC(), QDateTime(QDate(2007, 5, 17), QTime(7,40,26), Qt::UTC));
}

