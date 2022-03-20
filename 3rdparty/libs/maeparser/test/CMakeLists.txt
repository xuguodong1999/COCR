project(test_maeparser LANGUAGES CXX)

find_package(Boost COMPONENTS filesystem iostreams unit_test_framework QUIET)

if (NOT ${Boost_FOUND})
    message(WARNING "Boost not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIB_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} TEST_SOURCE)

add_executable(${PROJECT_NAME} ${TEST_SOURCE})
add_dependencies(${PROJECT_NAME} maeparser)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_SAMPLES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} maeparser)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${maeparser_INCLUDE_DIR})

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})