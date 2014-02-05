#include "trackRunBoxTest.h"

#include "trackRunBox.hpp"

Q_DECLARE_METATYPE(TrackRunBox::TableType)

QString prepare_flag_names(TrackRunFlags flags)
{
    QStringList flag_names;
    if(flags & DataOffsetPresent)
    {
        flag_names << "data offset";
    }
    if(flags & FirstSampleFlagsPresent)
    {
        flag_names << "first sample flags";
    }
    if(flags & SampleDurationPresent)
    {
        flag_names << "sample duration";
    }
    if(flags & SampleSizePresent)
    {
        flag_names << "sample size";
    }
    if(flags & SampleFlagsPresent)
    {
        flag_names << "sample flags";
    }
    if(flags & SampleCompositionTimeOffsetPresent)
    {
        flag_names << "sample composition time offset";
    }
    if(flag_names.isEmpty())
    {
        flag_names << "no flags";
    }
    return flag_names.join(", ");
}

BoxSize calculate_box_size(TrackRunFlags flags, uint32_t table_size)
{
    uint32_t size = sizeof(uint32_t);
    if(flags & DataOffsetPresent)
    {
        size += sizeof(int32_t);
    }
    if(flags & FirstSampleFlagsPresent)
    {
        size += sizeof(uint32_t);
    }
    if(flags & SampleDurationPresent)
    {
        size += table_size * sizeof(uint32_t);
    }
    if(flags & SampleSizePresent)
    {
        size += table_size * sizeof(uint32_t);
    }
    if(flags & SampleFlagsPresent)
    {
        size += table_size * sizeof(uint32_t);
    }
    if(flags & SampleCompositionTimeOffsetPresent)
    {
        size += table_size * sizeof(uint32_t);
    }
    return BoxSize(size);
}

void write_table(StreamWriter & stream_writer, TrackRunBox::TableType & table, TrackRunFlags flags)
{
    for(int i = 0; i < table.size(); i++)
    {
        TrackRunEntry & entry = table[i];
        if(flags & SampleDurationPresent)
        {
            stream_writer.write(std::get<0>(entry));
        }
        if(flags & SampleSizePresent)
        {
            stream_writer.write(std::get<1>(entry));
        }
        if(flags & SampleFlagsPresent)
        {
            stream_writer.write(std::get<2>(entry));
        }
        if(flags & SampleCompositionTimeOffsetPresent)
        {
            stream_writer.write(std::get<3>(entry));
        }
    }
}

void compare_table(const TrackRunBox::TableType & source, const TrackRunBox::TableType & read, TrackRunFlags flags)
{
    QVERIFY(source.size() == read.size());
    for(int i = 0; i < source.size(); i++)
    {
        const TrackRunEntry & source_entry = source[i];
        const TrackRunEntry & read_entry = read[i];
        if(flags & SampleDurationPresent)
        {
            QCOMPARE(std::get<0>(source_entry).value(), std::get<0>(read_entry).value());
        }
        if(flags & SampleSizePresent)
        {
            QCOMPARE(std::get<1>(source_entry).value(), std::get<1>(read_entry).value());
        }
        if(flags & SampleFlagsPresent)
        {
            QCOMPARE(std::get<2>(source_entry).value(), std::get<2>(read_entry).value());
        }
        if(flags & SampleCompositionTimeOffsetPresent)
        {
            QCOMPARE(std::get<3>(source_entry).value(), std::get<3>(read_entry).value());
        }
    }
}

TrackRunBoxTest::TrackRunBoxTest() :
    QObject()
{
}

void TrackRunBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<uint8_t>("version");
    QTest::addColumn<U_UInt24>("flag");
    QTest::addColumn<int32_t>("data_offset");
    QTest::addColumn<uint32_t>("first_sample_flags");
    QTest::addColumn<TrackRunBox::TableType>("table");

    uint8_t version_zero(0);
    U_UInt24 flag_empty;
    flag_empty.m_value = 0;

    TrackRunBox::TableType table(QVector<TrackRunEntry>(50, TrackRunEntry(89u, 3248u, 23489u, 0u)).toList());

    FlagStateGenerator<TrackRunFlags> fsg;
    fsg.add(DataOffsetPresent).add(FirstSampleFlagsPresent).add(SampleDurationPresent).add(SampleSizePresent).add(SampleFlagsPresent).add(SampleCompositionTimeOffsetPresent);

    FourCC fourCC = TrackRunBox::getFourCC();

    QString short_test_name("Positive test with 32 bit box size and %1 present");
    QString long_test_name("Positive test with 64 bit box size and %1 present");

    bool short_test = true;
    for(TrackRunFlags flags = fsg.begin(); flags <= fsg.end(); flags = fsg.next())
    {
        BoxSize box_size(calculate_box_size(flags, table.size()));
        U_UInt24 flag;
        flag.m_value = uint32_t(flags);
        if(short_test)
        {
            QTest::newRow(short_test_name.arg(prepare_flag_names(flags)).toLatin1().data())
                    << box_size.fullbox_size() << BoxSize::large_empty() << fourCC << version_zero << flag
                    << 4234 << 3247u << table;
        }
        else
        {
            QTest::newRow(long_test_name.arg(prepare_flag_names(flags)).toLatin1().data())
                    << BoxSize::empty() << box_size.fullbox_large_size() << fourCC << version_zero << flag
                    << -434 << 247u << table;
        }
        short_test = !short_test;
    }
    QTest::newRow("Negative test: wrong FourCC code")
            << BoxSize(sizeof(uint32_t)).fullbox_size() << BoxSize::large_empty() << FourCC()<< version_zero << flag_empty
            << 0 << 0u << table;
}

void TrackRunBoxTest::readingTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(int32_t, data_offset);
    QFETCH(uint32_t, first_sample_flags);
    QFETCH(TrackRunBox::TableType, table);

    TrackRunFlags flags = (TrackRunFlags)flag.m_value;

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
            .write(table.size());

    if(flags & DataOffsetPresent)
    {
        stream_writer.write(data_offset);
    }
    if(flags & FirstSampleFlagsPresent)
    {
        stream_writer.write(first_sample_flags);
    }
    write_table(stream_writer, table, flags);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    TrackRunBox * box = dynamic_cast<TrackRunBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not TrackRunBox.", Abort);
    QVERIFY2(box != NULL, "Box is not TrackRunBox");

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
    if(flags & DataOffsetPresent)
    {
        QVERIFY(box->getDataOffset().hasValue());
        QCOMPARE(data_offset, box->getDataOffset().value());
    }
    else
    {
        QCOMPARE(false, box->getDataOffset().hasValue());
    }
    if(flags & FirstSampleFlagsPresent)
    {
        QVERIFY(box->getFirstSampleFlags().hasValue());
        QCOMPARE(first_sample_flags, box->getFirstSampleFlags().value());
    }
    else
    {
        QCOMPARE(false, box->getFirstSampleFlags().hasValue());
    }
    compare_table(table, box->getTable(), flags);
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
