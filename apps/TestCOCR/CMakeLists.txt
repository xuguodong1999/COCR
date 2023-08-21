project(TestCOCR)
xgd_add_executable(${PROJECT_NAME} SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
xgd_link_libraries(${PROJECT_NAME} PRIVATE spdlog xgd_base xgd_math xgd_chem)
xgd_link_gtest(${PROJECT_NAME})
xgd_generate_text_assets(
        ${PROJECT_NAME}
        chemical_equations
        SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../assets/datasets/chemical-equations/subset.txt
)