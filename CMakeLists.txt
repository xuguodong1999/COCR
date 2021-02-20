cmake_minimum_required(VERSION 3.14)

project(jokejoker-project)

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()


# opencv, you need to compile it>=4.5.1 for crnn and yolo call
set(OPENCV_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv)

set(ANDROID_OPENCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/OpenCV-android-sdk)

if (MSVC)
    set(OpenCV_DIR ${OPENCV_INSTALL_DIR}/x64/vc16/lib)
elseif (APPLE)
    set(OpenCV_DIR ${OPENCV_INSTALL_DIR}/lib/cmake/opencv4)
elseif (ANDROID)
        set(ANDROID_NATIVE_API_LEVEL 21)
    #    set(ANDROID_BUILD_ABI_x86 ON)
    #    set(ANDROID_BUILD_ABI_armeabi-v7a ON)
    set(OpenCV_DIR ${ANDROID_OPENCV_DIR}/sdk/native/jni)
elseif (UNIX)
    set(QT_DIR /home/xgd/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5)
    set(Qt5_DIR /home/xgd/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5)
    #    set(OpenCV_DIR ${OPENCV_INSTALL_DIR}/lib/cmake/opencv4)
    set(OpenCV_DIR /home/xgd/shared/opencv4.5.1/lib/cmake/opencv4)
#    set(OpenCV_DIR /home/xgd/shared/opencv451-patched/lib/cmake/opencv4)
endif (MSVC)

# ncnn
set(ncnn_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib/cmake/ncnn)

add_subdirectory(openbabel)
add_subdirectory(data_gen)
add_subdirectory(app)

add_dependencies(app openbabel)
add_dependencies(data_gen openbabel)
