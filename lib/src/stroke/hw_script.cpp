#include "stroke/hw_script.hpp"
#include "base/std_util.hpp"
#include "opencv_util/opencv_util.hpp"
#include "stroke/hw_data.hpp"
#include <opencv2/imgproc.hpp>

void HwScript::push_back(HwStroke &_stroke) {
    mData.push_back(std::move(_stroke));
}

void HwScript::keepIf(const std::function<bool(const cv::Point2f &)> &_cond) {
    for (auto &stroke: mData) {
        stroke.keepIf(_cond);
    }
}

HwScript::HwScript(HwController *_hwController) : hwController(_hwController) {
    if (nullptr == _hwController) {
        hwController = &(HwBase::baseController);
    }
}

void HwScript::paintTo(cv::Mat &_canvas) const {
    for (auto &script: mData) {
        script.paintTo(_canvas);
    }
}

std::optional<cv::Rect2f> HwScript::getBoundingBox() const {
    if (mData.empty())return std::nullopt;
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &stroke: mData) {
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
    for (auto &stroke: mData) {
        stroke.rotateBy(_angle, _cent);
    }
}

void HwScript::moveBy(const cv::Point2f &_offset) {
    for (auto &stroke: mData) {
        stroke.moveBy(_offset);
    }
}

void HwScript::mulK(float _kx, float _ky) {
    for (auto &stroke: mData) {
        stroke.mulK(_kx, _ky);
    }
}

void HwScript::setHwController(HwController &_hwController) {
    hwController = &_hwController;
    for (auto &script: mData) {
        script.setHwController(*hwController);
    }
}

void HwScript::castToLine(const cv::Point2f &_p1, const cv::Point2f &_p2, const float _lThresh) {
    const float slideOffset = 0.05;
    // FIXME: 分离随机因素
    L:;
    float kLen1 = minMaxProb(slideOffset);
    float kLen2 = minMaxProb(slideOffset);
    cv::Point2f p1 = _p1 + (_p1 - _p2) * kLen1, p2 = _p2 + (_p2 - _p1) * kLen2;
//    Point p1=pp1,p2=pp2;
    float lenPP = getDistance2D(p1, p2);
    p1.x += lenPP * minMaxProb(slideOffset);
    p2.x += lenPP * minMaxProb(slideOffset);
    p1.y += lenPP * minMaxProb(slideOffset);
    p2.y += lenPP * minMaxProb(slideOffset);
    std::vector<cv::Point2f> from(4), to, input;
    for (auto &stroke: mData) {
        std::copy(stroke.begin(), stroke.end(),
                  std::inserter(input, input.end()));
    }
    cv::minAreaRect(input).points(from.data());
    auto s03 = getDistance2D(from[0], from[3]);
    auto s01 = getDistance2D(from[0], from[1]);
    while (s03 < std::numeric_limits<float>::epsilon()
           || s01 < std::numeric_limits<float>::epsilon()) {
        auto &dataLoader = HwDataLoader::getInstance();
        mData = std::move(dataLoader.GetShape(ShapeType::Line).mData);
        goto L;
    }
    cv::Point2f vec = p1 - p2, vecT(0, 1);
    if (fabs(vec.y) > std::numeric_limits<float>::epsilon()) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    float kk = s03 / s01;
    while (_lThresh < kk && kk < 1 / _lThresh) {
        auto &dataLoader = HwDataLoader::getInstance();
        mData = std::move(dataLoader.GetShape(ShapeType::Line).mData);
        goto L;
    }
    if (s03 > s01) {    // 长边是03、12，给【短长】
        auto offset = vecT * cv::norm(vec) / s03 * s01 / 2.0;
        to = {p1 - offset, p1 + offset, p2 + offset, p2 - offset};
    } else {            // 长边是01、32，给【长短】
        auto offset = vecT * cv::norm(vec) / s01 * s03;
        to = {p1 - offset, p2 - offset, p2 + offset, p1 + offset};
    }
    castBy(from, to);
}

void HwScript::castToCircle(const cv::Point2f &_center, float _rx, float _ry) {
    std::vector<cv::Point2f> from(4), to = {// just a square box
            cv::Point2f(0, 0), cv::Point2f(0, 100),
            cv::Point2f(100, 100), cv::Point2f(100, 0)
    };
    std::vector<cv::Point2f> input;
    for (auto &stroke: mData) {
        std::copy(stroke.begin(), stroke.end(),
                  std::inserter(input, input.end()));
    }
    cv::minAreaRect(input).points(from.data());
    castBy(from, to);
    resizeTo(_rx * 2, _ry * 2, false);
    moveCenterTo(_center);
}

void HwScript::castBy(const std::vector<cv::Point2f> &_from, const std::vector<cv::Point2f> &_to) {
    // 2x3 仿射变换矩阵
    auto affine = cv::getAffineTransform(_from.data(), _to.data());
    for (auto &stroke: mData) {
        for (auto &p: stroke) {
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
    for (auto &stroke: mData) {
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
    for (auto &stroke: mData) {
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

void HwScript::append(const HwScript &_hwScript) {
    for (auto &stroke: _hwScript.mData) {
        mData.push_back(stroke);
    }
}

std::shared_ptr<HwBase> HwScript::clone() const {
    auto sister = std::make_shared<HwScript>();
    sister->keepDirection = keepDirection;
    sister->mData = mData;
    if (hwController)
        sister->setHwController(*hwController);
    return sister;
}

