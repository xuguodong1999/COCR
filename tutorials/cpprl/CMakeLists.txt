project(tut_cpprl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${DEV_HEADER_DIR})

find_package(Torch QUIET)

if (NOT ${Torch_FOUND})
    message(WARNING "Torch not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

include_directories(${TORCH_INCLUDE_DIRS})
set(PROJECT_SOURCE_DIR_LIST ./ ./algorithms/ ./distributions/ ./generators/ ./model/)
list(LENGTH PROJECT_SOURCE_DIR_LIST PROJECT_SOURCE_DIR_LENGTH)
math(EXPR PROJECT_SOURCE_DIR_LOOP ${PROJECT_SOURCE_DIR_LENGTH}-1)

include(${CMAKE_SOURCE_DIR}/cmake/macro.cmake)
set(PROJECT_SOURCE)
foreach (i RANGE ${PROJECT_SOURCE_DIR_LOOP})
    list(GET PROJECT_SOURCE_DIR_LIST ${i} PROJECT_SOURCE_DIR)
    PATH_2_DIR_NAME(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE_${i}_${PROJECT_NAME})
    list(APPEND PROJECT_SOURCE ${PROJECT_SOURCE_${i}_${PROJECT_NAME}})
endforeach ()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_LIBRARIES})