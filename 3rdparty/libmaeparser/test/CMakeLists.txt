project(test_maeparser LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

find_package(Boost COMPONENTS filesystem iostreams unit_test_framework QUIET)

CHECK_BOOST(${PROJECT_NAME})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} TEST_SOURCE)

addExecutable(${PROJECT_NAME} "${TEST_SOURCE}")
linkMaeparser(${PROJECT_NAME})
linkBoost(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        "TEST_SAMPLES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})