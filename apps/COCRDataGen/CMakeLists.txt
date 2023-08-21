project(COCRDataGen)
xgd_add_executable(${PROJECT_NAME} SRC_FILES main.cpp)

xgd_link_qt(${PROJECT_NAME} PRIVATE Core Widgets Gui)

xgd_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        xgd_math xgd_data xgd_handwriting spdlog_stream xgd_base xgd_vision
)