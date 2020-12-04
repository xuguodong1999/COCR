#ifndef _OPENCV_YOLO_HPP_
#define _OPENCV_YOLO_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>

#include <memory>
#include <vector>
class OpenCVYolo {
    cv::dnn::Net net;
    std::vector<cv::String> outBlobNames;
    static std::vector<std::string> sClasses;
    static std::vector<cv::Scalar> sColors;
    inline static double confThresh = 0.1, iouThresh = 0.6;
public:
    static void setConfThresh(double confThresh);

    static void setIouThresh(double iouThresh);

    static void setSClasses(const std::vector<std::string> &sClasses);

    static void setSColors(const std::vector<cv::Scalar> &sColors);

    void init(const char *_cfgPath, const char *_weightsPath);

    void forward(const cv::Mat &_input, const int &_gridSize = 32, bool _debug = true);
};
#endif//_OPENCV_YOLO_HPP_
