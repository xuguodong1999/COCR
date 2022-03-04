project(tools LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${DEV_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(QT_COMPONENTS Widgets)
if (NOT WASM)
    list(APPEND QT_COMPONENTS MultimediaWidgets 3DCore 3DExtras 3DRender 3DInput)
endif ()
if (NOT ANDROID) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS ${QT_COMPONENTS} LinguistTools REQUIRED)
endif ()
find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)

# ncnn has not put its include directory in cmake config yet
include_directories(${ncnn_DIR}/../../../include)

# notice that qm file is generated through cmake job
set(TS_FILES ${DEV_ASSETS_DIR}/leafxy_zh_CN.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src ${TS_FILES})
writeFileIfChanged(${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"leafxy_zh_CN.qm\">leafxy_zh_CN.qm</file>\n</qresource>\n</RCC>")

if (WITH_OPENVINO)
    add_definitions(-DWITH_OPENVINO)
endif (WITH_OPENVINO)

GET_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src)
if (ANDROID)
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST camera)
elseif (WASM)
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST camera 3d)
endif ()

if (USE_OPENCV_DNN)
    qt5_add_big_resources(MODEL_QRC ${DEV_ASSETS_DIR}/leafxy_ocv_dnn.qrc)
else ()
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST opencv_dnn_impl)
    qt5_add_big_resources(MODEL_QRC ${DEV_ASSETS_DIR}/leafxy_ncnn.qrc)
endif ()

set(PROJECT_SOURCE
        ${MODEL_QRC}
        ${OpenBabel_QRC}
        ${DEV_ASSETS_DIR}/leafxy.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)
foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
endforeach ()

# include windows exe info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/leafxy.rc)
    include_directories(${DEV_ASSETS_DIR}/windows)
endif ()

# add exe
if (ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android CACHE INTERNAL "")
    add_library(${APP_NAME} SHARED ${PROJECT_SOURCE}
            ${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest.xml)
    # override no rtti and exceptions in ncnn
    set_target_properties(ncnn PROPERTIES INTERFACE_COMPILE_OPTIONS "-fexceptions")
else ()
    add_executable(${APP_NAME} ${PROJECT_SOURCE})
endif ()

# product suffix
set_target_properties(${APP_NAME} PROPERTIES DEBUG_POSTFIX "d")

unset(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt5::${QT_COMPONENT})
endforeach ()

# link opencv, patched openbabel and qt here
target_link_libraries(${APP_NAME} PRIVATE
        openbabel coordgenlibs ncnn
        ${QT_COMPONENT_TARGETS}
        opencv_core opencv_imgproc opencv_highgui)

# on desktop, USE opencv_dnn instead of ncnn
if (NOT ANDROID AND NOT WASM)
    target_link_libraries(${APP_NAME} PRIVATE opencv_dnn opencv_imgcodecs)
endif ()

if (MSVC)
    # remove cmd for windows release
    set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE YES LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (WASM)
    copyQtWasmTemplates("${CMAKE_CURRENT_SOURCE_DIR}/wasm/*" "${APP_NAME}" "${CMAKE_CURRENT_BINARY_DIR}")
endif ()