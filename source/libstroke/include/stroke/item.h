#pragma once

#include "els_stroke_export.h"
#include "stroke/script.h"

/**
 * 图元：笔迹的集合
 */
class ELS_STROKE_EXPORT HwItem : public HwBase {
protected:
    std::vector<HwScript> mData;
    HwController *hwController;
public:
    std::shared_ptr<HwBase> clone() const override;

    HwScript asScript() const;

    void setHwController(HwController &_hwController) override;

    void push_back(HwScript &_script);

    void push_back(HwScript &&_script);

    void keepIf(const std::function<bool(const point2f &)> &_cond);

    explicit HwItem(HwController *_hwController = nullptr);

    void paintTo(Mat &_canvas) const override;

    std::optional<rectf> getBoundingBox() const override;

    void rotateBy(float _angle, const point2f &_cent) override;

    void moveBy(const point2f &_offset) override;

    void mulK(float _kx, float _ky) override;
};