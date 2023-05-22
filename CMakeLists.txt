# Project config
cmake_minimum_required(VERSION 3.0.0)

project(ArduNetSwitch CXX)

# Add depends
add_subdirectory(dependencies)

# Main program sources
set(${CMAKE_PROJECT_NAME}_SOURCES
    "src/main.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${${CMAKE_PROJECT_NAME}_SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "include")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Ethernet)

target_link_arduino_libraries(${CMAKE_PROJECT_NAME} PRIVATE core)

target_enable_arduino_upload(${CMAKE_PROJECT_NAME})