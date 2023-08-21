# ade
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/ade-src/ade/sources/ade)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/source)
xgd_add_library(ade STATIC SRC_DIRS ${SRC_DIR} INCLUDE_DIRS ${INC_DIR})
