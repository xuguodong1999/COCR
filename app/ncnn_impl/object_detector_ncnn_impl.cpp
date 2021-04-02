#include "object_detector_ncnn_impl.hpp"

#include <opencv2/imgproc.hpp>
#include <ncnn/net.h>

#include <iostream>

bool xgd::ObjectDetectorNcnnImpl::initModel(
        const std::string &_ncnnBin, const std::string &_ncnnParam, const int &_maxWidth) {
    maxWidth = maxHeight = _maxWidth - sizeBase;
    try {
        net = std::make_shared<ncnn::Net>();
        net->opt.num_threads = numThread;
        net->opt.use_vulkan_compute = false;
        net->opt.use_winograd_convolution = true;
        net->opt.use_sgemm_convolution = true;
        net->opt.use_fp16_packed = true;
        net->opt.use_fp16_storage = true;
        net->opt.use_fp16_arithmetic = true;
        net->opt.use_packing_layout = true;
        net->opt.use_shader_pack8 = false;
        net->opt.use_image_storage = false;
        int ret = net->load_param(_ncnnParam.c_str());
        if (ret != 0)return false;
        ret = net->load_model(_ncnnBin.c_str());
        if (ret != 0)return false;
        std::vector<cv::Mat> outs;
        std::vector<int> minSize = {1, 32, 32};
        cv::Mat emptyBlob(3, minSize.data(), CV_8UC1);
        ncnn::Mat in = ncnn::Mat::from_pixels(
                emptyBlob.data, ncnn::Mat::PIXEL_BGR2RGB,
                emptyBlob.cols, emptyBlob.rows);
        const float mean_vals[3] = {0, 0, 0};
        const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
        in.substract_mean_normalize(mean_vals, norm_vals);
        ncnn::Extractor ex = net->create_extractor();
        ex.input("data", in);
        ncnn::Mat out;
        ex.extract("output", out);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

void xgd::ObjectDetectorNcnnImpl::freeModel() {
    net->clear();
    net = nullptr;
}

std::pair<cv::Mat, std::vector<xgd::DetectorObject>>
xgd::ObjectDetectorNcnnImpl::detect(const cv::Mat &_originImage) {
    cv::Mat input = preProcess(_originImage);
    ncnn::Mat in = ncnn::Mat::from_pixels(
            input.data, ncnn::Mat::PIXEL_GRAY,
            input.cols, input.rows);
    const float mean_vals[3] = {0, 0, 0};
    const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
    in.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Extractor ex = net->create_extractor();
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("output", out);
    std::vector<xgd::DetectorObject> objects;
    for (int i = 0; i < out.h; i++) {
        const auto vec = out.row(i);
        float x = vec[2] * input.cols;
        float y = vec[3] * input.rows;
        float w = vec[4] * input.cols - x;
        float h = vec[5] * input.rows - y;
        float prob = vec[1];
        int label = vec[0] - 1;
        objects.emplace_back(x, y, w, h, label, prob);
    }
    return {input, objects};
}

xgd::ObjectDetectorNcnnImpl::ObjectDetectorNcnnImpl() : numThread(4), net(nullptr) {

}

void xgd::ObjectDetectorNcnnImpl::setNumThread(int numThread) {
    ObjectDetectorNcnnImpl::numThread = numThread;
    if (net) { net->opt.num_threads = numThread; }
}
