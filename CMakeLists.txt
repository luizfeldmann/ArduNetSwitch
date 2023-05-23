# Project config
cmake_minimum_required(VERSION 3.0.0)

project(ArduNetSwitch CXX)

# Add depends
add_subdirectory(dependencies)

# Main program sources
set(${CMAKE_PROJECT_NAME}_SOURCES
    # Include
    "include/mac.h"
    "include/CBuffer.h"
    "include/CStringView.h"
    "include/CHttpServer.h"
    # Source
    "src/mac.c"
    "src/CBuffer.cpp"
    "src/CStringView.cpp"
    "src/CHttpServer.cpp"
    "src/main.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${${CMAKE_PROJECT_NAME}_SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "include")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Ethernet)

include("resources/LinkWebpages.cmake")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE LINK_WEBPAGES)

target_link_arduino_libraries(${CMAKE_PROJECT_NAME} PRIVATE core)

target_enable_arduino_upload(${CMAKE_PROJECT_NAME})