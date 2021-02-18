#ifndef _XGD_TEXT_RECOGNITION_HPP_
#define _XGD_TEXT_RECOGNITION_HPP_

#include <opencv2/core/mat.hpp>
#include <ncnn/net.h>
#include <string>
#include <opencv2/dnn.hpp>

namespace xgd {
    class TextRecognitionSolver {
    protected:
        bool isInited;
        std::vector<std::string> wordVec;
        const float meanValues = 127.5, normValues = 1.0 / 127.5;
        const int dstHeight = 32;

        void preProcess(cv::Mat &_src, const int &_dstWidth);

    public:
        virtual void freeModel() = 0;

        virtual std::pair<std::string, std::vector<float>> recognize(cv::Mat &_originImage) = 0;
    };

    class TextRecognitionOpenCVSolver : public TextRecognitionSolver {
        cv::dnn::TextRecognitionModel model;
    public:
        bool initModel(const std::string &_onnxFile, const std::string &_words, int _width);

        void freeModel() override;

        std::pair<std::string, std::vector<float>> recognize(cv::Mat &_originImage) override;
    };

    class TextRecognitionNCNNSolver : public TextRecognitionSolver {
        int numThread;
        ncnn::Net net;

        std::pair<std::string, std::vector<float>>
        recognize(const float *_outputData, const int &_h, const int _w);

    public:
        TextRecognitionNCNNSolver();

        bool initModel(const std::string &_ncnnBin, const std::string &_ncnnParam, const std::string &_words);

        std::pair<std::string, std::vector<float>> recognize(cv::Mat &_originImage) override;

        void freeModel() override;
    };
}
#endif//_XGD_TEXT_RECOGNITION_HPP_
