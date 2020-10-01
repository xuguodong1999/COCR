cmake_minimum_required(VERSION 3.16)

project(COCR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif ()

# config opencv
if (MSVC)
    set(Eigen3_DIR C:/Users/xgd/install/eigen/share/eigen3/cmake)
    set(OpenCV_DIR C:/Users/xgd/install/opencv/x64/vc16/lib)
elseif (UNIX)
    set(Eigen3_DIR /media/xgd/Windows-SSD/Users/xgd/install/eigen/share/eigen3/cmake)
    set(OpenCV_DIR /home/xgd/install/opencv-gpu/lib/cmake/opencv4)
endif ()

# config eigen for opencv
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(OpenCV REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(soso918)
add_subdirectory(lightnet)
