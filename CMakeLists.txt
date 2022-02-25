cmake_minimum_required(VERSION 3.16)

project(dev LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Boost is required to build 3rd tests" OFF)
option(BUILD_ELS_LAB "Libtorch is required to build cpp train" OFF)
option(BUILD_TUTORIALS "Libtorch is required to build 3rdparty code examples" OFF)
# path to assets
set(COCR_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets CACHE INTERNAL "")
# path to 3rdparty libraries
set(COCR_DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies CACHE INTERNAL "")
# path to header-only 3rdparty libraries
set(COCR_HEADER_DIR ${COCR_DEPENDENCIES_DIR}/header-only CACHE INTERNAL "")
set(COCR_INCLUDE_DIR ${COCR_DEPENDENCIES_DIR}/include CACHE INTERNAL "")
set(COCR_INCLUDE_DIRS ${COCR_INCLUDE_DIR} ${COCR_HEADER_DIR} CACHE INTERNAL "")
# remember to cite ${OPENBABEL_DATA_QRC_FILE} if you link openbabel
set(OPENBABEL_DATA_QRC_FILE ${COCR_DATA_DIR}/openbabel.qrc CACHE INTERNAL "")
# put all dll and exe in one directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)

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
endif ()

if (ANDROID)
    include(${CMAKE_SOURCE_DIR}/cmake/macro.cmake)
    GET_SUB_DIR_NAME(QT_MODULE_DIR_LIST ${QT_DIR}/../)
    foreach (QT_MODULE ${QT_MODULE_DIR_LIST})
        set(${QT_MODULE}_DIR ${QT_DIR}/../${QT_MODULE} CACHE INTERNAL "")
    endforeach ()
    unset(QT_MODULE_DIR_LIST)
elseif (NOT ${Qt5_DIR})
    set(Qt5_DIR ${QT_DIR} CACHE INTERNAL "")
endif ()
set(APP_NAME dev CACHE INTERNAL "")
set(QT_COMPONENTS Widgets MultimediaWidgets 3DCore 3DExtras 3DRender 3DInput)
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} LinguistTools REQUIRED)
unset(QT_COMPONENTS)

add_subdirectory(dependencies)
add_subdirectory(leafxy)

add_dependencies(dev openbabel)
add_dependencies(dev coordgenlibs)

if (NOT ANDROID)
    add_subdirectory(data_gen)
    add_dependencies(data_gen coordgenlibs)
    add_dependencies(data_gen openbabel)
endif ()

if (BUILD_ELS_LAB OR BUILD_TUTORIALS)
    find_package(OpenCV QUIET)
    find_package(Torch QUIET)
    set(LIBTORCH_READY ${Torch_FOUND} AND ${OpenCV_FOUND})
    if (NOT ${LIBTORCH_READY})
        message(WARNING "Torch_FOUND=" ${Torch_FOUND} ",OpenCV_FOUND=" ${OpenCV_FOUND}
                ", so BUILD_ELS_LAB and BUILD_TUTORIALS set to false")
        set(BUILD_ELS_LAB OFF)
        set(BUILD_TUTORIALS OFF)
    endif ()
    unset(LIBTORCH_READY)
endif ()

if (BUILD_ELS_LAB)
    add_subdirectory(elslab)
endif ()

if (BUILD_TUTORIALS)
    add_subdirectory(tutorials)
endif ()