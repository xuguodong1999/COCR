cmake_minimum_required(VERSION 3.16)

project(cocr-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(HEADERS_DIR C:/include)
    set(SHARED_LIBS_DIR C:/shared)
endif (MSVC)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${SRC_DIR})

set(CMAKE_DEBUG_POSTFIX d)

if (MSVC)
    set(doctest_DIR ${HEADERS_DIR}/doctest/lib/cmake/doctest)
    set(Eigen3_DIR ${HEADERS_DIR}/eigen3/cmake)
    set(nlohmann_json_DIR ${HEADERS_DIR}/nlohmann_json/lib/cmake/nlohmann_json)
    set(FunctionalPlus_DIR ${HEADERS_DIR}/FunctionalPlus/lib/cmake/FunctionalPlus)
    set(frugally-deep_DIR ${HEADERS_DIR}/frugally-deep/lib/cmake/frugally-deep)
    set(RapidJSON_DIR ${HEADERS_DIR}/rapidjson/cmake)
    set(Taskflow_DIR ${HEADERS_DIR}/taskflow/lib/cmake/Taskflow)
    set(Qt5_DIR C:/Qt/5.15.1/msvc2019_64/lib/cmake/Qt5)
    set(VTK_DIR ${SHARED_LIBS_DIR}/vtk/lib/cmake/vtk-8.2)
    set(OpenCV_DIR ${SHARED_LIBS_DIR}/opencv/x64/vc16/lib)
    set(Boost_DIR ${SHARED_LIBS_DIR}/boost/lib/cmake/Boost-1.74.0)
    set(Torch_DIR ${SHARED_LIBS_DIR}/libtorch/share/cmake/Torch)
    set(maeparser_DIR ${SHARED_LIBS_DIR}/maeparser/lib/cmake)
    set(coordgen_DIR ${SHARED_LIBS_DIR}/coordgenlibs/lib/cmake)
    set(rdkit_DIR ${SHARED_LIBS_DIR}/rdkit/lib/cmake/rdkit)
endif (MSVC)

if(MSVC)
add_subdirectory(app)
add_subdirectory(lab)
add_subdirectory(alkane)
add_subdirectory(data)
endif (MSVC)

add_subdirectory(train)