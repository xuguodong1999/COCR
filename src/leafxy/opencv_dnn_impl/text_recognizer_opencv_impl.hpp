#pragma once
#include "../ocr/text_recognizer.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <memory>


namespace cocr {
    /**
     * FIXME: onnx in opencv_dnn only support fixed image width
     */
    class TextRecognizerOpenCVImpl : public TextRecognizer {
        std::shared_ptr<cv::dnn::TextRecognitionModel> model;
        int dstWidth;

        cv::Mat preProcess(const cv::Mat &_src) override;

    public:
        bool initModel(const std::string &_onnxFile, const std::string &_words, int _width = 192);

        void freeModel() override;

        std::pair<std::string, std::vector<float>> recognize(const cv::Mat &_originImage) override;
    };
}