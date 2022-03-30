project(ref_torch LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${DEV_HEADER_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/public)

find_package(OpenCV QUIET)
find_package(Torch QUIET)
find_package(Boost COMPONENTS program_options QUIET)

if (NOT ${Torch_FOUND})
    message(WARNING "Torch not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

if (NOT ${OpenCV_FOUND})
    message(WARNING "OpenCV not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

if (NOT ${Boost_FOUND})
    message(WARNING "Boost not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIB_DIR})

# make codes under ./public a standalone lib
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/public PROJECT_SOURCE)
add_library(${PROJECT_NAME}_public ${PROJECT_SOURCE})
target_link_libraries(${PROJECT_NAME}_public PRIVATE
        ${TORCH_LIBRARIES} ${OpenCV_LIBS} png)
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}_public)
target_include_directories(${PROJECT_NAME}_public PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

GET_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})
list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST public)

foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    unset(PROJECT_SOURCE)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
    set(TARGET_NAME ${PROJECT_NAME}_${PROJECT_SOURCE_DIR})
    add_executable(${TARGET_NAME} ${PROJECT_SOURCE})
    add_dependencies(${TARGET_NAME} ${PROJECT_NAME}_public)
    set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
    target_link_libraries(${TARGET_NAME} PRIVATE
            ${TORCH_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} ${PROJECT_NAME}_public)
endforeach ()