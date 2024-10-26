cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME ONVIFPlayerAdditional)

project(${PROJECT_NAME} VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(portaudio)
add_subdirectory(pugixml)