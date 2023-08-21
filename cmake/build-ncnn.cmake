macro(xgd_build_ncnn_omp_pthread)
    xgd_add_library(
            ncnn_omp
            STATIC
            SRC_FILES ${NCNN_OMP_SRC}
            INCLUDE_DIRS ${NCNN_INC_DIR}
    )
    set_target_properties(ncnn_omp PROPERTIES LINKER_LANGUAGE C)
    target_include_directories(ncnn_omp PUBLIC ${NCNN_GEN_BASE_INC_DIR} ${NCNN_GEN_INC_DIR})
    target_compile_definitions(ncnn_omp PUBLIC NCNN_SIMPLEOMP)
    if (IOS OR APPLE)
        target_compile_options(ncnn_omp PUBLIC -Xpreprocessor -fopenmp)
    else ()
        target_compile_options(ncnn_omp PUBLIC -fopenmp)
    endif ()
    xgd_link_threads(ncnn_omp PUBLIC)
endmacro()

set(NCNN_ROOT_DIR ${XGD_THIRD_PARTY_DIR}/ncnn-src/ncnn)
set(NCNN_INC_DIR ${NCNN_ROOT_DIR}/src)
set(NCNN_SRC_DIR ${NCNN_INC_DIR})
set(NCNN_GEN_BASE_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/ncnn/include)
set(NCNN_GEN_INC_DIR ${NCNN_GEN_BASE_INC_DIR}/ncnn)
set(NCNN_CMAKE_DIR ${NCNN_ROOT_DIR}/cmake)
set(NCNN_OMP_SRC ${NCNN_SRC_DIR}/simpleomp.cpp ${NCNN_SRC_DIR}/cpu.cpp)
xgd_add_library(
        ncnn
        SRC_DIRS ${NCNN_SRC_DIR}
        INCLUDE_DIRS ${NCNN_INC_DIR}
        EXCLUDE_SRC_FILES ${NCNN_OMP_SRC}
)
if (XGD_USE_VK AND NOT (MSVC AND NOT BUILD_SHARED_LIBS))
    set(NCNN_VULKAN ON)
    set(NCNN_SYSTEM_GLSLANG ON)
endif ()
xgd_link_omp(ncnn)
get_target_property(_XGD_OMP_LINKED ncnn XGD_OMP_LINKED)
if (_XGD_OMP_LINKED)
    set(NCNN_SIMPLEOMP OFF)
    target_sources(ncnn PRIVATE ${NCNN_OMP_SRC})
else ()
    set(NCNN_SIMPLEOMP ON)
    xgd_build_ncnn_omp_pthread()
    xgd_link_ncnn_omp(ncnn)
endif ()
if (NOT EMSCRIPTEN)
    set(NCNN_RUNTIME_CPU ON)
endif ()
set(NCNN_SIMPLEOCV OFF)
set(NCNN_INT8 ON)
set(NCNN_BF16 ON)
set(NCNN_C_API ON)
set(NCNN_STDIO ON)
set(NCNN_THREADS ON)
set(NCNN_STRING ON)
set(NCNN_PIXEL ON)
set(NCNN_PIXEL_ROTATE ON)
set(NCNN_PIXEL_AFFINE ON)
set(NCNN_PIXEL_DRAWING ON)
set(NCNN_VERSION_STRING 1.0.20230517)
xgd_generate_export_header(ncnn "ncnn" ".h")
if (EMSCRIPTEN)
    set(NCNN_TARGET_ARCH x86)
    set(NCNN_SSE2 ON)
    set(NCNN_AVX OFF)
    set(NCNN_AVX2 OFF)
    set(NCNN_FMA ON)
    set(NCNN_F16C OFF)
    set(NCNN_XOP OFF)
endif ()
if (XGD_OPT_ARCH_X86)
    set(NCNN_TARGET_ARCH x86)
    if (XGD_FLAG_SSE_ALL)
        set(NCNN_SSE2 ON)
    endif ()
    if (XGD_FLAG_AVX)
        set(NCNN_AVX ON)
    endif ()
    if (XGD_FLAG_AVX2)
        set(NCNN_AVX2 ON)
    endif ()
    if (XGD_FLAG_FMA)
        set(NCNN_FMA ON)
    endif ()
    if (XGD_FLAG_F16C)
        set(NCNN_F16C ON)
    endif ()
    if (XGD_FLAG_XOP)
        set(NCNN_XOP ON)
    endif ()
elseif (XGD_OPT_ARCH_POWER)
    set(NCNN_TARGET_ARCH powerpc)
elseif (XGD_OPT_ARCH_MIPS)
    set(NCNN_TARGET_ARCH mips)
elseif (XGD_OPT_ARCH_ARM)
    set(NCNN_TARGET_ARCH arm)
    set(NCNN_VFPV4 ON)
    set(NCNN_ARM82 ON)
    set(NCNN_ARM82DOT ON)
    set(NCNN_ARM82FP16FML ON)
    set(NCNN_ARM84BF16 ON)
    set(NCNN_ARM84I8MM ON)
    set(NCNN_ARM86SVE ON)
    set(NCNN_ARM86SVE2 ON)
    set(NCNN_ARM86SVEBF16 ON)
    set(NCNN_ARM86SVEI8MM ON)
    set(NCNN_ARM86SVEF32MM ON)
endif ()

if (NCNN_TARGET_ARCH)
    target_include_directories(ncnn PRIVATE ${NCNN_SRC_DIR}/layer)
    target_include_directories(ncnn PRIVATE ${NCNN_SRC_DIR}/layer/${NCNN_TARGET_ARCH})
    target_include_directories(ncnn PRIVATE ${NCNN_GEN_INC_DIR}/layer/${NCNN_TARGET_ARCH})
endif ()

set(ncnn_SRCS)
include(${NCNN_CMAKE_DIR}/ncnn_add_shader.cmake)
include(${NCNN_CMAKE_DIR}/ncnn_add_layer.cmake)

set(NCNN_SHADER_SPV_HEX_FILES)
set(__LAYER_TYPE_ENUM_INDEX 0)
set(__LAYER_SHADER_TYPE_ENUM_INDEX 0)

ncnn_add_layer(ArgMax OFF)
ncnn_add_layer(SPP OFF)
set(_NCNN_LAYER_NAMES
        AbsVal
        BatchNorm Bias BinaryOp BNLL
        Cast Clip Concat Convolution Convolution1D Convolution3D ConvolutionDepthWise
        ConvolutionDepthWise1D ConvolutionDepthWise3D CopyTo Crop CumulativeSum
        Deconvolution Deconvolution1D Deconvolution3D DeconvolutionDepthWise
        DeconvolutionDepthWise1D DeconvolutionDepthWise3D DeepCopy DeformableConv2D Dequantize DetectionOutput Dropout
        Einsum Eltwise ELU Embed Exp ExpandDims
        Flatten Fold
        GELU Gemm GLU GridSample GroupNorm GRU
        HardSigmoid HardSwish
        InnerProduct Input InstanceNorm Interp
        LayerNorm Log LRN LSTM
        MatMul MemoryData Mish MultiHeadAttention MVN
        Noop Normalize
        Packing Padding Permute PixelShuffle Pooling Pooling1D Pooling3D Power PReLU PriorBox Proposal PSROIPooling
        Quantize
        Reduction ReLU Reorg Requantize Reshape RNN ROIAlign ROIPooling
        Scale SELU ShuffleChannel Sigmoid Slice Softmax Softplus Split Squeeze StatisticsPooling Swish
        TanH Threshold Tile
        UnaryOp Unfold
        YoloDetectionOutput Yolov3DetectionOutput)
foreach (_NCNN_LAYER_NAME ${_NCNN_LAYER_NAMES})
    ncnn_add_layer(${_NCNN_LAYER_NAME})
endforeach ()

if (NCNN_VULKAN)
    ncnn_add_shader(${NCNN_SRC_DIR}/convert_ycbcr.comp)
    ncnn_add_shader(${NCNN_SRC_DIR}/layer/vulkan/shader/vulkan_activation.comp)
endif ()

add_custom_target(ncnn-generate-spirv DEPENDS ${NCNN_SHADER_SPV_HEX_FILES})
add_dependencies(ncnn ncnn-generate-spirv)

set(TO_CONFIGURE_FILES
        platform.h
        layer_registry.h
        layer_type_enum.h
        layer_shader_registry.h
        layer_shader_spv_data.h
        layer_shader_type_enum.h
        layer_declaration.h)
foreach (TO_CONFIGURE_FILE ${TO_CONFIGURE_FILES})
    configure_file(
            ${NCNN_SRC_DIR}/${TO_CONFIGURE_FILE}.in
            ${NCNN_GEN_INC_DIR}/${TO_CONFIGURE_FILE}
    )
endforeach ()

target_include_directories(ncnn PUBLIC ${NCNN_GEN_INC_DIR})
if (WIN32)
    target_compile_definitions(ncnn PRIVATE NOMINMAX)
endif ()
if (ncnn_SRCS)
    target_sources(ncnn PRIVATE ${ncnn_SRCS})
endif ()
if (NCNN_VULKAN)
    target_include_directories(ncnn PRIVATE ${NCNN_GEN_INC_DIR}/layer/vulkan/shader ${NCNN_SRC_DIR}/layer)
    xgd_link_vulkan(ncnn LINK_TYPE PUBLIC)
endif ()
