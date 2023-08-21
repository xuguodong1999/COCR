# absl
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/abseil-src/abseil)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR}/absl)
xgd_add_library(
        absl
        STATIC
        SRC_DIRS
        ${SRC_DIR}/algorithm
        ${SRC_DIR}/base
        ${SRC_DIR}/base/internal
        ${SRC_DIR}/cleanup
        ${SRC_DIR}/cleanup/internal
        ${SRC_DIR}/container
        ${SRC_DIR}/container/internal
        ${SRC_DIR}/crc
        ${SRC_DIR}/crc/internal
        ${SRC_DIR}/debugging
        ${SRC_DIR}/debugging/internal
        ${SRC_DIR}/flags
        ${SRC_DIR}/flags/internal
        ${SRC_DIR}/functional
        ${SRC_DIR}/functional/internal
        ${SRC_DIR}/hash
        ${SRC_DIR}/hash/internal
        ${SRC_DIR}/log
        ${SRC_DIR}/log/internal
        ${SRC_DIR}/memory
        ${SRC_DIR}/meta
        ${SRC_DIR}/numeric
        ${SRC_DIR}/numeric/internal
        ${SRC_DIR}/profiling
        ${SRC_DIR}/profiling/internal
        ${SRC_DIR}/random
        ${SRC_DIR}/random/internal
        ${SRC_DIR}/status
        ${SRC_DIR}/status/internal
        ${SRC_DIR}/strings
        ${SRC_DIR}/strings/internal
        ${SRC_DIR}/strings/internal/str_format
        ${SRC_DIR}/synchronization
        ${SRC_DIR}/synchronization/internal
        ${SRC_DIR}/time
        ${SRC_DIR}/time/internal
        ${SRC_DIR}/time/internal/cctz/src
        ${SRC_DIR}/types
        ${SRC_DIR}/types/internal
        ${SRC_DIR}/utility
        INCLUDE_DIRS ${INC_DIR}
        EXCLUDE_SRC_FILES
        ${SRC_DIR}/hash/internal/print_hash_of.cc
        EXCLUDE_REGEXES
        "^(.*)test(.*)\\.cc"
        "^(.*)benchmark(.*)\\.cc"
        "^(.*)mock(.*)\\.cc"
)
if (WIN32)
    target_compile_definitions(absl PRIVATE NOMINMAX)
elseif (APPLE)
    find_library(CoreFoundation CoreFoundation REQUIRED)
endif ()
target_link_libraries(absl PRIVATE $<$<BOOL:${ANDROID}>:log> $<$<PLATFORM_ID:Darwin>:${CoreFoundation}>)