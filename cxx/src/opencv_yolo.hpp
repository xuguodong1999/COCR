#ifndef _OPENCV_YOLO_HPP_
#define _OPENCV_YOLO_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>

#include <memory>
#include <vector>
class OpenCVYolo{
    cv::dnn::Net net;
    std::vector<cv::String> outBlobNames;
public:
    void init(const char*_cfgPath,const char*_weightsPath);
    void forward(const cv::Mat&_input,const int&_gridSize=32);
};
#endif//_OPENCV_YOLO_HPP_
