project(leafxy_app LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ncnn has not put its include directory in cmake config yet
include_directories(${DEV_INCLUDE_DIR})

set(QT_COMPONENTS Widgets)
if (NOT WASM)
    list(APPEND QT_COMPONENTS MultimediaWidgets 3DCore 3DExtras 3DRender 3DInput)
endif ()
if (NOT ANDROID AND NOT WASM) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS ${QT_COMPONENTS} LinguistTools REQUIRED)
endif ()
find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)
include_directories(${ncnn_DIR}/../../../include)

# notice that qm file is generated through cmake job
set(TS_FILES ${DEV_ASSETS_DIR}/leafxy_zh_CN.ts)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"leafxy_zh_CN.qm\">leafxy_zh_CN.qm</file>\n</qresource>\n</RCC>")

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
if (WITH_OPENVINO)
    add_definitions(-DWITH_OPENVINO)
endif (WITH_OPENVINO)

# source dir
set(SRC_DIR_LIST ./ ./ui ./ncnn_impl ./2d ./chem ./ocr)
if (USE_OPENCV_DNN)
    list(APPEND SRC_DIR_LIST ./opencv_dnn_impl)
    qt5_add_big_resources(MODEL_QRC ${DEV_ASSETS_DIR}/leafxy_ocv_dnn.qrc)
else ()
    qt5_add_big_resources(MODEL_QRC ${DEV_ASSETS_DIR}/leafxy_ncnn.qrc)
endif ()

if (NOT WASM)
    list(APPEND SRC_DIR_LIST ./3d ./camera)
endif ()

if (NOT WASM AND NOT ANDROID)
    list(APPEND SRC_DIR_LIST ./camera)
endif ()

# collect source files
set(PROJECT_SOURCE ${OpenBabel_QRC} ${DEV_ASSETS_DIR}/leafxy.qrc ${CMAKE_CURRENT_BINARY_DIR}/leafxy_trans.qrc)

foreach (SRC_DIR ${SRC_DIR_LIST})
    aux_source_directory(${SRC_DIR} DIR_SRC)
    list(APPEND PROJECT_SOURCE ${DIR_SRC})
    unset(DIR_SRC)
endforeach ()

# include windows exe info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/leafxy.rc)
    include_directories(${DEV_ASSETS_DIR}/windows)
endif ()


# add exe
if (ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android CACHE INTERNAL "")
    add_library(${APP_NAME} SHARED
            ${PROJECT_SOURCE}
            ${MODEL_QRC}
            ${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest.xml)
    # override no rtti and exceptions in ncnn
    set_target_properties(ncnn PROPERTIES INTERFACE_COMPILE_OPTIONS "-fexceptions")
else ()
    add_executable(${APP_NAME} ${PROJECT_SOURCE} ${MODEL_QRC})
endif ()

# product suffix
set_target_properties(${APP_NAME} PROPERTIES DEBUG_POSTFIX "d")

# remove cmd for windows release
if (MSVC)
    set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE YES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif ()

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

if (WASM)
    set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS
            "-s WASM=1 -s FULL_ES2=1 -s FULL_ES3=1 -s USE_WEBGL2=1 -s EXIT_RUNTIME=1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=[\"UTF16ToString\",\"stringToUTF16\"] --bind -s FETCH=1 -O2 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s TOTAL_MEMORY=1GB")
endif ()