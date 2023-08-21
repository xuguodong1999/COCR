if(XGD_OPT_ARCH_ARM AND (APPLE OR IOS))
    message(STATUS "skip building cpufeatures")
    return()
endif()

# cpufeatures
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/cpufeatures-src/cpufeatures)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)
set(NDK_SRC_DIR ${ROOT_DIR}/ndk_compat)

xgd_add_library(
        cpufeatures_utils
        STATIC
        SRC_FILES
        ${SRC_DIR}/filesystem.c
        ${SRC_DIR}/stack_line_reader.c
        ${SRC_DIR}/string_view.c
        INCLUDE_DIRS
        ${INC_DIR}
        ${INC_DIR}/internal
)
target_compile_definitions(cpufeatures_utils PUBLIC STACK_LINE_READER_BUFFER_SIZE=1024)

if (UNIX AND NOT XGD_OPT_ARCH_X86 AND NOT EMSCRIPTEN)
    xgd_add_library(
            cpufeatures_unix_based_hardware_detection
            STATIC
            SRC_FILES
            ${SRC_DIR}/hwcaps.c
            INCLUDE_DIRS
            ${INC_DIR}
            ${INC_DIR}/internal
    )
    #        if (XGD_HAVE_DLFCN_H)
    #            target_compile_definitions(cpufeatures_unix_based_hardware_detection PRIVATE HAVE_DLFCN_H)
    #        endif ()
    if (XGD_HAVE_STRONG_GETAUXVAL)
        target_compile_definitions(cpufeatures_unix_based_hardware_detection PRIVATE HAVE_STRONG_GETAUXVAL)
    endif ()
    add_dependencies(cpufeatures_unix_based_hardware_detection cpufeatures_utils)
    target_link_libraries(cpufeatures_unix_based_hardware_detection PRIVATE cpufeatures_utils)
endif ()

xgd_add_library(
        cpufeatures
        STATIC
        SRC_DIRS
        ${SRC_DIR}
        INCLUDE_DIRS
        ${INC_DIR}
        ${INC_DIR}/internal
        EXCLUDE_SRC_FILES
        ${SRC_DIR}/filesystem.c
        ${SRC_DIR}/stack_line_reader.c
        ${SRC_DIR}/string_view.c
        ${SRC_DIR}/hwcaps.c
)
add_dependencies(cpufeatures cpufeatures_utils)
target_link_libraries(cpufeatures PRIVATE cpufeatures_utils)

if (UNIX AND NOT XGD_OPT_ARCH_X86 AND NOT EMSCRIPTEN)
    add_dependencies(cpufeatures cpufeatures_unix_based_hardware_detection)
    target_link_libraries(cpufeatures PRIVATE cpufeatures_unix_based_hardware_detection)
endif ()
if (APPLE AND XGD_OPT_ARCH_X86)
    target_compile_definitions(cpufeatures PRIVATE HAVE_SYSCTLBYNAME)
endif ()
add_library(CpuFeature::cpu_features ALIAS cpufeatures)

if (NOT EMSCRIPTEN)
    xgd_add_executable(
            list_cpu_features
            SRC_FILES
            ${SRC_DIR}/utils/list_cpu_features.c
    )
    add_dependencies(list_cpu_features cpufeatures)
    target_link_libraries(list_cpu_features PRIVATE cpufeatures)
    add_executable(CpuFeature::list_cpu_features ALIAS list_cpu_features)
endif ()

if (ANDROID)
    xgd_add_library(
            cpufeatures_ndk_compat
            STATIC
            SRC_DIRS
            ${NDK_SRC_DIR}
            INCLUDE_DIRS
            ${NDK_SRC_DIR}
            PRIVATE_INCLUDE_DIRS
            ${INC_DIR}
            ${INC_DIR}/internal
    )
    add_dependencies(cpufeatures_ndk_compat cpufeatures_utils)
    target_link_libraries(cpufeatures_ndk_compat PRIVATE cpufeatures_utils)
endif ()