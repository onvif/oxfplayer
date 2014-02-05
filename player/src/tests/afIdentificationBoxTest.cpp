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

#include "afIdentificationBox.hpp"

#include "afIdentificationBoxTest.h"

AfIdentificationBoxTest::AfIdentificationBoxTest()
{
}

void AfIdentificationBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<uint64_t>("startTime");
    QTest::addColumn<uint64_t>("duration");
    QTest::addColumn<QString>("predecessorURI");
    QTest::addColumn<QString>("successorURI");
    QTest::addColumn<uint16_t>("predecessorURISize");
    QTest::addColumn<uint16_t>("successorURISize");

    QString short_string_1(5, 'a'),
            short_string_2(6, 'b'),
            long_string_1(63, 'c'),
            long_string_2(95, 'd');

    BoxSize short_size( 3 * uuid_size() + 2 * sizeof(uint64_t) + 2 * sizeof(uint16_t) + short_string_1.size() + 1 + short_string_2.size() + 1);
    BoxSize long_size( 3 * uuid_size() + 2 * sizeof(uint64_t) + 2 * sizeof(uint16_t) + long_string_1.size() + 1 + long_string_2.size() + 1);


    FourCC fourCC = AFIdentificationBox::getFourCC();


    QTest::newRow("Positive test with 32 bit box size")
            << short_size.size() << BoxSize::large_empty() << fourCC
            << uint64_t() << uint64_t(321)
            << short_string_1 << short_string_2 << uint16_t(5) << uint16_t(6);

    QTest::newRow("Positive test with 64 bit box size")
            << BoxSize::empty() << long_size.large_size() << fourCC
            << datetime_write() << uint64_t(321)
            << long_string_1 << long_string_2 << uint16_t(63) << uint16_t(95);

    QTest::newRow("Negative test: wrong predecessorURISize")
            << short_size.size() << BoxSize::large_empty() << fourCC
            << datetime_write() << uint64_t(321)
            << short_string_1 << short_string_2 << uint16_t(2) << uint16_t(6);

    QTest::newRow("Negative test: wrong successorURISize")
            << short_size.size() << BoxSize::large_empty() << fourCC
            << datetime_write() << uint64_t(321)
            << short_string_1 << short_string_2 << uint16_t(5) << uint16_t(2);

    QTest::newRow("Negative test: wrong FourCC code")
            << BoxSize::empty() << long_size.large_size() << FourCC()
            << datetime_write() << uint64_t(321)
            << long_string_1 << long_string_2 << uint16_t(63) << uint16_t(95);
}

void AfIdentificationBoxTest::readingTest()
{
    QUuid fragmentUUID(QUuid::createUuid());
    QUuid predecessorUUID(QUuid::createUuid());
    QUuid successorUUID(QUuid::createUuid());
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint64_t, startTime);
    QFETCH(uint64_t, duration);
    QFETCH(QString, predecessorURI);
    QFETCH(QString, successorURI);
    QFETCH(uint16_t, predecessorURISize);
    QFETCH(uint16_t, successorURISize);

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
    }

    stream_writer
            .write(fragmentUUID)
            .write(predecessorUUID)
            .write(successorUUID)
            .write(startTime)
            .write(duration)
            .write(predecessorURISize)
            .write(successorURISize)
            .write(predecessorURI)
            .write(successorURI);

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    AFIdentificationBox * box = dynamic_cast<AFIdentificationBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not AFIdentificationBox.", Abort);
    QVERIFY2(box != NULL, "Box is not AFIdentificationBox");

    QVERIFY(0 == box->getBoxOffset());

    if((size == 1) && (large_size != 0))
    {
        QVERIFY(large_size == box->getBoxSize());
    }
    else
    {
        QVERIFY(size == box->getBoxSize());
    }

    QCOMPARE(fragmentUUID, box->getFragmentUUID());
    QCOMPARE(predecessorUUID, box->getPredecessorUUID());
    QCOMPARE(successorUUID, box->getSuccessorUUID());
    QCOMPARE(datetime_read(startTime), box->getStartTime());
    QCOMPARE(duration, box->getDuration());
    QCOMPARE(predecessorURISize, box->getPredecessorURISize());
    QCOMPARE(successorURISize, box->getSuccessorURISize());
    QCOMPARE(predecessorURI, box->getPredecessorURI());
    QCOMPARE(successorURI, box->getSuccessorURI());
    QEXPECT_FAIL("Negative test: wrong predecessorURISize", "Negative test: predecessorURISize should not be equal to predecessorURI string length.", Continue);
    QVERIFY(predecessorURISize == box->getPredecessorURI().size());
    QEXPECT_FAIL("Negative test: wrong successorURISize", "Negative test: successorURISize should not be equal to successorURI string length.", Continue);
    QVERIFY(successorURISize == box->getSuccessorURI().size());
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
