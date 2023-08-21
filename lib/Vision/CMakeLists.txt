project(xgd_vision)
xgd_add_library(
        ${PROJECT_NAME}
        SRC_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")

xgd_link_libraries(${PROJECT_NAME}
        PUBLIC
        xgd_base
        PRIVATE
        spdlog_stream
        spdlog opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui)
xgd_link_qt(${PROJECT_NAME} PRIVATE Gui)
