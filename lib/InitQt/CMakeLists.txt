project(xgd_init_qt)
xgd_add_init_libraries(${PROJECT_NAME})
xgd_link_qt(${PROJECT_NAME} PRIVATE Core)
xgd_link_libraries(${PROJECT_NAME} PRIVATE spdlog)