#include "trackHeaderBoxTest.h"

#include "trackHeaderBox.hpp"

Q_DECLARE_METATYPE(TrackHeaderMatrix)

TrackHeaderBoxTest::TrackHeaderBoxTest() :
    QObject()
{
}

void TrackHeaderBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<uint8_t>("version");
    QTest::addColumn<U_UInt24>("flag");
    QTest::addColumn<uint64_t>("creation_time");
    QTest::addColumn<uint64_t>("modification_time");
    QTest::addColumn<uint32_t>("track_id");
    QTest::addColumn<uint64_t>("duration");
    QTest::addColumn<int16_t>("layer");
    QTest::addColumn<int16_t>("alternate_group");
    QTest::addColumn<int16_t>("volume");
    QTest::addColumn<TrackHeaderMatrix>("matrix");
    QTest::addColumn<uint32_t>("width");
    QTest::addColumn<uint32_t>("height");

    uint8_t version_zero(0),
            version_one(1);
    U_UInt24 flag;
    flag.m_value = 0;

    TrackHeaderMatrix matrix;
    matrix[0] = matrix[4] = 0x00010000;
    matrix[1] = matrix[2] = matrix[3] = matrix[5], matrix[6] = matrix[7] = 0;
    matrix[8] = 0x40000000;

    FourCC fourCC = TrackHeaderBox::getFourCC();

    BoxSize v0_size( 9 * sizeof(uint32_t) + 4 * sizeof(int16_t) + 9 * sizeof(int32_t) ),
            v1_size( 3 * sizeof(uint64_t) + 6 * sizeof(uint32_t) + 4 * sizeof(int16_t) + 9 * sizeof(int32_t) );

    QTest::newRow("Positive test with 32 bit box size and version 0")
            << v0_size.fullbox_size() << BoxSize::large_empty() << fourCC << version_zero << flag
            << datetime_write() << datetime_write() << 879u << 38638ull << int16_t(876) << int16_t(13) << int16_t(100) << matrix << 320u << 240u;
    QTest::newRow("Positive test with 32 bit box size and version 1")
            << v1_size.fullbox_size() << BoxSize::large_empty() << fourCC  << version_one << flag
            << datetime_write() << datetime_write() << 766u << 67385ull << int16_t(125) << int16_t(-713) << int16_t(0) << matrix << 799u << 599u;
    QTest::newRow("Positive test with 64 bit box size and version 0")
            << BoxSize::empty() << v0_size.fullbox_large_size() << fourCC  << version_zero << flag
            << datetime_write() << datetime_write() << 12u << 6344ull << int16_t(5433) << int16_t(11233) << int16_t(30) << matrix << 1u << 1u;
    QTest::newRow("Positive test with 64 bit box size and version 1")
            << BoxSize::empty() << v1_size.fullbox_large_size() << fourCC  << version_one << flag
            << datetime_write() << datetime_write() << 87u << 54234ull << int16_t(534) << int16_t(-5653) << int16_t(-100) << matrix << 1280u << 240u;
    QTest::newRow("Negative test: wrong FourCC code")
            << v0_size.fullbox_size() << BoxSize::large_empty() << FourCC()  << version_zero << flag
            << datetime_write() << datetime_write() << 879u << 38638ull << int16_t(876) << int16_t(13) << int16_t(100) << matrix << 0u << 0u;
}

void TrackHeaderBoxTest::readingTest()
{
    uint32_t reserved0 = 0;
    uint64_t reserved1 = 0;
    uint16_t reserved2 = 0;

    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(uint64_t, creation_time);
    QFETCH(uint64_t, modification_time);
    QFETCH(uint32_t, track_id);
    QFETCH(uint64_t, duration);
    QFETCH(int16_t, layer);
    QFETCH(int16_t, alternate_group);
    QFETCH(int16_t, volume);
    QFETCH(TrackHeaderMatrix, matrix);
    QFETCH(uint32_t, width);
    QFETCH(uint32_t, height);

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
    }

    stream_writer
            .write(version)
            .write(flag);

    if(version == 0)
    {
        stream_writer
                .write(uint32_t(creation_time))
                .write(uint32_t(modification_time))
                .write(track_id)
                .write(reserved0)
                .write(uint32_t(duration));
    }
    else
    {
        stream_writer
                .write(creation_time)
                .write(modification_time)
                .write(track_id)
                .write(reserved0)
                .write(duration);
    }

    stream_writer
            .write(reserved1)
            .write(layer)
            .write(alternate_group)
            .write(volume)
            .write(reserved2);

    for(uint32_t i = 0; i < matrix.size(); ++i)
    {
        stream_writer.write(matrix[i]);
    }

    stream_writer
            .write(width)
            .write(height);

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    TrackHeaderBox * box = dynamic_cast<TrackHeaderBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not TrackHeaderBox.", Abort);
    QVERIFY2(box != NULL, "Box is not TrackHeaderBox");

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
    QCOMPARE(datetime_read(creation_time), box->getCreationTime());
    QCOMPARE(datetime_read(modification_time), box->getModificationTime());
    QCOMPARE(track_id, box->getTrackID());
    QCOMPARE(duration, box->getDuration());
    QCOMPARE(layer, box->getLayer());
    QCOMPARE(alternate_group, box->getAlternateGroup());
    QCOMPARE(volume, box->getVolume());
    QCOMPARE(matrix, box->getMatrix());
    QCOMPARE(width, box->getWidth());
    QCOMPARE(height, box->getHeight());
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
