#include "ocr/text_recognizer.hpp"
#include "ocr/text_corrector.hpp"

#include "../ocr_impl/text_recognizer_ncnn_impl.hpp"

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

const char *ncnnTextModel = ":/models/ncnn/text_57/vgg_lstm_57_fp16_mixFont.bin";
const char *ncnnTextModelCfg = ":/models/ncnn/text_57/vgg_lstm_57_fp16.param";

std::shared_ptr<cocr::TextRecognizer> cocr::TextRecognizer::MakeInstance() {
    auto recognizer = std::make_shared<cocr::TextRecognizerNcnnImpl>();
    recognizer->setNumThread(4);
    if (!recognizer->initModel(
            ncnnTextModel, ncnnTextModelCfg,
            cocr::TextCorrector::GetAlphabet(), 3200)) {
        std::cerr << "fail to init ncnn recognizer" << std::flush;
        recognizer->freeModel();
        return nullptr;
    }
    return recognizer;
}


