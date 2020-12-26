#ifndef _HW_STROKE_HPP_
#define _HW_STROKE_HPP_

#include "hw_base.hpp"
#include <vector>
#include <functional>

/**
 * 笔画：点的集合
 */
class HwStroke : public HwBase {
    std::vector<cv::Point2f> mData;
    HwController *hwController;
public:
    decltype(mData.begin())begin();
    decltype(mData.end())end();
    void setHwController(HwController &_hwController) override;

    void push_back(const cv::Point2f &_pt);

    void keepIf(const std::function<bool(const cv::Point2f &)> &_cond);

    HwStroke(HwController *_hwController = nullptr);


    void paintTo(cv::Mat &_canvas) const;

    std::optional<cv::Rect2f> getBoundingBox() const;

    void rotateBy(float _angle, const cv::Point2f &_cent);

    void moveBy(const cv::Point2f &_offset);

    void mulK(float _kx, float _ky);
};

#endif//_HW_STROKE_HPP_
