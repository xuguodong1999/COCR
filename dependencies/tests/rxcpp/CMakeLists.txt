project(RxCppTest LANGUAGES CXX)

include_directories(${COCR_INCLUDE_DIRS})

set(PROJECT_SOURCE_DIR_LIST ./ ./operators/ ./sources/ ./subjects/ ./subscriptions/)
set(PROJECT_SOURCE)
foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCES)
    list(APPEND PROJECT_SOURCE ${PROJECT_SOURCES})
endforeach ()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/bigobj>)

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})