# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CPP_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CPP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(OPENBABEL_LIBDIR ${CPP_LIB_DIR}/openbabel/${ANDROID_ABI})
set(OPENCV_LIBDIR ${CPP_LIB_DIR}/opencv/sdk/native/staticlibs/${ANDROID_ABI})
set(OPENCV_3RDPARTY_LIBDIR ${CPP_LIB_DIR}/opencv/sdk/native/3rdparty/libs/${ANDROID_ABI})

# Relative path of gradle build directory to CMakeLists.txt
set(build_DIR ${CMAKE_SOURCE_DIR}/build)

file(GLOB PYTORCH_INCLUDE_DIRS "${build_DIR}/pytorch_android*.aar/headers")
file(GLOB PYTORCH_LINK_DIRS "${build_DIR}/pytorch_android*.aar/jni/${ANDROID_ABI}")

set(BUILD_SUBDIR ${ANDROID_ABI})
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PYTORCH_INCLUDE_DIRS}
)

find_library(PYTORCH_LIBRARY pytorch_jni
  PATHS ${PYTORCH_LINK_DIRS}
  NO_CMAKE_FIND_ROOT_PATH)

find_library(FBJNI_LIBRARY fbjni
  PATHS ${PYTORCH_LINK_DIRS}
  NO_CMAKE_FIND_ROOT_PATH)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)
find_library(vulkan-lib vulkan)
find_library(android-lib android)
find_library(z-lib z)
find_library(jnigraphics-lib jnigraphics)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(native-lib
        # openbabel
        ${OPENBABEL_LIBDIR}/libopenbabel.a
        # opencv
        ${OPENCV_LIBDIR}/libopencv_dnn.a
        ${OPENCV_LIBDIR}/libopencv_imgcodecs.a
        ${OPENCV_LIBDIR}/libopencv_imgproc.a
        ${OPENCV_LIBDIR}/libopencv_calib3d.a
        ${OPENCV_LIBDIR}/libopencv_features2d.a
        ${OPENCV_LIBDIR}/libopencv_ml.a
        ${OPENCV_LIBDIR}/libopencv_stitching.a
        ${OPENCV_LIBDIR}/libopencv_objdetect.a
        ${OPENCV_LIBDIR}/libopencv_flann.a
        ${OPENCV_LIBDIR}/libopencv_video.a
        ${OPENCV_LIBDIR}/libopencv_gapi.a
        ${OPENCV_LIBDIR}/libopencv_photo.a
        ${OPENCV_LIBDIR}/libopencv_videoio.a
        ${OPENCV_LIBDIR}/libopencv_highgui.a
        ${OPENCV_LIBDIR}/libopencv_core.a
        # opencv 3rdparty.
        ${OPENCV_3RDPARTY_LIBDIR}/libade.a
        ${OPENCV_3RDPARTY_LIBDIR}/libcpufeatures.a
        ${OPENCV_3RDPARTY_LIBDIR}/libIlmImf.a
        ${OPENCV_3RDPARTY_LIBDIR}/libittnotify.a
        ${OPENCV_3RDPARTY_LIBDIR}/liblibjasper.a
        ${OPENCV_3RDPARTY_LIBDIR}/liblibjpeg-turbo.a
        ${OPENCV_3RDPARTY_LIBDIR}/liblibpng.a
        ${OPENCV_3RDPARTY_LIBDIR}/liblibprotobuf.a
        ${OPENCV_3RDPARTY_LIBDIR}/liblibtiff.a
        ${OPENCV_3RDPARTY_LIBDIR}/liblibwebp.a
        ${OPENCV_3RDPARTY_LIBDIR}/libquirc.a
        ${OPENCV_3RDPARTY_LIBDIR}/libtbb.a)

if (${ANDROID_ABI} STREQUAL x86 OR ${ANDROID_ABI} STREQUAL x86_64)
    target_link_libraries(native-lib
            # platform-dependent opencv 3rdparty.
            ${OPENCV_3RDPARTY_LIBDIR}/libippiw.a
            ${OPENCV_3RDPARTY_LIBDIR}/libippicv.a)
else ()
    target_link_libraries(native-lib
            # platform-dependent opencv 3rdparty.
            ${OPENCV_3RDPARTY_LIBDIR}/libtegra_hal.a)
endif ()

target_link_libraries( # Specifies the target library.
        native-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${vulkan-lib}
        ${android-lib}
        ${z-lib}
        ${jnigraphics-lib}
        ${PYTORCH_LIBRARY})
        ${FBJNI_LIBRARY})