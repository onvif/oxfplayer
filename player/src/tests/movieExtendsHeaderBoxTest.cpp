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
#include "movieExtendsHeaderBoxTest.h"
#include "movieExtendsHeaderBox.hpp"

void MovieExtendsHeaderBoxTest32::readingTest_data() { localTest_data(); }
void MovieExtendsHeaderBoxTest32::readingTest()      { localTest(); }

void MovieExtendsHeaderBoxTest64::readingTest_data() { localTest_data(); }
void MovieExtendsHeaderBoxTest64::readingTest()      { localTest(); }

template< typename DATA_TYPE >
void MovieExtendsHeaderBoxTest<DATA_TYPE>::localTest_data()
//void MovieExtendsHeaderBoxTest32::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<unsigned char>("version");
    QTest::addColumn<U_UInt24>("flag");
    QTest::addColumn<DATA_TYPE>("fragment_duration");

    BoxSize size(sizeof(DATA_TYPE));

    U_UInt24 flag;
    unsigned char version_zero(0);
    unsigned char version_one(1);

    DATA_TYPE d1 = 10;

    FourCC fourCC = MovieExtendsHeaderBox::getFourCC();

    QString  text = QString("Positive test with %1 bit box").arg(sizeof(DATA_TYPE)*8);

    if (sizeof(DATA_TYPE) == sizeof(uint32_t))
    {
      QTest::newRow(text.toLatin1())
            << size.fullbox_size() << BoxSize::large_empty() << fourCC
            << version_zero << flag << d1;

      QTest::newRow("Negative test: wrong FourCC code")
            << size.fullbox_size() << BoxSize::large_empty()  << FourCC()
            << version_zero << flag << d1 ;
    }
    else
    {
      QTest::newRow(text.toLatin1())
            << BoxSize::empty() << size.fullbox_large_size()  << fourCC
            << version_one << flag << d1;

      QTest::newRow("Negative test: wrong FourCC code")
            << BoxSize::empty() << size.fullbox_large_size()  << FourCC()
            << version_one << flag << d1 ;
    }
}

template< typename DATA_TYPE >
void MovieExtendsHeaderBoxTest<DATA_TYPE>::localTest()
//void MovieExtendsHeaderBoxTest32::readingTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(DATA_TYPE, fragment_duration);
    //DATA_TYPE d1 = 10;

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
        stream_writer.write(version);
        stream_writer.write(flag);
        stream_writer.write(fragment_duration);
    }
    else
    {
        stream_writer.write(version);
        stream_writer.write(flag);
        stream_writer.write(fragment_duration);
    }

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);
    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    MovieExtendsHeaderBox * box = dynamic_cast<MovieExtendsHeaderBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not MovieExtendsHeaderBox.", Abort);
    QVERIFY2(box != NULL, "Box is not MovieExtendsHeaderBox");

    QVERIFY(0 == box->getBoxOffset());

    if((size == 1) && (large_size != 0))
    {
        QVERIFY(large_size == box->getBoxSize());
        QCOMPARE((uint64_t)fragment_duration, box->getFragmentDuration());
    }
    else
    {
        QVERIFY(size == box->getBoxSize());
        QCOMPARE((uint64_t)fragment_duration, box->getFragmentDuration());
    }

    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}

#if 0
void MovieExtendsHeaderBoxTest64::readingTest_data()
{
    QTest::addColumn<uint32_t>("size");
    QTest::addColumn<uint64_t>("large_size");
    QTest::addColumn<FourCC>("fourCC");
    QTest::addColumn<unsigned char>("version");
    QTest::addColumn<U_UInt24>("flag");
    QTest::addColumn<uint64_t>("fragment_duration");

    BoxSize size(sizeof(uint64_t));

    U_UInt24 flag;
    unsigned char version_zero(0);

    uint64_t d1 = 10;

    FourCC fourCC = MovieExtendsHeaderBox::getFourCC();

    QString  text = QString("Positive test with %1 bit box").arg(sizeof(uint64_t)*8);


    QTest::newRow(text.toLatin1())
            << BoxSize::empty() << size.fullbox_large_size()  << fourCC
            << version_zero << flag << d1;

    QTest::newRow("Negative test: wrong FourCC code")
            << BoxSize::empty() << size.fullbox_large_size()  << FourCC()
            << version_zero << flag << d1 ;

}

//template< typename DATA_TYPE >
//void MovieExtendsHeaderBoxTest<DATA_TYPE>::localTest()
void MovieExtendsHeaderBoxTest64::readingTest()
{
    QFETCH(uint32_t, size);
    QFETCH(uint64_t, large_size);
    QFETCH(FourCC, fourCC);
    QFETCH(uint8_t, version);
    QFETCH(U_UInt24, flag);
    QFETCH(uint64_t, fragment_duration);
    //DATA_TYPE d1 = 10;

    std::shared_ptr<std::stringstream> stream_ptr(new std::stringstream());

    StreamWriter stream_writer(*stream_ptr);

    stream_writer.write(size).write(fourCC);

    if((size == 1) && (large_size != 0))
    {
        stream_writer.write(large_size);
        stream_writer.write(version);
        stream_writer.write(flag);
        stream_writer.write(fragment_duration);
    }
    else
    {
        stream_writer.write(version);
        stream_writer.write(flag);
        stream_writer.write(fragment_duration);
    }

    stream_ptr->seekg(0);

    LimitedStreamReader stream_reader(stream_ptr);
    FileBox file;

    bool has_more_data = BoxFactory::instance().parseBox(stream_reader, &file);
    QVERIFY2(file.getChildren().size() == 1, "Box was not created");

    MovieExtendsHeaderBox * box = dynamic_cast<MovieExtendsHeaderBox*>(file.getChildren()[0]);

    QEXPECT_FAIL("Negative test: wrong FourCC code", "Box should be not MovieExtendsHeaderBox.", Abort);
    QVERIFY2(box != NULL, "Box is not MovieExtendsHeaderBox");

    QVERIFY(0 == box->getBoxOffset());

    if((size == 1) && (large_size != 0))
    {
        QVERIFY(large_size == box->getBoxSize());
        QCOMPARE((uint64_t)fragment_duration, box->getFragmentDuration());
    }
    else
    {
        QVERIFY(size == box->getBoxSize());
        QCOMPARE((uint64_t)fragment_duration, box->getFragmentDuration());
    }

    QVERIFY(Box::SizeOk == box->getSizeError());
    QVERIFY(has_more_data == false);
}
#endif
