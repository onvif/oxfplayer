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

#include "SurveillanceExportBoxTest.h"

#include "surveillanceExportBox.hpp"

Q_DECLARE_METATYPE(SurveillanceAudioTable)
Q_DECLARE_METATYPE(SurveillanceVideoTable)

SurveillanceExportBoxTest::SurveillanceExportBoxTest() :
    QObject()
{
}

void SurveillanceExportBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<uint8_t>("version");
    QTest::addColumn<U_UInt24>("flag");
    QTest::addColumn<QString>("export_unit_name");
    QTest::addColumn<QString>("export_unit_url");
    QTest::addColumn<QString>("export_unit_mac");
    QTest::addColumn<uint64_t>("export_unit_time");
    QTest::addColumn<QString>("export_operator");
    QTest::addColumn<SurveillanceVideoTable>("video_table");
    QTest::addColumn<SurveillanceAudioTable>("audio_table");

    QString short_eun("ExportUnitName"),
            short_euu("ExportUnitUrl"),
            short_eum("ExportUnitMac"),
            short_eop("ExportOperator"),
            long_eun(324, 'a'),
            long_euu(583, 'b'),
            long_eum(312, 'c'),
            long_eop(942, 'd'),
            vtbl_1(6, 'e'),
            vtbl_2(42, 'f'),
            vtbl_3(64, 'g'),
            vtbl_4(25, 'i'),
            atbl_1(42, 'j'),
            atbl_2(246, 'k'),
            atbl_3(73, 'l');

    QUuid vtbl(QUuid::createUuid()),
          atbl(QUuid::createUuid());

    SurveillanceVideoTable short_video_table(QVector<SurveillanceVideoEntry>(5, SurveillanceVideoEntry(vtbl, vtbl_1, vtbl_2, vtbl_3, vtbl_4)).toList()),
            long_video_table(QVector<SurveillanceVideoEntry>(80, SurveillanceVideoEntry(vtbl, vtbl_1, vtbl_2, vtbl_3, vtbl_4)).toList()),
            empty_video_table;

    SurveillanceAudioTable short_audio_table(QVector<SurveillanceAudioEntry>(8, SurveillanceAudioEntry(atbl, atbl_1, atbl_2, atbl_3)).toList()),
            long_audio_table(QVector<SurveillanceAudioEntry>(123, SurveillanceAudioEntry(atbl, atbl_1, atbl_2, atbl_3)).toList()),
            empty_audio_table;

    BoxSize short_size( sizeof(uint64_t) + 2 * sizeof(uint8_t)
                        + short_eun.size() + 1
                        + short_euu.size() + 1
                        + short_eum.size() + 1
                        + short_eop.size() + 1
                        + short_video_table.size() * (uuid_size() + vtbl_1.size() + 1 + vtbl_2.size() + 1 + vtbl_3.size() + 1 + vtbl_4.size() + 1)
                        + short_audio_table.size() * (uuid_size() + atbl_1.size() + 1 + atbl_2.size() + 1 + atbl_3.size() + 1));
    BoxSize long_size( sizeof(uint64_t) + 2 * sizeof(uint8_t)
                       + long_eun.size() + 1
                       + long_euu.size() + 1
                       + long_eum.size() + 1
                       + long_eop.size() + 1
                       + long_video_table.size() * (uuid_size() + vtbl_1.size() + 1 + vtbl_2.size() + 1 + vtbl_3.size() + 1 + vtbl_4.size() + 1)
                       + long_audio_table.size() * (uuid_size() + atbl_1.size() + 1 + atbl_2.size() + 1 + atbl_3.size() + 1));
    BoxSize short_empty_video_size( sizeof(uint64_t) + 2 * sizeof(uint8_t)
                                    + short_eun.size() + 1
                                    + short_euu.size() + 1
                                    + short_eum.size() + 1
                                    + short_eop.size() + 1
                                    + short_audio_table.size() * (uuid_size() + atbl_1.size() + 1 + atbl_2.size() + 1 + atbl_3.size() + 1));
    BoxSize short_empty_tables_size( sizeof(uint64_t) + 2 * sizeof(uint8_t)
                                     + short_eun.size() + 1
                                     + short_euu.size() + 1
                                     + short_eum.size() + 1
                                     + short_eop.size() + 1);
    BoxSize long_empty_audio_size( sizeof(uint64_t) + 2 * sizeof(uint8_t)
                                   + long_eun.size() + 1
                                   + long_euu.size() + 1
                                   + long_eum.size() + 1
                                   + long_eop.size() + 1
                                   + long_video_table.size() * (uuid_size() + vtbl_1.size() + 1 + vtbl_2.size() + 1 + vtbl_3.size() + 1 + vtbl_4.size() + 1));

    FourCC fourCC = SurveillanceExportBox::getFourCC();

    uint8_t version_zero(0);
    U_UInt24 flag;

    QTest::newRow("Positive test with 32 bit box size")
            << short_size.fullbox_size() << BoxSize::large_empty() << fourCC << version_zero << flag
            << short_eun << short_euu << short_eum << datetime_write() << short_eop << short_video_table << short_audio_table;
    QTest::newRow("Positive test with 64 bit box size")
            << BoxSize::empty() << long_size.fullbox_large_size() << fourCC << version_zero << flag
            << long_eun << long_euu << long_eum << datetime_write() << long_eop << long_video_table << long_audio_table;
    QTest::newRow("Positive test with empty video table")
            << short_empty_video_size.fullbox_size() << BoxSize::large_empty() << fourCC << version_zero << flag
            << short_eun << short_euu << short_eum << datetime_write() << short_eop << empty_video_table << short_audio_table;
    QTest::newRow("Positive test with empty audio table")
            << BoxSize::empty() << long_empty_audio_size.fullbox_large_size() << fourCC << version_zero << flag
            << long_eun << long_euu << long_eum << datetime_write() << long_eop << long_video_table << empty_audio_table;
    QTest::newRow("Positive test with empty tables")
            << short_empty_tables_size.fullbox_size() << BoxSize::large_empty() << fourCC << version_zero << flag
            << short_eun << short_euu << short_eum << datetime_write() << short_eop << empty_video_table << empty_audio_table;
    QTest::newRow("Negative test: wrong FourCC code")
            << short_size.fullbox_size() << BoxSize::large_empty() << FourCC() << version_zero << flag
            << short_eun << short_euu << short_eum << datetime_write() << short_eop << short_video_table << short_audio_table;
}

void SurveillanceExportBoxTest::readingTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(QString, export_unit_name);
    QFETCH(QString, export_unit_url);
    QFETCH(QString, export_unit_mac);
    QFETCH(uint64_t, export_unit_time);
    QFETCH(QString, export_operator);
    QFETCH(SurveillanceVideoTable, video_table);
    QFETCH(SurveillanceAudioTable, audio_table);

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
            .write(export_unit_name)
            .write(export_unit_url)
            .write(export_unit_mac)
            .write(export_unit_time)
            .write(export_operator)
            .write( uint8_t(video_table.size()) );

    for(int i = 0; i < video_table.size(); i++)
    {
        stream_writer.write( video_table[i] );
    }

    stream_writer.write( uint8_t(audio_table.size()) );

    for(int i = 0; i < audio_table.size(); i++)
    {
        stream_writer.write( audio_table[i] );
    }

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    SurveillanceExportBox * box = dynamic_cast<SurveillanceExportBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not SurveillanceExportBox.", Abort);
    QVERIFY2(box != NULL, "Box is not SurveillanceExportBox");

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
    QCOMPARE(export_unit_name, box->getExportUnitName());
    QCOMPARE(export_unit_url, box->getExportUnitURL());
    QCOMPARE(export_unit_mac, box->getExportUnitMAC());
    QCOMPARE(datetime_read(export_unit_time), box->getExportUnitTime());
    QCOMPARE(export_operator, box->getExportOperator());
    QCOMPARE(video_table, box->getVideoEntries());
    QCOMPARE(audio_table, box->getAudioEntries());
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
