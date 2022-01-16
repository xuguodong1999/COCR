#ifndef _XGD_OBJECT_DETECTION_OPENCV_DNN_IMPL_HPP_
#define _XGD_OBJECT_DETECTION_OPENCV_DNN_IMPL_HPP_

#include "../ocr/object_detector.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <memory>
#include <vector>


namespace xgd {
    class ObjectDetectorOpenCVImpl : public ObjectDetector {
        cv::dnn::Net net;
        std::vector<cv::String> outBlobNames;
        float confThresh, iouThresh;
    public:
        void setConfThresh(float confThresh);

        void setIouThresh(float iouThresh);

    public:
        bool initModel(const std::string &_cfgFile, const std::string &_weightsFile);

        void freeModel() override;

        std::pair<cv::Mat, std::vector<DetectorObject>>
        detect(const cv::Mat &_originImage) override;
    };
}
#endif//_XGD_OBJECT_DETECTION_OPENCV_DNN_IMPL_HPP_
