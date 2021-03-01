#include "object_detection.hpp"
#include "ocr_types.hpp"
#include "color_types.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <ncnn/net.h>

#include <iostream>

xgd::DetectorObject::DetectorObject(
        const float &_x, const float &_y, const float &_w, const float &_h,
        const int &_label, const float &_prob)
        : rect(_x, _y, _w, _h), label(static_cast<DetectorObjectType>(_label)), prob(_prob) {}

bool xgd::DetectorObject::isValidLabel(const int &_label) {
    return minLabel <= _label && _label <= maxLabel;
}

const cv::Rect2f &xgd::DetectorObject::asRect() const {
    return rect;
}

const float &xgd::DetectorObject::x() const {
    return rect.x;
}

const float &xgd::DetectorObject::y() const {
    return rect.y;
}

const float &xgd::DetectorObject::w() const {
    return rect.width;
}

const float &xgd::DetectorObject::h() const {
    return rect.height;
}

/**
 * 默认行为：转单通道，边长向上转 sizeBase 的倍数，边长限制到 [maxWidth,maxHeight]
 * @param _src
 * @return 可以送进网络的图像
 */
cv::Mat xgd::ObjectDetector::preProcess(const cv::Mat &_src) {
    if (_src.empty())throw std::runtime_error("get empty image in ObjectDetector::preProcess");
    int w = _src.cols, h = _src.rows;
    if (w > maxWidth || h > maxHeight) {
        float kw = static_cast<float >(maxWidth) / w, kh = static_cast<float >(maxHeight) / h;
        float k = std::min(kw, kh);
        w *= k;
        h *= k;
    }
    w += (sizeBase - w % sizeBase);
    h += (sizeBase - h % sizeBase);
    if (w > maxWidth) w -= sizeBase;
    if (h > maxHeight)h -= sizeBase;
    cv::Mat procImg;
    if (3 == _src.channels()) {
        cv::cvtColor(_src, procImg, cv::ColorConversionCodes::COLOR_BGR2GRAY);
        if (w != _src.cols || h != _src.rows) {
            cv::resize(procImg, procImg, cv::Size(w, h), 0, 0, cv::INTER_CUBIC);
        }
    } else if (4 == _src.channels()) {
        cv::cvtColor(_src, procImg, cv::ColorConversionCodes::COLOR_BGRA2GRAY);
        if (w != _src.cols || h != _src.rows) {
            cv::resize(procImg, procImg, cv::Size(w, h), 0, 0, cv::INTER_CUBIC);
        }
    } else {
        if (w != _src.cols || h != _src.rows) {
            cv::resize(_src, procImg, cv::Size(w, h), 0, 0, cv::INTER_CUBIC);
        } else {
            procImg = _src;
        }
    }
    const size_t baseSize_2 = sizeBase / 2;
    cv::vconcat(procImg, cv::Mat(baseSize_2, procImg.cols, procImg.type(), cvColor(ColorName::rgbWhite)), procImg);
    cv::vconcat(cv::Mat(baseSize_2, procImg.cols, procImg.type(), cvColor(ColorName::rgbWhite)), procImg, procImg);
    cv::hconcat(procImg, cv::Mat(procImg.rows, baseSize_2, procImg.type(), cvColor(ColorName::rgbWhite)), procImg);
    cv::hconcat(cv::Mat(procImg.rows, baseSize_2, procImg.type(), cvColor(ColorName::rgbWhite)), procImg, procImg);
    return procImg;
}

xgd::ObjectDetector::ObjectDetector() : maxHeight(1280), maxWidth(1280) {

}


bool xgd::ObjectDetectorOpenCVImpl::initModel(const std::string &_cfgFile, const std::string &_weightsFile) {
    try {
        net = cv::dnn::readNetFromDarknet(_cfgFile, _weightsFile);
//        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
//        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        auto outLayers = net.getUnconnectedOutLayers();
        auto layersNames = net.getLayerNames();
        outBlobNames.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); i++) {
            outBlobNames[i] = layersNames[outLayers[i] - 1];
        }
        std::vector<cv::Mat> outs;
        std::vector<int> minSize = {1, 1, sizeBase, sizeBase};
        cv::Mat emptyBlob(4, minSize.data(), CV_32F);
        net.setInput(emptyBlob);
        net.forward(outs, outBlobNames);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;// FIXME: 不返回的话，gcc-Release编译报错
}

void xgd::ObjectDetectorOpenCVImpl::freeModel() {

}

std::pair<cv::Mat, std::vector<xgd::DetectorObject>>
xgd::ObjectDetectorOpenCVImpl::detect(const cv::Mat &_originImage) {
    cv::Mat blob;
    cv::Mat input = preProcess(_originImage);
    cv::dnn::blobFromImage(input, blob, 1 / 255.0);
    net.setInput(blob);
    std::vector<cv::Mat> outputBlobs;
    net.forward(outputBlobs, outBlobNames);
    blob.release();
    std::vector<float> confs;
    std::vector<cv::Rect2d> boxes;
    std::vector<int> labels;
    for (size_t i = 0; i < outputBlobs.size(); i++) {
        const auto &vec = outputBlobs[i];
        for (int j = 0; j < vec.rows; j++) {
            double maxProb;
            cv::Point2i maxLoc;
            minMaxLoc(vec.row(j).colRange(5, vec.cols),
                      nullptr, &maxProb, nullptr, &maxLoc);
            if (maxProb < confThresh) continue;
            const int &label = maxLoc.x;
            if (!DetectorObject::isValidLabel(label)) continue;
            float cx = vec.at<float>(j, 0) * input.cols;
            float cy = vec.at<float>(j, 1) * input.rows;
            float bw = vec.at<float>(j, 2) * input.cols;
            float bh = vec.at<float>(j, 3) * input.rows;
            boxes.emplace_back(cx - bw / 2, cy - bh / 2, bw, bh);
            labels.push_back(label);
            confs.push_back(maxProb);
        }
    }
    std::vector<int> selectedBoxIndices;
    cv::dnn::NMSBoxes(boxes, confs, confThresh, iouThresh, selectedBoxIndices);
    std::vector<DetectorObject> objects;
    for (const auto &i:selectedBoxIndices) {
        objects.emplace_back(boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height,
                             labels[i], confs[i]);
    }
    return {input, objects};
}

void xgd::ObjectDetectorOpenCVImpl::setConfThresh(float confThresh) {
    ObjectDetectorOpenCVImpl::confThresh = confThresh;
}

void xgd::ObjectDetectorOpenCVImpl::setIouThresh(float iouThresh) {
    ObjectDetectorOpenCVImpl::iouThresh = iouThresh;
}

bool xgd::ObjectDetectorNCNNImpl::initModel(
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
        int ret = 0;
        ret = net->load_param(_ncnnParam.c_str());
        if (ret != 0)return false;
        ret = net->load_model(_ncnnBin.c_str());
        if (ret != 0)return false;
        std::vector<cv::Mat> outs;
        std::vector<int> minSize = {1, 32, 32};
        cv::Mat emptyBlob(3, minSize.data(), CV_8UC1);
        ncnn::Mat in = ncnn::Mat::from_pixels_resize(
                emptyBlob.data, ncnn::Mat::PIXEL_BGR2RGB,
                emptyBlob.cols, emptyBlob.rows, emptyBlob.cols, emptyBlob.rows);
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

void xgd::ObjectDetectorNCNNImpl::freeModel() {
    net->clear();
    net = nullptr;
}

std::pair<cv::Mat, std::vector<xgd::DetectorObject>>
xgd::ObjectDetectorNCNNImpl::detect(const cv::Mat &_originImage) {
    cv::Mat input = preProcess(_originImage);
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(
            input.data, ncnn::Mat::PIXEL_GRAY,
            input.cols, input.rows, input.cols, input.rows);
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

xgd::ObjectDetectorNCNNImpl::ObjectDetectorNCNNImpl() : numThread(4), net(nullptr) {

}

void xgd::ObjectDetectorNCNNImpl::setNumThread(int numThread) {
    ObjectDetectorNCNNImpl::numThread = numThread;
    if (net)
        net->opt.num_threads = numThread;
}
