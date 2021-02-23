#include "hw_bond.hpp"
#include "opencv_util.hpp"
#include "std_util.hpp"
#include "hw_data.hpp"

#include <memory>
#include <iostream>
#include <opencv2/imgproc.hpp>

std::shared_ptr<HwBond> HwBond::GetHwBond(const JBondType &_bondType) {
    switch (_bondType) {
        case JBondType::SolidWedgeBond:
            return std::make_shared<HwSolidWedgeBond>();
        case JBondType::DashWedgeBond:
            return std::make_shared<HwDashWedgeBond>();
        case JBondType::WaveBond:
            return std::make_shared<HwWaveBond>();
        case JBondType::SingleBond :
        case JBondType::DelocalizedBond:// 离域键只画骨架，环另画
            return std::make_shared<HwSingleBond>();
        case JBondType::DoubleBond:
            return std::make_shared<HwDoubleBond>();
        case JBondType::TripleBond:
            return std::make_shared<HwTripleBond>();
        case JBondType::CircleBond:
            return std::make_shared<HwCircleBond>();
        default: {
            std::cerr << "Bond::GetBond: unknown bond type "
                      << static_cast<int>(_bondType) << std::endl;
            exit(-1);
        }
    }
}

void HwBond::setUseHwCharProb(float useHwCharProb) {
    HwBond::useHwCharProb = useHwCharProb;
}


void HwCircleBond::setVertices(const std::vector<cv::Point2f> &_pts) {
    if (_pts.size() < 2) {
        std::cerr << "圆键的pts约定为多边形顶点,but "
                  << _pts.size() << " pts got." << std::endl;
        exit(-1);
    }
    center.x = center.y = 0;
    for (auto &pt:_pts)
        center += pt;
    center.x /= _pts.size();
    center.y /= _pts.size();
    rx = 0;
    for (auto &pt:_pts)
        rx += getDistance2D(center, pt);
    rx /= _pts.size();
    // TODO: 分离随即要素
    const float minLen = 0.2;
    rx *= (minLen + 0.5 * belowProb(std::sqrt(3) / 2 - minLen));
    ry = rx;
    loadHwData();
}

void HwCircleBond::rotateBy(float _angle, const cv::Point2f &_cent) {
    HwItem::rotateBy(_angle, _cent);
    float theta = -M_PI * _angle / 180;
    float x = (center.x - _cent.x) * std::cos(theta) +
              (center.y - _cent.y) * std::sin(theta) + _cent.x;
    float y = (center.y - _cent.y) * std::cos(theta) -
              (center.x - _cent.x) * std::sin(theta) + _cent.y;
    center.x = x;
    center.y = y;
}

void HwCircleBond::moveBy(const cv::Point2f &_offset) {
    HwItem::moveBy(_offset);
    center += _offset;
}

void HwCircleBond::mulK(float _kx, float _ky) {
    rx *= _kx;
    ry *= _ky;
    HwItem::mulK(_kx, _ky);
}

void HwCircleBond::loadHwData() {
    auto &dataLoader = HwDataLoader::getInstance();
    auto circle = dataLoader.GetShape(ShapeType::Circle);
    circle.castToCircle(center, rx, ry);
    push_back(circle);
}

DetectorClasses HwCircleBond::getItemType() const {
    return DetectorClasses::ItemCircleBond;
}

std::shared_ptr<HwBase> HwCircleBond::clone() const {
    auto brother = std::make_shared<HwCircleBond>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    brother->rx = rx;
    brother->ry = ry;
    brother->center = center;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}

void HwSingleBond::loadHwData() {
    auto &dataLoader = HwDataLoader::getInstance();
    auto line = dataLoader.GetShape(ShapeType::Line);
    line.castToLine(from, to);
    push_back(line);
}

void HwSingleBond::setVertices(const std::vector<cv::Point2f> &_pts) {
    if (_pts.size() != 2) {
        std::cerr << "单双三楔波浪线的pts约定为起点和终点,but "
                  << _pts.size() << " pts got." << std::endl;
        exit(-1);
    }
    from = _pts[0];
    to = _pts[1];
    loadHwData();
}

void HwSingleBond::rotateBy(float _angle, const cv::Point2f &_cent) {
    HwItem::rotateBy(_angle, _cent);
    float theta = -M_PI * _angle / 180;
    float x, y;
    x = (from.x - _cent.x) * std::cos(theta) + (from.y - _cent.y) * std::sin(theta) + _cent.x;
    y = (from.y - _cent.y) * std::cos(theta) - (from.x - _cent.x) * std::sin(theta) + _cent.y;
    from.x = x;
    from.y = y;
    x = (to.x - _cent.x) * std::cos(theta) + (to.y - _cent.y) * std::sin(theta) + _cent.x;
    y = (to.y - _cent.y) * std::cos(theta) - (to.x - _cent.x) * std::sin(theta) + _cent.y;
    to.x = x;
    to.y = y;
}

void HwSingleBond::moveBy(const cv::Point2f &_offset) {
    HwItem::moveBy(_offset);
    from += _offset;
    to += _offset;
}

void HwSingleBond::mulK(float _kx, float _ky) {
    HwItem::mulK(_kx, _ky);
    from.x *= _kx;
    from.y *= _ky;
    to.x *= _kx;
    to.y *= _ky;
}

DetectorClasses HwSingleBond::getItemType() const {
    return DetectorClasses::ItemSingleBond;
}

std::shared_ptr<HwBase> HwSingleBond::clone() const {
    auto brother = std::make_shared<HwSingleBond>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    brother->from = from;
    brother->to = to;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}

void HwDoubleBond::loadHwData() {
    //FIXME: 设置间隔，这里和 MolItem::reloadHWData 有冲突
    const float intervalK = 0.2;
    cv::Point2f vec = from - to;
    cv::Point2f vecT(0, 1);
    if (vec.y != 0) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    auto length = getDistance2D(from, to);
    auto offset = vecT * length * intervalK;
    auto &dataLoader = HwDataLoader::getInstance();
    auto line1 = dataLoader.GetShape(ShapeType::Line);
    line1.castToLine(from + offset / 2, to + offset / 2);
    auto line2 = dataLoader.GetShape(ShapeType::Line);
    line2.castToLine(from - offset / 2, to - offset / 2);
    push_back(line1);
    push_back(line2);
}

DetectorClasses HwDoubleBond::getItemType() const {
    return DetectorClasses::ItemDoubleBond;
}

std::shared_ptr<HwBase> HwDoubleBond::clone() const {
    auto brother = std::make_shared<HwDoubleBond>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    brother->from = from;
    brother->to = to;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}

void HwTripleBond::loadHwData() {
    // TODO: 分离随机变量
    const float intervalK = 0.4;
    cv::Point2f vec = from - to;
    cv::Point2f vecT(0, 1);
    if (vec.y != 0) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    auto length = getDistance2D(from, to);
    auto offset = vecT * length * intervalK;
    auto &dataLoader = HwDataLoader::getInstance();
    auto line1 = dataLoader.GetShape(ShapeType::Line);
    line1.castToLine(from + offset / 2, to + offset / 2);
    auto line2 = dataLoader.GetShape(ShapeType::Line);
    line2.castToLine(from, to);
    auto line3 = dataLoader.GetShape(ShapeType::Line);
    line3.castToLine(from - offset / 2, to - offset / 2);
    push_back(line1);
    push_back(line2);
    push_back(line3);
}

DetectorClasses HwTripleBond::getItemType() const {
    return DetectorClasses::ItemTripleBond;
}

std::shared_ptr<HwBase> HwTripleBond::clone() const {
    auto brother = std::make_shared<HwTripleBond>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    brother->from = from;
    brother->to = to;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}

void HwSolidWedgeBond::loadHwData() {
    const float intervalK = 0.5;
    const int numOfSplit = 10;
    cv::Point2f vec = from - to;
    auto length = getDistance2D(from, to);
    cv::Point2f vecT(0, 1);
    if (vec.y != 0) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    auto offset = vecT * length * intervalK;
    auto to1 = to + offset / 2, to2 = to - offset / 2;
    // to1 到 to2 取若干个点，如 t，连 t 和 from
    float dx = to2.x - to1.x, dy = to2.y - to1.y;
    dx /= numOfSplit;
    dy /= numOfSplit;
    float x = to1.x, y = to1.y;
    auto &dataLoader = HwDataLoader::getInstance();
    for (float i = 0; i <= 1.0; i += 1.0 / numOfSplit) {
        auto line = dataLoader.GetShape(ShapeType::Line);
        line.castToLine(from, cv::Point2f(x, y));
        push_back(line);
        x += dx;
        y += dy;
    }
}

DetectorClasses HwSolidWedgeBond::getItemType() const {
    return DetectorClasses::ItemSolidWedgeBond;
}

std::shared_ptr<HwBase> HwSolidWedgeBond::clone() const {
    auto brother = std::make_shared<HwSolidWedgeBond>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    brother->from = from;
    brother->to = to;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}

void HwDashWedgeBond::loadHwData() {
    const float intervalK = 0.5;
    const int numOfSplit = 4 + randInt() % 3; // default: 5
    cv::Point2f vec = from - to;
    auto length = getDistance2D(from, to);
    cv::Point2f vecT(0, 1);
    if (vec.y != 0) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    auto offset = vecT * length * intervalK;
    auto to1 = to + offset / 2, to2 = to - offset / 2;
    // _ to1
    // from--< from-to1,from-to2采样
    // \__to2
    float d[4] = {
            -from.x + to1.x,
            -from.y + to1.y,
            -from.x + to2.x,
            -from.y + to2.y
    };
    for (int i = 0; i < 4; i++)d[i] /= numOfSplit;
    float x1, y1, x2, y2;
    x1 = x2 = from.x;
    y1 = y2 = from.y;
    float angle = minMaxProb(60);
    auto &dataLoader = HwDataLoader::getInstance();
    for (float i = 0; i <= 1.0; i += 1.0 / numOfSplit) {
        auto line = dataLoader.GetShape(ShapeType::Line);
        line.castToLine(cv::Point2f(x1, y1), cv::Point2f(x2, y2));
        line.rotate(angle);
        push_back(line);
        x1 += d[0];
        y1 += d[1];
        x2 += d[2];
        y2 += d[3];
    }
}

DetectorClasses HwDashWedgeBond::getItemType() const {
    return DetectorClasses::ItemDashWedgeBond;
}

std::shared_ptr<HwBase> HwDashWedgeBond::clone() const {
    auto brother = std::make_shared<HwDashWedgeBond>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    brother->from = from;
    brother->to = to;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}

void HwWaveBond::loadHwData() {
    float cyclesOfSin = belowProb(2) + 4;
    float end = M_PI * 2 * cyclesOfSin;
    HwStroke stroke;
    // TODO: 这里存在一个随机要素
    for (float start = belowProb(1); start < end; start += 0.0520603) {
        stroke.push_back(cv::Point2f(start, 2 * std::sin(start)));
    }
    HwScript script;
    script.push_back(stroke);
    script.castToLine(from, to, 1);
    push_back(script);
}

DetectorClasses HwWaveBond::getItemType() const {
    return DetectorClasses::ItemWaveBond;
}

std::shared_ptr<HwBase> HwWaveBond::clone() const {
    auto brother = std::make_shared<HwWaveBond>();
    brother->keepDirection = keepDirection;
    brother->mData = mData;
    brother->from = from;
    brother->to = to;
    if (hwController)
        brother->setHwController(*hwController);
    return brother;
}


void HwCircleBond::paintTo(cv::Mat &_canvas) const {
    if (byProb(useHwCharProb))
        HwBond::paintTo(_canvas);
    else {
        auto rf = getBoundingBox().value();
        cv::RotatedRect rect(getRectCenter2D(rf), cv::Size(rf.width, rf.height), 0);
        cv::ellipse(_canvas, rect,
                    hwController->getColor(),
                    hwController->getThickness(),
                    hwController->getLineType());
    }
}


void HwSolidWedgeBond::paintTo(cv::Mat &_canvas) const {
    if (byProb(useHwCharProb))
        HwBond::paintTo(_canvas);
    else {
        auto paint_wedge_template = [&](
                const cv::Rect2f &_rect, const int &_direction, const float &_k = 2) {
            int _w = std::round(_rect.width), _h = std::round(_rect.height);
            std::vector<cv::Point2i> pts(3);
            const float up = _k / (_k + 1);
            const float down = 1 - up;
            switch (_direction) {
                case 0:
                    pts[0] = {0, 0};
                    pts[1] = {_w - 1, (int) (_h * up)};
                    pts[2] = {(int) (_w * up), _h - 1};
                    break;
                case 1:
                    pts[0] = {_w - 1, 0};
                    pts[1] = {0, (int) (_h * up)};
                    pts[2] = {(int) (_w * down), _h - 1};
                    break;
                case 2:
                    pts[0] = {0, _h - 1};
                    pts[1] = {_w - 1, (int) (_h * down)};
                    pts[2] = {(int) (_w * up), 0};
                    break;
                case 3:
                    pts[0] = {_w - 1, _h - 1};
                    pts[1] = {0, (int) (_h * down)};
                    pts[2] = {(int) (_w * down), 0};
                    break;
            }
            for (auto &pt:pts)pt += cv::Point(_rect.tl());
            cv::fillPoly(_canvas, pts, hwController->getColor(), hwController->getLineType(), hwController->getShift());
        };
        auto rect = getBoundingBox().value();
        std::vector<std::pair<float, int>> dir = {
                {getDistance2D(rect.tl(), from),                                 0},
                {getDistance2D(cv::Point2f(rect.x + rect.width, rect.y), from),  1},
                {getDistance2D(cv::Point2f(rect.x, rect.y + rect.height), from), 2},
                {getDistance2D(rect.br(), from),                                 3}
        };
        std::sort(dir.begin(), dir.end(), [](const std::pair<float, int> &_a, const std::pair<float, int> &_b) {
            return _a.first < _b.first;
        });
        paint_wedge_template(rect, dir[0].second, betweenProb(1, 6));
    }
}
