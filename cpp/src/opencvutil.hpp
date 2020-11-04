#ifndef _OPENCVUTIL_HPP_
#define _OPENCVUTIL_HPP_

#include "config.hpp"
#include <string>
#include <utility>
#include <opencv2/core/mat.hpp>

/**
 *
 * @param _img
 * @param _newWidth
 * @param _newHeight
 * @return 图像，{比例、水平偏移、竖直偏移}
 */
std::pair<cv::Mat, std::tuple<float, float, float>>
padCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight);

#endif//_OPENCVUTIL_HPP_