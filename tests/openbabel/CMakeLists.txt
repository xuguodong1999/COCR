project(OpenBabelTest LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
include_directories(${DEV_INCLUDE_DIR})

set(QT_COMPONENTS Core)
find_package(Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIB_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/todo PROJECT_SOURCE)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE} ${OpenBabel_QRC})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_SAMPLES_PATH=std::string(\"${CMAKE_CURRENT_SOURCE_DIR}/\")")

set(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt5::${QT_COMPONENT})
endforeach ()

target_link_libraries(${PROJECT_NAME} ${QT_COMPONENT_TARGETS} ${Boost_LIBRARIES} openbabel)

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})