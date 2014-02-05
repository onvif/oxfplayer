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

#include "certificateBox.hpp"

#include "certificateBoxTest.h"

CertificateBoxTest::CertificateBoxTest()
{
}

void CertificateBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<QByteArray>("certificate");

    QByteArray short_cert(256, '\0'),
            long_cert(1024, '\0');

    BoxSize short_size( short_cert.size() );
    BoxSize long_size( long_cert.size() );

    FourCC fourCC = CertificateBox::getFourCC();

    QTest::newRow("Positive test with 32 bit box size")
            << short_size.size() << BoxSize::large_empty() << fourCC << short_cert;
    QTest::newRow("Positive test with 64 bit box size")
            << BoxSize::empty() << long_size.large_size() << fourCC << long_cert;
    QTest::newRow("Negative test: wrong FourCC code")
            << short_size.size() << BoxSize::large_empty() << FourCC()<< short_cert;
}

void CertificateBoxTest::readingTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(QByteArray, certificate);

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
    }

    stream_writer
            .write(certificate);

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    CertificateBox * box = dynamic_cast<CertificateBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not CertificateBox.", Abort);
    QVERIFY2(box != NULL, "Box is not CertificateBox");

    QVERIFY(0 == box->getBoxOffset());

    if((size == 1) && (large_size != 0))
    {
        QVERIFY(large_size == box->getBoxSize());
    }
    else
    {
        QVERIFY(size == box->getBoxSize());
    }

    QCOMPARE(certificate, box->getCertificateData());
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
