# freetype
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/freetype-src/freetype)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)
set(BUILD_DIR ${ROOT_DIR}/builds)
set(SRC_SUFFIX_LIST
        autofit/autofit.c
        base/ftbase.c base/ftbbox.c base/ftbdf.c base/ftbitmap.c base/ftcid.c base/ftfstype.c base/ftgasp.c base/ftglyph.c base/ftgxval.c base/ftinit.c base/ftmm.c base/ftotval.c base/ftpatent.c base/ftpfr.c base/ftstroke.c base/ftsynth.c base/fttype1.c base/ftwinfnt.c
        bdf/bdf.c
        bzip2/ftbzip2.c
        cache/ftcache.c
        cff/cff.c
        cid/type1cid.c
        gzip/ftgzip.c
        lzw/ftlzw.c
        pcf/pcf.c
        pfr/pfr.c
        psaux/psaux.c
        pshinter/pshinter.c
        psnames/psnames.c
        raster/raster.c
        sdf/sdf.c
        sfnt/sfnt.c
        smooth/smooth.c
        svg/svg.c
        truetype/truetype.c
        type1/type1.c type42/type42.c
        winfonts/winfnt.c)
set(SRC_FILES "")
foreach (SRC_SUFFIX ${SRC_SUFFIX_LIST})
    list(APPEND SRC_FILES ${SRC_DIR}/${SRC_SUFFIX})
endforeach ()

if (UNIX)
    list(APPEND SRC_FILES ${BUILD_DIR}/unix/ftsystem.c)
elseif (WIN32)
    list(APPEND SRC_FILES ${BUILD_DIR}/windows/ftsystem.c)
else ()
    list(APPEND SRC_FILES ${SRC_DIR}/base/ftsystem.c)
endif ()

if (WIN32)
    list(APPEND SRC_FILES ${BUILD_DIR}/windows/ftdebug.c)
elseif (WINCE)
    list(APPEND SRC_FILES ${BUILD_DIR}/wince/ftdebug.c)
else ()
    list(APPEND SRC_FILES ${SRC_DIR}/base/ftdebug.c)
endif ()

xgd_add_library(
        freetype
        SRC_FILES
        ${SRC_FILES}
        INCLUDE_DIRS
        ${INC_DIR}
)
target_compile_definitions(freetype PRIVATE FT2_BUILD_LIBRARY FT_CONFIG_OPTION_SYSTEM_ZLIB)
if (WIN32)
    target_compile_definitions(freetype PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(freetype PRIVATE DLL_EXPORT)
    endif ()
endif ()
xgd_link_libraries(freetype PRIVATE zlib png)
if (XGD_HAVE_FCNTL_H)
    target_compile_definitions(freetype PRIVATE HAVE_FCNTL_H)
endif ()
if (XGD_HAVE_UNISTD_H)
    target_compile_definitions(freetype PRIVATE HAVE_UNISTD_H)
endif ()