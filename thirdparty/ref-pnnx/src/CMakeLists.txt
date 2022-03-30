project(pnnx)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/pass_level0 pnnx_pass_level0_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/pass_level1 pnnx_pass_level1_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/pass_level2 pnnx_pass_level2_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/pass_level3 pnnx_pass_level3_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/pass_level4 pnnx_pass_level4_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/pass_level5 pnnx_pass_level5_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/pass_ncnn pnnx_pass_ncnn_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} pnnx_SRC)

if (NOT MSVC)
    add_definitions(-Wall -Wextra)
endif ()

add_executable(pnnx
        ${pnnx_SRC}
        ${pnnx_pass_level0_SRCS}
        ${pnnx_pass_level1_SRCS}
        ${pnnx_pass_level2_SRCS}
        ${pnnx_pass_level3_SRCS}
        ${pnnx_pass_level4_SRCS}
        ${pnnx_pass_level5_SRCS}
        ${pnnx_pass_ncnn_SRCS})

if (PNNX_COVERAGE)
    target_compile_options(${PROJECT_NAME} PUBLIC -coverage -fprofile-arcs -ftest-coverage)
    target_link_libraries(${PROJECT_NAME} PUBLIC -coverage -lgcov)
endif ()

if (TorchVision_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE TorchVision::TorchVision)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_LIBRARIES})
if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
endif ()
target_include_directories(${PROJECT_NAME} PRIVATE ${TORCH_INCLUDE_DIRS})
