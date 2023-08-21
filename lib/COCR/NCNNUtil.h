#pragma once

#include <net.h> // <ncnn/net.h>

inline void configNcnnNet(ncnn::Net &net) {
#if NCNN_VULKAN
    net.opt.use_vulkan_compute = true;
    net.opt.use_shader_pack8 = true;
    net.opt.use_shader_local_memory = true;
    net.opt.use_fp16_packed = true;
    net.opt.use_fp16_storage = true;
    net.opt.use_fp16_arithmetic = true;
#endif
    net.opt.use_winograd_convolution = true;
    net.opt.use_sgemm_convolution = true;
    net.opt.use_packing_layout = true;
    net.opt.use_bf16_storage = true;
}