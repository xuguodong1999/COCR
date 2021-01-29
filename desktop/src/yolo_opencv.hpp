#ifndef _YOLO_OPENCV_HPP_
#define _YOLO_OPENCV_HPP_

#include "yolo_object.hpp"
#include "yolo.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>

#include <vector>

class YoloOpenCVImpl : public YoloDetector {
    cv::dnn::Net net;
    std::vector<cv::String> outBlobNames;
public:
    bool init(const char *cfgPath, const char *parmPath) override;

    std::vector<YoloObject> detect(const cv::Mat &_img) override;
};

#endif//_YOLO_OPENCV_HPP_
