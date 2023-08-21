# coordgenlibs
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/coordgen-src)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR}/coordgen)
xgd_add_library(
        coordgen
        SRC_DIRS ${SRC_DIR}
        PRIVATE_INCLUDE_DIRS ${SRC_DIR}
        INCLUDE_DIRS ${INC_DIR}
)
xgd_link_libraries(coordgen PRIVATE maeparser)
xgd_generate_export_header(coordgen "coordgenlibs" ".hpp")