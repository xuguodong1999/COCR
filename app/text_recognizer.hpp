#ifndef _XGD_TEXT_RECOGNITION_HPP_
#define _XGD_TEXT_RECOGNITION_HPP_

#include <opencv2/core/mat.hpp>
#include <string>
#include <memory>


namespace xgd {
    class TextRecognition {
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
#endif//_XGD_TEXT_RECOGNITION_HPP_
