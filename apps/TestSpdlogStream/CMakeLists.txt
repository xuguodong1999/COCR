project(TestSpdlogStream)
xgd_add_executable(${PROJECT_NAME} SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
xgd_link_libraries(${PROJECT_NAME} PRIVATE spdlog spdlog_stream)
xgd_link_gtest(${PROJECT_NAME})
