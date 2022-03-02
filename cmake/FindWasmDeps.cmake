include(${CMAKE_SOURCE_DIR}/cmake/macro.cmake)
GET_SUB_DIR_NAME(QT_MODULE_DIR_LIST ${Qt5_DIR}/../)
foreach (QT_MODULE ${QT_MODULE_DIR_LIST})
    set(${QT_MODULE}_DIR ${Qt5_DIR}/../${QT_MODULE} CACHE INTERNAL "")
endforeach ()
unset(QT_MODULE_DIR_LIST)

set(QT_COMPONENTS Widgets)
set(_PROJECT_NAME ${PROJECT_NAME})
set(PROJECT_NAME ${APP_NAME})
find_package(Qt5 COMPONENTS ${QT_COMPONENTS} LinguistTools REQUIRED)
set(PROJECT_NAME ${_PROJECT_NAME})
unset(_PROJECT_NAME)
unset(QT_COMPONENTS)
include(FetchContent)
FetchContent_Declare(ncnn_android
        URL https://github.com/Tencent/ncnn/releases/download/20210720/ncnn-20210720-webassembly.zip)
FetchContent_MakeAvailable(ncnn_android)
FetchContent_Declare(opencv_android
        URL https://github.com/nihui/opencv-mobile/releases/download/v14/opencv-mobile-4.5.4-webassembly.zip)
FetchContent_MakeAvailable(opencv_android)
set(ncnn_DIR ${ncnn_android_SOURCE_DIR}/basic/lib/cmake/ncnn)
set(OpenCV_DIR ${opencv_android_SOURCE_DIR}/basic/lib/cmake/opencv4)
