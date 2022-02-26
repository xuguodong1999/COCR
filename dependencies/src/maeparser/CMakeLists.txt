project(maeparser LANGUAGES CXX)

include_directories(${COCR_INCLUDE_DIR})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)

# include windows dll info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${COCR_ASSETS_DIR}/windows/3rd.rc)
    include_directories(${COCR_ASSETS_DIR}/windows)
endif ()

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_COMPONENT_TARGETS})

target_compile_definitions(${PROJECT_NAME} PRIVATE MAEPARSER_LIBRARY)