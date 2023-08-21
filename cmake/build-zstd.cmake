# zstd
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/zstd-src/zstd/lib)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR})
xgd_add_library(
        zstd
        SRC_DIRS
        ${SRC_DIR}
        ${SRC_DIR}/common
        ${SRC_DIR}/compress
        ${SRC_DIR}/decompress
        ${SRC_DIR}/deprecated
        ${SRC_DIR}/dictBuilder
        ${SRC_DIR}/legacy
        INCLUDE_DIRS ${INC_DIR}
)
target_compile_definitions(zstd PRIVATE ZSTD_DISABLE_ASM ZSTD_MULTITHREAD)
xgd_link_threads(zstd)
if (BUILD_SHARED_LIBS AND WIN32)
    target_compile_definitions(zstd PRIVATE ZSTD_DLL_EXPORT PUBLIC ZSTD_DLL_IMPORT)
endif ()
