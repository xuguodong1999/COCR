# openbabel
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/openbabel-src/openbabel)
set(OB_INC_DIR ${ROOT_DIR}/include)
set(OB_SRC_DIR ${ROOT_DIR}/src)
set(OB_DATA_DIR ${ROOT_DIR}/data)
xgd_add_library(
        openbabel
        SRC_DIRS
        ${OB_SRC_DIR}
        ${OB_SRC_DIR}/charges
        ${OB_SRC_DIR}/descriptors
        ${OB_SRC_DIR}/forcefields
        ${OB_SRC_DIR}/ops
        ${OB_SRC_DIR}/depict
        ${OB_SRC_DIR}/fingerprints
        ${OB_SRC_DIR}/formats
        ${OB_SRC_DIR}/formats/json
        ${OB_SRC_DIR}/formats/xml
        ${OB_SRC_DIR}/math
        ${OB_SRC_DIR}/stereo
        EXCLUDE_SRC_FILES
        ${OB_SRC_DIR}/dlhandler_unix.cpp
        ${OB_SRC_DIR}/dlhandler_win32.cpp
        ${OB_SRC_DIR}/snprintf.c
        ${OB_SRC_DIR}/formats/xtcformat.cpp
        ${OB_SRC_DIR}/formats/exampleformat.cpp
        ${OB_SRC_DIR}/formats/ArrayList.c
        ${OB_SRC_DIR}/memcpy.c
        ${OB_SRC_DIR}/strncasecmp.c
        INCLUDE_DIRS ${OB_INC_DIR}
)

xgd_link_omp(openbabel)
xgd_link_libraries(openbabel
        PRIVATE
        zlib xml2 inchi maeparser rapidjson lbfgs cairo
        rdkit_Depictor rdkit_GraphMol rdkit_RDGeometryLib
        PUBLIC eigen)

xgd_generate_export_header(openbabel "openbabel" ".hpp")

set(HAVE_SYS_TIME_H ${XGD_HAVE_SYS_TIME_H})
set(HAVE_RPC_XDR_H ${XGD_HAVE_RPC_XDR_H})
set(HAVE_RINT ${XGD_HAVE_RINT})
set(HAVE_SNPRINTF ${XGD_HAVE_SNPRINTF})
set(HAVE_SRANDDEV ${XGD_HAVE_SRANDDEV})
set(HAVE_STRCASECMP ${XGD_HAVE_STRCASECMP})
set(HAVE_STRNCASECMP ${XGD_HAVE_STRNCASECMP})

set(HAVE_CONIO_H OFF)
set(HAVE_STATIC_LIBXML ON)
set(HAVE_STRINGS_H ON)
set(HAVE_TIME_H ON)
set(HAVE_REGEX_H ON)
set(HAVE_SHARED_POINTER ON)
set(HAVE_EIGEN ON)
set(HAVE_EIGEN3 ON)
set(HAVE_STATIC_INCHI ON)
set(HAVE_LIBZ ON)
set(HAVE_SSTREAM ON)
set(HAVE_CLOCK_T ON)
set(OB_SHARED_PTR_HEADER "memory")
set(OB_SHARED_PTR_IMPLEMENTATION "std::shared_ptr")
set(_OPENBABEL_GEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/openbabel/include)
configure_file(
        ${OB_SRC_DIR}/config.h.cmake
        ${_OPENBABEL_GEN_INCLUDE_DIR}/openbabel/babelconfig.h
)
target_include_directories(openbabel PUBLIC ${_OPENBABEL_GEN_INCLUDE_DIR})
if (HAVE_RPC_XDR_H)
    target_sources(openbabel PRIVATE ${OB_SRC_DIR}/formats/xtcformat.cpp)
endif ()

file(GLOB_RECURSE OB_DATA_FILES ${OB_DATA_DIR}/*)
list(REMOVE_ITEM OB_DATA_FILES ${OB_DATA_DIR}/splash.png)
xgd_generate_text_assets(openbabel openbabel SRC_FILES ${OB_DATA_FILES})

target_compile_options(
        openbabel PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wno-switch>
)
