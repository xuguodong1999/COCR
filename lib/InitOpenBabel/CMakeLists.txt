project(xgd_init_openbabel)
xgd_add_init_libraries(${PROJECT_NAME})
xgd_link_libraries(${PROJECT_NAME} PRIVATE openbabel)