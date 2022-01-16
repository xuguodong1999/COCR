#ifndef _HW_SCRIPT_HPP_
#define _HW_SCRIPT_HPP_

#include "hw_stroke.hpp"

class HwScript : public HwBase {
    std::vector<HwStroke> mData;
    HwController *hwController;
public:
    float getAvgMaxSize() const;

    size_t size() const;

    float getAvgPtsNum() const;

    void clear();

    void setHwController(HwController &_hwController) override;

    void push_back(HwStroke &_stroke);

    void keepIf(const std::function<bool(const cv::Point2f &)> &_cond);

    HwScript(HwController *_hwController = nullptr);

    HwScript(const HwScript &_hwScript) = default;

    HwScript(HwScript &&_hwScript);

    HwScript &operator=(HwScript &&_hwScript) = default;

    void paintTo(cv::Mat &_canvas) const;

    std::optional<cv::Rect2f> getBoundingBox() const;

    void rotateBy(float _angle, const cv::Point2f &_cent);

    void moveBy(const cv::Point2f &_offset);

    void mulK(float _kx, float _ky);

    void castBy(const std::vector<cv::Point2f> &_from, const std::vector<cv::Point2f> &_to);
};

#endif
