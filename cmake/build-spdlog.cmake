# spdlog
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/spdlog-src/spdlog)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)
xgd_add_library(
        spdlog
        SRC_DIRS ${SRC_DIR}
        INCLUDE_DIRS ${INC_DIR}
)
target_compile_definitions(spdlog PUBLIC SPDLOG_COMPILED_LIB)
if (BUILD_SHARED_LIBS)
    target_compile_definitions(
            spdlog
            PRIVATE SPDLOG_SHARED_LIB FMT_EXPORT
            PUBLIC FMT_SHARED
    )
endif ()
xgd_link_threads(spdlog)
target_link_libraries(spdlog PUBLIC $<$<BOOL:${ANDROID}>:log>)
