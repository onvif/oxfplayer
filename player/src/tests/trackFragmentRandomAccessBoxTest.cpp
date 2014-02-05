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

#include "trackFragmentRandomAccessBoxTest.h"

#include "trackFragmentRandomAccessBox.hpp"

Q_DECLARE_METATYPE(TrackFragmentRandomAccessBox::TableType)

template<typename T>
inline void assign(T & value, const uint32_t & source)
{
    value = source;
}

inline void assign(U_UInt24 & value, const uint32_t & source)
{
    value.m_value = source;
}

template<typename TVersioned, typename TTraf, typename TTrun, typename TSample>
void write_table_helper3(StreamWriter & stream_writer, const TrackFragmentRandomAccessBox::TableType & table)
{
    stream_writer.write(uint32_t(table.size()));
    for(int i = 0; i < table.size(); i++)
    {
        TVersioned time = std::get<0>(table[i]),
                moof_offset = std::get<1>(table[i]);
        TTraf traf_value;
        TTrun trun_value;
        TSample samlpe_value;
        assign(traf_value, std::get<2>(table[i]));
        assign(trun_value, std::get<3>(table[i]));
        assign(samlpe_value, std::get<4>(table[i]));
        stream_writer
                .write(time)
                .write(moof_offset)
                .write(traf_value)
                .write(trun_value)
                .write(samlpe_value);
    }
}

template<typename TVersioned, typename TTraf, typename TTrun>
inline void write_table_helper2(StreamWriter & stream_writer, const TrackFragmentRandomAccessBox::TableType & table, uint8_t sample_size)
{
    switch(sample_size)
    {
    case 0:
        write_table_helper3<TVersioned, TTraf, TTrun, uint8_t>(stream_writer, table);
        break;
    case 1:
        write_table_helper3<TVersioned, TTraf, TTrun, uint16_t>(stream_writer, table);
        break;
    case 2:
        write_table_helper3<TVersioned, TTraf, TTrun, U_UInt24>(stream_writer, table);
        break;
    default:
        write_table_helper3<TVersioned, TTraf, TTrun, uint32_t>(stream_writer, table);
    }
}

template<typename TVersioned, typename TTraf>
inline void write_table_helper1(StreamWriter & stream_writer, const TrackFragmentRandomAccessBox::TableType & table, uint8_t trun_size, uint8_t sample_size)
{
    switch(trun_size)
    {
    case 0:
        write_table_helper2<TVersioned, TTraf, uint8_t>(stream_writer, table, sample_size);
        break;
    case 1:
        write_table_helper2<TVersioned, TTraf, uint16_t>(stream_writer, table, sample_size);
        break;
    case 2:
        write_table_helper2<TVersioned, TTraf, U_UInt24>(stream_writer, table, sample_size);
        break;
    default:
        write_table_helper2<TVersioned, TTraf, uint32_t>(stream_writer, table, sample_size);
    }
}

template<typename TVersioned>
inline void write_table_helper0(StreamWriter & stream_writer, const TrackFragmentRandomAccessBox::TableType & table, uint8_t traf_size, uint8_t trun_size, uint8_t sample_size)
{
    switch(traf_size)
    {
    case 0:
        write_table_helper1<TVersioned, uint8_t>(stream_writer, table, trun_size, sample_size);
        break;
    case 1:
        write_table_helper1<TVersioned, uint16_t>(stream_writer, table, trun_size, sample_size);
        break;
    case 2:
        write_table_helper1<TVersioned, U_UInt24>(stream_writer, table, trun_size, sample_size);
        break;
    default:
        write_table_helper1<TVersioned, uint32_t>(stream_writer, table, trun_size, sample_size);
    }
}

inline void write_table(StreamWriter & stream_writer, const TrackFragmentRandomAccessBox::TableType & table, uint8_t version, uint8_t traf_size, uint8_t trun_size, uint8_t sample_size)
{
    switch(version)
    {
    case 0:
        write_table_helper0<uint32_t>(stream_writer, table, traf_size, trun_size, sample_size);
        break;
    default:
        write_table_helper0<uint64_t>(stream_writer, table, traf_size, trun_size, sample_size);
    }
}

TrackFragmentRandomAccessBoxTest::TrackFragmentRandomAccessBoxTest() :
    QObject()
{
}

void TrackFragmentRandomAccessBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<uint8_t>("version");
    QTest::addColumn<U_UInt24>("flag");
    QTest::addColumn<uint32_t>("track_id");
    QTest::addColumn<uint8_t>("traf_size");
    QTest::addColumn<uint8_t>("trun_size");
    QTest::addColumn<uint8_t>("sample_size");
    QTest::addColumn<TrackFragmentRandomAccessBox::TableType>("table");

    uint8_t version_zero(0),
            version_one(1);
    U_UInt24 flag;
    flag.m_value = 0;

    TrackFragmentRandomAccessBox::TableType table(QVector<TrackFragmentEntry>(30, TrackFragmentEntry(2131, 7896, 24, 75, 96)).toList());

    FourCC fourCC = TrackFragmentRandomAccessBox::getFourCC();

    QString short_test_name("Positive test with 32 bit box size, version %1 and sizes: [%2 , %3, %4]");
    QString long_test_name("Positive test with 64 bit box size, version %1 and sizes: [%2 , %3, %4]");
    for(uint8_t traf_size = 0; traf_size < 4; ++traf_size)
    {
        for(uint8_t trun_size = 0; trun_size < 4; ++trun_size)
        {
            for(uint8_t sample_size = 0; sample_size < 4; ++sample_size)
            {
                BoxSize v0_size( 3 * sizeof(uint32_t) + table.size() * ( 2 * sizeof(uint32_t) + trun_size + 1 + traf_size + 1 + sample_size + 1) ),
                        v1_size( 3 * sizeof(uint32_t) + table.size() * ( 2 * sizeof(uint64_t) + trun_size + 1 + traf_size + 1 + sample_size + 1) );
                if( sample_size %2 )
                {
                    QTest::newRow(short_test_name.arg(version_zero).arg(traf_size + 1).arg(trun_size + 1).arg(sample_size + 1).toLatin1().data())
                            << v0_size.fullbox_size() << BoxSize::large_empty() << fourCC << version_zero << flag
                            << 23u << traf_size << trun_size << sample_size << table;
                    QTest::newRow(long_test_name.arg(version_one).arg(traf_size + 1).arg(trun_size + 1).arg(sample_size + 1).toLatin1().data())
                            << BoxSize::empty() << v1_size.fullbox_large_size() << fourCC << version_one << flag
                            << 65234u << traf_size << trun_size << sample_size << table;
                }
                else
                {
                    QTest::newRow(short_test_name.arg(version_one).arg(traf_size + 1).arg(trun_size + 1).arg(sample_size + 1).toLatin1().data())
                            << v1_size.fullbox_size() << BoxSize::large_empty() << fourCC << version_one << flag
                            << 23u << traf_size << trun_size << sample_size << table;
                    QTest::newRow(long_test_name.arg(version_zero).arg(traf_size + 1).arg(trun_size + 1).arg(sample_size + 1).toLatin1().data())
                            << BoxSize::empty() << v0_size.fullbox_large_size() << fourCC << version_zero << flag
                            << 65234u << traf_size << trun_size << sample_size << table;
                }
            }
        }
    }

    QTest::newRow("Negative test: wrong FourCC code")
            << BoxSize( 3 * sizeof(uint32_t) ).fullbox_size() << BoxSize::large_empty() << FourCC() << version_zero << flag
            << 2353u << uint8_t(0) << uint8_t(0) << uint8_t(0) << TrackFragmentRandomAccessBox::TableType();
}

void TrackFragmentRandomAccessBoxTest::readingTest()
{
    U_UInt24 reserved;
    reserved.m_value = 0;

    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(uint32_t, track_id);
    QFETCH(uint8_t, traf_size);
    QFETCH(uint8_t, trun_size);
    QFETCH(uint8_t, sample_size);
    QFETCH(TrackFragmentRandomAccessBox::TableType, table);

    uint8_t number_size = (traf_size << 4) | (trun_size << 2) | sample_size;

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
            .write(track_id)
            .write(reserved)
            .write(number_size);

    write_table(stream_writer, table, version, traf_size, trun_size, sample_size);

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    TrackFragmentRandomAccessBox * box = dynamic_cast<TrackFragmentRandomAccessBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not TrackFragmentRandomAccessBox.", Abort);
    QVERIFY2(box != NULL, "Box is not TrackFragmentRandomAccessBox");

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
    QCOMPARE(track_id, box->getTrackID());
    QCOMPARE(traf_size, box->getTrafNumberSize());
    QCOMPARE(trun_size, box->getTrunNumberSize());
    QCOMPARE(sample_size, box->getSampleNumberSize());
    QCOMPARE(table, box->getTable());
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
