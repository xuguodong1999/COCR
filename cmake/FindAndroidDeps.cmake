include(${CMAKE_SOURCE_DIR}/cmake/macro.cmake)
findQtModules()

set(QT_COMPONENTS Widgets MultimediaWidgets 3DCore 3DExtras 3DRender 3DInput)

# apk shell begin
set(_PROJECT_NAME ${PROJECT_NAME})
set(PROJECT_NAME ${APP_NAME})
find_package(Qt5 COMPONENTS ${QT_COMPONENTS} LinguistTools REQUIRED)
set(PROJECT_NAME ${_PROJECT_NAME})
unset(_PROJECT_NAME)
unset(QT_COMPONENTS)
# apk shell end

include(FetchContent)
FetchContent_Declare(ncnn_android
        # URL http://127.0.0.1:5500/ncnn-20210720-android-vulkan.zip)
        URL https://github.com/Tencent/ncnn/releases/download/20210720/ncnn-20210720-android-vulkan.zip)
FetchContent_MakeAvailable(ncnn_android)
FetchContent_Declare(opencv_android
        # URL http://127.0.0.1:5500/opencv-4.5.5-android-sdk.zip)
        URL https://github.com/opencv/opencv/releases/download/4.5.5/opencv-4.5.5-android-sdk.zip)
FetchContent_MakeAvailable(opencv_android)
set(ncnn_DIR ${ncnn_android_SOURCE_DIR}/${ANDROID_ABI}/lib/cmake/ncnn)
set(OpenCV_DIR ${opencv_android_SOURCE_DIR}/sdk/native/jni/abi-${ANDROID_ABI})
