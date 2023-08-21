#pragma once

#include "xgd_handwriting_export.h"
#include "XGD/HandWriting/Base.h"
#include "XGD/HandWriting/Stroke.h"

/**
 * script: collection of strokes
 */
class XGD_HANDWRITING_EXPORT HwScript : public HwBase {
    std::vector<HwStroke> mData;
    HwController *hwController;
public:
    std::shared_ptr<HwBase> clone() const override;

    float getAvgMaxSize() const;

    size_t size() const;

    void append(const HwScript &_hwScript);

    float getAvgPtsNum() const;

    void clear();

    void setHwController(HwController &_hwController) override;

    void push_back(HwStroke &_stroke);

    void keepIf(const std::function<bool(const point2f &)> &_cond);

    explicit HwScript(HwController *_hwController = nullptr);

    HwScript(const HwScript &_hwScript) = default;

    HwScript(HwScript &&_hwScript) noexcept;

    HwScript &operator=(HwScript &&_hwScript) = default;

    HwScript &operator=(const HwScript &_hwScript) = default;

    void paintTo(Mat &_canvas) const override;

    std::optional<rectf> getBoundingBox() const override;

    void rotateBy(float _angle, const point2f &_cent) override;

    void moveBy(const point2f &_offset) override;

    void mulK(float _kx, float _ky) override;

    /**
     * cast all points from min-rect to rect with p1,p2 as opposite
    */
    void castToLine(const point2f &_p1, const point2f &_p2, const float _lThresh = 0.05);

    void castToCircle(const point2f &_center, float _rx, float _ry);

    /**
     * construct affine matrix by from and to, do transformation
     */
    void castBy(const std::vector<point2f> &_from, const std::vector<point2f> &_to);
};