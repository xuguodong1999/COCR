# yaehmop
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/yaehmop-src)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR}/yaehmop/tightbind)
xgd_add_library(
        yaehmop
        STATIC # yaehmop has no export header preparation
        SRC_DIRS
        ${SRC_DIR}
        INCLUDE_DIRS ${INC_DIR}
        PRIVATE_INCLUDE_DIRS ${SRC_DIR}
        EXCLUDE_SRC_FILES
        ${SRC_DIR}/test_driver.c
        ${SRC_DIR}/main.c
)
target_compile_definitions(yaehmop PUBLIC UNDERSCORE_FORTRAN PRIVATE NEED_DSIGN)
xgd_disable_warnings(yaehmop)
