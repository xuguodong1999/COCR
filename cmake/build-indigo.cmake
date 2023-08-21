# indigo
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/indigo-src/indigo)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/include/indigo)
set(API_LIB_DIR ${ROOT_DIR}/api)
set(CORE_LIB_DIR ${ROOT_DIR}/core/indigo-core)
set(RENDER_LIB_DIR ${ROOT_DIR}/core/render2d)
set(THIRD_LIB_DIR ${ROOT_DIR}/third_party)
set(CORE_EXCLUDE_REGEXES "shmem(.*)")
if (MSVC OR MINGW)
    list(APPEND CORE_EXCLUDE_REGEXES "(.*)posix(.*)")
else ()
    list(APPEND CORE_EXCLUDE_REGEXES "(.*)win32(.*)")
endif ()
xgd_add_library(
        indigo-core
        STATIC
        SRC_DIRS
        ${CORE_LIB_DIR}/common/base_c
        ${CORE_LIB_DIR}/common/base_cpp
        ${CORE_LIB_DIR}/common/gzip
        ${CORE_LIB_DIR}/common/lzw
        ${CORE_LIB_DIR}/common/math
        ${CORE_LIB_DIR}/graph/src
        # ${CORE_LIB_DIR}/layout/patmake
        ${CORE_LIB_DIR}/layout/src
        ${CORE_LIB_DIR}/molecule/src
        ${CORE_LIB_DIR}/reaction/src
        SRC_FILES
        INCLUDE_DIRS ${CORE_LIB_DIR}
        PRIVATE_INCLUDE_DIRS
        ${CORE_LIB_DIR}/common
        ${THIRD_LIB_DIR}/cppcodec
        ${THIRD_LIB_DIR}/object_threadsafe
        EXCLUDE_REGEXES ${CORE_EXCLUDE_REGEXES}
)
xgd_add_library(
        indigo-render2d
        STATIC
        SRC_DIRS
        ${RENDER_LIB_DIR}/src
        INCLUDE_DIRS
        ${CORE_LIB_DIR}
        ${RENDER_LIB_DIR}
        PRIVATE_INCLUDE_DIRS
        ${CORE_LIB_DIR}/common
        ${THIRD_LIB_DIR}/object_threadsafe
        ${THIRD_LIB_DIR}/googlefonts
)
target_compile_definitions(indigo-render2d PRIVATE "USE_FONT_MANAGER")
if (EMSCRIPTEN)
    target_compile_definitions(indigo-render2d PRIVATE "RENDER_EMSCRIPTEN")
endif ()
xgd_link_libraries(indigo-render2d PUBLIC freetype cairo indigo-core)
xgd_link_libraries(indigo-core PRIVATE inchi zlib tinyxml2 PUBLIC rapidjson)

xgd_generate_export_header(indigo-core "indigo_core" ".h")

xgd_add_library(
        bingo-nosql
        STATIC
        SRC_DIRS
        ${API_LIB_DIR}/c/bingo-nosql/src
        ${API_LIB_DIR}/c/bingo-nosql/src/mmf
        INCLUDE_DIRS
        ${API_LIB_DIR}/c/bingo-nosql
        PRIVATE_INCLUDE_DIRS
        ${CORE_LIB_DIR}/common
        ${THIRD_LIB_DIR}/object_threadsafe
        ${API_LIB_DIR}/c/indigo/src
)
xgd_generate_export_header(bingo-nosql "bingo_nosql" ".h")
xgd_link_libraries(bingo-nosql PUBLIC indigo PRIVATE indigo-core)

xgd_add_library(
        indigo
        STATIC
        SRC_DIRS
        ${API_LIB_DIR}/c/indigo/src
        INCLUDE_DIRS
        ${API_LIB_DIR}/c/indigo
        ${GEN_DIR}
        PRIVATE_INCLUDE_DIRS
        ${CORE_LIB_DIR}/common
        ${THIRD_LIB_DIR}/object_threadsafe
)
configure_file(
        ${API_LIB_DIR}/c/indigo/src/indigo_version.h.in
        ${GEN_DIR}/indigo_version.h
)
xgd_generate_export_header(indigo "indigo" ".h")
xgd_link_libraries(indigo PRIVATE indigo-core tinyxml2)

xgd_add_library(
        indigo-inchi
        STATIC
        SRC_DIRS
        ${API_LIB_DIR}/c/indigo-inchi/src
        INCLUDE_DIRS
        ${API_LIB_DIR}/c/indigo-inchi
        PRIVATE_INCLUDE_DIRS
        ${CORE_LIB_DIR}/common
        ${THIRD_LIB_DIR}/object_threadsafe
        ${API_LIB_DIR}/c/indigo/src
)
xgd_generate_export_header(indigo-inchi "indigo_inchi" ".h")
xgd_link_libraries(indigo-inchi PRIVATE indigo indigo-core inchi)

xgd_add_library(
        indigo-renderer
        STATIC
        SRC_DIRS
        ${API_LIB_DIR}/c/indigo-renderer/src
        INCLUDE_DIRS
        ${API_LIB_DIR}/c/indigo-renderer
        PRIVATE_INCLUDE_DIRS
        ${CORE_LIB_DIR}/common
        ${THIRD_LIB_DIR}/object_threadsafe
        ${API_LIB_DIR}/c/indigo/src
)
xgd_generate_export_header(indigo-renderer "indigo_renderer" ".h")
xgd_link_libraries(indigo-renderer PUBLIC indigo indigo-render2d PRIVATE indigo-core)

xgd_add_library(
        indigo-cpp
        STATIC
        SRC_DIRS
        ${API_LIB_DIR}/cpp/src
        INCLUDE_DIRS
        ${API_LIB_DIR}/cpp/src
)
xgd_link_libraries(indigo-cpp PUBLIC indigo indigo-inchi indigo-renderer bingo-nosql)
xgd_disable_warnings(indigo-core)
xgd_disable_warnings(indigo-render2d)
xgd_disable_warnings(indigo)
xgd_disable_warnings(indigo-inchi)
xgd_disable_warnings(indigo-renderer)
xgd_disable_warnings(indigo-cpp)
xgd_disable_warnings(bingo-nosql)
