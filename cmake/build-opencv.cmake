# reference: opencv/cmake/OpenCVCompilerOptimizations.cmake
macro(__ocv_add_dispatched_file filename target_src_var src_directory dst_directory precomp_hpp optimizations_var)
    set(__codestr "
#include \"${src_directory}/${precomp_hpp}\"
#include \"${src_directory}/${filename}.simd.hpp\"")

    set(__declarations_str "#define CV_CPU_SIMD_FILENAME \"${src_directory}/${filename}.simd.hpp\"")
    set(__dispatch_modes "BASELINE")

    set(__optimizations "${${optimizations_var}}")
    #        set(__optimizations "")
    foreach (OPT ${__optimizations})
        string(TOLOWER "${OPT}" OPT_LOWER)
        set(__file "${OCV_GENERATED_SRC_DIR}/${OCV_COMPONENT}/${dst_directory}${filename}.${OPT_LOWER}.cpp")
        if (EXISTS "${__file}")
            file(READ "${__file}" __content)
        else ()
            set(__content "")
        endif ()
        if (__content STREQUAL __codestr)
            # message(STATUS "${__file} contains up-to-date content")
        else ()
            file(WRITE "${__file}" "${__codestr}")
        endif ()

        if (";${CPU_DISPATCH_FINAL};" MATCHES "${OPT}" OR __CPU_DISPATCH_INCLUDE_ALL)
            if (EXISTS "${src_directory}/${filename}.${OPT_LOWER}.cpp")
                message(STATUS "Using overridden ${OPT} source: ${src_directory}/${filename}.${OPT_LOWER}.cpp")
            else ()
                list(APPEND ${target_src_var} "${__file}")
                xgd_mark_generated("${__file}")
            endif ()
            set(__declarations_str "${__declarations_str}
#define CV_CPU_DISPATCH_MODE ${OPT}
#include \"opencv2/core/private/cv_cpu_include_simd_declarations.hpp\"")
            set(__dispatch_modes "${OPT}, ${__dispatch_modes}")
        endif ()
    endforeach ()

    set(__declarations_str "${__declarations_str}
            #define CV_CPU_DISPATCH_MODES_ALL ${__dispatch_modes}
            #undef CV_CPU_SIMD_FILENAME")

    set(__file "${OCV_GENERATED_SRC_DIR}/${OCV_COMPONENT}/${dst_directory}${filename}.simd_declarations.hpp")
    if (EXISTS "${__file}")
        file(READ "${__file}" __content)
    endif ()
    if (__content STREQUAL __declarations_str)
        # message(STATUS "${__file} contains up-to-date content")
    else ()
        file(WRITE "${__file}" "${__declarations_str}")
    endif ()
endmacro()

macro(ocv_add_dispatched_file filename)
    set(__optimizations "${ARGN}")
    __ocv_add_dispatched_file(
            "${filename}" "OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED"
            "${OCV_MODULE_DIR}/${OCV_COMPONENT}/src" "" "precomp.hpp" __optimizations
    )
endmacro()

macro(ocv_add_dispatched_file_force_all)
    set(__CPU_DISPATCH_INCLUDE_ALL 1)
    ocv_add_dispatched_file(${ARGN})
    unset(__CPU_DISPATCH_INCLUDE_ALL)
endmacro()

# reference: opencv/cmake/OpenCVUtils.cmake
function(ocv_update_file filepath content)
    if (EXISTS "${filepath}")
        file(READ "${filepath}" actual_content)
    else ()
        set(actual_content "")
    endif ()
    if ("${actual_content}" STREQUAL "${content}")
        if (";${ARGN};" MATCHES ";VERBOSE;")
            message(STATUS "${filepath} contains the same content")
        endif ()
    else ()
        file(WRITE "${filepath}" "${content}")
    endif ()
endfunction()

macro(ocv_compiler_optimization_process_sources)
    # set(__result "")
    foreach (OPT ${CPU_DISPATCH_FINAL})
        set(__result_${OPT} "")
    endforeach ()
    foreach (fname ${ARGV})
        string(TOLOWER "${fname}" fname_LOWER)
        get_filename_component(fname_LOWER "${fname_LOWER}" NAME)
        if (fname_LOWER MATCHES ".+\\.([^\\.]*)\\.cpp$")
            string(TOUPPER "${CMAKE_MATCH_1}" OPT_)
            if (OPT_ MATCHES "(CUDA.*|DISPATCH.*|OCL)") # don't touch files like filename.cuda.cpp
                # list(APPEND __result "${fname}")
                #continue()
            elseif (CV_DISABLE_OPTIMIZATION OR NOT CV_ENABLE_INTRINSICS)
                # ocv_get_smart_file_name(fname_ "${fname}")
                # message(STATUS "Excluding from source files list (optimization is disabled): ${fname_}")
                #continue()
            else ()
                get_source_file_property(__definitions "${fname}" COMPILE_DEFINITIONS)
                if (__definitions)
                    list(APPEND __definitions "CV_CPU_DISPATCH_MODE=${OPT_}")
                else ()
                    set(__definitions "CV_CPU_DISPATCH_MODE=${OPT_}")
                endif ()
                set_source_files_properties("${fname}" PROPERTIES COMPILE_DEFINITIONS "${__definitions}")

                set(__opt_found 0)
                foreach (OPT ${CPU_BASELINE_FINAL})
                    string(TOLOWER "${OPT}" OPT_LOWER)
                    if (fname_LOWER MATCHES "\\.${OPT_LOWER}\\.cpp$")
                        #message("${fname} BASELINE-${OPT}")
                        set(__opt_found 1)
                        # list(APPEND __result "${fname}")
                        break()
                    endif ()
                endforeach ()
                foreach (OPT ${CPU_DISPATCH_FINAL})
                    set(OPT2 ${OPT})
                    # foreach (OPT2 ${CPU_DISPATCH_${OPT}_FORCED})
                    string(TOLOWER "${OPT2}" OPT2_LOWER)
                    if (fname_LOWER MATCHES "\\.${OPT2_LOWER}\\.cpp$")
                        list(APPEND __result_${OPT} "${fname}")
                        # math(EXPR CPU_${OPT}_USAGE_COUNT "${CPU_${OPT}_USAGE_COUNT}+1")
                        # set(CPU_${OPT}_USAGE_COUNT "${CPU_${OPT}_USAGE_COUNT}" CACHE INTERNAL "" FORCE)
                        #message("(${CPU_${OPT}_USAGE_COUNT})${fname} ${OPT}")
                        #message("    ${CPU_DISPATCH_${OPT}_INCLUDED}")
                        #message("    ${CPU_DISPATCH_DEFINITIONS_${OPT}}")
                        #message("    ${CPU_DISPATCH_FLAGS_${OPT}}")
                        set(__opt_found 1)
                        # break()
                    endif ()
                    # endforeach ()
                    if (__opt_found)
                        set(__opt_found 1)
                        break()
                    endif ()
                endforeach ()
                if (NOT __opt_found)
                    # ocv_get_smart_file_name(fname_ "${fname}")
                    # message(STATUS "Excluding from source files list: ${fname_}")
                endif ()
            endif ()
        else ()
            # list(APPEND __result "${fname}")
        endif ()
    endforeach ()

    foreach (OPT ${CPU_DISPATCH_FINAL})
        if (__result_${OPT})
            foreach (fname ${__result_${OPT}})
                get_source_file_property(__definitions "${fname}" COMPILE_DEFINITIONS)
                if (__definitions)
                    list(APPEND __definitions "${CPU_DISPATCH_DEFINITIONS_${OPT}}")
                else ()
                    set(__definitions "${CPU_DISPATCH_DEFINITIONS_${OPT}}")
                endif ()
                get_target_property(TARGET_COMPILE_DEFINITIONS opencv_${OCV_COMPONENT} COMPILE_DEFINITIONS)
                list(APPEND __definitions ${TARGET_COMPILE_DEFINITIONS})
                # message(STATUS "TARGET_COMPILE_DEFINITIONS=${TARGET_COMPILE_DEFINITIONS}")
                set_source_files_properties("${fname}" PROPERTIES COMPILE_DEFINITIONS "${__definitions}")

                get_source_file_property(__definitions "${fname}" COMPILE_DEFINITIONS)
                # message(STATUS "def for ${fname} is ${__definitions}")
                # set_source_files_properties("${fname}" PROPERTIES COMPILE_FLAGS "${CPU_DISPATCH_FLAGS_${OPT}}")
                # message(STATUS "define ${__definitions} for ${fname}")
            endforeach ()
            # list(APPEND __result ${__result_${OPT}})
        endif ()
    endforeach ()
    # set(${SOURCES_VAR_NAME} "${__result}")
endmacro()

function(xgd_link_opencv_internal TARGET)
    cmake_parse_arguments(param "" "" "PRIVATE;PUBLIC" ${ARGN})
    foreach (COMPONENT ${param_PRIVATE})
        add_dependencies(${TARGET} opencv_${COMPONENT})
        target_link_libraries(${TARGET} PRIVATE opencv_${COMPONENT})
    endforeach ()
    foreach (COMPONENT ${param_PUBLIC})
        add_dependencies(${TARGET} opencv_${COMPONENT})
        target_link_libraries(${TARGET} PUBLIC opencv_${COMPONENT})
    endforeach ()
endfunction()

# opencv
set(SUPPORTED_MODULES
        opencv_calib3d
        opencv_dnn
        opencv_features2d
        opencv_flann
        opencv_gapi
        opencv_imgcodecs
        opencv_imgproc
        opencv_ml
        opencv_objdetect
        opencv_photo
        opencv_stitching
        opencv_video)
set(CV_ENABLE_INTRINSICS ON)
set(CPU_BASELINE_FINAL "")
set(CPU_DISPATCH_FINAL "")
if (XGD_FLAG_SSE_ALL)
    list(APPEND CPU_BASELINE_FINAL SSE SSE2)
    if (NOT EMSCRIPTEN)
        list(APPEND CPU_BASELINE_FINAL SSE3 SSSE3 SSE4_1 SSE4_2)
    endif ()
endif ()
if (XGD_FLAG_AVX)
    list(APPEND CPU_BASELINE_FINAL AVX)
endif ()
if (XGD_FLAG_AVX2)
    list(APPEND CPU_BASELINE_FINAL AVX2)
endif ()
if (XGD_FLAG_NEON)
    list(APPEND CPU_BASELINE_FINAL NEON)
endif ()
if (EMSCRIPTEN)
    set(CPU_BASELINE_FINAL "")
endif ()
set(CPU_DISPATCH_FINAL ${CPU_BASELINE_FINAL})


set(OCV_ROOT ${XGD_THIRD_PARTY_DIR}/opencv-src/opencv)
set(OCV_MODULE_DIR ${OCV_ROOT}/modules)
set(OCV_GENERATED_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/opencv/include)
set(OCV_GENERATED_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/opencv/src)

set(OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE "")
set(OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE "")

foreach (OPT ${CPU_BASELINE_FINAL})
    set(OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE "${OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE}
#define CV_CPU_COMPILE_${OPT} 1
#define CV_CPU_BASELINE_COMPILE_${OPT} 1
")
endforeach ()


set(OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE "${OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE}
#define CV_CPU_BASELINE_FEATURES 0 \\")
foreach (OPT ${CPU_BASELINE_FINAL})
    if (NOT DEFINED CPU_${OPT}_FEATURE_ALIAS OR NOT "x${CPU_${OPT}_FEATURE_ALIAS}" STREQUAL "x")
        set(OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE "${OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE}
    , CV_CPU_${OPT} \\")
    endif ()
endforeach ()
set(OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE "${OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE}\n")

set(__dispatch_modes "")
foreach (OPT ${CPU_DISPATCH_FINAL})
    list(APPEND __dispatch_modes ${CPU_DISPATCH_${OPT}_FORCE} ${OPT})
endforeach ()
list(REMOVE_DUPLICATES __dispatch_modes)
foreach (OPT ${__dispatch_modes})
    set(OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE "${OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE}
#define CV_CPU_DISPATCH_COMPILE_${OPT} 1")
endforeach ()

set(OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE "${OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE}
\n\n#define CV_CPU_DISPATCH_FEATURES 0 \\")
foreach (OPT ${__dispatch_modes})
    if (NOT DEFINED CPU_${OPT}_FEATURE_ALIAS OR NOT "x${CPU_${OPT}_FEATURE_ALIAS}" STREQUAL "x")
        set(OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE "${OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE}
    , CV_CPU_${OPT} \\")
    endif ()
endforeach ()
set(OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE "${OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE}\n")

configure_file(${OCV_ROOT}/cmake/templates/cv_cpu_config.h.in
        ${OCV_GENERATED_INC_DIR}/cv_cpu_config.h)

configure_file(${OCV_ROOT}/cmake/templates/custom_hal.hpp.in
        ${OCV_GENERATED_INC_DIR}/custom_hal.hpp @ONLY)
set(OPENCV_DATA_CONFIG_FILE "${OCV_GENERATED_SRC_DIR}/opencv_data_config.hpp")
set(OPENCV_DATA_CONFIG_STR "#pragma once
#define OPENCV_BUILD_DIR \"./\"
#define OPENCV_DATA_BUILD_DIR_SEARCH_PATHS \"./\"
")

if (EXISTS "${OPENCV_DATA_CONFIG_FILE}")
    file(READ "${OPENCV_DATA_CONFIG_FILE}" __content)
endif ()
if (NOT OPENCV_DATA_CONFIG_STR STREQUAL "${__content}")
    file(WRITE "${OPENCV_DATA_CONFIG_FILE}" "${OPENCV_DATA_CONFIG_STR}")
endif ()

function(xgd_internal_build_opencv OCV_COMPONENT)
    set(OCV_COMPONENT_DIR ${OCV_MODULE_DIR}/${OCV_COMPONENT})
    if (NOT EXISTS ${OCV_COMPONENT_DIR})
        message(FATAL_ERROR "${OCV_COMPONENT_DIR} not exist for ${OCV_COMPONENT}")
    endif ()
    cmake_parse_arguments(param "" "" "SRC_FILES;SRC_DIRS;PRIVATE_INCLUDE_DIRS;EXCLUDE_SRC_FILES" ${ARGN})
    set(OCV_COMPONENT_INC_DIR ${OCV_COMPONENT_DIR}/include)
    set(OCV_COMPONENT_SRC_DIR ${OCV_COMPONENT_DIR}/src)
    set(OCV_COMPONENT_GEN_DIR ${OCV_GENERATED_SRC_DIR}/${OCV_COMPONENT})

    file(GLOB cl_kernels ${OCV_COMPONENT_SRC_DIR}/opencl/*.cl)
    set(OUTPUT_SRC "")
    if (cl_kernels)
        set(OCL_NAME opencl_kernels_${OCV_COMPONENT})
        set(OUTPUT_SRC ${OCV_COMPONENT_GEN_DIR}/${OCL_NAME}.cpp)
        add_custom_command(
                OUTPUT ${OUTPUT_SRC}
                COMMAND
                ${CMAKE_COMMAND}
                "-DMODULE_NAME=${OCV_COMPONENT}"
                "-DCL_DIR=${OCV_COMPONENT_SRC_DIR}/opencl"
                "-DOUTPUT=${OUTPUT_SRC}"
                -P
                "${OCV_ROOT}/cmake/cl2cpp.cmake"
                DEPENDS ${cl_kernels} "${OCV_ROOT}/cmake/cl2cpp.cmake"
                COMMENT "Processing OpenCL kernels (${OCV_COMPONENT})"
        )
        xgd_mark_generated(${OUTPUT_SRC} ${OCV_GENERATED_SRC_DIR}/${OCV_COMPONENT}/${OCL_NAME}.hpp)
    endif ()

    set(IGNORE_FILE_REGEXES "^(.*)\\.lasx(.*)\\.cpp$" "^(.*)\\.mm$")
    if (EMSCRIPTEN OR NOT XGD_FLAG_SSE_ALL)
        list(APPEND IGNORE_FILE_REGEXES "^(.*)\\.sse4_1(.*)\\.cpp$")
    endif ()
    if (NOT XGD_FLAG_AVX)
        list(APPEND IGNORE_FILE_REGEXES "^(.*)\\.avx(.*)\\.cpp$")
    endif ()
    if (NOT XGD_FLAG_AVX2)
        list(APPEND IGNORE_FILE_REGEXES "^(.*)\\.avx2(.*)\\.cpp$")
    endif ()
    xgd_add_library(
            opencv_${OCV_COMPONENT}
            SRC_DIRS
            ${param_SRC_DIRS}
            ${OCV_COMPONENT_SRC_DIR}

            SRC_FILES
            ${OUTPUT_SRC}
            ${param_SRC_FILES}

            INCLUDE_DIRS
            ${OCV_COMPONENT_INC_DIR}
            ${OCV_GENERATED_INC_DIR}
            ${OCV_ROOT}/include

            PRIVATE_INCLUDE_DIRS
            ${param_PRIVATE_INCLUDE_DIRS}
            ${OCV_GENERATED_SRC_DIR}
            ${OCV_COMPONENT_GEN_DIR}
            ${OCV_COMPONENT_SRC_DIR} # for gapi

            EXCLUDE_SRC_FILES
            ${param_EXCLUDE_SRC_FILES}

            EXCLUDE_REGEXES
            ${IGNORE_FILE_REGEXES}
    )
    set(CXX20_NO_WARNING_FLAGS -Wno-deprecated-enum-enum-conversion)
    list(APPEND CXX20_NO_WARNING_FLAGS -Wno-deprecated-enum-float-conversion)
    set(CXX20_NO_WARNING_FLAGS_CLANG -Wno-deprecated-anon-enum-enum-conversion)
    target_compile_options(
            opencv_${OCV_COMPONENT} PUBLIC
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:${CXX20_NO_WARNING_FLAGS}>
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${CXX20_NO_WARNING_FLAGS_CLANG}>
    )
    if (MSVC)
        target_compile_options(opencv_${OCV_COMPONENT} PUBLIC /wd5055)
    endif ()
    xgd_link_libraries(opencv_${OCV_COMPONENT} PRIVATE eigen)
    xgd_link_omp(opencv_${OCV_COMPONENT})
    if (EMSCRIPTEN)
        target_compile_definitions(opencv_${OCV_COMPONENT} PRIVATE CV_FORCE_SIMD128_CPP)
    endif ()
    target_compile_definitions(opencv_${OCV_COMPONENT} PRIVATE __OPENCV_BUILD CVAPI_EXPORTS)
    ocv_compiler_optimization_process_sources(${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED})

    if (NOT TARGET opencv_all)
        add_library(opencv_all INTERFACE)
    endif ()
    xgd_link_libraries(opencv_all INTERFACE opencv_${OCV_COMPONENT})
endfunction()

function(xgd_build_opencv_core)
    set(OCV_COMPONENT core)
    set(the_module opencv_${OCV_COMPONENT})
    ocv_add_dispatched_file(mathfuncs_core SSE2 AVX AVX2)
    ocv_add_dispatched_file(stat SSE4_2 AVX2)
    ocv_add_dispatched_file(arithm SSE2 SSE4_1 AVX2 VSX3)
    ocv_add_dispatched_file(convert SSE2 AVX2 VSX3)
    ocv_add_dispatched_file(convert_scale SSE2 AVX2)
    ocv_add_dispatched_file(count_non_zero SSE2 AVX2)
    ocv_add_dispatched_file(has_non_zero SSE2 AVX2)
    ocv_add_dispatched_file(matmul SSE2 SSE4_1 AVX2 AVX512_SKX NEON_DOTPROD)
    ocv_add_dispatched_file(mean SSE2 AVX2)
    ocv_add_dispatched_file(merge SSE2 AVX2)
    ocv_add_dispatched_file(split SSE2 AVX2)
    ocv_add_dispatched_file(sum SSE2 AVX2)

    set(OPENCV_BUILD_INFO_STR "\"Build from ${PROJECT_NAME}\"")
    set(OPENCV_BUILD_INFO_FILE "${OCV_GENERATED_SRC_DIR}/version_string.inc")
    if (EXISTS "${OPENCV_BUILD_INFO_FILE}")
        file(READ "${OPENCV_BUILD_INFO_FILE}" __content)
    else ()
        set(__content "")
    endif ()
    if ("${__content}" STREQUAL "${OPENCV_BUILD_INFO_STR}")
        # message(STATUS "${OPENCV_BUILD_INFO_FILE} contains the same content")
    else ()
        file(WRITE "${OPENCV_BUILD_INFO_FILE}" "${OPENCV_BUILD_INFO_STR}")
    endif ()

    xgd_internal_build_opencv(
            core
            SRC_DIRS
            ${OCV_MODULE_DIR}/core/src/utils
            ${OCV_MODULE_DIR}/core/src/parallel

            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
    if (ANDROID)
        target_compile_definitions(opencv_core PRIVATE "HAVE_CPUFEATURES=1")
        add_dependencies(opencv_core cpufeatures_ndk_compat)
        target_link_libraries(opencv_core PRIVATE cpufeatures_ndk_compat)
    endif ()
endfunction()
xgd_build_opencv_core()

function(xgd_build_opencv_imgproc)
    set(OCV_COMPONENT imgproc)
    set(the_module opencv_${OCV_COMPONENT})
    ocv_add_dispatched_file(accum SSE4_1 AVX AVX2)
    ocv_add_dispatched_file(bilateral_filter SSE2 AVX2)
    ocv_add_dispatched_file(box_filter SSE2 SSE4_1 AVX2)
    ocv_add_dispatched_file(filter SSE2 SSE4_1 AVX2)
    ocv_add_dispatched_file(color_hsv SSE2 SSE4_1 AVX2)
    ocv_add_dispatched_file(color_rgb SSE2 SSE4_1 AVX2)
    ocv_add_dispatched_file(color_yuv SSE2 SSE4_1 AVX2)
    ocv_add_dispatched_file(median_blur SSE2 SSE4_1 AVX2)
    ocv_add_dispatched_file(morph SSE2 SSE4_1 AVX2)
    ocv_add_dispatched_file(smooth SSE2 SSE4_1 AVX2)
    ocv_add_dispatched_file(sumpixels SSE2 AVX2 AVX512_SKX)
    xgd_internal_build_opencv(
            imgproc
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_imgproc()

function(xgd_build_opencv_imgcodecs)
    set(OCV_COMPONENT imgcodecs)
    set(the_module opencv_${OCV_COMPONENT})
    set(_OCV_COMPONENT_DIR ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src)
    xgd_internal_build_opencv(
            imgcodecs
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
    # set(EXTRA_SRC_FILES)
    # set(EXTRA_LIBS)
    # if (APPLE OR APPLE_FRAMEWORK)
    #     list(APPEND EXTRA_SRC_FILES ${_OCV_COMPONENT_DIR}/apple_conversions.h)
    #     list(APPEND EXTRA_SRC_FILES ${_OCV_COMPONENT_DIR}/apple_conversions.mm)
    # endif ()
    # if (IOS)
    #     list(APPEND EXTRA_SRC_FILES ${_OCV_COMPONENT_DIR}/ios_conversions.mm)
    #     list(APPEND EXTRA_LIBS "-framework UIKit")
    # endif ()
    # if (APPLE AND (NOT IOS))
    #     list(APPEND EXTRA_SRC_FILES ${_OCV_COMPONENT_DIR}/macosx_conversions.mm)
    #     list(APPEND EXTRA_LIBS "-framework AppKit")
    # endif ()
    # if (APPLE_FRAMEWORK)
    #     list(APPEND EXTRA_LIBS "-framework Accelerate" "-framework CoreGraphics" "-framework QuartzCore")
    # endif ()
    # if (EXTRA_SRC_FILES)
    #     target_sources(opencv_imgcodecs PRIVATE ${EXTRA_SRC_FILES})
    # endif ()
    # if (EXTRA_LIBS)
    #     target_link_libraries(opencv_imgcodecs PRIVATE ${EXTRA_LIBS})
    # endif ()
endfunction()
xgd_build_opencv_imgcodecs()

function(xgd_build_opencv_calib3d)
    set(OCV_COMPONENT calib3d)
    set(the_module opencv_${OCV_COMPONENT})
    ocv_add_dispatched_file(undistort SSE2 AVX2)
    xgd_internal_build_opencv(
            calib3d
            SRC_DIRS
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/usac
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_calib3d()

function(xgd_build_opencv_features2d)
    set(OCV_COMPONENT features2d)
    set(the_module opencv_${OCV_COMPONENT})
    ocv_add_dispatched_file(sift SSE4_1 AVX2 AVX512_SKX)
    xgd_internal_build_opencv(
            features2d
            SRC_DIRS
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/kaze
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_features2d()

function(xgd_build_opencv_flann)
    set(OCV_COMPONENT flann)
    set(the_module opencv_${OCV_COMPONENT})
    xgd_internal_build_opencv(
            flann
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_flann()


function(xgd_build_opencv_gapi)
    set(OCV_COMPONENT gapi)
    set(the_module opencv_${OCV_COMPONENT})
    ocv_add_dispatched_file(backends/fluid/gfluidimgproc_func SSE4_1 AVX2)
    ocv_add_dispatched_file(backends/fluid/gfluidcore_func SSE4_1 AVX2)
    xgd_internal_build_opencv(
            gapi
            SRC_DIRS
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/api
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/common
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/cpu
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/cpu
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/fluid
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/ie
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/oak
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/onnx
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/plaidml
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/python
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/render
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/backends/streaming

            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/compiler
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/compiler/passes
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/executor
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/streaming/gstreamer
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/streaming/onevpl
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/streaming/onevpl/asseletators
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/streaming/onevpl/asseletators/surface
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/streaming/onevpl/asseletators/utils
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/streaming/onevpl/demux
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/engine
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/engine/decode
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/engine/precode
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/engine/transcode
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/utils
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
    xgd_link_libraries(opencv_gapi PRIVATE ade)
    if (WIN32)
        target_link_libraries(opencv_gapi PRIVATE wsock32 ws2_32)
    endif ()
    add_dependencies(opencv_gapi freetype)
    target_link_libraries(opencv_gapi PRIVATE freetype)
    target_compile_definitions(opencv_gapi PRIVATE HAVE_FREETYPE)
endfunction()
xgd_build_opencv_gapi()

function(xgd_build_opencv_dnn)
    set(OCV_COMPONENT dnn)
    set(the_module opencv_${OCV_COMPONENT})
    ocv_add_dispatched_file_force_all(layers/layers_common AVX AVX2 AVX512_SKX RVV LASX)
    ocv_add_dispatched_file_force_all(int8layers/layers_common AVX2 AVX512_SKX LASX)
    ocv_add_dispatched_file_force_all(layers/cpu_kernels/conv_block AVX AVX2)
    ocv_add_dispatched_file_force_all(layers/cpu_kernels/conv_depthwise AVX AVX2 RVV LASX)
    ocv_add_dispatched_file_force_all(layers/cpu_kernels/conv_winograd_f63 AVX AVX2)
    xgd_internal_build_opencv(
            dnn
            SRC_DIRS
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/misc/caffe
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/misc/onnx
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/misc/tensorflow
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/caffe
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/darknet
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/int8layers
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/layers
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/layers/cpu_kernels
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/onnx
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/tensorflow
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/tflite
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/torch
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
            PRIVATE_INCLUDE_DIRS
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/misc/caffe
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/misc/onnx
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/misc/tensorflow
    )
    target_compile_definitions(opencv_dnn PRIVATE "HAVE_PROTOBUF=1")
    xgd_link_libraries(opencv_dnn PRIVATE protobuf)
endfunction()
xgd_build_opencv_dnn()

function(xgd_build_opencv_ml)
    set(OCV_COMPONENT ml)
    set(the_module opencv_${OCV_COMPONENT})
    xgd_internal_build_opencv(
            ml
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_ml()

function(xgd_build_opencv_objdetect)
    set(OCV_COMPONENT objdetect)
    set(the_module opencv_${OCV_COMPONENT})
    xgd_internal_build_opencv(
            objdetect
            SRC_DIRS
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/aruco
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/aruco/apriltag
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/barcode_decoder
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/barcode_decoder/common
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/barcode_detector
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_objdetect()

function(xgd_build_opencv_photo)
    set(OCV_COMPONENT photo)
    set(the_module opencv_${OCV_COMPONENT})
    xgd_internal_build_opencv(
            photo
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_photo()

function(xgd_build_opencv_stitching)
    set(OCV_COMPONENT stitching)
    set(the_module opencv_${OCV_COMPONENT})
    xgd_internal_build_opencv(
            stitching
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_stitching()

function(xgd_build_opencv_video)
    set(OCV_COMPONENT video)
    set(the_module opencv_${OCV_COMPONENT})
    xgd_internal_build_opencv(
            video
            SRC_DIRS
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/tracking
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/tracking/detail
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
    )
endfunction()
xgd_build_opencv_video()

function(xgd_build_opencv_highgui)
    set(OCV_COMPONENT highgui)
    set(the_module opencv_${OCV_COMPONENT})
    xgd_internal_build_opencv(
            highgui
            SRC_FILES
            ${OPENCV_MODULE_${the_module}_SOURCES_DISPATCHED}
            EXCLUDE_SRC_FILES
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/window_winrt.cpp
            ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/window_winrt_bridge.cpp
    )
    set(RESOURCES_SRC "")
    qt_add_resources(RESOURCES_SRC ${OCV_MODULE_DIR}/${OCV_COMPONENT}/src/window_QT.qrc)
    target_sources(opencv_highgui PRIVATE ${RESOURCES_SRC})
    target_compile_definitions(opencv_highgui PRIVATE HAVE_QT)
    xgd_link_qt(opencv_highgui PRIVATE Core Widgets Test)

    set(OPENCV_HIGHGUI_BUILTIN_BACKEND "QT")
    set(CONFIG_STR "// Auto-generated file
#define OPENCV_HIGHGUI_BUILTIN_BACKEND_STR \"${OPENCV_HIGHGUI_BUILTIN_BACKEND}\"
")
    if (OPENCV_HIGHGUI_BUILTIN_BACKEND STREQUAL "NONE")
        set(CONFIG_STR "${CONFIG_STR}
#define OPENCV_HIGHGUI_WITHOUT_BUILTIN_BACKEND 1
")
    endif ()
    ocv_update_file("${OCV_GENERATED_SRC_DIR}/highgui/opencv_highgui_config.hpp" "${CONFIG_STR}")
endfunction()

if (XGD_USE_QT)
    message(STATUS "opencv: enable highgui Qt")
    set(HAVE_QT ON)
    set(HAVE_QT6 ON)
    list(APPEND SUPPORTED_MODULES opencv_highgui)
    xgd_build_opencv_highgui()
    xgd_link_opencv_internal(opencv_highgui PUBLIC imgproc imgcodecs) # optional videoio
endif ()

xgd_link_opencv_internal(opencv_calib3d PUBLIC imgproc features2d flann) # optional highgui on debug
xgd_link_opencv_internal(opencv_features2d PUBLIC imgproc flann) # optional highgui on debug
xgd_link_opencv_internal(opencv_flann PUBLIC core)
xgd_link_opencv_internal(opencv_gapi PUBLIC imgproc video calib3d)
xgd_link_opencv_internal(opencv_imgcodecs PUBLIC imgproc)
xgd_link_opencv_internal(opencv_imgproc PUBLIC core)
xgd_link_opencv_internal(opencv_ml PUBLIC core)
xgd_link_opencv_internal(opencv_objdetect PUBLIC core imgproc calib3d dnn)
xgd_link_opencv_internal(opencv_photo PUBLIC imgproc)
xgd_link_opencv_internal(opencv_stitching PUBLIC imgproc features2d calib3d flann)
xgd_link_opencv_internal(opencv_video PUBLIC imgproc calib3d dnn)

xgd_link_opencv_internal(opencv_dnn PUBLIC core imgproc)
# xgd_link_opencv_internal(opencv_videoio PUBLIC imgproc imgcodecs)

set(OPENCV_MODULE_DEFINITIONS_CONFIGMAKE "")
foreach (m ${SUPPORTED_MODULES})
    string(TOUPPER "${m}" m)
    set(OPENCV_MODULE_DEFINITIONS_CONFIGMAKE "${OPENCV_MODULE_DEFINITIONS_CONFIGMAKE}#define HAVE_${m}\n")
endforeach ()
set(OPENCV_MODULE_DEFINITIONS_CONFIGMAKE "${OPENCV_MODULE_DEFINITIONS_CONFIGMAKE}\n")
configure_file(${OCV_ROOT}/cmake/templates/opencv_modules.hpp.in
        ${OCV_GENERATED_INC_DIR}/opencv2/opencv_modules.hpp)

set(HAVE_PNG ON)
set(HAVE_EIGEN ON)
configure_file(${OCV_ROOT}/cmake/templates/cvconfig.h.in
        ${OCV_GENERATED_INC_DIR}/cvconfig.h)
configure_file(${OCV_ROOT}/cmake/templates/cvconfig.h.in
        ${OCV_GENERATED_INC_DIR}/opencv2/cvconfig.h)

xgd_link_libraries(opencv_core PRIVATE zlib)
xgd_link_libraries(opencv_imgcodecs PRIVATE zlib png)

if (ANDROID)
    target_link_libraries(opencv_core PRIVATE log)
    target_link_libraries(opencv_objdetect PRIVATE log)
    # target_link_libraries(opencv_videoio PRIVATE log)
endif ()