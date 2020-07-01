cmake_minimum_required(VERSION 3.16)
project(TestModuleImport)
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

# 配置Qt
if (UNIX)
    # FIXME: linux下修改到本地 Qt 库的cmake文件夹位置
    set(Qt5_DIR "/home/xgd/Qt/5.15.0/gcc_64/lib/cmake/Qt5")
elseif (MSVC)
    # FIXME: windows下修改到本地 Qt 库的cmake文件夹位置
    set(Qt5_DIR "C:/Qt/5.12.6/msvc2019_64/lib/cmake/Qt5")
endif ()
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 配置 OpenCV
if (UNIX)
    set(OpenCV_DIR "lib/opencv/lib/cmake/opencv4")
elseif (MSVC)
    set(OpenCV_DIR "lib/opencv/x64/vc16/lib")
endif ()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}/opencv2)

# 配置 OpenBabel
set(OPENBABEL2_INCLUDE_DIR lib/openbabel/include/openbabel3)
if (MSVC)
    set(OPENBABEL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/openbabel/bin/openbabel-3.lib)
elseif (UNIX)
    set(OPENBABEL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/openbabel/lib/libopenbabel.so)
endif ()
include_directories(${OPENBABEL2_INCLUDE_DIR})

# Eigen3 Support
set(Eigen3_DIR "lib/eigen/share/eigen3/cmake")
find_package(Eigen3 REQUIRED NO_MODULE)
include_directories(${EIGEN3_INCLUDE_DIR})

# 自己的代码
include_directories(include)
aux_source_directory(src DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        ${OpenCV_LIBS}
        ${OPENBABEL2_LIBRARIES}
        Qt5::Widgets
)