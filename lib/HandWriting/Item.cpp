#include "XGD/HandWriting/Item.h"

void HwItem::setHwController(HwController &_hwController) {
    hwController = &_hwController;
    for (auto &script: mData) {
        script.setHwController(*hwController);
    }
}

void HwItem::push_back(HwScript &_script) {
    mData.push_back(std::move(_script));
}

void HwItem::push_back(HwScript &&_script) {
    mData.push_back(_script);
}

void HwItem::keepIf(const std::function<bool(const point2f &)> &_cond) {
    for (auto &script: mData) {
        script.keepIf(_cond);
    }
}

HwItem::HwItem(HwController *_hwController) : hwController(_hwController) {
    if (nullptr == _hwController) {
        hwController = &HwBase::baseController;
    }
}

void HwItem::paintTo(Mat &_canvas) const {
    for (auto &script: mData) {
        script.paintTo(_canvas);
    }
}

std::optional<rectf> HwItem::getBoundingBox() const {
    if (mData.empty())return std::nullopt;
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &script: mData) {
        auto bbox = script.getBoundingBox();
        if (!bbox)continue;
        const auto &[lt, br] = bbox.value();
        const auto &[x0, y0] = lt;
        const auto &[x1, y1] = br;
        minx = std::min(minx, x0);
        miny = std::min(miny, y0);
        maxx = std::max(maxx, x1);
        maxy = std::max(maxy, y1);
    }
    if (minx > maxx) {
        return std::nullopt;
    }
    return rectf({minx, miny}, {maxx, maxy});
}

void HwItem::rotateBy(float _angle, const point2f &_cent) {
    for (auto &script: mData) {
        script.rotateBy(_angle, _cent);
    }
}

void HwItem::moveBy(const point2f &_offset) {
    for (auto &script: mData) {
        script.moveBy(_offset);
    }
}

void HwItem::mulK(float _kx, float _ky) {
    for (auto &script: mData) {
        script.mulK(_kx, _ky);
    }
}

HwScript HwItem::asScript() const {
    HwScript hwScript;
    for (auto &script: mData) {
        hwScript.append(script);
    }
    return std::move(hwScript);
}

std::shared_ptr<HwBase> HwItem::clone() const {
    auto brother = std::make_shared<HwItem>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}
