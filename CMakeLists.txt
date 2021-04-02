cmake_minimum_required(VERSION 3.16)

project(leafxy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# qt-android require app has same name with cmake-top-project
set(APP_NAME ${PROJECT_NAME})
# app linked qt components
set(QT_COMPONENTS Widgets 3DCore 3DExtras 3DRender 3DInput LinguistTools)

# use this flag to build static exe for msvc. dont set it to false on unix
# dont set it unless you known what you are doing
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if (NOT MSVC)
    set(BUILD_SHARED_LIBS ON)
endif (NOT MSVC)

# use these flags to find 3rdparty lib
set(LEAFXY_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(RES_ROOT ${LEAFXY_ROOT}/resources)
set(3RD_LIB_DIR ${LEAFXY_ROOT}/3rdparty)
set(ANDROID_LIB_DIR ${3RD_LIB_DIR}/android)

# openmp for alkane generation acceleration
if (NOT ANDROID)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else (OPENMP_FOUND)
        message(WARNING "openmp not found. Acceleration for alkane generation disabled.")
    endif (OPENMP_FOUND)
endif (NOT ANDROID)

# ncnn for crnn and yolo
# use build.sh or build-shared.bat to install ncnn from source
if (ANDROID)
    set(ncnn_DIR ${ANDROID_LIB_DIR}/ncnn-20210124-android/${CMAKE_ANDROID_ARCH_ABI}/lib/cmake/ncnn)
else (ANDROID)
    set(ncnn_DIR ${3RD_LIB_DIR}/ncnn-lib/lib/cmake/ncnn)
endif (ANDROID)

# opencv>=4.5.1 for for common image utils, crnn and yolo
# use build.sh or build-shared.bat to install opencv from source
if (MSVC)
    set(OpenCV_DIR ${3RD_LIB_DIR}/opencv-lib/x64/vc16/lib)
elseif (APPLE)
    set(OpenCV_DIR ${3RD_LIB_DIR}/opencv-lib/lib/cmake/opencv4)
elseif (ANDROID)
    set(OpenCV_DIR ${ANDROID_LIB_DIR}/OpenCV-android-sdk/sdk/native/jni)
elseif (UNIX)
    set(OpenCV_DIR ${3RD_LIB_DIR}/opencv-lib/lib/cmake/opencv4)
    # set(OpenCV_DIR /home/xgd/shared/opencv4.5.1/lib/cmake/opencv4)
endif (MSVC)

# qt for ui
if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(QT_DIR C:/shared/qt5.15.2/lib/cmake/Qt5)
    # for M_PI macro
    add_compile_definitions(_USE_MATH_DEFINES)
elseif (APPLE)
    set(QT_DIR /Users/xgd/Qt/5.15.2/clang_64/lib/cmake/Qt5)
elseif (ANDROID)
    set(ANDROID_NATIVE_API_LEVEL 21)
    #    set(ANDROID_BUILD_ABI_armeabi-v7a ON)
    set(QT_DIR /home/xgd/shared/Qt/5.15.2/android/lib/cmake/Qt5)
elseif (UNIX)
    set(QT_DIR /home/xgd/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5)
endif (MSVC)

# override config for static build on msvc only
if (MSVC AND NOT BUILD_SHARED_LIBS)
    set(QT_DIR C:/static/qt5.15.2/lib/cmake/Qt5)
    set(OpenCV_DIR ${3RD_LIB_DIR}/opencv-static-lib/x64/vc16/staticlib)
    set(ncnn_DIR ${3RD_LIB_DIR}/ncnn-static-lib/lib/cmake/ncnn)
    # enable CMAKE_MSVC_RUNTIME_LIBRARY
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif (MSVC AND NOT BUILD_SHARED_LIBS)

set(Qt5_DIR ${QT_DIR})

# preload qt for android here to avoid conflict in add_library stage.
# qt-android build shared-lib to load in apk
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} REQUIRED)

# app build support all platforms
add_subdirectory(${LEAFXY_ROOT}/app)
add_subdirectory(${3RD_LIB_DIR}/openbabel)
add_subdirectory(${3RD_LIB_DIR}/coordgenlibs)
add_dependencies(${APP_NAME} openbabel)
add_dependencies(${APP_NAME} coordgen)

# data_gen build support all desktop platforms by shared build
if (NOT ANDROID AND BUILD_SHARED_LIBS)
    # openmp for alkane generation acceleration
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else (OPENMP_FOUND)
        message(WARNING "openmp not found. Acceleration for alkane generation disabled.")
    endif (OPENMP_FOUND)
    add_subdirectory(data_gen)
    add_dependencies(data_gen coordgen)
    add_dependencies(data_gen openbabel)
endif (NOT ANDROID AND BUILD_SHARED_LIBS)
