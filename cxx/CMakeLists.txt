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
    set(OpenCV_DIR /home/xgd/shared/opencv4.5.0/lib/cmake/opencv4)
    include_directories(${OpenCV_DIR}/include/opencv4)
    set(Torch_DIR /home/xgd/shared/libtorch1.7.0/share/cmake/Torch)
    set(Qt5_DIR /home/xgd/Qt/5.15.2/gcc_64/lib/cmake/Qt5)
endif (MSVC)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${SRC_DIR})

add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(box2graph)
