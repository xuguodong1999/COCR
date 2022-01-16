#ifndef _YOLO_NCNN_HPP_
#define _YOLO_NCNN_HPP_

#include "yolo_object.hpp"
#include "yolo.hpp"
#include <ncnn/net.h>

class YoloNCNNImpl : public YoloDetector {
    ncnn::Net net;
public:
    bool init(const char *cfgPath, const char *parmPath) override;

    std::vector<YoloObject> detect(const cv::Mat &_img) override;
};

#endif//_YOLO_NCNN_HPP_
