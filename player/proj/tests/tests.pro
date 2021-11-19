#-------------------------------------------------
#
# Project created by QtCreator 2013-06-10T10:29:05
#
#-------------------------------------------------

QT       += core testlib gui network

TARGET = tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH +=  ../../src \
                ../../src/common \
                ../../src/player \
                ../../src/parser \
                ../../src/tests
				
DEFINES += DECODE_USING_QUEUE
#DEFINES += DECODE_WITHOUT_QUEUE

#DEFINES += MEMORY_INFO

INCLUDEPATH += ../../ext/FFMPEG-1.2/include
INCLUDEPATH += ../../ext/PortAudio/include
INCLUDEPATH += ../../ext/OpenSSL-1.0.1/include

SOURCES += main.cpp \
    ../../src/common/fragmentInfo.cpp \
    ../../src/parser/basic/box.cpp \
    ../../src/parser/basic/mandatoryBox.cpp \
    ../../src/parser/basic/unknownBox.cpp \
    ../../src/parser/boxFactory.cpp \
    ../../src/parser/certificateStorage.cpp \
    ../../src/parser/consistencyChecker.cpp \
    ../../src/parser/fourcc.cpp \
    ../../src/parser/fragmentExtractor.cpp \
    ../../src/parser/mediaParser.cpp \
    ../../src/parser/oxfverifier.cpp \
    ../../src/parser/signatureExtractor.cpp \
    ../../src/parser/validatorISO.cpp \
    ../../src/parser/validatorOXF.cpp \
    ../../src/parser/validatorSurveillance.cpp \
	../../src/tests/afIdentificationBoxTest.cpp \
    ../../src/tests/cameraMicrophoneIdentificationBoxTest.cpp \
    ../../src/tests/certificateBoxTest.cpp \
    ../../src/tests/engineTest.cpp \
    ../../src/tests/compactSampleSizeBoxTest.cpp \
    ../../src/tests/movieHeaderBoxTest.cpp \
    ../../src/tests/movieExtendsHeaderBoxTest.cpp \
    ../../src/tests/mediaHeaderBoxTest.cpp \
    ../../src/tests/additionalUserInformationBoxTest.cpp \
    ../../src/tests/surveillanceExportBoxTest.cpp \
    ../../src/tests/editListBoxTest.cpp \
    ../../src/tests/trackFragmentHeaderBoxTest.cpp \
    ../../src/tests/sampleDependencyTypeBoxTest.cpp \
    ../../src/tests/sampleSizeBoxTest.cpp \
    ../../src/tests/signatureBoxTest.cpp \
    ../../src/tests/signatureConfigurationBoxTest.cpp \
    ../../src/tests/surveillanceMetadataSampleConfigBoxTest.cpp \
    ../../src/tests/trackFragmentRandomAccessBoxTest.cpp \
    ../../src/tests/trackHeaderBoxTest.cpp \
    ../../src/tests/trackRunBoxTest.cpp \
    ../../src/tests/certificateSSLTest.cpp \
    ../../src/tests/surveillanceMetadataSampleEntryBoxTest.cpp

HEADERS  += \
    ../../src/common/crosscompilation_cxx11.h \
    ../../src/common/crosscompilation_inttypes.h \
    ../../src/common/defines.h \
    ../../src/common/enums.h \
    ../../src/common/ffmpeg.h \
    ../../src/common/segmentInfo.h \
    ../../src/common/ONVIFSignInfo.h \
    ../../src/common/queue.h \
    ../../src/common/signingInformation.h \
    ../../src/common/types.h \
    ../../src/parser/additionalUserInformation.hpp \
    ../../src/parser/afIdentificationBox.hpp \
    ../../src/parser/basic/box.h \
    ../../src/parser/basic/contentBox.hpp \
    ../../src/parser/basic/dataBox.hpp \
    ../../src/parser/basic/fileBox.hpp \
    ../../src/parser/basic/fullBox.hpp \
    ../../src/parser/basic/mandatoryBox.h \
    ../../src/parser/basic/mixin/children.hpp \
    ../../src/parser/basic/mixin/data.hpp \
    ../../src/parser/basic/mixin/table.hpp \
    ../../src/parser/basic/superBox.hpp \
    ../../src/parser/basic/superFullBox.hpp \
    ../../src/parser/basic/tableBox.hpp \
    ../../src/parser/basic/unknownBox.h \
    ../../src/parser/boxFactory.h \
    ../../src/parser/cameraMicrophoneIdentificationBox.hpp \
    ../../src/parser/certificateBox.hpp \
    ../../src/parser/certificateStorage.h \
    ../../src/parser/compactSampleSizeBox.hpp \
    ../../src/parser/consistencyChecker.h \
    ../../src/parser/editListBox.hpp \
    ../../src/parser/fileTypeBox.hpp \
    ../../src/parser/fourcc.h \
    ../../src/parser/fragmentExtractor.h \
    ../../src/parser/helpers/endian.hpp \
    ../../src/parser/helpers/is_a.hpp \
    ../../src/parser/helpers/istream.hpp \
    ../../src/parser/helpers/optional.hpp \
    ../../src/parser/helpers/property.hpp \
    ../../src/parser/helpers/uint24.hpp \
    ../../src/parser/mediaHeaderBox.hpp \
    ../../src/parser/mediaParser.h \
    ../../src/parser/movieExtendsHeaderBox.hpp \
    ../../src/parser/movieHeaderBox.hpp \
    ../../src/parser/oxfverifier.h \
    ../../src/parser/sampleDependencyTypeBox.hpp \
    ../../src/parser/sampleSizeBox.hpp \
    ../../src/parser/signatureBox.hpp \
    ../../src/parser/signatureConfigurationBox.hpp \
    ../../src/parser/signatureExtractor.h \
    ../../src/parser/surveillanceExportBox.hpp \
    ../../src/parser/surveillanceMetadataSampleConfigBox.hpp \
    ../../src/parser/surveillanceMetadataSampleEntryBox.hpp \
    ../../src/parser/templateContentBoxes.hpp \
    ../../src/parser/templateFullBoxes.hpp \
    ../../src/parser/templateSuperBoxes.hpp \
    ../../src/parser/templateSuperFullBoxes.hpp \
    ../../src/parser/templateTableBoxes.hpp \
    ../../src/parser/trackFragmentHeaderBox.hpp \
    ../../src/parser/trackFragmentRandomAccessBox.hpp \
    ../../src/parser/trackHeaderBox.hpp \
    ../../src/parser/trackRunBox.hpp \
    ../../src/parser/validatorISO.h \
    ../../src/parser/validatorOXF.h \
    ../../src/parser/validatorSurveillance.h \
    ../../src/tests/afIdentificationBoxTest.h \
    ../../src/tests/boxTestsCommon.h \
    ../../src/tests/cameraMicrophoneIdentificationBoxTest.h \
    ../../src/tests/certificateBoxTest.h \
    ../../src/tests/engineTest.h \
    ../../src/tests/ostream.hpp \
    ../../src/tests/compactSampleSizeBoxTest.h \
    ../../src/tests/movieHeaderBoxTest.h \
    ../../src/tests/movieExtendsHeaderBoxTest.h \
    ../../src/tests/mediaHeaderBoxTest.h \
    ../../src/tests/additionalUserInformationBoxTest.h \
    ../../src/tests/surveillanceExportBoxTest.h \
    ../../src/tests/editListBoxTest.h \
    ../../src/tests/trackFragmentHeaderBoxTest.h \
    ../../src/tests/sampleDependencyTypeBoxTest.h \
    ../../src/tests/sampleSizeBoxTest.h \
    ../../src/tests/signatureBoxTest.h \
    ../../src/tests/signatureConfigurationBoxTest.h \
    ../../src/tests/surveillanceMetadataSampleConfigBoxTest.h \
    ../../src/tests/trackFragmentRandomAccessBoxTest.h \
    ../../src/tests/trackHeaderBoxTest.h \
    ../../src/tests/trackRunBoxTest.h \
    ../../src/tests/certificateSSLTest.h \
    ../../src/tests/surveillanceMetadataSampleEntryBoxTest.h

win32:LIBS += -L../../ext/FFMPEG-1.2/lib/Windows -L../../ext/PortAudio/lib/Windows -L../../ext/OpenSSL-1.0.1/lib
unix:LIBS += -L../../ext/FFMPEG-1.2/lib/Unix -L../../ext/PortAudio/lib/Unix -L/usr/lib/i386-linux-gnu

LIBS += -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale -lssl -lcrypto

win32:LIBS += -lportaudio.dll
unix:LIBS += -lportaudio
