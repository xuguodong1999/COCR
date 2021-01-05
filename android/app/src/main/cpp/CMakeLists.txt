cmake_minimum_required(VERSION 3.16.3)

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/include)
set(COCR_LIBS_DIR ${CMAKE_SOURCE_DIR}/3rdparty/lib/${ANDROID_ABI})

set(COCR_LIB_NAMES libncnn.a
        libSPIRV.a libglslang.a libOSDependent.a libOGLCompiler.a
        libopencv_stitching.a libopencv_features2d.a libopencv_objdetect.a
        libopencv_calib3d.a libopencv_dnn.a libopencv_ml.a libopencv_flann.a libopencv_video.a
        libopencv_gapi.a libopencv_photo.a libopencv_videoio.a libopencv_highgui.a
        libopencv_imgcodecs.a libopencv_imgproc.a libopencv_core.a libade.a libcpufeatures.a
        libIlmImf.a libittnotify.a liblibjpeg-turbo.a liblibopenjp2.a liblibpng.a
        liblibprotobuf.a liblibtiff.a liblibwebp.a libtbb.a libquirc.a)

set(COCR_LIBS)
foreach (COCR_LIB_NAME ${COCR_LIB_NAMES})
    list(APPEND COCR_LIBS ${COCR_LIBS_DIR}/${COCR_LIB_NAME})
endforeach (list)

if (${ANDROID_ABI} STREQUAL x86 OR ${ANDROID_ABI} STREQUAL x86_64)
    list(APPEND COCR_LIBS ${COCR_LIBS_DIR}/libippiw.a ${COCR_LIBS_DIR}/libippicv.a)
else ()
    list(APPEND COCR_LIBS ${COCR_LIBS_DIR}/libtegra_hal.a)
endif ()

add_library(native-lib SHARED native-lib.cpp)

target_link_libraries(native-lib
        ${COCR_LIBS}
        log vulkan android z jnigraphics)