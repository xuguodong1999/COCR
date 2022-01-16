#ifndef _UTILS_H_
#define _UTILS_H_

#include "interface.hpp"

#include <vector>


cv::Rect2f getRect(const Stroke &stroke);

cv::Rect2f getRect(const Script &script);

template<typename _Tp>
inline _Tp distance(const cv::Point_<_Tp> &p1, const cv::Point_<_Tp> &p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}


#endif