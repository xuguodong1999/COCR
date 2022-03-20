#pragma once
#include <cocr_ocr_export.h>
#include <opencv2/core/mat.hpp>
#include <string>
#include <memory>


namespace cocr {
    class COCR_OCR_EXPORT TextRecognizer {
    protected:
        std::vector<std::string> wordVec;
        const float meanValues = 127.5, normValues = 1.0 / 127.5;
        const int dstHeight = 32;

        virtual cv::Mat preProcess(const cv::Mat &_src);

    public:
        virtual void freeModel() = 0;

        virtual std::pair<std::string, std::vector<float>> recognize(const cv::Mat &_originImage) = 0;
    };
}