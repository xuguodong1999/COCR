#ifndef _XGD_OBJECT_DETECTION_NCNN_IMPL_HPP_
#define _XGD_OBJECT_DETECTION_NCNN_IMPL_HPP_

#include "object_detector.hpp"
#include <string>
#include <memory>
#include <vector>

namespace ncnn {
    class Net;
}

namespace xgd {
    class ObjectDetectorNcnnImpl : public ObjectDetector {
        int numThread;
    public:
        void setNumThread(int numThread);

    private:
        std::shared_ptr<ncnn::Net> net;

    public:
        ObjectDetectorNcnnImpl();

        bool initModel(const std::string &_ncnnBin, const std::string &_ncnnParam, const int &_maxWidth);

        void freeModel();

        std::pair<cv::Mat, std::vector<DetectorObject>>
        detect(const cv::Mat &_originImage);
    };
}
#endif//_XGD_OBJECT_DETECTION_NCNN_IMPL_HPP_
