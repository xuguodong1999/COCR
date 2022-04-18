project(test_openbabel LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core QUIET)
find_package(Boost COMPONENTS unit_test_framework QUIET)

CHECK_BOOST(${PROJECT_NAME})
CHECK_QT(${PROJECT_NAME} Core)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} TEST_SOURCE)
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/todo TEST_SOURCE)

addExecutable(${PROJECT_NAME} "${TEST_SOURCE};${openbabel_QRC}")
linkOpenBabel(${PROJECT_NAME})
linkQt(${PROJECT_NAME} Core)
linkBoost(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        "TEST_SAMPLES_PATH=std::string(\"${CMAKE_CURRENT_SOURCE_DIR}/\")")

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})