project(tutorials LANGUAGES CXX)

include_directories(${COCR_INCLUDE_DIRS})

set(PROJECT_SOURCE_DIR_LIST ./eigen/ ./fdeep/ ./rx/)
list(LENGTH PROJECT_SOURCE_DIR_LIST PROJECT_SOURCE_DIR_LENGTH)
math(EXPR PROJECT_SOURCE_DIR_LOOP ${PROJECT_SOURCE_DIR_LENGTH}-1)

foreach (i RANGE ${PROJECT_SOURCE_DIR_LOOP})
    list(GET PROJECT_SOURCE_DIR_LIST ${i} PROJECT_SOURCE_DIR)
    string(REGEX REPLACE "(.*\\/)(.*)(\\/)" "\\2" PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE_${i}_${PROJECT_NAME})
    set(SUB_PROJECT_NAME ${PROJECT_NAME}_${PROJECT_SOURCE_DIR})
    add_executable(${SUB_PROJECT_NAME} ${PROJECT_SOURCE_${i}_${PROJECT_NAME}})
    # openmp for apple clang
    if (APPLE)
        target_link_libraries(${SUB_PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endif (APPLE)
    set_target_properties(${SUB_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
endforeach ()

if (BUILD_ELS_LAB)
    add_subdirectory(libtorch)
endif ()