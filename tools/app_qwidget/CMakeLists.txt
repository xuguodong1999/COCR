project(app_qwidget LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT_COMPONENTS Widgets)
if (NOT WASM)
    list(APPEND QT_COMPONENTS MultimediaWidgets 3DCore 3DExtras 3DRender 3DInput)
endif ()
if (NOT ANDROID) # do it in root/CMakeLists.txt
    find_package(Qt5 COMPONENTS ${QT_COMPONENTS} LinguistTools REQUIRED)
endif ()
find_package(OpenCV REQUIRED)

find_package(ncnn REQUIRED)
# ncnn has not put its include directory in cmake config yet
include_directories(${ncnn_DIR}/../../../include)
if (ANDROID)
    # override no rtti and exceptions in ncnn
    set_target_properties(ncnn PROPERTIES INTERFACE_COMPILE_OPTIONS "-fexceptions")
endif ()

# notice that qm file is generated through cmake job
set(TS_FILES ${DEV_ASSETS_DIR}/leafxy_zh_CN.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
writeFileIfChanged(${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"leafxy_zh_CN.qm\">leafxy_zh_CN.qm</file>\n</qresource>\n</RCC>")

if (USE_OPENVINO)
    add_definitions(-DUSE_OPENVINO)
endif (USE_OPENVINO)

if (USE_OPENCV_DNN)
    add_definitions(-DUSE_OPENCV_DNN)
    qt5_add_big_resources(MODEL_QRC ${DEV_ASSETS_DIR}/leafxy_ocv_dnn.qrc)
else ()
    qt5_add_big_resources(MODEL_QRC ${DEV_ASSETS_DIR}/leafxy_ncnn.qrc)
endif ()

GET_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})
if (ANDROID)
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST camera)
elseif (WASM)
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST camera 3d)
endif ()

set(PROJECT_SOURCE
        ${MODEL_QRC}
        ${openbabel_QRC}
        ${DEV_ASSETS_DIR}/leafxy.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
endforeach ()

# include windows exe info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/leafxy.rc)
    include_directories(${DEV_ASSETS_DIR}/windows)
endif ()

# add exe
if (ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android CACHE INTERNAL "")
    add_library(${APP_NAME} SHARED ${PROJECT_SOURCE} ${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest.xml)
else ()
    add_executable(${APP_NAME} ${PROJECT_SOURCE})
endif ()

# product suffix
set_target_properties(${APP_NAME} PROPERTIES DEBUG_POSTFIX "d")

unset(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt5::${QT_COMPONENT})
endforeach ()

target_include_directories(${APP_NAME} PRIVATE
        ${DEV_INCLUDE_DIR}
        ${cocr_INCLUDE_DIR})

# link opencv, patched openbabel and qt here
target_link_libraries(${APP_NAME} PRIVATE
        cocr::chem
        cocr::base
        cocr::ocr
        cocr::ncnn_crnn
        cocr::opencv_util
        openbabel
        coordgenlibs
        ncnn
        opencv_core
        opencv_imgproc
        opencv_highgui
        ${QT_COMPONENT_TARGETS})

# on desktop, USE opencv_dnn instead of ncnn
if (USE_OPENCV_DNN)
    target_link_libraries(${APP_NAME} PRIVATE cocr::ocv_yolo)
else ()
    target_link_libraries(${APP_NAME} PRIVATE cocr::ncnn_yolo)
endif ()

if (MSVC)
    # remove cmd for windows release
    set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE YES LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (WASM)
    copyQtWasmTemplates("${CMAKE_CURRENT_SOURCE_DIR}/wasm/*" "${APP_NAME}" "${CMAKE_CURRENT_BINARY_DIR}")
endif ()