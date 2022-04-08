cmake_minimum_required(VERSION 3.16)

project(dev)

# C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# gcc -fvisibility=hidden, but qt-android may need reflection
if (NOT ANDROID)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
endif ()

# data_gen for handwriting ocsr job
option(BUILD_DATA_GEN "Generate data for yolo and crnn training" ON)
# backend for yolo and crnn in app
option(USE_OPENCV_DNN "Whether use opencv::dnn instead of ncnn" OFF)
# run on intel NCS 2
option(USE_OPENVINO "Whether use opencv::dnn with openvino backend" OFF)
# 1. build codes under private dir, need libtorch
# 2. enable load models from local disk instead of qrc file system
option(BUILD_PRIVATE "Whether build elslab and other private modules, libtorch and Qml are required." OFF)

# how add_library works
option(BUILD_SHARED_LIBS "Build as shared or static libs" ON)

# mainly for 3rdparty tests and tutorials
option(BUILD_TESTS "Boost is required to build 3rd tests" OFF)
option(BUILD_REFERENCES "Libtorch is required to build 3rdparty code examples" OFF)

# android apk name
set(APP_NAME leafxy CACHE INTERNAL "")

# path to assets
set(DEV_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets CACHE INTERNAL "")
# path to 3rdparty libraries
set(DEV_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CACHE INTERNAL "")
# remember to cite ${openbabel_QRC} if you link openbabel
set(openbabel_QRC ${DEV_DEPS_DIR}/libopenbabel/data/openbabel.qrc CACHE INTERNAL "")

# path to header-only 3rdparty libraries
set(DEV_HEADER_DIR ${DEV_DEPS_DIR}/header-only CACHE INTERNAL "")
# other 3rdparty include dirs
set(inchi_INCLUDE_DIR ${DEV_DEPS_DIR}/libinchi/include CACHE INTERNAL "")
set(maeparser_INCLUDE_DIR ${DEV_DEPS_DIR}/libmaeparser/include CACHE INTERNAL "")
set(coordgenlibs_INCLUDE_DIR ${DEV_DEPS_DIR}/libcoordgenlibs/include CACHE INTERNAL "")
set(openbabel_INCLUDE_DIR ${DEV_DEPS_DIR}/libopenbabel/include CACHE INTERNAL "")
set(cocr_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cocr_core/include CACHE INTERNAL "")
# remember to convert <ncnn/header.h> to <header.h>
set(ncnn_INCLUDE_DIR ${DEV_DEPS_DIR}/libncnn/include)

# always enable openmp
find_package(OpenMP)
if (OPENMP_FOUND AND NOT ANDROID)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

# allow qt5 deprecated warnings
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)

if (MSVC)
    # correct charset
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    # for M_PI macro
    add_compile_definitions(_USE_MATH_DEFINES)
    if (BUILD_SHARED_LIBS)
        # fix boost config
        set(Boost_USE_STATIC_LIBS OFF CACHE INTERNAL "")
    endif ()
elseif (APPLE AND NOT IOS)
    # for omp.h
    include_directories(/opt/homebrew/include/)
elseif (ANDROID)
    # notice qt-5.15.2 only support single apk in one cmake project
    # download ncnn and opencv android-sdk
    include(${CMAKE_SOURCE_DIR}/cmake/FindAndroidDeps.cmake)
elseif (WASM)
    # download ncnn and opencv wasm-sdk
    include(${CMAKE_SOURCE_DIR}/cmake/FindWasmDeps.cmake)
endif ()

if (NOT WASM AND NOT ANDROID AND NOT IOS)
    # in desktop env, put all dll and exe in one directory, for convenience
    # wasm and android has specific output archive
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)
else ()
    # disable unrelated project for wasm and android
    message(WARNING "Target non-desktop platform, USE_OPENCV_DNN, BUILD_DATA_GEN, BUILD_TESTS, BUILD_PRIVATE and BUILD_REFERENCES set to OFF")
    set(USE_OPENCV_DNN OFF)

    set(BUILD_DATA_GEN OFF)
    set(BUILD_TESTS OFF)
    set(BUILD_PRIVATE OFF)
    set(BUILD_REFERENCES OFF)
endif ()

# build ncnn from source
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)
add_subdirectory(thirdparty)

add_subdirectory(cocr_core)
add_subdirectory(cocr_tools)
add_subdirectory(private)