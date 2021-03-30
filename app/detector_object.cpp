#include "detector_object.hpp"

bool xgd::DetectorObject::isValidLabel(const int &_label) {
    return minLabel <= _label && _label <= maxLabel;
}

const cv::Rect2f &xgd::DetectorObject::asRect() const {
    return rect;
}

const float &xgd::DetectorObject::x() const {
    return rect.x;
}

const float &xgd::DetectorObject::y() const {
    return rect.y;
}

const float &xgd::DetectorObject::w() const {
    return rect.width;
}

const float &xgd::DetectorObject::h() const {
    return rect.height;
}

