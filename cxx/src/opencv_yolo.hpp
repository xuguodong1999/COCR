#ifndef _OPENCV_YOLO_HPP_
#define _OPENCV_YOLO_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>

#include <memory>
#include <vector>

struct gt_box {
    cv::Rect2d bBox;
    int label;

    gt_box(const cv::Rect2d &_bBox, const int &_label) : bBox(_bBox), label(_label) {}
};

extern std::vector<std::string> CLASSES;
extern std::vector<cv::Scalar> COLORS;

class OpenCVYolo {
    cv::dnn::Net net;
    std::vector<cv::String> outBlobNames;
    inline static double confThresh = 0.1, iouThresh = 0.6;
public:
    static void setConfThresh(double confThresh);

    static void setIouThresh(double iouThresh);

    void init(const char *_cfgPath, const char *_weightsPath);

    std::pair<std::vector<gt_box>, cv::Mat>
    forward(const cv::Mat &_input, bool _debug = false, const int &_gridSize = 32);
};
#endif//_OPENCV_YOLO_HPP_
