project(xgd_base)
xgd_add_library(
        ${PROJECT_NAME}
        SRC_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Data
        INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")
xgd_link_libraries(${PROJECT_NAME}
        PRIVATE
        spdlog
        PUBLIC
        boost_header spdlog_stream)
xgd_link_qt(${PROJECT_NAME} PUBLIC Core Gui)