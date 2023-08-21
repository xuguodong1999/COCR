# eigen
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/eigen-src/eigen)
add_library(eigen INTERFACE)
target_include_directories(eigen INTERFACE ${ROOT_DIR})
target_compile_definitions(eigen INTERFACE EIGEN_USE_THREADS)
xgd_link_omp(eigen LINK_TYPE INTERFACE)
xgd_link_threads(eigen LINK_TYPE INTERFACE)
