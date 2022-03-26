project(test_openbabel LANGUAGES CXX)

set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

find_package(Boost COMPONENTS unit_test_framework QUIET)

if (NOT ${Boost_FOUND})
    message(WARNING "Boost not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIB_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} TEST_SOURCE)
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/todo TEST_SOURCE)

add_executable(${PROJECT_NAME} ${TEST_SOURCE} ${openbabel_QRC})
add_dependencies(${PROJECT_NAME} openbabel)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_SAMPLES_PATH=std::string(\"${CMAKE_CURRENT_SOURCE_DIR}/\")")

target_link_libraries(${PROJECT_NAME} Qt5::Core ${Boost_LIBRARIES} openbabel)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${coordgenlibs_INCLUDE_DIR}
        ${maeparser_INCLUDE_DIR}
        ${inchi_INCLUDE_DIR}
        ${openbabel_INCLUDE_DIR}
        ${DEV_HEADER_DIR})

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})