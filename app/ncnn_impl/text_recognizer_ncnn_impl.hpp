#ifndef _XGD_TEXT_RECOGNITION_NCNN_IMPL_HPP_
#define _XGD_TEXT_RECOGNITION_NCNN_IMPL_HPP_

#include "text_recognizer.hpp"
#include <string>
#include <memory>

namespace ncnn {
    class Net;
}

namespace xgd {
    class TextRecognizerNcnnImpl : public TextRecognizer {
        int maxWidth;
        int numThread;
        std::shared_ptr<ncnn::Net> net;

        std::pair<std::string, std::vector<float>>
        recognize(const float *_outputData, const int &_h, const int _w);


        cv::Mat preProcess(const cv::Mat &_src) override;

    public:
        void setNumThread(int numThread);

        TextRecognizerNcnnImpl();

        bool initModel(const std::string &_ncnnBin, const std::string &_ncnnParam, const std::string &_words,
                       const int &_maxWidth);

        std::pair<std::string, std::vector<float>> recognize(const cv::Mat &_originImage) override;

        void freeModel() override;
    };
}
#endif//_XGD_TEXT_RECOGNITION_NCNN_IMPL_HPP_
