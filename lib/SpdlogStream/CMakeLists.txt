project(spdlog_stream)
xgd_add_library(
        ${PROJECT_NAME}
        SRC_FILES ./Stream.cpp
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")
xgd_link_libraries(${PROJECT_NAME} PRIVATE spdlog)