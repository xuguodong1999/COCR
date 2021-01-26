#include "yolo_ncnn.hpp"

#include <iostream>

bool YoloNCNNImpl::init(const char *cfgPath, const char *parmPath) {
    try {
        net.clear();
        net.opt.num_threads = 6;
        net.opt.use_vulkan_compute = false;
        net.opt.use_winograd_convolution = true;
        net.opt.use_sgemm_convolution = true;
        net.opt.use_fp16_packed = true;
        net.opt.use_fp16_storage = true;
        net.opt.use_fp16_arithmetic = true;
        net.opt.use_packing_layout = true;
        net.opt.use_shader_pack8 = false;
        net.opt.use_image_storage = false;
        int ret = 0;
        ret = net.load_param(cfgPath);
        if (ret != 0)return false;
        ret = net.load_model(parmPath);
        if (ret != 0)return false;
        std::vector<cv::Mat> outs;
        std::vector<int> minSize = {3, 32, 32};
        cv::Mat emptyBlob(3, minSize.data(), CV_8UC3);
        ncnn::Mat in = ncnn::Mat::from_pixels_resize(
                emptyBlob.data, ncnn::Mat::PIXEL_BGR2RGB,
                emptyBlob.cols, emptyBlob.rows, emptyBlob.cols, emptyBlob.rows);
        const float mean_vals[3] = {0, 0, 0};
        const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
        in.substract_mean_normalize(mean_vals, norm_vals);
        ncnn::Extractor ex = net.create_extractor();
        ex.input("data", in);
        ncnn::Mat out;
        ex.extract("output", out);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

std::vector<YoloObject> YoloNCNNImpl::detect(const cv::Mat &_img) {
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(
            _img.data, ncnn::Mat::PIXEL_BGR2RGB,
            _img.cols, _img.rows, _img.cols, _img.rows);
    const float mean_vals[3] = {0, 0, 0};
    const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
    in.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Extractor ex = net.create_extractor();
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("output", out);
    std::vector<YoloObject> objects;
    for (int i = 0; i < out.h; i++) {
        const auto vec = out.row(i);
        float x = vec[2] * _img.cols;
        float y = vec[3] * _img.rows;
        float w = vec[4] * _img.cols - x;
        float h = vec[5] * _img.rows - y;
        float prob = vec[1];
        int label = vec[0] - 1;
        objects.emplace_back(x, y, w, h, label, prob);
    }
    return std::move(objects);
}
