project(app)

set(CMAKE_CXX_STANDARD 17)

## for openssl integration in android build
#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()
# qt5, you need to install it >=5.15.2 for bug-free qt3d call
set(QT_COMPONENTS Widgets 3DCore 3DExtras 3DRender LinguistTools)
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)
# set project source here
set(TS_FILES ../resources/${PROJECT_NAME}_zh_CN.ts)
qt_create_translation(QM_FILES ../app ${TS_FILES})

qt5_add_big_resources(BIG_RES_FILES ../resources/big_res.qrc)
set(RES_SRCS ${BIG_RES_FILES} ../resources/obdata.qrc)

include_directories(../openbabel/include ../ncnn/include ../app)
aux_source_directory(../app DIR_SRCS)
aux_source_directory(../app/ui UI_SRCS)
aux_source_directory(../app/ncnn_impl NCNN_SRCS)
aux_source_directory(../app/opencv_dnn_impl OPENCV_DNN_SRCS)
set(PROJECT_SOURCES ${DIR_SRCS} ${RES_SRCS} ${TS_FILES} ${NCNN_SRCS} ${UI_SRCS})

if (NOT ANDROID)
    list(APPEND PROJECT_SOURCES ${OPENCV_DNN_SRCS})
endif (NOT ANDROID)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
else ()
    if (ANDROID)
        add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
    else ()
        add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    endif ()
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# link opencv, patched openbabel and qt here
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::3DCore
        Qt${QT_VERSION_MAJOR}::3DExtras
        Qt${QT_VERSION_MAJOR}::3DRender
        Qt${QT_VERSION_MAJOR}::Widgets
        opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui
        ncnn
        openbabel
        coordgen)

if (NOT ANDROID)
    target_link_libraries(${PROJECT_NAME} PRIVATE opencv_dnn)
endif (NOT ANDROID)
