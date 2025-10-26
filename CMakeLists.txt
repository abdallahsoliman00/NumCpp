cmake_minimum_required(VERSION 3.16)

if (NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 17)
    set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(NumCpp
    VERSION 0.0.1
    LANGUAGES CXX
)

add_library(NumCpp INTERFACE)

target_include_directories(
    NumCpp
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/NumCpp
)