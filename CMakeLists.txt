cmake_minimum_required(VERSION 3.15)
project(COCR-dev)
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    add_compile_options($<$<C_COMPILER_ID:MSVC>:/utf-8>)
    add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
endif ()

# 配置Qt
if (UNIX)       # FIXME: linux下修改到Qt库的cmake文件夹位置
    set(Qt5_DIR /home/xgd/Qt/5.15.0/gcc_64/lib/cmake/Qt5)
elseif (MSVC)   # FIXME: windows下修改到Qt库的cmake文件夹位置
    set(Qt5_DIR C:/Qt/5.12.6/msvc2019_64/lib/cmake/Qt5)
    # set(Qt5_DIR lib/Qt/5.15.0/msvc2017_64/lib/cmake/Qt5)
endif ()
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 配置 OpenCV
if (UNIX)
    set(OpenCV_DIR lib/opencv/lib/cmake/opencv4)
elseif (MSVC)
    set(OpenCV_DIR lib/opencv/x64/vc16/staticlib)
endif ()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}/opencv2)

# 配置 OpenBabel
set(OPENBABEL2_INCLUDE_DIR lib/openbabel/include/openbabel3)
if (MSVC)
    if (CMAKE_BUILD_TYPE)
        if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            set(OPENBABEL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/openbabel/bin/openbabel-3d.lib)
        else ()
            set(OPENBABEL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/openbabel/bin/openbabel-3.lib)
        endif ()
    else ()
        set(OPENBABEL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/openbabel/bin/openbabel-3.lib)
    endif ()
elseif (UNIX)
    set(OPENBABEL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/openbabel/lib/libopenbabel.so)
endif ()
include_directories(${OPENBABEL2_INCLUDE_DIR})

# 配置 Eigen3
set(Eigen3_DIR lib/eigen/share/eigen3/cmake)
find_package(Eigen3 REQUIRED NO_MODULE)
include_directories(${EIGEN3_INCLUDE_DIR})

# 配置 FrugallyDeep
include_directories(lib/json/include)
include_directories(lib/FunctionalPlus/include)
include_directories(lib/frugally-deep/include)

# 配置 RapidJson
include_directories(lib/rapidjson/include)

# 自己的代码
include_directories(include)
aux_source_directory(src DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})

# 修改MSVC为静态编译
if (MSVC)
    if (CMAKE_BUILD_TYPE)
        if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            target_compile_options(${PROJECT_NAME} PRIVATE /MTd)
        else ()
            target_compile_options(${PROJECT_NAME} PRIVATE /MT)
        endif ()
    else ()
        target_compile_options(${PROJECT_NAME} PRIVATE /MT)
    endif ()
endif ()

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        ${OPENBABEL2_LIBRARIES}
        ${OpenCV_LIBS}
        Qt5::Widgets)
