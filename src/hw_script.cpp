#include "hw_script.hpp"
#include "std_util.hpp"
#include "opencv_util.hpp"
#include <opencv2/imgproc.hpp>

void HwScript::push_back(HwStroke &_stroke) {
    mData.push_back(std::move(_stroke));
}

void HwScript::keepIf(const std::function<bool(const cv::Point2f &)> &_cond) {
    for (auto &stroke:mData) {
        stroke.keepIf(_cond);
    }
}

HwScript::HwScript(HwController *_hwController) : hwController(_hwController) {
    if (nullptr == _hwController) {
        hwController = &(HwBase::baseController);
    }
}

void HwScript::paintTo(cv::Mat &_canvas) const {
    for (auto &script:mData) {
        script.paintTo(_canvas);
    }
}

std::optional<cv::Rect2f> HwScript::getBoundingBox() const {
    if (mData.empty())return std::nullopt;
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &stroke:mData) {
        auto bbox = stroke.getBoundingBox();
        if (!bbox)continue;
        minx = std::min(minx, bbox->x);
        miny = std::min(miny, bbox->y);
        maxx = std::max(maxx, bbox->x + bbox->width);
        maxy = std::max(maxy, bbox->y + bbox->height);
    }
    if (minx > maxx)return std::nullopt;
    return cv::Rect2f(minx, miny, maxx - minx, maxy - miny);
}

void HwScript::rotateBy(float _angle, const cv::Point2f &_cent) {
    for (auto &stroke:mData) {
        stroke.rotateBy(_angle, _cent);
    }
}

void HwScript::moveBy(const cv::Point2f &_offset) {
    for (auto &stroke:mData) {
        stroke.moveBy(_offset);
    }
}

void HwScript::mulK(float _kx, float _ky) {
    for (auto &stroke:mData) {
        stroke.mulK(_kx, _ky);
    }
}

void HwScript::setHwController(HwController &_hwController) {
    hwController = &_hwController;
    for (auto &script:mData) {
        script.setHwController(*hwController);
    }
}

void HwScript::castBy(const std::vector<cv::Point2f> &_from, const std::vector<cv::Point2f> &_to) {
    auto affine = cv::getAffineTransform(_from.data(), _to.data());
    for (auto &stroke:mData) {
        for (auto &p:stroke) {
            cv::Mat pm = affine * cv::Mat({static_cast<double>(p.x),
                                           static_cast<double>(p.y), 1.});
            p.x = pm.at<double>(0, 0);
            p.y = pm.at<double>(1, 0);
        }
    }
}

void HwScript::clear() {
    mData.clear();
}

float HwScript::getAvgMaxSize() const {
    if (mData.empty())return 0;
    float mmSize = 0;
    size_t counter = 0;
    for (auto &stroke:mData) {
        auto rect = stroke.getBoundingBox();
        if (rect) {
            counter++;
            mmSize += std::max(rect->width, rect->height);
        }
    }
    if (counter) {
        return mmSize / counter;
    } else {
        return 0;
    }
}

float HwScript::getAvgPtsNum() const {
    if (mData.empty())return 0;
    float mmSize = 0;
    for (auto &stroke:mData) {
        mmSize += stroke.size();
    }
    return mmSize / mData.size();
}

HwScript::HwScript(HwScript &&_hwScript) {
    mData = std::move(_hwScript.mData);
}

size_t HwScript::size() const {
    return mData.size();
}
