project(xgd_handwriting)
xgd_add_library(
        ${PROJECT_NAME}
        SRC_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")
xgd_link_libraries(${PROJECT_NAME}
        PRIVATE
        spdlog
        PUBLIC
        xgd_base xgd_vision xgd_math spdlog_stream)

target_compile_definitions(${PROJECT_NAME} PRIVATE "XGD_ASSET_DIR=\"${CMAKE_SOURCE_DIR}/assets\"")
