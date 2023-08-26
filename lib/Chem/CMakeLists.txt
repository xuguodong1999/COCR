project(xgd_chem)
xgd_add_library(
        ${PROJECT_NAME}
        SRC_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Impl
        ${CMAKE_CURRENT_SOURCE_DIR}/Equation
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")
xgd_link_libraries(${PROJECT_NAME}
        PRIVATE
        openbabel spdlog spdlog_stream boost_algorithm
        PUBLIC
        xgd_base xgd_math)