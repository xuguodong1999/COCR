# zlib
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/zlib-src/zlib)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR})
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/zlib/include)
xgd_add_library(
        zlib
        SRC_DIRS ${SRC_DIR}
        INCLUDE_DIRS ${INC_DIR} ${GEN_DIR}
)
xgd_generate_export_header(zlib "zlib" ".h")
if (BUILD_SHARED_LIBS)
    target_compile_definitions(zlib PRIVATE ZLIB_DLL)
endif ()
# zlib uses extern to declare functions
xgd_disable_warnings(zlib)
