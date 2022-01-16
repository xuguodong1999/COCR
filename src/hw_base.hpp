#ifndef _HW_BASE_HPP_
#define _HW_BASE_HPP_

#include "cocr_types.hpp"
#include <opencv2/core/mat.hpp>
#include <optional>

class HwController {
    cv::Scalar color;
public:
    const cv::Scalar &getColor() const;

    int getThickness() const;

    int getLineType() const;

    int getShift() const;

    float getRevertColorProb() const;

private:
    int thickness, lineType, shift;
    float revertColorProb;
public:
    HwController(const int &_thickness = 1);
};

class HwBase {
    bool keepDirection;
public:
    HwBase();

    virtual DetectorClasses getItemType() const;

    virtual bool isDirectionKept() const;

    void setKeepDirection(bool _keepDirection = true);

    inline static HwController baseController;

    virtual void setHwController(HwController &_hwController) = 0;

    virtual void paintTo(cv::Mat &_canvas) const = 0;

    virtual std::optional<cv::Rect2f> getBoundingBox() const = 0;

    virtual void rotate(float _angle);

    virtual void rotateBy(float _angle, const cv::Point2f &_cent) = 0;

    virtual std::optional<cv::Point2f> getCenter();

    virtual void moveBy(const cv::Point2f &_offset) = 0;

    virtual void moveCenterTo(const cv::Point2f &_newCenter);

    virtual void moveLeftTopTo(const cv::Point2f &_leftTop);

    virtual void resizeTo(float _w, float _h, bool _keepRatio = true);

    virtual void mulK(float _kx, float _ky) = 0;
};

#endif
