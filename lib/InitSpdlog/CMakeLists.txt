project(xgd_init_spdlog)
xgd_add_init_libraries(${PROJECT_NAME})
xgd_link_libraries(${PROJECT_NAME} PRIVATE spdlog)