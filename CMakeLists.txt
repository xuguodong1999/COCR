cmake_minimum_required(VERSION 3.16)

project(dev LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# put all dll and exe in one directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)

option(BUILD_TESTS "Boost is required to build 3rd tests" OFF)
option(BUILD_DATA_GEN "Generate data for yolo and crnn training" ON)
option(USE_OPENCV_DNN "Whether use opencv::dnn instead of ncnn" ON)
option(BUILD_ELS_LAB "Libtorch is required to build cpp train" OFF)
option(BUILD_TUTORIALS "Libtorch is required to build 3rdparty code examples" OFF)
# android apk name
set(APP_NAME leafxy CACHE INTERNAL "")
# path to assets
set(DEV_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets CACHE INTERNAL "")
# path to 3rdparty libraries
set(DEV_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies CACHE INTERNAL "")
# path to header-only 3rdparty libraries
set(DEV_HEADER_DIR ${DEV_DEPS_DIR}/header-only CACHE INTERNAL "")
set(DEV_INCLUDE_DIR ${DEV_DEPS_DIR}/include ${DEV_DEPS_DIR}/include/inchi ${DEV_HEADER_DIR} CACHE INTERNAL "")
# remember to cite ${OpenBabel_QRC} if you link openbabel
set(OpenBabel_QRC ${DEV_ASSETS_DIR}/openbabel.qrc CACHE INTERNAL "")

# always need openmp
find_package(OpenMP)
if (OPENMP_FOUND AND NOT ANDROID)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS})
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

if (ANDROID OR WASM)
    set(BUILD_DATA_GEN OFF)
    set(USE_OPENCV_DNN OFF)
endif ()

add_subdirectory(dependencies)

add_subdirectory(app) # target as ${APP_NAME}
add_dependencies(${APP_NAME} openbabel)
add_dependencies(${APP_NAME} coordgenlibs)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()

if (BUILD_DATA_GEN)
    add_subdirectory(data_gen)
    add_dependencies(data_gen coordgenlibs)
    add_dependencies(data_gen openbabel)
endif ()

if (BUILD_ELS_LAB)
    add_subdirectory(elslab)
endif ()

if (BUILD_TUTORIALS)
    add_subdirectory(tutorials)
endif ()
