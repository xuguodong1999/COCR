# cairo
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/cairo-src/cairo)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/cairo/include)
set(CAIRO_COMPILE_DEFINITIONS "")
set(CAIRO_BOILERPLATE_SRC_FILES
        boilerplate/cairo-boilerplate-test-surfaces.c
        boilerplate/check-link.c
        )
set(CAIRO_SRC_FILES
        boilerplate/cairo-boilerplate.c boilerplate/cairo-boilerplate.c boilerplate/cairo-boilerplate-getopt.c boilerplate/cairo-boilerplate-system.c
        src/cairo-analysis-surface.c src/cairo-arc.c src/cairo-array.c src/cairo-atomic.c
        src/cairo-base64-stream.c src/cairo-base85-stream.c src/cairo-bentley-ottmann-rectangular.c src/cairo-bentley-ottmann-rectilinear.c src/cairo-bentley-ottmann.c src/cairo-botor-scan-converter.c src/cairo-boxes-intersect.c src/cairo-boxes.c
        src/cairo-cache.c src/cairo-clip-boxes.c src/cairo-clip-polygon.c src/cairo-clip-region.c src/cairo-clip-surface.c src/cairo-clip-tor-scan-converter.c src/cairo-clip.c src/cairo-color.c src/cairo-composite-rectangles.c src/cairo-compositor.c src/cairo-contour.c
        src/cairo-damage.c src/cairo-debug.c src/cairo-default-context.c src/cairo-device.c
        src/cairo-error.c
        src/cairo-fallback-compositor.c src/cairo-fixed.c src/cairo-font-face-twin-data.c src/cairo-font-face-twin.c src/cairo-font-face.c src/cairo-font-options.c src/cairo-freed-pool.c src/cairo-freelist.c
        src/cairo-gstate.c
        src/cairo-hash.c src/cairo-hull.c
        src/cairo-image-compositor.c src/cairo-image-info.c src/cairo-image-source.c src/cairo-image-surface.c
        src/cairo-line.c src/cairo-lzw.c
        src/cairo-mask-compositor.c src/cairo-matrix.c src/cairo-mempool.c src/cairo-mesh-pattern-rasterizer.c src/cairo-misc.c src/cairo-mono-scan-converter.c src/cairo-mutex.c
        src/cairo-no-compositor.c
        src/cairo-observer.c src/cairo-output-stream.c
        src/cairo-paginated-surface.c src/cairo-path-bounds.c src/cairo-path-fill.c src/cairo-path-fixed.c src/cairo-path-in-fill.c src/cairo-path-stroke-boxes.c src/cairo-path-stroke-polygon.c src/cairo-path-stroke-traps.c src/cairo-path-stroke-tristrip.c src/cairo-path-stroke.c src/cairo-path.c src/cairo-pattern.c src/cairo-pen.c src/cairo-polygon-intersect.c src/cairo-polygon-reduce.c src/cairo-polygon.c
        src/cairo-raster-source-pattern.c src/cairo-recording-surface.c src/cairo-rectangle.c src/cairo-rectangular-scan-converter.c src/cairo-region.c src/cairo-rtree.c
        src/cairo-scaled-font.c src/cairo-shape-mask-compositor.c src/cairo-slope.c src/cairo-spans-compositor.c src/cairo-spans.c src/cairo-spline.c src/cairo-stroke-dash.c src/cairo-stroke-style.c src/cairo-surface-clipper.c src/cairo-surface-fallback.c src/cairo-surface-observer.c src/cairo-surface-offset.c src/cairo-surface-snapshot.c src/cairo-surface-subsurface.c src/cairo-surface-wrapper.c src/cairo-surface.c
        src/cairo-time.c src/cairo-tor-scan-converter.c src/cairo-tor22-scan-converter.c src/cairo-toy-font-face.c src/cairo-traps-compositor.c src/cairo-traps.c src/cairo-tristrip.c
        src/cairo-unicode.c src/cairo-user-font.c
        src/cairo-version.c
        src/cairo-wideint.c
        src/cairo.c src/cairo-cff-subset.c src/cairo-scaled-font-subsets.c src/cairo-truetype-subset.c src/cairo-type1-fallback.c src/cairo-type1-glyph-names.c src/cairo-type1-subset.c src/cairo-type3-glyph-surface.c src/cairo-pdf-operators.c src/cairo-pdf-shading.c src/cairo-tag-attributes.c src/cairo-tag-stack.c src/cairo-deflate-stream.c)
set(CAIRO_PNG_SRC_FILES src/cairo-png.c)
set(CAIRO_FT_SRC_FILES src/cairo-ft-font.c src/cairo-colr-glyph-render.c src/cairo-svg-glyph-render.c)
set(CAIRO_XLIB_SRC_FILES boilerplate/cairo-boilerplate-xlib.c src/cairo-xlib-display.c src/cairo-xlib-core-compositor.c src/cairo-xlib-fallback-compositor.c src/cairo-xlib-render-compositor.c src/cairo-xlib-screen.c src/cairo-xlib-source.c src/cairo-xlib-surface.c src/cairo-xlib-surface-shm.c src/cairo-xlib-visual.c src/cairo-xlib-xcb-surface.c)
set(CAIRO_XCB_SRC_FILES boilerplate/cairo-boilerplate-xcb.c src/cairo-xcb-connection.c src/cairo-xcb-connection-core.c src/cairo-xcb-connection-render.c src/cairo-xcb-connection-shm.c src/cairo-xcb-screen.c src/cairo-xcb-shm.c src/cairo-xcb-surface.c src/cairo-xcb-surface-core.c src/cairo-xcb-surface-render.c src/cairo-xcb-resources.c)
set(CAIRO_QUARTZ_SRC_FILES boilerplate/cairo-boilerplate-quartz.c src/cairo-quartz-surface.c)
set(CAIRO_QUARTZ_IMAGE_SRC_FILES src/cairo-quartz-image-surface.c)
set(CAIRO_QUARTZ_FONT_SRC_FILES src/cairo-quartz-font.c)
set(CAIRO_WIN32_SRC_FILES boilerplate/cairo-boilerplate-win32.c boilerplate/cairo-boilerplate-win32-printing.c src/win32/cairo-win32-debug.c src/win32/cairo-win32-device.c src/win32/cairo-win32-gdi-compositor.c src/win32/cairo-win32-system.c src/win32/cairo-win32-surface.c src/win32/cairo-win32-display-surface.c src/win32/cairo-win32-printing-surface.c)

set(CAIRO_WIN32_FONT_SRC_FILES src/win32/cairo-win32-font.c)
set(CAIRO_DWRITE_FONT_SRC_FILES src/win32/cairo-dwrite-font.cpp)
set(CAIRO_SCRIPT_SRC_FILES boilerplate/cairo-boilerplate-script.c src/cairo-script-surface.c)
set(CAIRO_PS_SRC_FILES boilerplate/cairo-boilerplate-ps.c src/cairo-ps-surface.c)
set(CAIRO_PDF_SRC_FILES boilerplate/cairo-boilerplate-pdf.c src/cairo-pdf-surface.c src/cairo-pdf-interchange.c)
set(CAIRO_SVG_SRC_FILES boilerplate/cairo-boilerplate-svg.c src/cairo-svg-surface.c)
set(CAIRO_XML_SRC_FILES src/cairo-xml-surface.c)
set(CAIRO_TEE_SRC_FILES src/cairo-tee-surface.c)

set(CAIRO_CONF_LIST
        CAIRO_CAN_TEST_PDF_SURFACE
        CAIRO_CAN_TEST_PS_SURFACE
        CAIRO_CAN_TEST_SVG_SURFACE
        CAIRO_CAN_TEST_TTX_FONT
        CAIRO_FEATURES_H
        CAIRO_HAS_DLSYM
        CAIRO_HAS_DWRITE_FONT
        CAIRO_HAS_FC_FONT
        CAIRO_HAS_FT_FONT
        CAIRO_HAS_GOBJECT_FUNCTIONS
        CAIRO_HAS_HIDDEN_SYMBOLS
        CAIRO_HAS_IMAGE_SURFACE
        CAIRO_HAS_INTERPRETER
        CAIRO_HAS_MIME_SURFACE
        CAIRO_HAS_OBSERVER_SURFACE
        CAIRO_HAS_PDF_SURFACE
        CAIRO_HAS_PNG_FUNCTIONS
        CAIRO_HAS_PS_SURFACE
        CAIRO_HAS_PTHREAD
        CAIRO_HAS_QUARTZ_FONT
        CAIRO_HAS_QUARTZ_IMAGE_SURFACE
        CAIRO_HAS_QUARTZ_SURFACE
        CAIRO_HAS_REAL_PTHREAD
        CAIRO_HAS_RECORDING_SURFACE
        CAIRO_HAS_SCRIPT_SURFACE
        CAIRO_HAS_SPECTRE
        CAIRO_HAS_SVG_SURFACE
        CAIRO_HAS_SYMBOL_LOOKUP
        CAIRO_HAS_TEE_SURFACE
        CAIRO_HAS_TRACE
        CAIRO_HAS_USER_FONT
        CAIRO_HAS_WIN32_FONT
        CAIRO_HAS_WIN32_SURFACE
        CAIRO_HAS_XCB_SHM_FUNCTIONS
        CAIRO_HAS_XCB_SURFACE
        CAIRO_HAS_XLIB_SURFACE
        CAIRO_HAS_XLIB_XCB_FUNCTIONS
        CAIRO_HAS_XLIB_XRENDER_SURFACE
        CAIRO_HAS_XML_SURFACE)
foreach (CAIRO_CONF ${CAIRO_CONF_LIST})
    set(${CAIRO_CONF} 0)
endforeach ()
set(CAIRO_HAS_SVG_SURFACE 1)
set(CAIRO_HAS_PDF_SURFACE 1)
set(CAIRO_HAS_PS_SURFACE 1)
set(CAIRO_HAS_PNG_FUNCTIONS 1)
set(CAIRO_HAS_IMAGE_SURFACE 1)
set(CAIRO_HAS_USER_FONT 1)

if (UNIX OR APPLE OR MINGW)
    list(APPEND CAIRO_COMPILE_DEFINITIONS "_GNU_SOURCE=1")
    list(APPEND CAIRO_COMPILE_DEFINITIONS "HAVE_UINT64_T")
    if (XGD_HAVE_STDINT_H)
        list(APPEND CAIRO_COMPILE_DEFINITIONS "HAVE_STDINT_H")
    endif ()
    if (XGD_HAVE_UNISTD_H)
        list(APPEND CAIRO_COMPILE_DEFINITIONS "HAVE_UNISTD_H")
    endif ()
    if (CMAKE_USE_PTHREADS_INIT)
        set(CAIRO_HAS_PTHREAD 1)
    endif ()
endif ()
if (0 AND APPLE)
    set(CAIRO_HAS_QUARTZ_FONT 1)
    set(CAIRO_HAS_QUARTZ_SURFACE 1)
    set(CAIRO_HAS_QUARTZ_IMAGE_SURFACE 1)
else ()
    set(CAIRO_HAS_FT_FONT 1)
endif ()
if (MSVC)
    list(APPEND CAIRO_COMPILE_DEFINITIONS "DISABLE_SOME_FLOATING_POINT")
endif ()
if (MSVC OR MINGW)
    set(CAIRO_HAS_WIN32_FONT 1)
    set(CAIRO_HAS_WIN32_SURFACE 1)
    if (NOT BUILD_SHARED_LIBS)
        set(CAIRO_WIN32_STATIC_BUILD 1)
        list(APPEND CAIRO_COMPILE_DEFINITIONS "CAIRO_WIN32_STATIC_BUILD")
    endif ()
endif ()

set(CAIRO_CONFIG_FILE ${GEN_DIR}/cairo-features.h)
set(CAIRO_CONFIG_FILE_2 ${GEN_DIR}/config.h)
set(CAIRO_CONFIG_FILE_CONTENT "#pragma once")
foreach (CAIRO_CONF ${CAIRO_CONF_LIST})
    set(CAIRO_CONFIG_FILE_CONTENT "${CAIRO_CONFIG_FILE_CONTENT}\n#define ${CAIRO_CONF} ${${CAIRO_CONF}}")
    # message(STATUS "${CAIRO_CONF}=${${CAIRO_CONF}}")
endforeach ()

if (EXISTS "${CAIRO_CONFIG_FILE}")
    file(READ "${CAIRO_CONFIG_FILE}" _CAIRO_CONFIG_FILE_CONTENT)
else ()
    set(_CAIRO_CONFIG_FILE_CONTENT "")
endif ()
if ("${_CAIRO_CONFIG_FILE_CONTENT}" STREQUAL "${CAIRO_CONFIG_FILE_CONTENT}")
else ()
    file(WRITE "${CAIRO_CONFIG_FILE}" "${CAIRO_CONFIG_FILE_CONTENT}")
endif ()

if (EXISTS "${CAIRO_CONFIG_FILE_2}")
    file(READ "${CAIRO_CONFIG_FILE_2}" _CAIRO_CONFIG_FILE_CONTENT)
else ()
    set(_CAIRO_CONFIG_FILE_CONTENT "")
endif ()
if ("${_CAIRO_CONFIG_FILE_CONTENT}" STREQUAL "${CAIRO_CONFIG_FILE_CONTENT}")
else ()
    file(WRITE "${CAIRO_CONFIG_FILE_2}" "${CAIRO_CONFIG_FILE_CONTENT}")
endif ()

set(CAIRO_HAS_SVG_SURFACE 1)
set(CAIRO_HAS_PDF_SURFACE 1)
set(CAIRO_HAS_PNG_FUNCTIONS 1)
set(CAIRO_HAS_IMAGE_SURFACE 1)
set(CAIRO_HAS_USER_FONT 1)
list(APPEND CAIRO_SRC_FILES ${CAIRO_PNG_SRC_FILES} ${CAIRO_SVG_SRC_FILES} ${CAIRO_PDF_SRC_FILES})
if (WIN32)
    list(APPEND CAIRO_SRC_FILES ${CAIRO_WIN32_SRC_FILES} ${CAIRO_WIN32_FONT_SRC_FILES})
endif ()
if (0 AND APPLE)
    list(APPEND CAIRO_SRC_FILES ${CAIRO_QUARTZ_SRC_FILES} ${CAIRO_QUARTZ_IMAGE_SRC_FILES} ${CAIRO_QUARTZ_FONT_SRC_FILES})
else ()
    list(APPEND CAIRO_SRC_FILES ${CAIRO_FT_SRC_FILES})
endif ()

set(CAIRO_SRC_FILE_LIST "")
foreach (CAIRO_SRC_FILE ${CAIRO_SRC_FILES})
    list(APPEND CAIRO_SRC_FILE_LIST ${ROOT_DIR}/${CAIRO_SRC_FILE})
endforeach ()
xgd_add_library(
        cairo
        SRC_FILES ${CAIRO_SRC_FILE_LIST}
        INCLUDE_DIRS ${ROOT_DIR}/src ${ROOT_DIR}/boilerplate ${GEN_DIR}
)
xgd_generate_export_header(cairo "cairo" ".h")
target_compile_definitions(cairo PRIVATE ${CAIRO_COMPILE_DEFINITIONS})
if (APPLE)
    target_link_libraries(cairo PRIVATE "-framework CoreFoundation -framework ApplicationServices")
elseif (WIN32)
    target_link_libraries(cairo PRIVATE msimg32 gdi32)
endif ()
xgd_link_threads(cairo)
xgd_link_libraries(cairo PRIVATE png freetype pixman zlib)
xgd_disable_warnings(cairo)