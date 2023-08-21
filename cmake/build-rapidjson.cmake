# rapidjson
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/rapidjson-src/rapidjson/include)
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${ROOT_DIR})
