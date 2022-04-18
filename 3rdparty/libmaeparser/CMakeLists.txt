project(maeparser LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)

addLibrary(${PROJECT_NAME} "${PROJECT_SOURCE}")

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()