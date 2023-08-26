# maeparser
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/maeparser-src)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR}/maeparser)
xgd_add_library(
        maeparser
        SRC_DIRS ${SRC_DIR}
        PRIVATE_INCLUDE_DIRS ${SRC_DIR}
        INCLUDE_DIRS ${INC_DIR}
)
xgd_link_libraries(maeparser
        PRIVATE boost_iostreams boost_spirit boost_algorithm
        PUBLIC boost_dynamic_bitset)
xgd_generate_export_header(maeparser "maeparser" ".hpp")
