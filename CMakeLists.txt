cmake_minimum_required(VERSION 3.14)

project(jokejoker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

# qt5, you need to install it >=5.15.2 for bug-free qt3d call
set(QT_COMPONENTS Core
    Quick
    3DAnimation 3DCore 3DExtras 3DInput 3DLogic 3DRender
    LinguistTools)
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
    ${QT_COMPONENTS} REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# opencv, you need to compile it>=4.5.1 for crnn and yolo call
if(MSVC)
    set(OpenCV_DIR C:/shared/opencv4.5.1/x64/vc16/lib)
elseif(APPLE)
elseif(ANDROID)
    set(ANDROID_NATIVE_API_LEVEL 21)
    set(ANDROID_BUILD_ABI_x86 ON)
    set(ANDROID_BUILD_ABI_armeabi-v7a ON)
    set(OpenCV_DIR /home/xgd/shared/OpenCV-android-sdk/sdk/native/jni)
elseif(UNIX)
    set(OpenCV_DIR /home/xgd/shared/opencv4.5.1/lib/cmake/opencv4)
endif(MSVC)
find_package(OpenCV REQUIRED)

# rapidjson, eigen3 and openbabel. they haved been included~
set(3RDPARTY_DIR 3rdparty)

include_directories(${3RDPARTY_DIR}/include)
include_directories(${3RDPARTY_DIR}/include/inchi)

aux_source_directory(${3RDPARTY_DIR}/src OB_SRC_0)
aux_source_directory(${3RDPARTY_DIR}/src/charges/ OB_SRC_1)
aux_source_directory(${3RDPARTY_DIR}/src/depict/ OB_SRC_2)
aux_source_directory(${3RDPARTY_DIR}/src/descriptors/ OB_SRC_3)
aux_source_directory(${3RDPARTY_DIR}/src/fingerprints/ OB_SRC_4)
aux_source_directory(${3RDPARTY_DIR}/src/forcefields/ OB_SRC_5)
aux_source_directory(${3RDPARTY_DIR}/src/formats/ OB_SRC_6)
aux_source_directory(${3RDPARTY_DIR}/src/math/ OB_SRC_7)
aux_source_directory(${3RDPARTY_DIR}/src/ops/ OB_SRC_8)
aux_source_directory(${3RDPARTY_DIR}/src/stereo/ OB_SRC_9)
aux_source_directory(${3RDPARTY_DIR}/src/formats/json OB_SRC_10)
aux_source_directory(${3RDPARTY_DIR}/src/formats/libinchi OB_SRC_11)

set(OB_SRCS ${OB_SRC_0} ${OB_SRC_1} ${OB_SRC_2} ${OB_SRC_3}
    ${OB_SRC_4} ${OB_SRC_5} ${OB_SRC_6} ${OB_SRC_7}
    ${OB_SRC_8} ${OB_SRC_9} ${OB_SRC_10} ${OB_SRC_11})

# set project source here
aux_source_directory(src DIR_SRCS)
qt5_add_big_resources(BIG_RES_FILES res/big_res.qrc)
set(TS_FILES res/${PROJECT_NAME}_zh_CN.ts)
qt_create_translation(QM_FILES src ${TS_FILES})
set(RES_SRCS ${BIG_RES_FILES} src/qml.qrc res/obdata.qrc)
set(PROJECT_SOURCES ${DIR_SRCS} ${RES_SRCS} ${TS_FILES} ${OB_SRCS})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(jokejoker ${PROJECT_SOURCES})
else()
    if(ANDROID)
        add_library(jokejoker SHARED ${PROJECT_SOURCES})
    else()
        add_executable(jokejoker ${PROJECT_SOURCES})
    endif()
endif()

target_compile_definitions(jokejoker PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# link opencv and qt here
target_link_libraries(jokejoker PRIVATE
    Qt${QT_VERSION_MAJOR}::3DAnimation
    Qt${QT_VERSION_MAJOR}::3DCore
    Qt${QT_VERSION_MAJOR}::3DExtras
    Qt${QT_VERSION_MAJOR}::3DInput
    Qt${QT_VERSION_MAJOR}::3DLogic
    Qt${QT_VERSION_MAJOR}::3DRender

    Qt${QT_VERSION_MAJOR}::Quick

    Qt${QT_VERSION_MAJOR}::Core

    ${OpenCV_LIBS}
    )
