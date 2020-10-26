cmake_minimum_required(VERSION 3.17)

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

#add_subdirectory(app)
add_subdirectory(exp)
#add_subdirectory(train)
add_subdirectory(alkane)
#add_subdirectory(data)
