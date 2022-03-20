project(test_rxqt LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Test REQUIRED)

GET_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})

foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    unset(PROJECT_SOURCE)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
    set(TARGET_NAME ${PROJECT_NAME}_${PROJECT_SOURCE_DIR})

    add_executable(${TARGET_NAME} ${PROJECT_SOURCE})
    set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
    target_link_libraries(${TARGET_NAME} Qt5::Test)
    target_include_directories(${TARGET_NAME} PRIVATE
            ${DEV_HEADER_DIR})
    add_test(NAME ${TARGET_NAME}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endforeach ()