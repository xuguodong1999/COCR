#include "hw_item.hpp"

void HwItem::setHwController(HwController &_hwController) {
    hwController = &_hwController;
    for (auto &script:mData) {
        script.setHwController(_hwController);
    }
}

void HwItem::push_back(HwScript &_script) {
    mData.push_back(std::move(_script));
}

void HwItem::push_back(HwScript &&_script) {
    mData.push_back(_script);
}

void HwItem::keepIf(const std::function<bool(const cv::Point2f &)> &_cond) {
    for (auto &script:mData) {
        script.keepIf(_cond);
    }
}

HwItem::HwItem(HwController *_hwController) : hwController(_hwController) {
    if (nullptr == _hwController) {
        hwController = &HwBase::baseController;
    }
}

void HwItem::paintTo(cv::Mat &_canvas) const {
    for (auto &script:mData) {
        script.paintTo(_canvas);
    }
}

std::optional<cv::Rect2f> HwItem::getBoundingBox() const {
    if (mData.empty())return std::nullopt;
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &script:mData) {
        auto bbox = script.getBoundingBox();
        if (!bbox)continue;
        minx = std::min(minx, bbox->x);
        miny = std::min(miny, bbox->y);
        maxx = std::max(maxx, bbox->x + bbox->width);
        maxy = std::max(maxy, bbox->y + bbox->height);
    }
    if (minx > maxx)return std::nullopt;
    return cv::Rect2f(minx, miny, maxx - minx, maxy - miny);
}

void HwItem::rotateBy(float _angle, const cv::Point2f &_cent) {
    for (auto &script:mData) {
        script.rotateBy(_angle, _cent);
    }
}

void HwItem::moveBy(const cv::Point2f &_offset) {
    for (auto &script:mData) {
        script.moveBy(_offset);
    }
}

void HwItem::mulK(float _kx, float _ky) {
    for (auto &script:mData) {
        script.mulK(_kx, _ky);
    }
}
