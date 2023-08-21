#pragma once

#include "xgd_handwriting_export.h"
#include "XGD/HandWriting/Base.h"
#include <functional>

/**
 * stroke: collection of points
 */
class XGD_HANDWRITING_EXPORT HwStroke : public HwBase {
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

    explicit HwStroke(HwController *_hwController = nullptr);

    void paintTo(Mat &_canvas) const override;

    std::optional<rectf> getBoundingBox() const override;

    void rotateBy(float _angle, const point2f &_cent) override;

    void moveBy(const point2f &_offset) override;

    void mulK(float _kx, float _ky) override;
};