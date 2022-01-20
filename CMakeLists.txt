# cmake .. -G "Ninja" -DOpenCV_DIR=~/shared/opencv4/lib/cmake/opencv4 -DQt5_DIR=~/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5 -Dncnn_DIR=~/shared/ncnn/lib/cmake/ncnn -DBoost_DIR=~/shared/boost/lib/cmake/Boost-1.77.0
# cmake .. -G "Ninja" -DOpenCV_DIR=C:/Library/Shared/opencv4/x64/vc16/lib -DQt5_DIR=C:/Library/Shared/qt-5.15.2/lib/cmake/Qt5 -Dncnn_DIR=C:/Library/Shared/ncnn/lib/cmake/ncnn -DBoost_DIR=C:/Library/Shared/boost/lib/cmake/Boost-1.77.0 -DVulkan_LIBRARY=C:/Library/Shared/vulkan/Lib/vulkan-1.lib -DVulkan_INCLUDE_DIR=C:/Library/Shared/vulkan/Include

cmake_minimum_required(VERSION 3.16)
project(COCR LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# always need openmp
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else (OPENMP_FOUND)
    message(WARNING "openmp not found. Acceleration for alkane generation disabled.")
endif (OPENMP_FOUND)

if (MSVC)
    set(Boost_USE_STATIC_LIBS OFF CACHE INTERNAL "")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_definitions(_USE_MATH_DEFINES) # for M_PI macro
endif ()

#set(LIB_NAME_LIST Qt5 OpenCV Boost ncnn)
#set(MY_LIB_DIR /home/xgd/shared)
#set(LIB_CMAKE_DIR_LIST
#        ${MY_LIB_DIR}/Qt/5.15.2/gcc_64/lib/cmake/Qt5
#        ${MY_LIB_DIR}/opencv4/lib/cmake/opencv4
#        ${MY_LIB_DIR}/boost/lib/cmake/Boost-1.77.0
#        ${MY_LIB_DIR}/ncnn/lib/cmake/ncnn)
#
#list(LENGTH LIB_NAME_LIST LIB_LENGTH)
#math(EXPR LIB_LENGTH_LOOP ${LIB_LENGTH}-1)
#
## test not found case
##set(Qt5_DIR /tmp)
#
#foreach (i RANGE ${LIB_LENGTH_LOOP})
#    list(GET LIB_NAME_LIST ${i} LIB_NAME)
#    if (NOT ${LIB_NAME}_DIR)
#        list(GET LIB_CMAKE_DIR_LIST ${i} LIB_CMAKE_DIR)
#        set(${LIB_NAME}_DIR ${LIB_CMAKE_DIR} CACHE INTERNAL "")
#    endif ()
#    message(STATUS "set " ${LIB_NAME}_DIR "=" ${${LIB_NAME}_DIR})
#endforeach ()
#
## 兼容 Qt6
#set(QT_DIR ${Qt5_DIR} CACHE INTERNAL "")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
