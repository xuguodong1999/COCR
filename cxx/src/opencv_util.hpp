#ifndef _OPENCV_UTIL_HPP_
#define _OPENCV_UTIL_HPP_

#include <optional>
#include <string>
#include <utility>
#include <opencv2/core/mat.hpp>

/**
 * 将图像填充到指定尺寸，先进行缩放
 * @param _img
 * @param _newWidth
 * @param _newHeight
 * @return 图像，{比例、水平偏移、竖直偏移}
 */
std::pair<cv::Mat, std::tuple<float, float, float>>
resizeCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight,
              const cv::Scalar &_padColor = cv::Scalar(255., 255., 255.));

/**
 * 将图像填充到更大尺寸，不进行缩放
 * @param _img
 * @param _newWidth
 * @param _newHeight
 * @param _padColor
 * @return
 */
std::pair<cv::Mat, std::tuple<float, float, float>>
padCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight,
           const cv::Scalar &_padColor = cv::Scalar(255., 255., 255.));

/**
 * 在单通道黑白[0-255]位图上，寻找非 _bgPixel 像素的最小正包围盒
 * @param _uMat 填充为uchar，与 _bgPixel 可作相等比较
 * @param _bgPixel 背景颜色值
 * @return 正包围盒
 */
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
#endif//_OPENCV_UTIL_HPP_