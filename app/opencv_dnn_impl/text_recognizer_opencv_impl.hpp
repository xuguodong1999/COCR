#ifndef _XGD_TEXT_RECOGNITION_OPENCV_DNN_IMPL_HPP_
#define _XGD_TEXT_RECOGNITION_OPENCV_DNN_IMPL_HPP_

#include "text_recognizer.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <memory>


namespace xgd {
    class TextRecognitionOpenCVImpl : public TextRecognition {
        std::shared_ptr<cv::dnn::TextRecognitionModel> model;
        int dstWidth;

        cv::Mat preProcess(const cv::Mat &_src) override;

    public:
        bool initModel(const std::string &_onnxFile, const std::string &_words, int _width = 320);

        void freeModel() override;

        std::pair<std::string, std::vector<float>> recognize(const cv::Mat &_originImage) override;
    };
}
#endif//_XGD_TEXT_RECOGNITION_OPENCV_DNN_IMPL_HPP_
