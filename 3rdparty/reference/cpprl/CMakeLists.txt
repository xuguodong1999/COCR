project(ref_cpprl LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Torch QUIET)

if (NOT ${Torch_FOUND})
    message(WARNING "Torch not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

include_directories(${TORCH_INCLUDE_DIRS})

GET_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
endforeach ()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${DEV_HEADER_DIR})