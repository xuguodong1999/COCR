# pixman
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/pixman-src/pixman/pixman)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR})
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/pixman/include)
set(EXCLUDE_REGEXES "")
if (NOT XGD_OPT_ARCH_MIPS)
    list(APPEND EXCLUDE_REGEXES "(.*)-mips-(.*)")
    list(APPEND EXCLUDE_REGEXES "(.*)pixman-mips-dspr2.c")
endif ()
# not work yet
if (TRUE OR NOT XGD_FLAG_NEON)
    list(APPEND EXCLUDE_REGEXES "(.*)-arm-(.*)")
    list(APPEND EXCLUDE_REGEXES "(.*)pixman-arm-simd.c")
    list(APPEND EXCLUDE_REGEXES "(.*)pixman-arm-neon.c")
endif ()
if (NOT XGD_FLAG_SSE_ALL)
    list(APPEND EXCLUDE_REGEXES "(.*)-sse2(.*)")
    list(APPEND EXCLUDE_REGEXES "(.*)-ssse3(.*)")
endif ()
xgd_add_library(
        pixman
        SRC_DIRS
        ${SRC_DIR}
        INCLUDE_DIRS
        ${INC_DIR}
        ${GEN_DIR}
        EXCLUDE_SRC_FILES
        ${SRC_DIR}/pixman-region.c
        ${SRC_DIR}/pixman-vmx.c
        EXCLUDE_REGEXES
        ${EXCLUDE_REGEXES}
)
set(PIXMAN_VERSION_MAJOR 0)
set(PIXMAN_VERSION_MINOR 42)
set(PIXMAN_VERSION_MICRO 2)
configure_file(
        ${SRC_DIR}/pixman-version.h.in
        ${GEN_DIR}/pixman-version.h
)
target_compile_definitions(pixman PRIVATE -DPACKAGE)
if (EMSCRIPTEN)
    target_compile_definitions(pixman PRIVATE -DPIXMAN_NO_TLS)
else ()
    target_compile_definitions(pixman PRIVATE -DHAVE_PTHREADS)
endif ()
if (ANDROID)
    add_dependencies(pixman cpufeatures_ndk_compat)
    target_link_libraries(pixman PRIVATE cpufeatures_ndk_compat)
endif ()
# not work yet
if (FALSE AND XGD_FLAG_NEON)
    target_compile_definitions(pixman PRIVATE USE_ARM_NEON USE_ARM_SIMD)
    target_sources(
            pixman
            PRIVATE
            ${SRC_DIR}/pixman-arm-neon-asm.S
            ${SRC_DIR}/pixman-arm-neon-asm-bilinear.S
            ${SRC_DIR}/pixman-arma64-neon-asm.S
            ${SRC_DIR}/pixman-arma64-neon-asm-bilinear.S
    )
endif ()
xgd_generate_export_header(pixman "pixman" ".h")
xgd_link_threads(pixman)
