find_package(Threads REQUIRED)

# system libraries
option(XGD_USE_OPENMP "" ON)
option(XGD_USE_CUDA "" ON)
option(XGD_USE_QT "" ON)
option(XGD_USE_VK "" ON)
option(XGD_USE_TORCH "" ON)
option(XGD_USE_CCACHE "" ON)

# platform related optimization
option(XGD_OPT_ARCH_X86 "" ON)
option(XGD_OPT_ARCH_ARM "" ON)
option(XGD_OPT_ARCH_MIPS "" ON)
option(XGD_OPT_ARCH_POWER "" ON)
option(XGD_OPT_ARCH_32 "" ON)
option(XGD_OPT_ARCH_64 "" ON)
option(XGD_OPT_RC "" OFF)

option(XGD_FLAG_NEON "" ON)
option(XGD_FLAG_FMA "" ON)
option(XGD_FLAG_F16C "" ON)
option(XGD_FLAG_XOP "" ON)
option(XGD_FLAG_SSE_ALL "" ON)
option(XGD_FLAG_AVX "" ON)
option(XGD_FLAG_AVX2 "" ON)

option(XGD_FLAG_MARCH_NATIVE "" ON)
option(XGD_FLAG_WASM_SIMD128 "" ON)

# detailed project config
option(XGD_BUILD_WITH_GRADLE "" OFF)
option(XGD_NO_DEBUG_CONSOLE "" OFF)

if (NOT DEFINED XGD_WINE64_RUNTIME)
    set(XGD_WINE64_RUNTIME "" CACHE FILEPATH "path to wine64")
endif ()
if (NOT DEFINED XGD_NODEJS_RUNTIME)
    set(XGD_NODEJS_RUNTIME "" CACHE FILEPATH "path to node")
endif ()
if (NOT DEFINED XGD_POSTFIX)
    set(XGD_POSTFIX "" CACHE STRING "param passed to CMAKE_DEBUG_POSTFIX")
endif ()
if (NOT DEFINED XGD_WASM_ENV)
    set(XGD_WASM_ENV "" CACHE STRING "param passed to -sENVIRONMENT")
endif ()
if (NOT DEFINED XGD_QT_MODULES)
    set(XGD_QT_MODULES "Core;Gui;OpenGL;Widgets;Test" CACHE STRING "")
endif ()
if (NOT DEFINED XGD_OPTIONAL_QT_MODULES)
    set(XGD_OPTIONAL_QT_MODULES "3DCore;3DExtras" CACHE STRING "")
endif ()

# directory layout
if (NOT XGD_BUILD_WITH_GRADLE) # gradle relies on directory layout to copy libraries
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/archive)
endif ()

# find system packages
xgd_external_find_package()

# find system runtimes
xgd_external_find_runtime()

# check supported compiler flags, link options, etc.
xgd_external_check_env()

# enable_testing()
