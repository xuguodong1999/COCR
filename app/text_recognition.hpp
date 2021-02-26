#ifndef _XGD_TEXT_RECOGNITION_HPP_
#define _XGD_TEXT_RECOGNITION_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <memory>

namespace ncnn {
    class Net;
}

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

    class TextRecognitionOpenCVImpl : public TextRecognition {
        std::shared_ptr<cv::dnn::TextRecognitionModel> model;
        int dstWidth;

        cv::Mat preProcess(const cv::Mat &_src) override;

    public:
        bool initModel(const std::string &_onnxFile, const std::string &_words, int _width = 320);

        void freeModel() override;

        std::pair<std::string, std::vector<float>> recognize(const cv::Mat &_originImage) override;
    };

    class TextRecognitionNcnnImpl : public TextRecognition {
        int numThread;
        std::shared_ptr<ncnn::Net> net;

        std::pair<std::string, std::vector<float>>
        recognize(const float *_outputData, const int &_h, const int _w);

        int maxWidth;

        cv::Mat preProcess(const cv::Mat &_src) override;

    public:
        TextRecognitionNcnnImpl();

        bool initModel(const std::string &_ncnnBin, const std::string &_ncnnParam, const std::string &_words,
                       const int &_maxWidth);

        std::pair<std::string, std::vector<float>> recognize(const cv::Mat &_originImage) override;

        void freeModel() override;
    };
}
#endif//_XGD_TEXT_RECOGNITION_HPP_
