cmake_minimum_required(VERSION 3.16)

project(cocr-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)

include(${CMAKE_SOURCE_DIR}/cmake/SetupMSVC.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupUNIX.cmake)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${SRC_DIR})

set(CMAKE_DEBUG_POSTFIX d)

if (MSVC)
#    add_subdirectory(app)
    add_subdirectory(lab)
    add_subdirectory(alkane)
    add_subdirectory(data)
    add_subdirectory(train)
    add_subdirectory(simple)
elseif (APPLE)
#    add_subdirectory(app)
elseif (UNIX)
    add_subdirectory(unix)
endif (MSVC)