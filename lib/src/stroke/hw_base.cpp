#include "stroke/hw_base.hpp"
#include "base/std_util.hpp"
#include "base/cocr_types.hpp"
#include "opencv_util/opencv_util.hpp"
#include <opencv2/imgproc.hpp>

const cv::Scalar &HwController::getColor() const {
    return color;
}

int HwController::getThickness() const {
    return thickness;
}

int HwController::getLineType() const {
    return lineType;
}

int HwController::getShift() const {
    return shift;
}

HwController::HwController(const int &_thickness)
        : color(0, 0, 0), lineType(cv::LINE_AA), thickness(_thickness), shift(0),
          revertColorProb(0.5) {
}

float HwController::getRevertColorProb() const {
    return revertColorProb;
}

std::optional<cv::Point2f> HwBase::getCenter() {
    auto bBox = getBoundingBox();
    if (!bBox) {
        return std::nullopt;
    }
    return getRectCenter2D(bBox.value());
}

void HwBase::moveCenterTo(const cv::Point2f &_newCenter) {
    auto oldCenter = getCenter();
    if (!oldCenter)return;
    auto offset = _newCenter - oldCenter.value();
    moveBy(offset);
}

void HwBase::moveLeftTopTo(const cv::Point2f &_leftTop) {
    auto bbox = getBoundingBox();
    if (!bbox)return;
    auto offset = _leftTop - bbox.value().tl();
    moveBy(offset);
}

void HwBase::resizeTo(float _w, float _h, bool _keepRatio) {
    auto bbox = getBoundingBox();
    if (!bbox)return;
    float kx = _w / bbox->width;
    float ky = _h / bbox->height;
    auto oldCenter = getRectCenter2D(bbox.value());
    moveLeftTopTo(cv::Point2f(0, 0));
    if (_keepRatio) {
        kx = ky = std::min(kx, ky);
    }
    mulK(kx, ky);
    moveCenterTo(oldCenter);
}

void HwBase::rotate(float _angle) {
    auto center = getCenter();
    if (!center)return;
    rotateBy(_angle, center.value());
}

bool HwBase::isDirectionKept() const {
    return keepDirection;
}

HwBase::HwBase() : keepDirection(false) {
}

void HwBase::setKeepDirection(bool _keepDirection) {
    keepDirection = _keepDirection;
}

DetectorClasses HwBase::getItemType() const {
    return DetectorClasses::ItemEmpty;
}

