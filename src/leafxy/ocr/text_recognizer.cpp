#include "text_recognizer.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>

cv::Mat cocr::TextRecognizer::preProcess(const cv::Mat &_src) {
    // default behavior: resized to dstHeight
    if (_src.empty())throw std::runtime_error("get empty image in TextRecognizer::preProcess");
    float scaleX = (float) dstHeight / (float) _src.rows;
    int dstWidth = int((float) _src.cols * scaleX);
    cv::Mat procImg;
    cv::resize(_src, procImg, cv::Size(dstWidth, dstHeight), 0, 0, cv::INTER_CUBIC);
    return procImg;
}
