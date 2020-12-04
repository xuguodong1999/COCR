#ifndef _GT_BOX_HPP_
#define _GT_BOX_HPP_

#include <opencv2/core/types.hpp>

struct gt_box {
    cv::Rect2d bBox;
    int label;

    gt_box(const cv::Rect2d &_bBox, const int &_label) : bBox(_bBox), label(_label) {}
};

#endif//_GT_BOX_HPP_
