#ifndef _HW_ITEM_HPP_
#define _HW_ITEM_HPP_

#include "hw_script.hpp"

/**
 * 图元：笔迹的集合
 */
class HwItem : public HwBase {
protected:
    std::vector<HwScript> mData;
    HwController *hwController;
public:
    HwScript asScript() const;

    void setHwController(HwController &_hwController) override;

    void push_back(HwScript &_script);

    void push_back(HwScript &&_script);

    void keepIf(const std::function<bool(const cv::Point2f &)> &_cond);

    HwItem(HwController *_hwController = nullptr);

    void paintTo(cv::Mat &_canvas) const override;

    std::optional<cv::Rect2f> getBoundingBox() const override;

    void rotateBy(float _angle, const cv::Point2f &_cent) override;

    void moveBy(const cv::Point2f &_offset) override;

    void mulK(float _kx, float _ky) override;
};

#endif//_HW_ITEM_HPP_
