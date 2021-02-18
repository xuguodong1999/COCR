#include "hw_stroke.hpp"
#include "opencv_util.hpp"
#include "std_util.hpp"
#include <opencv2/imgproc.hpp>

void HwStroke::paintTo(cv::Mat &_canvas) const {
    if (mData.empty())return;
    if (mData.size() == 1) {
        cv::line(_canvas, mData[0], mData[0],
                 hwController->getColor(),
                 hwController->getThickness(),
                 hwController->getLineType(),
                 hwController->getShift());
        return;
    }
    for (decltype(mData.size()) i = 1; i < mData.size(); ++i) {
        cv::line(_canvas, mData[i - 1], mData[i],
                 hwController->getColor(),
                 hwController->getThickness(),
                 hwController->getLineType(),
                 hwController->getShift());
    }
}

std::optional<cv::Rect2f> HwStroke::getBoundingBox() const {
    if (mData.empty()) {
        return std::nullopt;
    }
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &pt:mData) {
        minx = std::min(minx, pt.x);
        miny = std::min(miny, pt.y);
        maxx = std::max(maxx, pt.x);
        maxy = std::max(maxy, pt.y);
    }
    return cv::Rect2f(minx, miny, maxx - minx, maxy - miny);
}

void HwStroke::rotateBy(float _angle, const cv::Point2f &_cent) {
    float theta = -M_PI * _angle / 180;
    float x, y;
    for (auto &pt:mData) {
        x = (pt.x - _cent.x) * std::cos(theta) + (pt.y - _cent.y) * std::sin(theta) + _cent.x;
        y = (pt.y - _cent.y) * std::cos(theta) - (pt.x - _cent.x) * std::sin(theta) + _cent.y;
        pt.x = x;
        pt.y = y;
    }
}


void HwStroke::moveBy(const cv::Point2f &_offset) {
    for (auto &pt:mData) pt += _offset;
}


void HwStroke::mulK(float _kx, float _ky) {
    for (auto &pt:mData) {
        pt.x *= _kx;
        pt.y *= _ky;
    }
}


HwStroke::HwStroke(HwController *_hwController) : hwController(_hwController) {
    if (nullptr == _hwController) {
        hwController = &(HwBase::baseController);
    }
}

void HwStroke::push_back(const cv::Point2f &_pt) {
    mData.push_back(_pt);
}

void HwStroke::keepIf(const std::function<bool(const cv::Point2f &)> &_cond) {
    decltype(mData) newData;
    for (auto &pt:mData) {
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

void HwStroke::setData(std::vector<cv::Point2f> &_data) {
    mData.swap(_data);
    _data.clear();
}

size_t HwStroke::size() const {
    return mData.size();
}
