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

#include "trackFragmentHeaderBoxTest.h"

#include "trackFragmentHeaderBox.hpp"

QString prepare_flag_names(TrackFragmentFlags flags)
{
    QStringList flag_names;
    if(flags & BaseDataOffsetPresent)
    {
        flag_names << "data offset";
    }
    if(flags & SampleDescriptionIndexPresent)
    {
        flag_names << "sample description index";
    }
    if(flags & DefaultSampleDurationPresent)
    {
        flag_names << "default sample duration";
    }
    if(flags & DefaultSampleSizePresent)
    {
        flag_names << "default sample size";
    }
    if(flags & DefaultSampleFlagsPresent)
    {
        flag_names << "default sample flags";
    }
    if(flag_names.isEmpty())
    {
        flag_names << "no flags";
    }
    return flag_names.join(", ");
}

BoxSize calculate_box_size(TrackFragmentFlags flags)
{
    uint32_t size = sizeof(uint32_t);
    if(flags & BaseDataOffsetPresent)
    {
        size += sizeof(uint64_t);
    }
    if(flags & SampleDescriptionIndexPresent)
    {
        size += sizeof(uint32_t);
    }
    if(flags & DefaultSampleDurationPresent)
    {
        size += sizeof(uint32_t);
    }
    if(flags & DefaultSampleSizePresent)
    {
        size += sizeof(uint32_t);
    }
    if(flags & DefaultSampleFlagsPresent)
    {
        size += sizeof(uint32_t);
    }
    return BoxSize(size);
}

TrackFragmentHeaderBoxTest::TrackFragmentHeaderBoxTest() :
    QObject()
{
}

void TrackFragmentHeaderBoxTest::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<uint8_t>("version");
    QTest::addColumn<U_UInt24>("flag");
    QTest::addColumn<uint32_t>("track_id");
    QTest::addColumn<uint64_t>("base_data_offset");
    QTest::addColumn<uint32_t>("sample_description_index");
    QTest::addColumn<uint32_t>("default_sample_duration");
    QTest::addColumn<uint32_t>("default_sample_size");
    QTest::addColumn<uint32_t>("default_sample_flags");

    uint8_t version_zero = 0;
    U_UInt24 flag_empty;
    flag_empty.m_value = 0;

    FlagStateGenerator<TrackFragmentFlags> fsg;
    fsg.add(BaseDataOffsetPresent).add(SampleDescriptionIndexPresent).add(DefaultSampleDurationPresent).add(DefaultSampleSizePresent).add(DefaultSampleFlagsPresent);

    FourCC fourCC = TrackFragmentHeaderBox::getFourCC();

    QString short_test_name("Positive test with 32 bit box size and %1 present");
    QString long_test_name("Positive test with 64 bit box size and %1 present");

    bool short_test = true;
    for(TrackFragmentFlags flags = fsg.begin(); flags <= fsg.end(); flags = fsg.next())
    {
        BoxSize box_size(calculate_box_size(flags));
        U_UInt24 flag;
        flag.m_value = uint32_t(flags);
        if(short_test)
        {
            QTest::newRow(short_test_name.arg(prepare_flag_names(flags)).toLatin1().data())
                    << box_size.fullbox_size() << BoxSize::large_empty() << fourCC << version_zero << flag
                    << 4664u << 4234ull << 3247u << 24837u << 5345u << 23442u;
        }
        else
        {
            QTest::newRow(long_test_name.arg(prepare_flag_names(flags)).toLatin1().data())
                    << BoxSize::empty() << box_size.fullbox_large_size() << fourCC << version_zero << flag
                    << 1u << 434ull << 247u << 432u << 0u << 7u;
        }
        short_test = !short_test;
    }

    QTest::newRow("Negative test: wrong FourCC code")
            << BoxSize(sizeof(uint32_t)).fullbox_size() << BoxSize::large_empty() << FourCC()<< version_zero << flag_empty
            << 534u << 0ull << 0u << 0u << 0u << 0u;
}

void TrackFragmentHeaderBoxTest::readingTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(uint32_t, track_id);
    QFETCH(uint64_t, base_data_offset);
    QFETCH(uint32_t, sample_description_index);
    QFETCH(uint32_t, default_sample_duration);
    QFETCH(uint32_t, default_sample_size);
    QFETCH(uint32_t, default_sample_flags);

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
            .write(track_id);

    if(flag.m_value & BaseDataOffsetPresent)
        stream_writer.write(base_data_offset);
    if(flag.m_value & SampleDescriptionIndexPresent)
        stream_writer.write(sample_description_index);
    if(flag.m_value & DefaultSampleDurationPresent)
        stream_writer.write(default_sample_duration);
    if(flag.m_value & DefaultSampleSizePresent)
        stream_writer.write(default_sample_size);
    if(flag.m_value & DefaultSampleFlagsPresent)
        stream_writer.write(default_sample_flags);

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    TrackFragmentHeaderBox * box = dynamic_cast<TrackFragmentHeaderBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not TrackFragmentHeaderBox.", Abort);
    QVERIFY2(box != NULL, "Box is not TrackFragmentHeaderBox");

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
    if(flag.m_value & BaseDataOffsetPresent)
    {
        QVERIFY(box->getBaseDataOffset().hasValue());
        QCOMPARE(base_data_offset, box->getBaseDataOffset().value());
    }
    else
    {
        QCOMPARE(false, box->getBaseDataOffset().hasValue());
    }
    if(flag.m_value & SampleDescriptionIndexPresent)
    {
        QVERIFY(box->getSampleDescriptionIndex().hasValue());
        QCOMPARE(sample_description_index, box->getSampleDescriptionIndex().value());
    }
    else
    {
        QCOMPARE(false, box->getSampleDescriptionIndex().hasValue());
    }
    if(flag.m_value & DefaultSampleDurationPresent)
    {
        QVERIFY(box->getDefaultSampleDuration().hasValue());
        QCOMPARE(default_sample_duration, box->getDefaultSampleDuration().value());
    }
    else
    {
        QCOMPARE(false, box->getDefaultSampleDuration().hasValue());
    }
    if(flag.m_value & DefaultSampleSizePresent)
    {
        QVERIFY(box->getDefaultSampleSize().hasValue());
        QCOMPARE(default_sample_size, box->getDefaultSampleSize().value());
    }
    else
    {
        QCOMPARE(false, box->getDefaultSampleSize().hasValue());
    }
    if(flag.m_value & DefaultSampleFlagsPresent)
    {
        QVERIFY(box->getDefaultSampleFlags().hasValue());
        QCOMPARE(default_sample_flags, box->getDefaultSampleFlags().value());
    }
    else
    {
        QCOMPARE(false, box->getDefaultSampleFlags().hasValue());
    }
    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
