project(app)

set(CMAKE_CXX_STANDARD 17)

# for desktop app, import qt in sub modules
if (NOT ANDROID)
    set(QT_COMPONENTS Widgets 3DCore 3DExtras 3DRender 3DInput LinguistTools)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} REQUIRED)
endif (NOT ANDROID)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)

# notice that qm file is generated through cmake job
set(TS_FILES ../resources/${APP_NAME}_zh_CN.ts)
qt5_create_translation(QM_FILES ../app ${TS_FILES})

# on all cases, import model through qrc
qt5_add_big_resources(BIG_RES_FILES ../resources/big_res.qrc)

set(RES_SRCS ${BIG_RES_FILES} ../resources/obdata.qrc ../resources/app.qrc)

# include dir
include_directories(../openbabel/include ../coordgenlibs/include ../ncnn/include ./)

# source dir
set(SRC_DIR_LIST ./ ./ui ./ncnn_impl ./2d ./3d ./composer)
if (NOT ANDROID)
    list(APPEND SRC_DIR_LIST ./opencv_dnn_impl)
endif (NOT ANDROID)

# collect source files
set(PROJECT_SOURCES ${RES_SRCS})
foreach (SRC_DIR ${SRC_DIR_LIST})
    aux_source_directory(${SRC_DIR} PROJECT_SOURCE)
    list(APPEND PROJECT_SOURCES ${PROJECT_SOURCE})
endforeach (SRC_DIR SRC_DIR_LIST)

if (MSVC)
    set(WIN_RC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../resources)
    list(APPEND PROJECT_SOURCES ${WIN_RC_DIR}/leafxy.rc)
    include_directories(${WIN_RC_DIR})
endif (MSVC)

# add exe
if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${APP_NAME} ${PROJECT_SOURCES})
else ()
    if (ANDROID)
        add_library(${APP_NAME} SHARED ${PROJECT_SOURCES})
    else ()
        add_executable(${APP_NAME} ${PROJECT_SOURCES})
    endif ()
endif ()

# for M_PI macro
if (MSVC)
    add_compile_definitions(_USE_MATH_DEFINES)
endif (MSVC)

# product suffix
set_target_properties(${APP_NAME} PROPERTIES DEBUG_POSTFIX "d")

# for future qml cases
target_compile_definitions(${APP_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# link opencv, patched openbabel and qt here
target_link_libraries(${APP_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::3DCore
        Qt${QT_VERSION_MAJOR}::3DInput
        Qt${QT_VERSION_MAJOR}::3DExtras
        Qt${QT_VERSION_MAJOR}::3DRender
        Qt${QT_VERSION_MAJOR}::Widgets
        opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui
        ncnn
        openbabel
        coordgen)

# on desktop, i prefer to use large yolo weights by opencv_dnn, instead of ncnn
if (NOT ANDROID)
    target_link_libraries(${APP_NAME} PRIVATE opencv_dnn)
endif (NOT ANDROID)
