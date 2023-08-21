# rxcpp
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/rxcpp-src/rxcpp/Rx/v2/src)
add_library(rxcpp INTERFACE)
target_include_directories(rxcpp INTERFACE ${ROOT_DIR})
