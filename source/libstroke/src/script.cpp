#include "stroke/script.h"
#include "base/std_util.h"
#include "stroke/couch_sym.h"
#include "ocv/algorithm.h"

void HwScript::push_back(HwStroke &_stroke) {
    mData.push_back(std::move(_stroke));
}

void HwScript::keepIf(const std::function<bool(const point2f &)> &_cond) {
    for (auto &stroke: mData) {
        stroke.keepIf(_cond);
    }
}

HwScript::HwScript(HwController *_hwController) : hwController(_hwController) {
    if (nullptr == _hwController) {
        hwController = &(HwBase::baseController);
    }
}

void HwScript::paintTo(Mat &_canvas) const {
    for (auto &script: mData) {
        script.paintTo(_canvas);
    }
}

std::optional<rectf> HwScript::getBoundingBox() const {
    if (mData.empty())return std::nullopt;
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &stroke: mData) {
        auto bbox = stroke.getBoundingBox();
        if (!bbox)continue;
        const auto&[lt, br]=bbox.value();
        const auto&[x0, y0]=lt;
        const auto&[x1, y1]=br;
        minx = std::min(minx, x0);
        miny = std::min(miny, y0);
        maxx = std::max(maxx, x1);
        maxy = std::max(maxy, y1);
    }
    if (minx > maxx) {
        return std::nullopt;
    }
    return rectf{{minx, miny},
                 {maxx, maxy}};
}

void HwScript::rotateBy(float _angle, const point2f &_cent) {
    for (auto &stroke: mData) {
        stroke.rotateBy(_angle, _cent);
    }
}

void HwScript::moveBy(const point2f &_offset) {
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

void HwScript::castToLine(const point2f &_p1, const point2f &_p2, const float _lThresh) {
    const float slideOffset = 0.05;
    // FIXME: 分离随机因素
    L:;
    float kLen1 = StdUtil::minMaxProb(slideOffset);
    float kLen2 = StdUtil::minMaxProb(slideOffset);
    point2f p1 = _p1 + (_p1 - _p2) * kLen1, p2 = _p2 + (_p2 - _p1) * kLen2;
//    Point p1=pp1,p2=pp2;
    float lenPP = getDistance(p1, p2);
    auto&[x1, y1]=p1;
    auto&[x2, y2]=p2;
    x1 += lenPP * StdUtil::minMaxProb(slideOffset);
    x2 += lenPP * StdUtil::minMaxProb(slideOffset);
    y1 += lenPP * StdUtil::minMaxProb(slideOffset);
    y2 += lenPP * StdUtil::minMaxProb(slideOffset);
    std::vector<point2f> from, to, input;
    for (auto &stroke: mData) {
        std::copy(stroke.begin(), stroke.end(),
                  std::inserter(input, input.end()));
    }
    from = CvUtil::GetMinAreaRect(input);
    auto s03 = getDistance(from[0], from[3]);
    auto s01 = getDistance(from[0], from[1]);
    while (s03 < std::numeric_limits<float>::epsilon()
           || s01 < std::numeric_limits<float>::epsilon()) {
        auto &dataLoader = HwDataLoader::getInstance();
        mData = std::move(dataLoader.GetShape(ShapeType::Line).mData);
        goto L;
    }
    point2f vec = p1 - p2, vecT({0, 1});
    if (fabs(vec.second) > std::numeric_limits<float>::epsilon()) {
        vecT = point2f(-1, vec.first / vec.second);
        vecT /= ::norm(vecT);
    }
    float kk = s03 / s01;
    while (_lThresh < kk && kk < 1 / _lThresh) {
        auto &dataLoader = HwDataLoader::getInstance();
        mData = std::move(dataLoader.GetShape(ShapeType::Line).mData);
        goto L;
    }
    if (s03 > s01) {    // 长边是03、12，给【短长】
        auto offset = vecT * ::norm(vec) / s03 * s01 / 2.0;
        to = {p1 - offset, p1 + offset, p2 + offset, p2 - offset};
    } else {            // 长边是01、32，给【长短】
        auto offset = vecT * ::norm(vec) / s01 * s03;
        to = {p1 - offset, p2 - offset, p2 + offset, p1 + offset};
    }
    castBy(from, to);
}

void HwScript::castToCircle(const point2f &_center, float _rx, float _ry) {

    std::vector<point2f> input;
    for (auto &stroke: mData) {
        std::copy(stroke.begin(), stroke.end(),
                  std::inserter(input, input.end()));
    }
    std::vector<point2f> from = CvUtil::GetMinAreaRect(input);
    std::vector<point2f> to{ // just a square box
            point2f(0, 0), point2f(0, 100),
            point2f(100, 100), point2f(100, 0)
    };
    castBy(from, to);
    resizeTo(_rx * 2, _ry * 2, false);
    moveCenterTo(_center);
}

void HwScript::castBy(const std::vector<point2f> &_from, const std::vector<point2f> &_to) {
    AffineHelper helper;
    helper.calc(_from, _to);
    for (auto &stroke: mData) {
        for (auto &p: stroke) {
            p = helper.transform(p);
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
            const auto[width, height]=::getSize(rect.value());
            mmSize += std::max(width, height);
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

