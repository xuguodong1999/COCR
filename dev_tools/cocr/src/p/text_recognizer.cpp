#include "ocr/text_recognizer.hpp"
#include "ocr/text_corrector.hpp"

#ifdef USE_OPENCV_DNN

#include "../ocr_impl/text_recognizer_opencv_impl.hpp"

#else

#include "../ocr_impl/text_recognizer_ncnn_impl.hpp"

#endif

#include <opencv2/imgproc.hpp>

#include <QDebug>

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

std::shared_ptr<cocr::TextRecognizer> cocr::TextRecognizer::MakeInstance() {
#ifdef USE_OPENCV_DNN
    std::string onnxTextModel = MODEL_DIR + std::string("/deprecated/onnx-crnn-57.onnx");
    auto recognizer = std::make_shared<cocr::TextRecognizerOpenCVImpl>();
    if (!recognizer->initModel(onnxTextModel, cocr::TextCorrector::GetAlphabet(), 192)) {
        qDebug() << "fail to init opencv recognizer";
        recognizer->freeModel();
        return nullptr;
    }
#else
    std::string ncnnTextModel = MODEL_DIR + std::string("/crnn57.fp16.bin");
    std::string ncnnTextModelCfg = MODEL_DIR + std::string("/crnn57.fp16.param");
    auto recognizer = std::make_shared<cocr::TextRecognizerNcnnImpl>();
    recognizer->setNumThread(4);
    if (!recognizer->initModel(
            ncnnTextModel, ncnnTextModelCfg,
            cocr::TextCorrector::GetAlphabet(), 3200)) {
        qDebug() << "fail to init ncnn recognizer";
        recognizer->freeModel();
        return nullptr;
    }
    qDebug() << "init ncnn recognizer success";
#endif
    return recognizer;
}


