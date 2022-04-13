#pragma once

#include "els_ocv_wrapper_export.h"
#include "base/point2.h"
#include <vector>

namespace cv {
    class Mat;
}
class ELS_OCV_WRAPPER_EXPORT AffineHelper {
    std::shared_ptr<cv::Mat> affine;
public:
    void calc(const std::vector<point2f> &from, const std::vector<point2f> &to);

    point2f transform(const point2f &p);
};
class ELS_OCV_WRAPPER_EXPORT CvUtil{
public:
    static std::vector<point2f> GetMinAreaRect(const std::vector<point2f>& input);
};