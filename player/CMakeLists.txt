cmake_minimum_required(VERSION 3.28.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(ONVIFPlayer CXX)

find_package(Qt6 COMPONENTS Widgets)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################

set(PROJECT_NAME ONVIFPlayer)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "src/common/segmentInfo.cpp"
    "src/main.cpp"
    "src/parser/segmentExtractor.cpp"
#    "src/resources/movie_frame.png"
#    "Debug/moc_segmentExtractor.cpp"
#    "GeneratedFiles/qrc_resources.cpp"
#    "Release/moc_segmentExtractor.cpp"
)
source_group("" FILES ${no_group_source_files})

set(boxes
    "src/parser/afIdentificationBox.hpp"
    "src/parser/basic/contentBox.hpp"
    "src/parser/basic/dataBox.hpp"
    "src/parser/basic/fileBox.hpp"
    "src/parser/basic/fullBox.hpp"
    "src/parser/basic/superBox.hpp"
    "src/parser/basic/superFullBox.hpp"
    "src/parser/basic/tableBox.hpp"
    "src/parser/cameraMicrophoneIdentificationBox.hpp"
    "src/parser/certificateBox.hpp"
    "src/parser/compactSampleSizeBox.hpp"
    "src/parser/correctstarttimebox.hpp"
    "src/parser/editListBox.hpp"
    "src/parser/fileTypeBox.hpp"
    "src/parser/mediaHeaderBox.hpp"
    "src/parser/movieExtendsHeaderBox.hpp"
    "src/parser/movieHeaderBox.hpp"
    "src/parser/sampleDependencyTypeBox.hpp"
    "src/parser/sampleSizeBox.hpp"
    "src/parser/signatureBox.hpp"
    "src/parser/signatureConfigurationBox.hpp"
    "src/parser/surveillanceExportBox.hpp"
    "src/parser/surveillanceMetadataSampleConfigBox.hpp"
    "src/parser/surveillanceMetadataSampleEntryBox.hpp"
    "src/parser/templateContentBoxes.hpp"
    "src/parser/templateFullBoxes.hpp"
    "src/parser/templateSuperBoxes.hpp"
    "src/parser/templateSuperFullBoxes.hpp"
    "src/parser/templateTableBoxes.hpp"
    "src/parser/trackFragmentHeaderBox.hpp"
    "src/parser/trackFragmentRandomAccessBox.hpp"
    "src/parser/trackHeaderBox.hpp"
    "src/parser/trackRunBox.hpp"
)
source_group("boxes" FILES ${boxes})

set(external
    "ext/pugixml/src/pugixml.cpp"
)
source_group("external" FILES ${external})

set(player
    "src/player/audioContext.cpp"
    "src/player/audioPlayback.cpp"
    "src/player/avFrameWrapper.cpp"
    "src/player/controller.cpp"
    "src/player/engine.cpp"
    "src/player/portAudioPlayback.cpp"
    "src/player/portAudioThread.cpp"
    "src/player/queuedAudioDecoder.cpp"
    "src/player/queuedMetadataDecoder.cpp"
    "src/player/queuedVideoDecoder.cpp"
    "src/player/streamReader.cpp"
    "src/player/syncThread.cpp"
    "src/player/videoContext.cpp"
    "src/player/videoPlayback.cpp"
    "src/playerUI/videoFrameWidget.cpp"
)
source_group("player" FILES ${player})

set(playerUI
    "src/parser/mediaParser.cpp"
    "src/playerUI/clickableSlider.cpp"
    "src/playerUI/controlsWidget.cpp"
    "src/playerUI/controlsWidget.ui"
    "src/playerUI/fullscreenPlayerWidget.cpp"
    "src/playerUI/movingOutArea.cpp"
    "src/playerUI/movingOutArea.cpp"
    "src/playerUI/playerWidget.ui"
    "src/resources/resources.qrc"
)
source_group("playerUI" FILES ${playerUI})

set(verifier
    "src/parser/basic/box.cpp"
    "src/parser/basic/mandatoryBox.cpp"
    "src/parser/basic/unknownBox.cpp"
    "src/parser/boxFactory.cpp"
    "src/parser/certificateStorage.cpp"
    "src/parser/consistencyChecker.cpp"
    "src/parser/fourcc.cpp"
    "src/parser/oxfverifier.cpp"
    "src/parser/signatureExtractor.cpp"
    "src/parser/validatorISO.cpp"
    "src/parser/validatorOXF.cpp"
    "src/parser/validatorSurveillance.cpp"
    "src/parserUI/certificateStorageDialog.cpp"
    "src/parserUI/parserWidget.cpp"
    "src/parserUI/verifyerdialog.cpp"
    "src/playerUI/playerWidget.cpp"
    "src/parserUI/parserWidget.ui"
)
source_group("verifier" FILES ${verifier})

set(ALL_FILES
    ${no_group_source_files}
    ${boxes}
    ${external}
    ${player}
    ${playerUI}
    ${verifier}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE ONVIFPlayer)

#set_target_properties(${PROJECT_NAME} PROPERTIES
#    VS_GLOBAL_KEYWORD "Qt4VSv1.0"
#)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "ONVIFPlayer"
    TARGET_NAME_RELEASE "ONVIFPlayer"
)
################################################################################
# Output directory
################################################################################
#set_target_properties(${PROJECT_NAME} PROPERTIES
#    OUTPUT_DIRECTORY_DEBUG   "${OUTPUT_DIRECTORY}"
#    OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
#)
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:Debug>:
        MultiThreadedDebugDLL
    >
    $<$<CONFIG:Release>:
        MultiThreadedDLL
    >
    $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
    ">"
    "$<$<CONFIG:Release>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/release"
    ">"
    "${CMAKE_CURRENT_SOURCE_DIR}/src;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/player;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/playerUI;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parserUI;"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/FFMPEG-4.4/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/PortAudio/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/OpenSSL-1.0.1/include;"
    "$ENV{QTDIR}/include;"
    "$ENV{QTDIR}/include/QtMultimedia;"
    "$ENV{QTDIR}/include/QtWidgets;"
    "$ENV{QTDIR}/include/QtNetwork;"
    "$ENV{QTDIR}/include/QtGui;"
    "$ENV{QTDIR}/include/QtANGLE;"
    "$ENV{QTDIR}/include/QtCore;"
    "${CMAKE_CURRENT_SOURCE_DIR}/.;"
    "$ENV{QTDIR}/mkspecs/win32-msvc2012;"
    "${CMAKE_CURRENT_SOURCE_DIR}/GeneratedFiles"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "D_COM_DISABLE_EXPLICIT_OPERATOR_BOOL;"
        "QT_RESTRICTED_CAST_FROM_ASCII"
    ">"
    "$<$<CONFIG:Release>:"
        "QT_NO_DEBUG;"
        "NDEBUG"
    ">"
    "_WINDOWS;"
    "UNICODE;"
    "WIN32;"
    "DECODE_USING_QUEUE;"
    "QT_MULTIMEDIA_LIB;"
    "QT_WIDGETS_LIB;"
    "QT_NETWORK_LIB;"
    "QT_GUI_LIB;"
    "QT_CORE_LIB;"
    "QT_OPENGL_ES_2"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od;
            /Zi;
            /permissive-
        >
        $<$<CONFIG:Release>:
            /O2;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        /std:c++17;
        /W3;
        /GR;
        -Zm200;
        -w34100;
        -w34189;
        /bigobj;
        /Zc:__cplusplus;
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Zc:wchar_t
    )
    string(CONCAT FILE_CL_OPTIONS
        "/Y-"
    )
#    source_file_compile_options(GeneratedFiles/qrc_resources.cpp ${FILE_CL_OPTIONS})
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /DEBUG
        >
        $<$<CONFIG:Release>:
            /INCREMENTAL:NO
        >
        /NOLOGO;
        /SUBSYSTEM:WINDOWS;
        /NXCOMPAT;
        /DYNAMICBASE;
        /SAFESEH:NO
    )
endif()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:"
        "Qt6Multimediad;"
        "Qt6Widgetsd;"
        "Qt6Networkd;"
        "Qt6Guid;"
        "Qt6Cored"
    ">"
    "$<$<CONFIG:Release>:"
        "Qt6Multimedia;"
        "Qt6Widgets;"
        "Qt6Network;"
        "Qt6Gui;"
        "Qt6Core;"
        "gdi32"
    ">"
    "avcodec;"
    "avdevice;"
    "avfilter;"
    "avformat;"
    "avutil;"
    "swresample;"
    "swscale;"
    "libeay32;"
    "ssleay32;"
    "portaudio_x64;"
    "user32;"
    "Ws2_32"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "$ENV{QTDIR}/lib;"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/FFMPEG-4.4/lib;"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/PortAudio/lib;"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/OpenSSL-1.0.1/lib"
)

