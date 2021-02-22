#include "object_detection.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>

xgd::DetectorObject::DetectorObject(const float &_x, const float &_y, const float &_w, const float &_h,
                                    const int &_label,
                                    const float &_prob) : x(_x), y(_y), w(_w), h(_h), label(_label), prob(_prob) {}

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
        if (kh < kw) {
            w *= kh;
            h *= kh;
        } else {
            w *= kw;
            h *= kw;
        }
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
    return procImg;
}

xgd::ObjectDetector::ObjectDetector() :maxHeight(1280),maxWidth(1280){

}


bool xgd::ObjectDetectorOpenCVImpl::initModel(const std::string &_cfgFile, const std::string &_weightsFile) {
    try {
        net = cv::dnn::readNetFromDarknet(_cfgFile, _weightsFile);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
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
    std::vector<float> probs;
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
            float cx = vec.at<float>(j, 0) * input.cols;
            float cy = vec.at<float>(j, 1) * input.rows;
            float bw = vec.at<float>(j, 2) * input.cols;
            float bh = vec.at<float>(j, 3) * input.rows;
            boxes.emplace_back(cx - bw / 2, cy - bh / 2, bw, bh);
            labels.push_back(maxLoc.x);
            probs.push_back(maxProb);
        }
    }
    std::vector<int> selectedBoxIndices;
    cv::dnn::NMSBoxes(boxes, probs, confThresh, iouThresh, selectedBoxIndices);
    std::vector<DetectorObject> objects;
    for (const auto &i:selectedBoxIndices) {
        objects.emplace_back(boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height,
                             labels[i], probs[i]);
    }
    return {input, objects};
}

void xgd::ObjectDetectorOpenCVImpl::setConfThresh(float confThresh) {
    ObjectDetectorOpenCVImpl::confThresh = confThresh;
}

void xgd::ObjectDetectorOpenCVImpl::setIouThresh(float iouThresh) {
    ObjectDetectorOpenCVImpl::iouThresh = iouThresh;
}
