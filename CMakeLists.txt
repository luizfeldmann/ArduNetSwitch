# Project config
cmake_minimum_required(VERSION 3.0.0)

project(ArduNetSwitch CXX)

# Add depends
add_subdirectory(dependencies)

# Main program sources
set(${CMAKE_PROJECT_NAME}_SOURCES
    # Include
    "include/mac.h"
    "include/base64.h"
    "include/persistent.h"
    "include/CBuffer.h"
    "include/stringview.h"
    "include/CStringView.h"
    "include/Switch.h"
    "include/HttpConstants.h"
    "include/CHttpResponse.h"
    "include/CHttpServer.h"
    "include/CAppServer.h"
    # Source
    "src/mac.c"
    "src/base64.c"
    "src/persistent.c"
    "src/CBuffer.cpp"
    "src/stringview.c"
    "src/Switch.cpp"
    "src/CStringView.cpp"
    "src/HttpConstants.c"
    "src/CHttpServer.cpp"
    "src/CAppServer.cpp"
    "src/CHttpResponse.cpp"
    "src/main.cpp" )

add_executable(${CMAKE_PROJECT_NAME} ${${CMAKE_PROJECT_NAME}_SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "include")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Ethernet)

include("resources/LinkWebpages.cmake")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE LINK_WEBPAGES)

target_link_arduino_libraries(${CMAKE_PROJECT_NAME} PRIVATE core)

target_enable_arduino_upload(${CMAKE_PROJECT_NAME})