cmake_minimum_required(VERSION 3.16)
project(dev LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Boost is required to build 3rd tests" OFF)
# path to assets
set(COCR_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets CACHE INTERNAL "")
# path to 3rdparty libraries
set(COCR_DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies CACHE INTERNAL "")
# path to header-only 3rdparty libraries
set(COCR_INCLUDE_DIR ${COCR_DEPENDENCIES_DIR}/include CACHE INTERNAL "")
# remember to cite ${OPENBABEL_DATA_QRC_FILE} if you link openbabel
set(OPENBABEL_DATA_QRC_FILE ${COCR_DATA_DIR}/openbabel.qrc CACHE INTERNAL "")
# put all dll and exe in one directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out)

# always need openmp
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else (OPENMP_FOUND)
    message(WARNING "openmp not found. Acceleration for alkane generation disabled.")
endif (OPENMP_FOUND)

if (MSVC)
    set(Boost_USE_STATIC_LIBS OFF CACHE INTERNAL "")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_definitions(_USE_MATH_DEFINES) # for M_PI macro
elseif (APPLE)
    include_directories(/opt/homebrew/include/)
endif ()


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies)