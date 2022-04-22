#pragma once

#include "cocr/text_recognizer.h"
#include "ocv/mat.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

#include <QFile>
#include <QDebug>

#include <iostream>
#include <string>
#include <memory>


/**
 * FIXME: onnx in opencv_dnn only support fixed image width
 */
class TextRecognizerOpenCVImpl : public TextRecognizer {
    std::shared_ptr<cv::dnn::TextRecognitionModel> model;
    int dstWidth;

    Mat preProcess(const Mat &_src) override {
        Mat srcResized0 = TextRecognizer::preProcess(_src);
        cv::Mat& srcResized = *(srcResized0.getHolder());
        if (srcResized.cols < dstWidth) {
            cv::hconcat(srcResized, cv::Mat(dstHeight, dstWidth - srcResized.cols, CV_8UC1, cv::Scalar(255)),
                        srcResized);
        } else if (srcResized.cols > dstWidth) {
            cv::resize(srcResized, srcResized, cv::Size(dstWidth, dstHeight), 0, 0, cv::INTER_CUBIC);
        }
        return srcResized0;
    }


public:
    bool initModel(const std::string &_onnxFile, const std::string &_words, int _width = 192) {
        dstWidth = _width;
        QFile onnxFile(_onnxFile.c_str());
        if (!onnxFile.open(QIODevice::ReadOnly)) {
            qDebug() << "fail in QFile read" << _onnxFile.c_str();
            return false;
        }
        auto buffer = onnxFile.readAll();
        onnxFile.close();
        try {
            auto net = cv::dnn::readNetFromONNX(buffer.data(), buffer.length());
            model = std::make_shared<cv::dnn::TextRecognitionModel>(net);
            model->setDecodeType("CTC-greedy");
            for (auto &c: _words) {
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

    void freeModel() override {
        model = nullptr;
    }

    std::pair<std::string, std::vector<float>> recognize(const Mat &_originImage) override {
        Mat srcResized = preProcess(_originImage);
        std::string recognitionResult = model->recognize(*(srcResized.getHolder()));
        std::vector<float> scores(recognitionResult.size(), -1);
        return {recognitionResult, scores};
    }
};
