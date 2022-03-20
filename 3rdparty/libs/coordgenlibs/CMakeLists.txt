project(coordgenlibs LANGUAGES CXX)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)

# include windows dll info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/3rd.rc)
    include_directories(${DEV_ASSETS_DIR}/windows)
endif ()

add_library(${PROJECT_NAME} ${PROJECT_SOURCE})
add_dependencies(${PROJECT_NAME} maeparser)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${coordgenlibs_INCLUDE_DIR}
        ${maeparser_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE maeparser)

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()