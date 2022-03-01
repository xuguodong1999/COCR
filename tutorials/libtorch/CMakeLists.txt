project(tut_torch LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/macro.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${DEV_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/public)

find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIB_DIR})

# collect all directory-path as a list
GET_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})
list(LENGTH PROJECT_SOURCE_DIR_LIST PROJECT_SOURCE_DIR_LENGTH)
math(EXPR PROJECT_SOURCE_DIR_LOOP ${PROJECT_SOURCE_DIR_LENGTH}-1)

# collect dir name from directory-path list
set(TMP_PROJECT_SOURCE_DIR_LIST)
foreach (i RANGE ${PROJECT_SOURCE_DIR_LOOP})
    list(GET PROJECT_SOURCE_DIR_LIST ${i} PROJECT_SOURCE_DIR)
    # skip ./public
    if (NOT ${PROJECT_SOURCE_DIR} STREQUAL "public")
        list(APPEND TMP_PROJECT_SOURCE_DIR_LIST ${PROJECT_SOURCE_DIR})
    endif ()
endforeach ()
set(PROJECT_SOURCE_DIR_LIST ${TMP_PROJECT_SOURCE_DIR_LIST})
unset(TMP_PROJECT_SOURCE_DIR_LIST)

# make codes under ./public a standalone lib
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/public PROJECT_SOURCE_PUBLIC)
add_library(${PROJECT_NAME}_public SHARED ${PROJECT_SOURCE_PUBLIC})
target_link_libraries(${PROJECT_NAME}_public PRIVATE
        ${TORCH_LIBRARIES} ${OpenCV_LIBS} png)

list(LENGTH PROJECT_SOURCE_DIR_LIST PROJECT_SOURCE_DIR_LENGTH)
math(EXPR PROJECT_SOURCE_DIR_LOOP ${PROJECT_SOURCE_DIR_LENGTH}-1)

foreach (i RANGE ${PROJECT_SOURCE_DIR_LOOP})
    list(GET PROJECT_SOURCE_DIR_LIST ${i} PROJECT_SOURCE_DIR)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE_${i}_${PROJECT_NAME})
    set(SUB_PROJECT_NAME ${PROJECT_NAME}_${PROJECT_SOURCE_DIR})
    add_executable(${SUB_PROJECT_NAME} ${PROJECT_SOURCE_${i}_${PROJECT_NAME}})
    add_dependencies(${SUB_PROJECT_NAME} ${PROJECT_NAME}_public)
    set_target_properties(${SUB_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
    target_link_libraries(${SUB_PROJECT_NAME} PRIVATE
            ${TORCH_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} ${PROJECT_NAME}_public)
endforeach ()