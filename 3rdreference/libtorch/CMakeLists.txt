project(ref_torch LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/public)

find_package(OpenCV QUIET)
find_package(Torch QUIET)
find_package(Boost COMPONENTS program_options QUIET)

CHECK_OPENCV(${PROJECT_NAME})
CHECK_LIBTORCH(${PROJECT_NAME})
CHECK_BOOST(${PROJECT_NAME})

# make codes under ./public a standalone lib
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/public PROJECT_SOURCE)
addLibrary(${PROJECT_NAME}_public "${PROJECT_SOURCE}")
linkLibtorch(${PROJECT_NAME}_public)
linkOpenCV(${PROJECT_NAME}_public)
target_link_libraries(${PROJECT_NAME}_public PRIVATE png)

GET_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})
list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST public)

foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    unset(PROJECT_SOURCE)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
    set(TARGET_NAME ${PROJECT_NAME}_${PROJECT_SOURCE_DIR})
    addExecutable(${TARGET_NAME} "${PROJECT_SOURCE}")
    add_dependencies(${TARGET_NAME} ${PROJECT_NAME}_public)
    target_link_libraries(${TARGET_NAME} PRIVATE ${PROJECT_NAME}_public)
endforeach ()
