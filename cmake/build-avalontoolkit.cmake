# avalontoolkit
set(INC_DIR ${XGD_THIRD_PARTY_DIR}/avalontoolkit-src/avalontoolkit/SourceDistribution/common)
set(SRC_DIR ${INC_DIR})
xgd_add_library(
        avalontoolkit
        STATIC # avalontoolkit has no export header preparation
        SRC_DIRS
        ${SRC_DIR}
        INCLUDE_DIRS
        ${INC_DIR}
        EXCLUDE_REGEXES
        "^(.*)test(.*)\\.c"
)
xgd_disable_warnings(avalontoolkit)