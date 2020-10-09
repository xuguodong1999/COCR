cmake_minimum_required(VERSION 3.16)

project(COCR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#if (UNIX)
#    set(Eigen3_DIR /media/xgd/Windows-SSD/Users/xgd/install/eigen/share/eigen3/cmake)
#    set(OpenCV_DIR /home/xgd/install/opencv-gpu/lib/cmake/opencv4)
#    include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
#endif (UNIX)

find_package(OpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(HEADERS_DIR C:/Headers)
    set(SHARED_LIBS_DIR C:/SharedLibs)
    set(STATIC_LIBS_DIR C:/StaticLibs)

    set(Eigen3_DIR ${HEADERS_DIR}/eigen/share/eigen3/cmake)
    find_package(Eigen3 REQUIRED)
    include_directories(${EIGEN3_INCLUDE_DIR})

    set(Qt5_DIR C:/Qt/5.15.0/msvc2019_64/lib/cmake/Qt5)
    find_package(Qt5 COMPONENTS Core Quick LinguistTools)

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    set(VTK_DIR ${SHARED_LIBS_DIR}/vtk/lib/cmake/vtk-8.2)
    find_package(VTK REQUIRED)

    set(OpenCV_DIR ${SHARED_LIBS_DIR}/opencv/x64/vc16/lib)
    find_package(OpenCV REQUIRED)

    set(Boost_DIR ${SHARED_LIBS_DIR}/boost/lib/cmake/Boost-1.74.0)
    set(Boost_USE_STATIC_RUNTIME OFF)
    # COMPONENTS coroutine filesystem thread
    find_package(Boost REQUIRED COMPONENTS coroutine filesystem thread)

    set(PYTORCH_VERSION "1.6.0")
    set(Torch_DIR ${SHARED_LIBS_DIR}/libtorch/share/cmake/Torch)
    find_package(Torch ${PYTORCH_VERSION})

    set(RapidJson_DIR ${HEADERS_DIR}/rapidjson/cmake)
    find_package(RapidJson REQUIRED)
    include_directories(${RAPIDJSON_INCLUDE_DIRS})

endif (MSVC)

if (MSVC)
    set(OpenBabel_DIR ${STATIC_LIBS_DIR}/openbabel-3.1.1)
    include_directories(${OpenBabel_DIR}/include)
    if (CMAKE_BUILD_TYPE)
        if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            set(OpenBabel_LIBS ${OpenBabel_DIR}/staticlibs/openbabel-3d.lib)
        endif ()
    endif ()
    if (NOT OpenBabel_LIBS)
        set(OpenBabel_LIBS ${OpenBabel_DIR}/staticlibs/openbabel-3.lib)
    endif ()
elseif (UNIX)
    set(OpenBabel_DIR /home/xgd/install/openbabel)
    set(OpenBabel_LIBS ${OpenBabel_DIR}/lib/libopenbabel.so)
endif ()


set(CMAKE_DEBUG_POSTFIX d)

add_subdirectory(demo)
add_subdirectory(train)
