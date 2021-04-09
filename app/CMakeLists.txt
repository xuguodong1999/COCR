project(app CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)

# notice that qm file is generated through cmake job
set(TS_FILES ${RES_ROOT}/${APP_NAME}_zh_CN.ts)
qt5_create_translation(QM_FILES ${LEAFXY_ROOT}/app ${TS_FILES})

# on all cases, import model through qrc
qt5_add_big_resources(BIG_RES_FILES ${RES_ROOT}/big_res.qrc)

set(RES_SRCS ${BIG_RES_FILES} ${RES_ROOT}/obdata.qrc ${RES_ROOT}/app.qrc)

# include dir
include_directories(${3RD_LIB_DIR}/openbabel/include ${3RD_LIB_DIR}/coordgenlibs/include ./)
if (MSVC AND NOT BUILD_SHARED_LIBS)
    include_directories(${3RD_LIB_DIR}/ncnn-static-lib/include)
else (MSVC AND NOT BUILD_SHARED_LIBS)
    include_directories(${3RD_LIB_DIR}/ncnn-lib/include)
endif (MSVC AND NOT BUILD_SHARED_LIBS)

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
    list(APPEND PROJECT_SOURCES ${RES_ROOT}/leafxy.rc)
    include_directories(${RES_ROOT})
endif (MSVC)

#set_source_files_properties(${PROJECT_SOURCES} PROPERTIES LANGUAGE CXX)

if (NOT BUILD_SHARED_LIBS)
    add_definitions(-DLEAFXY_STATIC)
endif (NOT BUILD_SHARED_LIBS)

if (WITH_OPENVINO)
    add_definitions(-DWITH_OPENVINO)
endif (WITH_OPENVINO)

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

# product suffix
set_target_properties(${APP_NAME} PROPERTIES DEBUG_POSTFIX "d")

# for future qml cases
target_compile_definitions(${APP_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# link opencv, patched openbabel and qt here
target_link_libraries(${APP_NAME} PRIVATE
        openbabel coordgen ncnn
        Qt${QT_VERSION_MAJOR}::3DCore
        Qt${QT_VERSION_MAJOR}::3DInput
        Qt${QT_VERSION_MAJOR}::3DExtras
        Qt${QT_VERSION_MAJOR}::3DRender
        Qt${QT_VERSION_MAJOR}::Widgets
        opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui)

# on desktop, i prefer to use large yolo weights by opencv_dnn, instead of ncnn
if (NOT ANDROID)
    target_link_libraries(${APP_NAME} PRIVATE opencv_dnn)
endif (NOT ANDROID)
