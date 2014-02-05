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

#include "compactSampleSizeBox.hpp"

#include "compactSampleSizeBoxTest.h"

void write_table4(StreamWriter & stream_writer, CompactSampleSizeBox::TableType table)
{
    if(table.size() % 2 == 1)
    {
        table.append(0);
    }
    for(int i = 0; i < table.size(); i += 2)
    {
        uint8_t value( uint8_t(table[i]) << 4 | uint8_t( table[i+1] ) );
        stream_writer.write(value);
    }
}

template<typename TValueType>
void write_table(StreamWriter & stream_writer, CompactSampleSizeBox::TableType table)
{
    for(int i = 0; i < table.size(); i++)
    {
        TValueType value( table[i] );
        stream_writer.write(value);
    }
}

CompactSampleSizeBoxTest::CompactSampleSizeBoxTest()
{
}

void CompactSampleSizeBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<unsigned char>("version");
    QTest::addColumn<U_UInt24>("flag");
    QTest::addColumn<unsigned char>("field_size");
    QTest::addColumn<CompactSampleSizeBox::TableType>("table");

    CompactSampleSizeBox::TableType short_table(QVector<CompactSampleSizeEntry>(30, 1).toList()),
            long_table(QVector<CompactSampleSizeEntry>(501, 13).toList());

    U_UInt24 flag;

    unsigned char version_zero(0),
            field_size_4(4),
            field_size_8(8),
            field_size_16(16);

    BoxSize short_size_4( 2 * sizeof(uint32_t) + short_table.size() * sizeof(uint8_t) / 2 );
    BoxSize short_size_8( 2 * sizeof(uint32_t) + short_table.size() * sizeof(uint8_t) );
    BoxSize short_size_16( 2 * sizeof(uint32_t) + short_table.size() * sizeof(uint16_t) );
    BoxSize long_size_4( 2  * sizeof(uint32_t) + (long_table.size() + 1) * sizeof(uint8_t) / 2);
    BoxSize long_size_8( 2  * sizeof(uint32_t) + long_table.size() * sizeof(uint8_t) );
    BoxSize long_size_16( 2  * sizeof(uint32_t) + long_table.size() * sizeof(uint16_t) );

    FourCC fourCC = CompactSampleSizeBox::getFourCC();

    QTest::newRow("Positive test with 32 bit box size and field size 4")
            << short_size_4.fullbox_size() << BoxSize::large_empty() << fourCC
            << version_zero << flag << field_size_4 << short_table;

    QTest::newRow("Positive test with 32 bit box size and field size 8")
            << short_size_8.fullbox_size() << BoxSize::large_empty() << fourCC
            << version_zero << flag << field_size_8 << short_table;

    QTest::newRow("Positive test with 32 bit box size and field size 16")
            << short_size_16.fullbox_size() << BoxSize::large_empty() << fourCC
            << version_zero << flag << field_size_16 << short_table;

    QTest::newRow("Positive test with 64 bit box size and field size 4")
            << BoxSize::empty() << long_size_4.fullbox_large_size() << fourCC
            << version_zero << flag << field_size_4 << long_table;

    QTest::newRow("Positive test with 64 bit box size and field size 8")
            << BoxSize::empty() << long_size_8.fullbox_large_size() << fourCC
            << version_zero << flag << field_size_8 << long_table;

    QTest::newRow("Positive test with 64 bit box size and field size 16")
            << BoxSize::empty() << long_size_16.fullbox_large_size() << fourCC
            << version_zero << flag << field_size_16 << long_table;

    QTest::newRow("Negative test: wrong FourCC code")
            << short_size_8.fullbox_size() << BoxSize::large_empty() << FourCC()
            << version_zero << flag << field_size_8 << short_table;
}

void CompactSampleSizeBoxTest::readingTest()
{
    U_UInt24 reserved;
    reserved.m_value = 0;

    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(uint8_t, field_size);
    QFETCH(CompactSampleSizeBox::TableType, table);

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
    }

    stream_writer
            .write(version)
            .write(flag)
            .write(reserved)
            .write(field_size)
            .write(table.size());

    switch(field_size)
    {
    case 4:
        write_table4(stream_writer, table);
        break;
    case 8:
        write_table<uint8_t>(stream_writer, table);
        break;
    case 16:
        write_table<uint16_t>(stream_writer, table);
        break;
    }

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    CompactSampleSizeBox * box = dynamic_cast<CompactSampleSizeBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not CompactSampleSizeBox.", Abort);
    QVERIFY2(box != NULL, "Box is not CompactSampleSizeBox");

    QVERIFY(0 == box->getBoxOffset());

    if((size == 1) && (large_size != 0))
    {
        QVERIFY(large_size == box->getBoxSize());
    }
    else
    {
        QVERIFY(size == box->getBoxSize());
    }

    QCOMPARE(uint32_t(version), box->getVersion());
    QCOMPARE(flag_to_bits(flag), box->getFlags());
    QCOMPARE(uint32_t(field_size), box->getFieldSize());
    QCOMPARE(table, box->getTable());
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
