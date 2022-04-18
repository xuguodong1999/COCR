#pragma once

#include "els_stroke_export.h"
#include "stroke/abc.h"
#include <functional>

/**
 * 笔画：点的集合
 */
class ELS_STROKE_EXPORT HwStroke : public HwBase {
    std::vector<point2f> mData;
    HwController *hwController;
public:
    std::shared_ptr<HwBase> clone() const override;

    size_t size() const;

    void setData(std::vector<point2f> &_data);

    decltype(mData.begin()) begin();

    decltype(mData.end()) end();

    void setHwController(HwController &_hwController) override;

    void push_back(const point2f &_pt);

    void keepIf(const std::function<bool(const point2f &)> &_cond);

    HwStroke(HwController *_hwController = nullptr);

    void paintTo(Mat &_canvas) const;

    std::optional<rectf> getBoundingBox() const;

    void rotateBy(float _angle, const point2f &_cent);

    void moveBy(const point2f &_offset);

    void mulK(float _kx, float _ky);
};