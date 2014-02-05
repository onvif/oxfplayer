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
#include "movieHeaderBoxTest.h"
#include "movieHeaderBox.hpp"


void MovieHeaderBoxTest32::readingTest_data() { localTest_Data();  }
void MovieHeaderBoxTest32::readingTest()      { localTest(); }

void MovieHeaderBoxTest64::readingTest_data() { localTest_Data(); }
void MovieHeaderBoxTest64::readingTest()      { localTest(); }


/*
 if (version==1) {
    unsigned int(64)  creation_time;
    unsigned int(64)  modification_time;
    unsigned int(32)  timescale;
    unsigned int(64)  duration;
  } else { // version==0
    unsigned int(32)  creation_time;
    unsigned int(32)  modification_time;
    unsigned int(32)  timescale;
    unsigned int(32)  duration;
  }
  template int(32)  rate = 0x00010000; // typically 1.0
  template int(16)  volume = 0x0100;  // typically, full volume
  const bit(16)  reserved = 0;
  const unsigned int(32)[2]  reserved = 0;
  template int(32)[9]  matrix =
    { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };
    // Unity matrix
  bit(32)[6]  pre_defined = 0;
  unsigned int(32)  next_track_ID;

*/
template< typename DATA_TYPE >
void MovieHeaderBoxTestLocal<DATA_TYPE>::localTest_Data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<unsigned char>("version");
    QTest::addColumn<U_UInt24>("flag");

    QTest::addColumn<DATA_TYPE>("creation_time");
    QTest::addColumn<DATA_TYPE>("modification_time");
    QTest::addColumn<uint32_t>("timescale");
    QTest::addColumn<DATA_TYPE>("duration");

    U_UInt24 flag;
    unsigned char version_zero(0);
    unsigned char version_one(1);

    BoxSize size(sizeof(DATA_TYPE)*3 + sizeof(uint32_t) * 21);
    FourCC fourCC = MovieHeaderBox::getFourCC();

    DATA_TYPE  t1 = datetime_write();
    DATA_TYPE  t2 = datetime_write();
    uint32_t   t3 = 10;
    DATA_TYPE  t4 = 20;

    if (sizeof(DATA_TYPE) == sizeof(uint32_t))
    {
        QTest::newRow("Positive test with 32 bit box size")
            << size.fullbox_size() << BoxSize::large_empty() << fourCC
            << version_zero << flag << t1 << t2 << t3 << t4;

        QTest::newRow("Negative test: wrong FourCC code")
            << size.fullbox_size() << BoxSize::large_empty() << FourCC()
            << version_zero << flag << t1 << t2 << t3 << t4;
    } else {

        QTest::newRow("Positive test with 64 bit box size")
                << BoxSize::empty() << size.fullbox_large_size()  << fourCC
                << version_one << flag << t1 << t2 << t3 << t4;

        QTest::newRow("Negative test: wrong FourCC code")
            << BoxSize::empty() << size.fullbox_large_size() << FourCC()
            << version_one << flag << t1 << t2 << t3 << t4;
    }
}

template< typename DATA_TYPE >
void MovieHeaderBoxTestLocal<DATA_TYPE>::localTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(DATA_TYPE, modification_time);
    QFETCH(uint32_t, timescale);
    QFETCH(DATA_TYPE, duration);
    QFETCH(DATA_TYPE, creation_time);

    // common data
    int32_t  rate = 0x00010000;
    int16_t   volume = 0x0100;
    uint16_t  reserved = 0;
    uint32_t  res1 = 0;
    uint32_t  res2 = 0;

    uint32_t  matrix[9] = {0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };
    uint32_t  res3[6] = {0,0,0,0,0,0};
    uint32_t  trackid = 10;

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
    }
    stream_writer.write(version);
    stream_writer.write(flag);
    stream_writer.write(creation_time);
    stream_writer.write(modification_time);
    stream_writer.write(timescale);
    stream_writer.write(duration);
    stream_writer.write(rate);
    stream_writer.write(volume);
    stream_writer.write(reserved);
    stream_writer.write(res1);
    stream_writer.write(res2);
    for (int j=0; j < 9; ++j)
        stream_writer.write(matrix[j]);
    for (int j=0; j < 6; ++j)
        stream_writer.write(res3[j]);
    stream_writer.write(trackid);

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);

    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    MovieHeaderBox * box = dynamic_cast<MovieHeaderBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not MovieHeaderBox.", Abort);
    QVERIFY2(box != NULL, "Box is not MovieHeaderBox");

    QVERIFY(0 == box->getBoxOffset());

    QDateTime mtime = datetime_read(modification_time);
    QVERIFY(mtime == box->getModificationTime());

    mtime = datetime_read(creation_time);
    QVERIFY(mtime == box->getCreationTime());

    QVERIFY(timescale == box->getTimeScale());
    QVERIFY(duration == box->getDuration());

    QVERIFY(1 == box->getRate());
    QVERIFY(1 == box->getVolume());

    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
