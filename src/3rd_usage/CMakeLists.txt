project(3rdUsage LANGUAGES CXX)

include_directories(${COCR_INCLUDE_DIR})

set(PROJECT_SOURCE_DIR_LIST ./eigen/ ./fdeep/ ./rx/)
list(LENGTH PROJECT_SOURCE_DIR_LIST PROJECT_SOURCE_DIR_LENGTH)
math(EXPR PROJECT_SOURCE_DIR_LOOP ${PROJECT_SOURCE_DIR_LENGTH}-1)

foreach (i RANGE ${PROJECT_SOURCE_DIR_LOOP})
    list(GET PROJECT_SOURCE_DIR_LIST ${i} PROJECT_SOURCE_DIR)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE_${i})
    string(REGEX REPLACE "(.*\\/)(.*)(\\/)" "\\2" DIR_NAME ${PROJECT_SOURCE_DIR})
    set(SUB_PROJECT_NAME ${PROJECT_NAME}_${DIR_NAME})
    add_executable(${SUB_PROJECT_NAME} ${PROJECT_SOURCE_${i}})
    # openmp for apple clang
    if (APPLE)
        target_link_libraries(${SUB_PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endif (APPLE)
    set_target_properties(${SUB_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
endforeach ()

