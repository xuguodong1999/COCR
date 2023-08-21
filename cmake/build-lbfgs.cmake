# lbfgs
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/lbfgs-src/lbfgs/include)
add_library(lbfgs INTERFACE)
target_include_directories(lbfgs INTERFACE ${ROOT_DIR})
