project(coordgenlibs LANGUAGES CXX)

include_directories(${COCR_INCLUDE_DIRS})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)

# include windows dll info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${COCR_DATA_DIR}/windows/3rd.rc)
    include_directories(${COCR_DATA_DIR}/windows)
endif ()

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE maeparser)

target_compile_definitions(${PROJECT_NAME} PRIVATE COORDGENLIBS_LIBRARY)