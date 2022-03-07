#include "detector_object.hpp"

bool cocr::DetectorObject::isValidLabel(const int &_label) {
    return minLabel <= _label && _label <= maxLabel;
}

const cv::Rect2f &cocr::DetectorObject::asRect() const {
    return rect;
}

const float &cocr::DetectorObject::x() const {
    return rect.x;
}

const float &cocr::DetectorObject::y() const {
    return rect.y;
}

const float &cocr::DetectorObject::w() const {
    return rect.width;
}

const float &cocr::DetectorObject::h() const {
    return rect.height;
}

