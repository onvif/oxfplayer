#include "surveillanceMetadataSampleEntryBoxTest.h"

#include "surveillanceMetadataSampleEntryBox.hpp"
#include "surveillanceMetadataSampleConfigBox.hpp"

SurveillanceMetadataSampleEntryBoxTest::SurveillanceMetadataSampleEntryBoxTest() :
    QObject()
{
}

void SurveillanceMetadataSampleEntryBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<uint16_t>("data_reference_index");

    QTest::addColumn<uint32_t>("smsc_size");
    QTest::addColumn<uint64_t>("smsc_large_size");
    QTest::addColumn<FourCC>("smsc_fourCC");
    QTest::addColumn<uint8_t>("smsc_version");
    QTest::addColumn<U_UInt24>("smsc_flags");
    QTest::addColumn<uint8_t>("smsc_version2");

    QTest::addColumn<QVector<uint64_t>>("timestamp");

    uint8_t smsc_version_zero(0);
    U_UInt24 smsc_empty_flag;
    smsc_empty_flag.m_value = 0;

    FourCC fourCC = SurveillanceMetadataSampleEntryBox::getFourCC();
    FourCC smscFourCC = SurveillanceMetadataSampleConfigBox::getFourCC();

    QVector<uint64_t> short_table(1, 44789234ull),
            long_table(58, 2347ull);

    BoxSize smsc_box_size( sizeof(uint8_t) );
    BoxSize short_box_size_32( sizeof(uint32_t) + 2 * sizeof(uint16_t) + smsc_box_size.fullbox_size() + short_table.size() * sizeof(uint64_t) ),
            short_box_size_64( sizeof(uint32_t) + 2 * sizeof(uint16_t) + smsc_box_size.fullbox_large_size() + short_table.size() * sizeof(uint64_t) ),
            long_box_size_32( sizeof(uint32_t) + 2 * sizeof(uint16_t) + smsc_box_size.fullbox_size() + long_table.size() * sizeof(uint64_t) ),
            long_box_size_64( sizeof(uint32_t) + 2 * sizeof(uint16_t) + smsc_box_size.fullbox_large_size() + long_table.size() * sizeof(uint64_t) );

    QTest::newRow("Positive test with 32 bit box size and 32 bit child size")
            << short_box_size_32.size() << BoxSize::large_empty() << fourCC
            << uint16_t(234)
            << smsc_box_size.fullbox_size() << BoxSize::large_empty() << smscFourCC << smsc_version_zero << smsc_empty_flag
            << uint8_t(42)
            << short_table;
    QTest::newRow("Positive test with 32 bit box size and 64 bit child size")
            << short_box_size_64.size() << BoxSize::large_empty() << fourCC
            << uint16_t(87)
            << BoxSize::empty() << smsc_box_size.fullbox_large_size() << smscFourCC << smsc_version_zero << smsc_empty_flag
            << uint8_t(26)
            << short_table;
    QTest::newRow("Positive test with 64 bit box size and 32 bit child size")
            << BoxSize::empty() << long_box_size_32.large_size() << fourCC
            << uint16_t(7923)
            << smsc_box_size.fullbox_size() << BoxSize::large_empty() << smscFourCC << smsc_version_zero << smsc_empty_flag
            << uint8_t(7)
            << long_table;
    QTest::newRow("Positive test with 64 bit box size and 64 bit child size")
            << BoxSize::empty() << long_box_size_64.large_size() << fourCC
            << uint16_t(13)
            << BoxSize::empty() << smsc_box_size.fullbox_large_size() << smscFourCC << smsc_version_zero << smsc_empty_flag
            << uint8_t(0)
            << long_table;
    QTest::newRow("Negative test: wrong FourCC code")
            << short_box_size_32.size() << BoxSize::large_empty() << FourCC()
            << uint16_t(234)
            << smsc_box_size.fullbox_size() << BoxSize::large_empty() << smscFourCC << smsc_version_zero << smsc_empty_flag
            << uint8_t(42)
            << short_table;
}

void SurveillanceMetadataSampleEntryBoxTest::readingTest()
{
    uint32_t reserved0 = 0;
    uint16_t reserved1 = 0;

    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint16_t, data_reference_index);

    QFETCH(uint32_t, smsc_size);
    QFETCH(uint64_t, smsc_large_size);
    QFETCH(FourCC, smsc_fourCC);
    QFETCH(uint8_t, smsc_version);
    QFETCH(U_UInt24, smsc_flags);
    QFETCH(uint8_t, smsc_version2);

    QFETCH(QVector<uint64_t>, timestamp);

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
    }

    stream_writer
            .write(reserved0)
            .write(reserved1)
            .write(data_reference_index);

    stream_writer.write(smsc_size).write(smsc_fourCC);
    if((smsc_size == 1) && (smsc_large_size != 0))
    {
        stream_writer.write(smsc_large_size);
    }

    stream_writer
            .write(smsc_version)
            .write(smsc_flags)
            .write(smsc_version2);

    for(int i = 0; i < timestamp.size(); ++i)
    {
        stream_writer
                .write(timestamp[i]);
    }

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    SurveillanceMetadataSampleEntryBox * box = dynamic_cast<SurveillanceMetadataSampleEntryBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not SurveillanceMetadataSampleEntryBox.", Abort);
    QVERIFY2(box != NULL, "Box is not SurveillanceMetadataSampleEntryBox");

    QVERIFY(0 == box->getBoxOffset());

    if((size == 1) && (large_size != 0))
    {
        QVERIFY(large_size == box->getBoxSize());
    }
    else
    {
        QVERIFY(size == box->getBoxSize());
    }
    QCOMPARE(data_reference_index, box->getDataReferenceIndex());
    QCOMPARE(timestamp, box->getTimestamp());
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
