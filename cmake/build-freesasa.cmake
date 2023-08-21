# freesasa
set(INC_DIR ${XGD_THIRD_PARTY_DIR}/freesasa-src/freesasa/src)
set(SRC_DIR ${INC_DIR})
xgd_add_library(
        freesasa
        STATIC # freesasa has no export header preparation
        SRC_DIRS
        ${SRC_DIR}
        INCLUDE_DIRS
        ${INC_DIR}
        EXCLUDE_SRC_FILES
        ${SRC_DIR}/cif.cc
        ${SRC_DIR}/example.c
        ${SRC_DIR}/json.c
        ${SRC_DIR}/main.cc
)
xgd_link_libraries(freesasa PRIVATE xml2)
xgd_disable_warnings(freesasa)