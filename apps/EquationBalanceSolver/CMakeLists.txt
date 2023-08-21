project(EquationBalanceSolver)
xgd_add_executable(${PROJECT_NAME} SRC_FILES main.cpp)

xgd_link_libraries(${PROJECT_NAME} PRIVATE xgd_chem spdlog)
