cmake_minimum_required(VERSION 3.16)

project(dev)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Boost is required to build 3rd tests" OFF)
option(BUILD_SHARED_LIBS "Build as shared or static libs" ON)
option(BUILD_DATA_GEN "Generate data for yolo and crnn training" ON)
option(USE_OPENCV_DNN "Whether use opencv::dnn instead of ncnn" ON)
option(BUILD_ELS_LAB "Libtorch is required to build cpp train" OFF)
option(BUILD_TUTORIALS "Libtorch is required to build 3rdparty code examples" OFF)
# android apk name
set(APP_NAME leafxy CACHE INTERNAL "")
# path to assets
set(DEV_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets CACHE INTERNAL "")
# path to 3rdparty libraries
set(DEV_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty CACHE INTERNAL "")
# path to header-only 3rdparty libraries
set(DEV_HEADER_DIR ${DEV_DEPS_DIR}/header-only CACHE INTERNAL "")
set(inchi_INCLUDE_DIR ${DEV_DEPS_DIR}/libs/inchi/include CACHE INTERNAL "")
set(maeparser_INCLUDE_DIR ${DEV_DEPS_DIR}/libs/maeparser/include CACHE INTERNAL "")
set(coordgenlibs_INCLUDE_DIR ${DEV_DEPS_DIR}/libs/coordgenlibs/include CACHE INTERNAL "")
set(openbabel_INCLUDE_DIR ${DEV_DEPS_DIR}/libs/openbabel/include CACHE INTERNAL "")
set(cocr_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/include CACHE INTERNAL "")
# remember to cite ${OpenBabel_QRC} if you link openbabel
set(OpenBabel_QRC ${DEV_ASSETS_DIR}/openbabel.qrc CACHE INTERNAL "")

# always need openmp
find_package(OpenMP)
if (OPENMP_FOUND AND NOT ANDROID)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

if (MSVC)
    set(Boost_USE_STATIC_LIBS OFF CACHE INTERNAL "")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_definitions(_USE_MATH_DEFINES) # for M_PI macro
elseif (APPLE)
    include_directories(/opt/homebrew/include/)
elseif (ANDROID) # use apk_shell for android-qt project
    include(${CMAKE_SOURCE_DIR}/cmake/FindAndroidDeps.cmake)
elseif (WASM)
    include(${CMAKE_SOURCE_DIR}/cmake/FindWasmDeps.cmake)
endif ()

if (NOT WASM AND NOT ANDROID)
    # put all dll and exe in one directory
    # wasm and android has specific output archive
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)
else ()
    # disable unrelated project for wasm and android
    message(WARNING "Target non-desktop platform, USE_OPENCV_DNN, BUILD_DATA_GEN, BUILD_TESTS, BUILD_ELS_LAB and BUILD_TUTORIALS set to OFF")
    set(USE_OPENCV_DNN OFF)

    set(BUILD_DATA_GEN OFF)
    set(BUILD_TESTS OFF)
    set(BUILD_ELS_LAB OFF)
    set(BUILD_TUTORIALS OFF)
endif ()

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)

add_subdirectory(3rdparty)
add_subdirectory(lib)
add_subdirectory(tools)

# add_subdirectory(tools) # target as ${APP_NAME}
# add_dependencies(${APP_NAME} openbabel)
# add_dependencies(${APP_NAME} coordgenlibs)
#
#if (NOT ANDROID AND NOT IOS)
#    add_subdirectory(tools_qml)
#endif ()
#
#if (BUILD_TESTS)
#    add_subdirectory(tests/maeparser)
#    add_subdirectory(tests/coordgenlibs)
#    add_subdirectory(tests/rxcpp)
#    add_subdirectory(tests/rxqt)
#    add_subdirectory(tests/openbabel)
#endif ()
#
#if (BUILD_DATA_GEN)
#    add_subdirectory(data_gen)
#    add_dependencies(data_gen openbabel)
#endif ()
#
#if (BUILD_ELS_LAB)
#    add_subdirectory(elslab)
#endif ()
#
#if (BUILD_TUTORIALS)
#    add_subdirectory(tutorials)
#    # standalone proj
#    add_subdirectory(tutorials/cpprl)
#    add_subdirectory(tutorials/libtorch)
#endif ()