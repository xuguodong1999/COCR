#pragma once
#include "../ocr/object_detector.hpp"
#include <string>
#include <memory>
#include <vector>

namespace ncnn {
    class Net;
}

namespace cocr {
    class ObjectDetectorNcnnImpl : public ObjectDetector {
        int numThread;

    private:
        std::shared_ptr<ncnn::Net> net;

    public:
        void setNumThread(int numThread);

        ObjectDetectorNcnnImpl();

        bool initModel(const std::string &_ncnnBin, const std::string &_ncnnParam, const int &_maxWidth);

        void freeModel() override;

        std::pair<cv::Mat, std::vector<DetectorObject>>
        detect(const cv::Mat &_originImage) override;
    };
}