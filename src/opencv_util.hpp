#ifndef _OPENCV_UTIL_HPP_
#define _OPENCV_UTIL_HPP_

#include <optional>
#include <string>
#include <utility>
#include <opencv2/core/mat.hpp>

std::pair<cv::Mat, std::tuple<float, float, float>>
resizeCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight,
              const cv::Scalar &_padColor = cv::Scalar(255., 255., 255.));

std::pair<cv::Mat, std::tuple<float, float, float>>
padCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight,
           const cv::Scalar &_padColor = cv::Scalar(255., 255., 255.));

std::optional<cv::Rect2i> getBoundBoxForBWFont(const cv::Mat &_uMat, const uchar &_bgPixel = 255);

template<typename _Tp>
inline _Tp getDistance2D(const cv::Point_<_Tp> &p1, const cv::Point_<_Tp> &p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

template<typename _Tp>
inline _Tp getDistance3D(const cv::Point3_<_Tp> &p1, const cv::Point3_<_Tp> &p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2) + std::pow(p1.z - p2.z, 2));
}

template<typename _Tp>
inline cv::Point_<_Tp> getRectCenter2D(const cv::Rect_<_Tp> &_rect) {
    return cv::Point_<_Tp>(_rect.x + _rect.width / 2, _rect.y + _rect.height / 2);
}

template<typename _Tp>
inline _Tp getScalarSum(const cv::Scalar_<_Tp> &_scalar) {
    const auto val = _scalar.val;
    _Tp sum = 0;
    for (size_t i = 0; i < 4; i++) {
        sum += val[i];
    }
    return sum;
}

#endif