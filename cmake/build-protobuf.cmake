# protobuf
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/protobuf-src/protobuf)
set(INC_DIR ${ROOT_DIR}/src)
set(SRC_DIR ${ROOT_DIR}/src/google/protobuf)
set(utf8_range_DIR ${ROOT_DIR}/third_party/utf8_range)
xgd_add_library(
        protobuf
        SRC_DIRS
        ${SRC_DIR}
        ${SRC_DIR}/io
        ${SRC_DIR}/json
        ${SRC_DIR}/json/internal
        ${SRC_DIR}/stubs
        ${SRC_DIR}/util
        SRC_FILES
        ${utf8_range_DIR}/naive.c
        ${utf8_range_DIR}/range2-neon.c
        ${utf8_range_DIR}/range2-sse.c
        ${utf8_range_DIR}/utf8_validity.cc
        INCLUDE_DIRS ${INC_DIR}
        PRIVATE_INCLUDE_DIRS ${utf8_range_DIR}
        EXCLUDE_REGEXES
        "^(.*)test(.*)\\.c"
        "(.*)lazy_field_heavy.cc"
)
if (BUILD_SHARED_LIBS)
    target_compile_definitions(
            protobuf
            PUBLIC PROTOBUF_USE_DLLS
            PRIVATE LIBPROTOBUF_EXPORTS
    )
endif ()
xgd_link_libraries(protobuf PUBLIC absl)
target_link_libraries(protobuf PRIVATE $<$<BOOL:${ANDROID}>:log>)
