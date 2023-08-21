# taskflow
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/taskflow-src/taskflow)
add_library(taskflow INTERFACE)
target_include_directories(taskflow INTERFACE ${ROOT_DIR})
