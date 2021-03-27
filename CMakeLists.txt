cmake_minimum_required(VERSION 3.16)

project(leafxy)

# qt-android require app has same name with cmake-top-project
set(APP_NAME ${PROJECT_NAME})

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
if (ANDROID)
    set(ncnn_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/ncnn-20210124-android/${CMAKE_ANDROID_ARCH_ABI}/lib/cmake/ncnn)
else (ANDROID)
    set(ncnn_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib/cmake/ncnn)
endif (ANDROID)

# opencv>=4.5.1 for for common image utils, crnn and yolo\
set(OPENCV_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv)
if (MSVC)
    set(OpenCV_DIR ${OPENCV_INSTALL_DIR}/x64/vc16/lib)
elseif (APPLE)
    set(OpenCV_DIR ${OPENCV_INSTALL_DIR}/lib/cmake/opencv4)
elseif (ANDROID)
    set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/OpenCV-android-sdk/sdk/native/jni)
elseif (UNIX)
    set(OpenCV_DIR ${OPENCV_INSTALL_DIR}/lib/cmake/opencv4)
    # set(OpenCV_DIR /home/xgd/shared/opencv4.5.1/lib/cmake/opencv4)
endif (MSVC)

# qt for ui
if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(QT_DIR C:/shared/qt5.15.2/lib/cmake/Qt5)
elseif (APPLE)
    set(QT_DIR /Users/xgd/Qt/5.15.2/clang_64/lib/cmake/Qt5)
elseif (ANDROID)
    set(ANDROID_NATIVE_API_LEVEL 21)
    #    set(ANDROID_BUILD_ABI_armeabi-v7a ON)
    set(QT_DIR /home/xgd/shared/Qt/5.15.2/android/lib/cmake/Qt5)
elseif (UNIX)
    set(QT_DIR /home/xgd/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5)
endif (MSVC)

set(Qt5_DIR ${QT_DIR})

# preload qt for android here to avoid conflict in add_library stage.
# qt-android build shared-lib to load in apk
if (ANDROID)
    set(QT_COMPONENTS Widgets 3DCore 3DExtras 3DRender 3DInput LinguistTools)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} REQUIRED)
endif (ANDROID)

# app build support all platforms
add_subdirectory(app)
add_subdirectory(openbabel)
add_subdirectory(coordgenlibs)
add_dependencies(${APP_NAME} openbabel)
add_dependencies(${APP_NAME} coordgen)

# data_gen build support all desktop platforms
if (NOT ANDROID)
    add_subdirectory(data_gen)
    add_dependencies(data_gen coordgen)
    add_dependencies(data_gen openbabel)
endif (NOT ANDROID)
