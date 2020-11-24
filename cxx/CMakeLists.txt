cmake_minimum_required(VERSION 3.16)

project(cocr-cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)

set(CMAKE_DEBUG_POSTFIX d)

if (MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupMSVC.cmake)
elseif (UNIX)
    set(Torch_DIR /home/xgd/shared/libtorch/share/cmake/Torch)
    set(Qt5_DIR /home/xgd/Qt/5.15.1/gcc_64/lib/cmake/Qt5)
endif (MSVC)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${SRC_DIR})

add_subdirectory(core)
