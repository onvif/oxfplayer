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

#include "AdditionalUserInformationBoxTest.h"

#include "additionalUserInformation.hpp"
#include "boxTestsCommon.h"

AdditionalUserInformationBoxTest::AdditionalUserInformationBoxTest() :
    QObject()
{
}

void AdditionalUserInformationBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<QString>("info_string");

    QString info_string(5, 'a');

    BoxSize box_size(info_string.size() + 1);

    FourCC fourCC = AdditionalUserInformationBox::getFourCC();

    QTest::newRow("Positive test with 32 bit box size")
            << box_size.size() << BoxSize::large_empty() << fourCC
            << info_string;

    QTest::newRow("Positive test with 64 bit box size")
            << BoxSize::empty() << box_size.large_size() << fourCC
            << info_string;

    QTest::newRow("Negative test with wrong size of data")
            << box_size.size() << BoxSize::large_empty() << fourCC
            << QString(info_string.size() + 10, 'x');
}

void AdditionalUserInformationBoxTest::readingTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(QString, info_string);

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
    }

    stream_writer
            .write(info_string);

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    AdditionalUserInformationBox * box = dynamic_cast<AdditionalUserInformationBox*>(file.getChildren()[0]);

    QVERIFY2(box != NULL, "Box is not AdditionalUserInformationBox");

    QVERIFY(0 == box->getBoxOffset());

    if((size == 1) && (large_size != 0))
    {
        QVERIFY(large_size == box->getBoxSize());
    }
    else
    {
        QVERIFY(size == box->getBoxSize());
    }

    QEXPECT_FAIL("Negative test with wrong size of data", "Content does not match to size", Abort);
    QCOMPARE(info_string, box->getUserInformation());

    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
