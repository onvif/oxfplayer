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

#include "sampleDependencyTypeBoxTest.h"

#include "sampleDependencyTypeBox.hpp"
#include "boxTestsCommon.h"

#include <QVector>

SampleDependencyTypeBoxTest::SampleDependencyTypeBoxTest() :
    QObject()
{
}

void SampleDependencyTypeBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<uint8_t>("version");
    QTest::addColumn<U_UInt24>("flags");
    QTest::addColumn< QList<uint8_t> >("table_content");

    fillData();

    BoxSize box_size(DATA_SIZE);

    FourCC fourCC = SampleDependencyTypeBox::getFourCC();

    U_UInt24 flags;
    flags.m_value = 0;

    QTest::newRow("Positive test with 32 bit box size")
            << box_size.fullbox_size() << BoxSize::large_empty() << fourCC
            << (uint8_t)0 << flags
            << table_data;

    QTest::newRow("Positive test with 64 bit box size")
            << BoxSize::empty() << box_size.fullbox_large_size() << fourCC
            << (uint8_t)0 << flags
            << table_data;

    QTest::newRow("Negative test with 32 bit box size")
            << box_size.fullbox_size() << BoxSize::large_empty() << fourCC
            << (uint8_t)0 << flags
            << short_table_data;
}

void SampleDependencyTypeBoxTest::readingTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flags);
    QFETCH(QList<uint8_t>, table_content);

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
    }

    stream_writer
            .write(version)
            .write(flags);

    for(int i = 0; i < table_content.size(); ++i)
        stream_writer.write(table_content[i]);

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    SampleDependencyTypeBox * box = dynamic_cast<SampleDependencyTypeBox*>(file.getChildren()[0]);

    QVERIFY2(box != NULL, "Box is not SampleDependencyTypeBox");

    QVERIFY(0 == box->getBoxOffset());

    if((size == 1) && (large_size != 0))
    {
        QVERIFY(large_size == box->getBoxSize());
    }
    else
    {
        QVERIFY(size == box->getBoxSize());
    }

    QList<uint8_t> box_table = box->getTable();

    QVERIFY(box_table.size() == DATA_SIZE);

    QEXPECT_FAIL("Negative test with 32 bit box size", "Wrong columns count", Abort);
    QVERIFY(box_table == table_data);

    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}

void SampleDependencyTypeBoxTest::fillData()
{
    table_data.clear();
    for(int i = 0; i < DATA_SIZE; ++i)
        table_data.push_back(0);
    short_table_data.clear();
    for(int i = 0; i < SHORT_DATA_SIZE; ++i)
        short_table_data.push_back(0);
}
