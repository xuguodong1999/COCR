#include "stroke/stroke.h"

void HwStroke::paintTo(Mat &_canvas) const {
    if (mData.empty())return;
    if (mData.size() == 1) {
        _canvas.drawLine(mData[0], mData[0], hwController->getColor(), hwController->getThickness());
        return;
    }
    for (decltype(mData.size()) i = 1; i < mData.size(); i++) {
        _canvas.drawLine(mData[i - 1], mData[i], hwController->getColor(), hwController->getThickness());
    }
}

std::optional<rectf> HwStroke::getBoundingBox() const {
    if (mData.empty()) {
        return std::nullopt;
    }
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &pt: mData) {
        const auto&[x, y]=pt;
        minx = std::min(minx, x);
        miny = std::min(miny, y);
        maxx = std::max(maxx, x);
        maxy = std::max(maxy, y);
    }
    return rectf{{minx, miny},
                 {maxx, maxy}};
}

void HwStroke::rotateBy(float _angle, const point2f &_cent) {
    float theta = -M_PI * _angle / 180;
    float x0, y0;
    const auto&[cx, cy]=_cent;
    for (auto &pt: mData) {
        auto&[x, y]=pt;
        x0 = (x - cx) * std::cos(theta) + (y - cy) * std::sin(theta) + cx;
        y0 = (y - cy) * std::cos(theta) - (x - cx) * std::sin(theta) + cy;
        x = x0;
        y = y0;
    }
}


void HwStroke::moveBy(const point2f &_offset) {
    for (auto &pt: mData) pt += _offset;
}


void HwStroke::mulK(float _kx, float _ky) {
    for (auto &pt: mData) {
        auto&[x, y]=pt;
        x *= _kx;
        y *= _ky;
    }
}


HwStroke::HwStroke(HwController *_hwController) : hwController(_hwController) {
    if (nullptr == _hwController) {
        hwController = &(HwBase::baseController);
    }
}

void HwStroke::push_back(const point2f &_pt) {
    mData.push_back(_pt);
}

void HwStroke::keepIf(const std::function<bool(const point2f &)> &_cond) {
    decltype(mData) newData;
    for (auto &pt: mData) {
        if (_cond(pt)) {
            newData.push_back(pt);
        }
    }
    mData.swap(newData);
}

void HwStroke::setHwController(HwController &_hwController) {
    HwStroke::hwController = &_hwController;
}

decltype(HwStroke::mData.begin()) HwStroke::begin() {
    return mData.begin();
}

decltype(HwStroke::mData.end()) HwStroke::end() {
    return mData.end();
}

void HwStroke::setData(std::vector<point2f> &_data) {
    mData.swap(_data);
    _data.clear();
}

size_t HwStroke::size() const {
    return mData.size();
}

std::shared_ptr<HwBase> HwStroke::clone() const {
    auto brother = std::make_shared<HwStroke>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}
