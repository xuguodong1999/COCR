#include "text_recognizer_opencv_impl.hpp"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


bool cocr::TextRecognizerOpenCVImpl::initModel(
        const std::string &_onnxFile, const std::string &_words, int _width) {
    try {
        dstWidth = _width;
        model = std::make_shared<cv::dnn::TextRecognitionModel>(_onnxFile);
        model->setDecodeType("CTC-greedy");
        for (auto &c:_words) {
            wordVec.push_back(std::string(1, c));
        }
        model->setVocabulary(wordVec);
        model->setInputParams(normValues, cv::Size(dstWidth, dstHeight), cv::Scalar(meanValues));
        // FIXME: opencv_dnn's crnn work at fixed image width, which is slow for most cases
        model->setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        model->setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    } catch (...) {
        return false;
    }
    return true;
}

void cocr::TextRecognizerOpenCVImpl::freeModel() {
    model = nullptr;
}

std::pair<std::string, std::vector<float>> cocr::TextRecognizerOpenCVImpl::recognize(
        const cv::Mat &_originImage) {
    cv::Mat srcResized = preProcess(_originImage);
    std::string recognitionResult = model->recognize(srcResized);
    std::vector<float> scores(recognitionResult.size(), -1);
    return {recognitionResult, scores};
}

cv::Mat cocr::TextRecognizerOpenCVImpl::preProcess(const cv::Mat &_src) {
    cv::Mat srcResized = TextRecognizer::preProcess(_src);
    if (srcResized.cols < dstWidth) {
        cv::hconcat(srcResized, cv::Mat(dstHeight, dstWidth - srcResized.cols, CV_8UC1, cv::Scalar(255)), srcResized);
    } else if (srcResized.cols > dstWidth) {
        cv::resize(srcResized, srcResized, cv::Size(dstWidth, dstHeight), 0, 0, cv::INTER_CUBIC);
    }
    return srcResized;
}
