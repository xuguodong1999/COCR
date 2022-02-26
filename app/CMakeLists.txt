project(leafxy_app LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ncnn has not put its include directory in cmake config yet
include_directories(${COCR_INCLUDE_DIR})

set(QT_COMPONENTS Widgets MultimediaWidgets 3DCore 3DExtras 3DRender 3DInput)
if (NOT ANDROID) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS  ${QT_COMPONENTS} LinguistTools REQUIRED)
endif ()
find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)
include_directories(${ncnn_DIR}/../../../include)

# notice that qm file is generated through cmake job
set(TS_FILES ${COCR_ASSETS_DIR}/leafxy_zh_CN.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"leafxy_zh_CN.qm\">leafxy_zh_CN.qm</file>\n</qresource>\n</RCC>")

if (WITH_OPENVINO)
    add_definitions(-DWITH_OPENVINO)
endif (WITH_OPENVINO)

if (ANDROID)
    set(ANDROID_MIN_SDK_VERSION 24)
    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/settings CACHE INTERNAL "")
    qt5_add_big_resources(LEAFXY_MODEL_QRC_FILE ${COCR_ASSETS_DIR}/leafxy_ncnn.qrc)
else ()
    qt5_add_big_resources(LEAFXY_MODEL_QRC_FILE ${COCR_ASSETS_DIR}/leafxy_ocv_dnn.qrc)
endif ()

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# source dir
set(SRC_DIR_LIST ./ ./ui ./ncnn_impl ./2d ./3d ./chem ./ocr)
if (NOT ANDROID)
    list(APPEND SRC_DIR_LIST ./opencv_dnn_impl)
endif ()

# collect source files
set(PROJECT_SOURCE ${LEAFXY_MODEL_QRC_FILE} ${COCR_ASSETS_DIR}/leafxy.qrc ${OpenBabel_QRC})
foreach (SRC_DIR ${SRC_DIR_LIST})
    set(_PROJECT_SOURCE)
    aux_source_directory(${SRC_DIR} PROJECT_SOURCES)
    list(APPEND PROJECT_SOURCE ${PROJECT_SOURCES})
endforeach ()

# include windows exe info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${COCR_ASSETS_DIR}/windows/leafxy.rc)
    include_directories(${COCR_ASSETS_DIR}/windows)
endif ()

list(APPEND PROJECT_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc)

# add exe
if (ANDROID)
    add_library(${APP_NAME} SHARED ${PROJECT_SOURCE} ${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest.xml)
    # override no rtti and exceptions in ncnn
    set_target_properties(ncnn PROPERTIES INTERFACE_COMPILE_OPTIONS "-fexceptions")
else ()
    add_executable(${APP_NAME} ${PROJECT_SOURCE})
endif ()

# product suffix
set_target_properties(${APP_NAME} PROPERTIES DEBUG_POSTFIX "d")

# remove cmd for windows release
if (MSVC)
    set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE YES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif ()

set(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt5::${QT_COMPONENT})
endforeach ()

# link opencv, patched openbabel and qt here
target_link_libraries(${APP_NAME} PRIVATE
        openbabel coordgenlibs ncnn
        ${QT_COMPONENT_TARGETS}
        opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui)

# on desktop, i prefer to use large yolo weights by opencv_dnn, instead of ncnn
if (NOT ANDROID)
    target_link_libraries(${APP_NAME} PRIVATE opencv_dnn)
endif ()