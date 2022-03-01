project(MaeparserTest LANGUAGES CXX)

include_directories(${DEV_INCLUDE_DIR})

find_package(Boost COMPONENTS filesystem iostreams unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIB_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_SAMPLES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} maeparser)

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})