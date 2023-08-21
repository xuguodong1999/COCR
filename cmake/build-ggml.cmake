# ggml
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/ggml-src/ggml)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)
xgd_add_library(
        ggml
        STATIC
        SRC_FILES
        ${SRC_DIR}/ggml.c
        PRIVATE_INCLUDE_DIRS
        ${INC_DIR}/ggml
        INCLUDE_DIRS
        ${INC_DIR}
)