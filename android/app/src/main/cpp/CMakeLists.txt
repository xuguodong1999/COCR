cmake_minimum_required(VERSION 3.4.1)

# config 3rdparty path
set(CPP_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../../cpp/include)
set(CPP_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/3rdparty)
set(OPENBABEL_LIBDIR ${CPP_LIB_DIR}/openbabel/${ANDROID_ABI})
set(OPENCV_LIBDIR ${CPP_LIB_DIR}/opencv/sdk/native/staticlibs/${ANDROID_ABI})
set(OPENCV_3RDPARTY_LIBDIR ${CPP_LIB_DIR}/opencv/sdk/native/3rdparty/libs/${ANDROID_ABI})

# config include dir
include_directories(${CPP_HEADER_DIR})

# add your cpp codes here
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src SRC_FILES)
add_library(native-lib SHARED ${SRC_FILES})

# link your code here

# file(GLOB OPENCV_STATIC_LIB "${OPENBABEL_LIBDIR}/*.a")
# file(GLOB OPENCV_3RDPARTY_STATIC_LIB "${OPENCV_3RDPARTY_LIBDIR}/.a")
# 注意：静态库链接有顺序要求
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

target_link_libraries(native-lib
        # openbabel needs zlib.
        log vulkan android z jnigraphics)

