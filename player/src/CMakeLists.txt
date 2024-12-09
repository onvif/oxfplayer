if(NOT LINUX)
  message( FATAL_ERROR "Only for linux build" )
endif()

# Common includes
include_directories(${FFMPEG_PATH}/include)
include_directories(.)
include_directories(./common)
include_directories(./parser)
include_directories(./parser/basic)
include_directories(./parser/basic/mixin)
include_directories(./parser/helpers)
include_directories(./parserUI)
include_directories(./player)
include_directories(./playerUI)
include_directories(./resources)

# Where to search FFmpeg libs
link_directories(${FFMPEG_PATH}/lib)

# Where to put output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/../bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/../bin/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/../bin/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/../bin/Release)

# Add library to use in application and tests
file(GLOB_RECURSE LIBRARY_SRC common/*.* parser/*.* parserUI/*.* player/*.* playerUI/*.* resources/*.*)

add_library(ONVIFPlayerLib STATIC ${LIBRARY_SRC})

target_link_libraries(ONVIFPlayerLib
    PUBLIC
	    Qt5::Core
        Qt5::Widgets
        Qt5::Network
        Qt5::Multimedia
        OpenSSL::SSL
        PortAudio
        pugixml-static
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        postproc
        swresample
        swscale
        signed-media-framework
        PkgConfig::GLIB
        PkgConfig::GST
)

# Add executable
add_executable(ONVIFPlayer
    resources/resources.qrc
    main.cpp
)

target_link_libraries(ONVIFPlayer
    PUBLIC
    ONVIFPlayerLib
)

# Add tests

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin/tests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/../bin/Debug/tests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/../bin/Release/tests)

file(GLOB TESTS_SRC tests/*.cpp)

foreach(TEST_SRC ${TESTS_SRC})
    get_filename_component(FILE_NAME ${TEST_SRC} NAME_WLE)
    add_executable(${FILE_NAME} ${TEST_SRC})
    add_test(NAME ${FILE_NAME} COMMAND ${FILE_NAME})
    target_link_libraries(${FILE_NAME} PRIVATE Qt5::Test ONVIFPlayerLib)
endforeach()

