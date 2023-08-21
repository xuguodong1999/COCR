project(xgd_data)
xgd_add_library(
        ${PROJECT_NAME}
        SRC_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Impl
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")
xgd_link_libraries(${PROJECT_NAME}
        PRIVATE
        spdlog coordgen openbabel spdlog_stream
        PUBLIC
        xgd_base xgd_chem xgd_math xgd_vision xgd_handwriting)
xgd_link_qt(${PROJECT_NAME} PRIVATE Gui)
target_compile_definitions(${PROJECT_NAME} PRIVATE "XGD_ASSET_DIR=\"${CMAKE_SOURCE_DIR}/assets\"")