#include "stroke/abs.h"

std::optional<point2f> HwBase::getCenter() {
    auto bBox = getBoundingBox();
    if (!bBox) {
        return std::nullopt;
    }
    return ::getCenter(bBox.value());
}

void HwBase::moveCenterTo(const point2f &_newCenter) {
    auto oldCenter = getCenter();
    if (!oldCenter)return;
    auto offset = _newCenter - oldCenter.value();
    moveBy(offset);
}

void HwBase::moveLeftTopTo(const point2f &_leftTop) {
    auto bbox = getBoundingBox();
    if (!bbox)return;
    auto offset = _leftTop - bbox.value().first;
    moveBy(offset);
}

void HwBase::resizeTo(float _w, float _h, bool _keepRatio) {
    auto bbox = getBoundingBox();
    if (!bbox)return;
    const auto[width, height] = getSize(bbox.value());
    float kx = _w / width;
    float ky = _h / height;
    auto oldCenter = ::getCenter(bbox.value());
    moveLeftTopTo({0.0, 0.0});
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
