# inchi
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/inchi-src/inchi)
set(INC_DIR ${ROOT_DIR}/INCHI_BASE/src)
set(SRC_DIR ${INC_DIR})
set(API_SRC_DIR ${ROOT_DIR}/INCHI_API/libinchi/src)
xgd_add_library(
        inchi
        SRC_DIRS ${SRC_DIR} ${API_SRC_DIR} ${API_SRC_DIR}/ixa
        PRIVATE_INCLUDE_DIRS ${SRC_DIR}/ixa ${SRC_DIR}
        INCLUDE_DIRS ${INC_DIR}
)
xgd_generate_export_header(inchi "inchi" ".h")
target_compile_definitions(inchi PRIVATE TARGET_API_LIB)
xgd_disable_warnings(inchi)