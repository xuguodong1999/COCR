project(app LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT_COMPONENTS Widgets MultimediaWidgets 3DCore 3DExtras 3DRender 3DInput)
if (WASM)
    list(REMOVE_ITEM QT_COMPONENTS MultimediaWidgets 3DCore 3DExtras 3DRender 3DInput)
elseif (ANDROID)
    list(REMOVE_ITEM QT_COMPONENTS MultimediaWidgets)
endif ()

if (NOT ANDROID) # do it in root/CMakeLists.txt
    find_package(Qt5 COMPONENTS ${QT_COMPONENTS} LinguistTools REQUIRED)
endif ()
find_package(OpenCV REQUIRED)

# notice that qm file is generated through cmake job
set(TS_FILES ${DEV_ASSETS_DIR}/leafxy_zh_CN.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
writeFileIfChanged(${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"leafxy_zh_CN.qm\">leafxy_zh_CN.qm</file>\n</qresource>\n</RCC>")

if (NOT ${BUILD_PRIVATE})
    if (USE_OPENCV_DNN)
        qt5_add_big_resources(model_QRC ${DEV_ASSETS_DIR}/leafxy_ocv_dnn.qrc)
    else ()
        qt5_add_big_resources(model_QRC ${DEV_ASSETS_DIR}/leafxy_ncnn.qrc)
    endif ()
endif ()

GET_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})
if (ANDROID)
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST camera)
elseif (WASM)
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST camera 3d)
endif ()

set(PROJECT_SOURCE
        ${model_QRC} # empty if set BUILD_PRIVATE to ON
        ${openbabel_QRC}
        ${DEV_ASSETS_DIR}/leafxy.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
endforeach ()

# add exe
if (ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR ${DEV_ASSETS_DIR}/android CACHE INTERNAL "")
    add_library(${APP_NAME} SHARED ${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest.xml ${PROJECT_SOURCE})
else ()
    addExecutable(${APP_NAME} "${PROJECT_SOURCE}")
endif ()

target_include_directories(${APP_NAME} PRIVATE ${cocr_INCLUDE_DIR})
addLibraryDeps(${APP_NAME} els_ckit)
addLibraryDeps(${APP_NAME} els_base)
addLibraryDeps(${APP_NAME} els_cocr)
addLibraryDeps(${APP_NAME} els_ocv)

linkOpenCV(${APP_NAME})
linkQt(${APP_NAME} "${QT_COMPONENTS}")

if (MSVC)
    # remove cmd for windows release
    set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE YES LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (WASM)
    copyQtWasmTemplates("${DEV_ASSETS_DIR}/wasm/*" "${APP_NAME}" "${CMAKE_CURRENT_BINARY_DIR}")
endif ()