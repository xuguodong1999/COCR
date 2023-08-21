# cutlass
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/cutlass-src/cutlass)
add_library(cutlass INTERFACE)
target_include_directories(cutlass INTERFACE ${ROOT_DIR}/include ${ROOT_DIR}/tools/util/include)
target_link_libraries(cutlass INTERFACE CUDA::cudart)
target_include_directories(cutlass INTERFACE ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})