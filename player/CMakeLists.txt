cmake_minimum_required(VERSION 3.25)

if(NOT LINUX)
  message( FATAL_ERROR "Only for linux build" )
endif()

project(ONVIFPlayer VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Add a compiler flag
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Allow test - muts be in root CMakeLists.txt
enable_testing(true)

# Find Qt
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Multimedia REQUIRED)
find_package(Qt5 COMPONENTS Test REQUIRED)
# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add 3rd party as submodules
add_subdirectory(3rdparty/portaudio)
add_subdirectory(3rdparty/pugixml)

# Add signed media framework

# signed-media-framework
set(SIGNED_MEDIA_FRAMEWORK_VERSION 0.0.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.22 gstreamer-sdp-1.0>=1.22
                  gstreamer-video-1.0>=1.22 gstreamer-app-1.0>=1.22)

# signed-media-framework plugin
project(
  ${PLUGIN_NAME}
  VERSION ${SIGNED_MEDIA_FRAMEWORK_VERSION}
  LANGUAGES C)
add_library(${PLUGIN_NAME} STATIC
            ./3rdparty/signed-media-framework/lib/plugins/${PLUGIN_NAME}/plugin.c)
target_include_directories(
  ${PLUGIN_NAME}
  PUBLIC ./3rdparty/signed-media-framework/lib/src
  PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(${PLUGIN_NAME} INTERFACE ${GLIB_LDFLAGS})

# signed-media-framework library
project(
  signed-media-framework
  VERSION ${SIGNED_MEDIA_FRAMEWORK_VERSION}
  LANGUAGES C)
file(GLOB SIGNED_MEDIA_FRAMEWORK_LIBRARY_SRC 3rdparty/signed-media-framework/lib/src/*)
add_library(signed-media-framework STATIC ${SIGNED_MEDIA_FRAMEWORK_LIBRARY_SRC})
target_include_directories(
  signed-media-framework
  PUBLIC 
    ./3rdparty/signed-media-framework/lib/src
    ./3rdparty/signed-media-framework/lib/src/includes
  PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(signed-media-framework PRIVATE ${PLUGIN_NAME} PkgConfig::GLIB PkgConfig::GST)

# # Add source code
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
add_subdirectory(src)
