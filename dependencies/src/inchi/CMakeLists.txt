project(inchi LANGUAGES C)

include_directories(${DEV_DEPS_DIR}/include/inchi)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/ixa IXA_PROJECT_SOURCE)

# include windows dll info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/3rd.rc)
    include_directories(${DEV_ASSETS_DIR}/windows)
endif ()

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE} ${IXA_PROJECT_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_API_LIB)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})