#-------------------------------------------------
#
# Project created by QtCreator 2013-05-21T22:35:29
#
#-------------------------------------------------

QT       += core gui widgets multimedia network

TARGET = ONVIFPlayer

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH +=  ../../src \
                ../../src/common \
                ../../src/player \
                ../../src/playerUI \
                ../../src/parser \
                ../../src/parserUI

DEFINES += DECODE_USING_QUEUE
#DEFINES += DECODE_WITHOUT_QUEUE

#DEFINES += MEMORY_INFO

win32:DEFINES += WIN32
unix:DEFINES  += UNIX

INCLUDEPATH += ../../ext/FFMPEG-1.2/include
INCLUDEPATH += ../../ext/PortAudio/include
INCLUDEPATH += ../../ext/OpenSSL-1.0.1/include

SOURCES += ../../src/main.cpp \
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
    ../../src/parserUI/certificateStorageDialog.cpp \
    ../../src/parserUI/parserWidget.cpp \
    ../../src/parserUI/verifyerdialog.cpp \
    ../../src/player/audioContext.cpp \
    ../../src/player/audioPlayback.cpp \
    ../../src/player/avFrameWrapper.cpp \
    ../../src/player/controller.cpp \
    ../../src/player/engine.cpp \
    ../../src/player/mainContext.cpp \
    ../../src/player/nonqueuedAudioDecoder.cpp \
    ../../src/player/nonqueuedVideoDecoder.cpp \
    ../../src/player/portAudioPlayback.cpp \
    ../../src/player/portAudioThread.cpp \
    ../../src/player/queuedAudioDecoder.cpp \
    ../../src/player/queuedVideoDecoder.cpp \
    ../../src/player/syncThread.cpp \ 
    ../../src/player/videoContext.cpp \
    ../../src/player/videoPlayback.cpp \
    ../../src/playerUI/clickableSlider.cpp \
    ../../src/playerUI/controlsWidget.cpp \
    ../../src/playerUI/fragmentListWidget.cpp \
    ../../src/playerUI/fullscreenPlayerWidget.cpp \
    ../../src/playerUI/movingOutArea.cpp \
    ../../src/playerUI/playerWidget.cpp \
    ../../src/playerUI/videoFrameWidget.cpp

HEADERS  += \
    ../../src/common/crosscompilation_cxx11.h \
    ../../src/common/crosscompilation_inttypes.h \
    ../../src/common/defines.h \
    ../../src/common/enums.h \
    ../../src/common/ffmpeg.h \
    ../../src/common/fragmentInfo.h \
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
    ../../src/parserUI/certificateStorageDialog.h \
    ../../src/parserUI/parserWidget.h \
    ../../src/parserUI/verifyerdialog.h \
    ../../src/player/audioContext.h \
    ../../src/player/audioPlayback.h \
    ../../src/player/avFrameWrapper.h \
    ../../src/player/basePlayback.h \
    ../../src/player/controller.h \
    ../../src/player/decoder.h \
    ../../src/player/engine.h \
    ../../src/player/mainContext.h \
    ../../src/player/nonqueuedAudioDecoder.h \
    ../../src/player/nonqueuedDecoder.h \
    ../../src/player/nonqueuedVideoDecoder.h \
    ../../src/player/portAudioPlayback.h \
    ../../src/player/portAudioThread.h \
    ../../src/player/queuedAudioDecoder.h \
    ../../src/player/queuedDecoder.h \
    ../../src/player/queuedVideoDecoder.h \
    ../../src/player/syncThread.h \ 
    ../../src/player/videoContext.h \
    ../../src/player/videoPlayback.h \
    ../../src/playerUI/clickableSlider.h \
    ../../src/playerUI/controlsWidget.h \
    ../../src/playerUI/fragmentListWidget.h \
    ../../src/playerUI/fullscreenPlayerWidget.h \
    ../../src/playerUI/movingOutArea.h \
    ../../src/playerUI/playerWidget.h \
    ../../src/playerUI/playerWidgetInterface.h \
    ../../src/playerUI/videoFrameWidget.h

FORMS    += \
    ../../src/parserUI/certificateStorageDialog.ui \
    ../../src/parserUI/parserWidget.ui \
    ../../src/parserUI/verifyerdialog.ui \ 
    ../../src/playerUI/controlsWidget.ui \
    ../../src/playerUI/playerWidget.ui

RESOURCES += ../../src/resources/resources.qrc

win32:RC_FILE = ../../src/resources/ONVIFPlayer.rc

win32:LIBS += -L../../ext/FFMPEG-1.2/lib/Windows -L../../ext/PortAudio/lib/Windows -L../../ext/OpenSSL-1.0.1/lib
unix:LIBS += -L../../ext/FFMPEG-1.2/lib/Unix -L../../ext/PortAudio/lib/Unix -L/usr/lib/i386-linux-gnu

LIBS += -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale -lssl -lcrypto

win32:LIBS += -lportaudio.dll
unix:LIBS += -lportaudio

