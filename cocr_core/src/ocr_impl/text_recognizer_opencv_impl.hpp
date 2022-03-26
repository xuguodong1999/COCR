#pragma once

#include "ocr/text_recognizer.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <string>
#include <memory>


namespace cocr {
    /**
     * FIXME: onnx in opencv_dnn only support fixed image width
     */
    class TextRecognizerOpenCVImpl : public TextRecognizer {
        std::shared_ptr<cv::dnn::TextRecognitionModel> model;
        int dstWidth;

        cv::Mat preProcess(const cv::Mat &_src) override {
            cv::Mat srcResized = TextRecognizer::preProcess(_src);
            if (srcResized.cols < dstWidth) {
                cv::hconcat(srcResized, cv::Mat(dstHeight, dstWidth - srcResized.cols, CV_8UC1, cv::Scalar(255)),
                            srcResized);
            } else if (srcResized.cols > dstWidth) {
                cv::resize(srcResized, srcResized, cv::Size(dstWidth, dstHeight), 0, 0, cv::INTER_CUBIC);
            }
            return srcResized;
        }


    public:
        bool initModel(const std::string &_onnxFile, const std::string &_words, int _width = 192) {
            try {
                dstWidth = _width;
                model = std::make_shared<cv::dnn::TextRecognitionModel>(_onnxFile);
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

        std::pair<std::string, std::vector<float>> recognize(const cv::Mat &_originImage) override {
            cv::Mat srcResized = preProcess(_originImage);
            std::string recognitionResult = model->recognize(srcResized);
            std::vector<float> scores(recognitionResult.size(), -1);
            return {recognitionResult, scores};
        }
    };
}