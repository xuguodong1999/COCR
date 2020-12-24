/**
 * FIXME: This file is shit. Dont base new codes on it.
 */
#include "hw.hpp"
#include "colors.hpp"
#include "opencv_util.hpp"
#include "couch_data.hpp"
#include <opencv2/opencv.hpp>

using namespace std;

ShapeInterface::ShapeInterface() : isRotateAllowed(true) {
}

const Point ShapeInterface::getCenter() {
    auto rect = getBoundingBox();
    return Point(rect.x + rect.width / 2.0, rect.y + rect.height / 2.0);
}

ShapeItem::ShapeItem() : mLabel(-1) {
}

/**
 * 将当前数据画入 canvas
 * @param canvas 目标画布
 */
void ShapeItem::paintTo(cv::Mat &canvas) {
    for (auto &stroke:mData) {
        if (stroke.size() == 0)continue;
        RC::update_shape_attr();
        cv::line(canvas, stroke[0], stroke[0],
                 RC::get_shape_color(),
                 RC::get_shape_thickness(),
                 RC::get_shape_lineType(),
                 RC::get_shape_shift());
        for (size_t i = 1; i < stroke.size(); i++) {
            RC::update_shape_attr();
            cv::line(canvas, stroke[i], stroke[i - 1],
                     RC::get_shape_color(),
                     RC::get_shape_thickness(),
                     RC::get_shape_lineType(),
                     RC::get_shape_shift());
        }
    }
}

/**
 * 获取平行于二维坐标轴的最小边框
 * @return
 */
const cv::Rect2f ShapeItem::getBoundingBox() const {
    return getRect(mData);
}

/**
 * 顺时针旋转
 * @param angle 角度制
 */
void ShapeItem::rotate(float angle) {
    float theta = -M_PI * angle / 180;
    cv::Rect2f bBox = getBoundingBox();
    Point oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    moveCenterTo(Point(0, 0));
    rotateTheta(theta);
    moveCenterTo(oldCenter);
}

void ShapeItem::rotateBy(float angle, const Point &cent) {
    float theta = -M_PI * angle / 180;
    rotateThetaBy(theta, cent);
}

/**
 * 将数据点整体移动 offset
 * @param offset
 */
void ShapeItem::move(const Point &offset) {
    for (auto &stroke:mData) {
        for (auto &p:stroke)
            p += offset;
    }
}

/**
 * 将数据点的中心移动到 newCenter
 * @param newCenter
 */
void ShapeItem::moveCenterTo(const Point &newCenter) {
    cv::Rect2f bBox = getBoundingBox();
    Point oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    Point offset = newCenter - oldCenter;
    move(offset);
}

/**
 * 将数据点的边框左上角移动到 leftTop
 * @param leftTop
 */
void ShapeItem::moveLeftTopTo(const Point &leftTop) {
    cv::Rect2f bBox = getBoundingBox();
    Point offset = -bBox.tl() + leftTop;
    move(offset);
}

/**
 * 将数据点整体缩放到 (w,h)
 * @param w 目标宽度
 * @param h 目标长度
 * @param keepRatio 是否保持比例，为 true 时撑满长边
 */
void ShapeItem::resizeTo(float w, float h, bool keepRatio) {
    cv::Rect2f bBox = getBoundingBox();
    Point oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    float oldW = bBox.width, oldH = bBox.height;
    float kx = w / oldW;
    float ky = h / oldH;
    moveLeftTopTo(Point(0, 0));
    if (keepRatio) {
        kx = ky = std::min(kx, ky);
    }
    mulK(kx, ky);
    moveCenterTo(oldCenter);
}

/**
 *
 * @param center 中心
 * @param rx 椭圆水平半径
 * @param ry 椭圆竖直半径
 */
void ShapeItem::castToCircle(const Point &center, float rx, float ry) {
    vector<Point> from(4), to = {// just a square box
            Point(0, 0), Point(0, 100),
            Point(100, 100), Point(100, 0)
    };
    vector<Point> input;
    for (auto &stroke:mData) {
        std::copy(stroke.begin(), stroke.end(),
                  std::inserter(input, input.end()));
    }
    cv::minAreaRect(input).points(from.data());
    castBy(from, to);
    resizeTo(rx * 2, ry * 2, false);
    moveCenterTo(center);
}

/**
 * 算一个映射：从【最小矩形】向【等比例的以(p1,p2)为中长轴的矩形】，为所有点做这个映射，指向随机
 * @param p1 起点
 * @param p2 终点
 */
void ShapeItem::castToLine(const Point &pp1, const Point &pp2, const float lThresh) {
    const float slideOffset = 0.05;
    // FIXME: 分离随机因素
    L:;
    float kLen1 = minMaxProb(slideOffset);
    float kLen2 = minMaxProb(slideOffset);
    Point p1 = pp1 + (pp1 - pp2) * kLen1, p2 = pp2 + (pp2 - pp1) * kLen2;
//    Point p1=pp1,p2=pp2;
    float lenPP = getDistance2D(p1, p2);
    p1.x += lenPP * minMaxProb(slideOffset);
    p2.x += lenPP * minMaxProb(slideOffset);
    p1.y += lenPP * minMaxProb(slideOffset);
    p2.y += lenPP * minMaxProb(slideOffset);
    vector<Point> from(4), to;
    vector<Point> input;
    for (auto &stroke:mData) {
        std::copy(stroke.begin(), stroke.end(),
                  std::inserter(input, input.end()));
    }
    cv::minAreaRect(input).points(from.data());
    auto s03 = getDistance2D(from[0], from[3]);
    auto s01 = getDistance2D(from[0], from[1]);
    while (s03 < std::numeric_limits<float>::epsilon()
           || s01 < std::numeric_limits<float>::epsilon()) {
        mData.clear();
        mData = std::move(CouchLoader::GetShape("line").mData);
        goto L;// FIXME: i heard that goto considered harmful?
    }
    Point vec = p1 - p2;
    Point vecT(0, 1);
    if (fabs(vec.y) > std::numeric_limits<float>::epsilon()) {
        vecT = Point(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    float kk = s03 / s01;
    while (lThresh < kk && kk < 1 / lThresh) {
        mData.clear();
        mData = std::move(CouchLoader::GetShape("line").mData);
//        mData.emplace_back(Stroke({p1, p2}));
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

/**
 * 构造一个from->to的仿射变换，为所有点执行这个仿射变换
 * @param from 三角形1
 * @param to 三角形2
 */
void ShapeItem::castBy(const vector<Point> &from, const vector<Point> &to) {
    // 2x3 仿射变换矩阵
    auto affine = cv::getAffineTransform(from.data(), to.data());
    for (auto &stroke:mData) {
        for (auto &p:stroke) {
            cv::Mat pm = affine * cv::Mat({static_cast<double>(p.x),
                                           static_cast<double>(p.y), 1.});
            p.x = pm.at<double>(0, 0);
            p.y = pm.at<double>(1, 0);
        }
    }
}

Script &ShapeItem::getData() {
    return mData;
}

void ShapeItem::mulK(float kx, float ky) {
    for (auto &stroke:mData) {
        for (auto &p:stroke) {
            p.x *= kx;
            p.y *= ky;
        }
    }
}

void ShapeItem::rotateTheta(float theta) {
    float x, y;
    for (auto &stroke:mData) {
        for (auto &p:stroke) {
            x = p.x * std::cos(theta) + p.y * std::sin(theta);
            y = p.y * std::cos(theta) - p.x * std::sin(theta);
            p.x = x;
            p.y = y;
        }
    }
}

void ShapeItem::rotateThetaBy(float theta, const cv::Point2f &cent) {
    float x, y;
    for (auto &stroke:mData) {
        for (auto &p:stroke) {
            x = (p.x - cent.x) * std::cos(theta) + (p.y - cent.y) * std::sin(theta) + cent.x;
            y = (p.y - cent.y) * std::cos(theta) - (p.x - cent.x) * std::sin(theta) + cent.y;
            p.x = x;
            p.y = y;
        }
    }
}

cv::Rect2f getRect(const Stroke &stroke) {
    if (stroke.empty()) {
        std::cerr << "stroke.empty() in cv::Rect2f getRect(const Stroke &stroke)" << std::endl;
        exit(-1);
//        return cv::Rect2f(0, 0, 0, 0);
    }
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &p:stroke) {
        minx = std::min(minx, p.x);
        miny = std::min(miny, p.y);
        maxx = std::max(maxx, p.x);
        maxy = std::max(maxy, p.y);
    }
    return cv::Rect2f(minx, miny, maxx - minx, maxy - miny);
}

cv::Rect2f getRect(const Script &script) {
    if (script.empty()) {
        std::cerr << "script.empty() in cv::Rect2f getRect(const Script &script)" << std::endl;
        exit(-1);
//        return cv::Rect2f(0, 0, 0, 0);
    }
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &stroke:script) {
        if (stroke.empty()) {
            continue;
        }
        auto rect = getRect(stroke);
        minx = std::min(minx, rect.x);
        miny = std::min(miny, rect.y);
        maxx = std::max(maxx, rect.x + rect.width);
        maxy = std::max(maxy, rect.y + rect.height);
    }
    return cv::Rect2f(minx, miny, maxx - minx, maxy - miny);
}

void ShapeGroup::setSNormSizeK(float sNormSizeK) {
    RC::sNormSizeK = sNormSizeK;
}

void ShapeGroup::setSSubSizeK(float sSubSizeK) {
    RC::sSubSizeK = sSubSizeK;
}

void ShapeGroup::setSNormOffsetKx(float sNormOffsetKx) {
    RC::sNormOffsetKx = sNormOffsetKx;
}

void ShapeGroup::setSNormOffsetKy(float sNormOffsetKy) {
    RC::sNormOffsetKy = sNormOffsetKy;
}

void ShapeGroup::setSSubOffsetKx(float sSubOffsetKx) {
    RC::sSubOffsetKx = sSubOffsetKx;
}

void ShapeGroup::setSSubOffsetKy(float sSubOffsetKy) {
    RC::sSubOffsetKy = sSubOffsetKy;
}

void ShapeGroup::setSAngleK(float sAngleK) {
    RC::sAngleK = sAngleK;
}

shared_ptr<ShapeGroup> ShapeGroup::GetShapeGroup(const string &_textType) {
    shared_ptr<ShapeGroup> text;
    if (_textType == "positive") {
        auto sym = CouchLoader::GetByStrLabel("⊕");
        text = std::make_shared<ShapeGroup>();
        text->shapes.emplace_back(std::move(sym));
    } else if (_textType == "negative") {
        auto sym1 = CouchLoader::GetShape("circle");
        sym1.resizeTo(100, 100);
        sym1.moveCenterTo(cv::Point2f(50, 50));
        auto sym2 = CouchLoader::GetByStrLabel("-");
        // TODO: 分离随机要素
        sym2.resizeTo(80, 80);
        sym2.moveCenterTo(cv::Point2f(50, 50));
        text = std::make_shared<ShapeGroup>();
        text->shapes.emplace_back(std::move(sym1));
        text->shapes.emplace_back(std::move(sym2));
    } else {
        text = std::make_shared<ShapeGroup>(makeNotedStringByChar(_textType));
    }
    return text;
}

void ShapeGroup::append(const NChar &c) {
    auto sym = CouchLoader::GetByStrLabel(c.second);
    sym.rotate(30 * RC::sAngleK);
    float s = 100 * RC::sNormSizeK;
    float sx = 100 * RC::sNormOffsetKx;
    float sy = 100 * RC::sNormOffsetKy;
    float ss = 50 * RC::sSubSizeK;
    float ssx = 50 * RC::sSubOffsetKx;
    float ssy = 50 * RC::sSubOffsetKy;
    if (shapes.empty()) {
        sym.resizeTo(s, s, true);
        sym.moveLeftTopTo(cv::Point2f(0, 0));
    } else {
        auto rect = getBoundingBox();
        if (rect.height < 100) {
            rect.height = 100;
        }
        switch (c.first) {
            case RightButtom: {
                sym.resizeTo(ss, ss);
                sym.moveLeftTopTo(cv::Point2f(
                        rect.x + rect.width + ssx, 50 + ssy));
                break;
            }
            case RightTop: {
                sym.resizeTo(ss, ss);
                sym.moveLeftTopTo(cv::Point2f(
                        rect.x + rect.width + ssx, ssy));
                break;
            }
            case Up: {
                rect = shapes.back().getBoundingBox();
                sym.resizeTo(ss, ss);
                sym.moveLeftTopTo(cv::Point2f(
                        rect.x + ssx, ssy));
                break;
            }
            case Down: {
                break;
            }
            case UpRand: {
                break;
            }
            case DownRand: {
                break;
            }
            default: {
                sym.resizeTo(s, s);
                sym.moveLeftTopTo(cv::Point2f(
                        rect.x + rect.width + sx, sy));
            }
        }
    }
//    if (!shapes.empty()) {
//        auto rect = shapes.back().getBoundingBox();
//        // 左上角对齐右上角
//        sym.resizeTo(std::numeric_limits<float>::max(), rect.height);
//        sym.moveLeftTopTo(cv::Point2f(
//                rect.x + 1.2 * rect.width, rect.y));
//    }
    shapes.emplace_back(std::move(sym));
}

void ShapeGroup::resizeTo(float w, float h, bool keepRatio) {
    cv::Rect2f bBox = getBoundingBox();
    cv::Point2f oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    float oldW = bBox.width, oldH = bBox.height;
    float kx = w / oldW;
    float ky = h / oldH;
    moveLeftTopTo(cv::Point2f(0, 0));
    if (keepRatio) {
        kx = ky = std::min(kx, ky);
    }
    for (auto &s:shapes) {
        s.mulK(kx, ky);
    }
    moveCenterTo(oldCenter);
}

void ShapeGroup::moveLeftTopTo(const cv::Point2f &leftTop) {
    cv::Rect2f bBox = getBoundingBox();
    cv::Point2f offset = -bBox.tl() + leftTop;
    move(offset);
}

void ShapeGroup::moveCenterTo(const cv::Point2f &newCenter) {
//    cv::Rect2f bBox = getBoundingBox();
//    cv::Point2f oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    auto oldCenter = getCenter();
//    std::cout<<"in ShapeGroup::moveCenterTo: old cent="<<oldCenter<<std::endl;
    cv::Point2f offset = newCenter - oldCenter;
    move(offset);
}

void ShapeGroup::move(const cv::Point2f &offset) {
    for (auto &s:shapes) {
        s.move(offset);
    }
}

void ShapeGroup::rotate(float angle) {
    float theta = -M_PI * angle / 180.0;
    auto oldCenter = getCenter();
//    std::cout<<"in ShapeGroup::rotate: old cent="<<oldCenter<<std::endl;
//    moveCenterTo(cv::Point2f(0, 0));
    for (auto &s:shapes) {
//        s.rotateTheta(theta);
        s.rotateThetaBy(theta, oldCenter);
    }
//    moveCenterTo(oldCenter);
}

void ShapeGroup::rotateBy(float angle, const cv::Point2f &cent) {
    float theta = -M_PI * angle / 180;
    for (auto &s:shapes) {
        s.rotateThetaBy(theta, cent);
    }
}

const cv::Rect2f ShapeGroup::getBoundingBox() const {
    if (shapes.empty()) {
        return cv::Rect2f(0, 0, 1, 1);
    }
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &s :shapes) {
        auto rect = s.getBoundingBox();
        minx = std::min(minx, rect.x);
        miny = std::min(miny, rect.y);
        maxx = std::max(maxx, rect.x + rect.width);
        maxy = std::max(maxy, rect.y + rect.height);
    }
    return cv::Rect2f(minx - 1, miny - 1, maxx - minx + 2, maxy - miny + 2);
}

void ShapeGroup::paintTo(cv::Mat &canvas) {
    for (auto &shape:shapes) {
        shape.paintTo(canvas);
    }
}

const string &ShapeGroup::getName() const {
    return name;
}

void ShapeGroup::removePointIf(const std::function<bool(const cv::Point2f &)> &_cond) {
    decltype(shapes) newShapes;
    for (auto &shape:shapes) {
        ShapeItem newShape;
        for (auto &stroke:shape.mData) {
            Stroke newStroke;
            for (auto &pt:stroke) {
                if (_cond(pt)) {
                    newStroke.push_back(pt);
                }
            }
            if (!newStroke.empty()) {
                newShape.mData.push_back(std::move(newStroke));
            }
        }
        if (!newShape.mData.empty()) {
            newShapes.push_back(std::move(newShape));
        }
    }
    shapes = std::move(newShapes);
}

void ShapeGroup::addShapeItem(ShapeItem &item) {
    shapes.push_back(std::move(item));
}

BondItem::BondItem()
        : mUseHWChar(true), isLatest(false) {
    isRotateAllowed = true;
}

void BondItem::updateShapes() {
    if (mUseHWChar) {
        isLatest = true;
    }
    shapes.clear();
}

void BondItem::setUseHandWrittenWChar(bool useHandWrittenChar) {
    mUseHWChar = useHandWrittenChar;
}

shared_ptr<BondItem> BondItem::GetBond(const JBondType &_bondType) {
    shared_ptr<BondItem> bond;
    switch (_bondType) {
        case JBondType::SolidWedgeBond:
            bond = std::make_shared<SolidWedgeBond>();
            break;
        case JBondType::DashWedgeBond:
            bond = std::make_shared<DashWedgeBond>();
            break;
        case JBondType::WaveBond:
            bond = std::make_shared<WaveBond>();
            break;
        case JBondType::SingleBond :
        case JBondType::DelocalizedBond:// 离域键只画骨架，环另画
            bond = std::make_shared<SingleBond>();
            break;
        case JBondType::DoubleBond:
            bond = std::make_shared<DoubleBond>();
            break;
        case JBondType::TripleBond:
            bond = std::make_shared<TripleBond>();
            break;
        case JBondType::CircleBond:
            bond = std::make_shared<CircleBond>();
            break;
        default: {
            std::cerr << "Bond::GetBond: unknown bond type "
                      << static_cast<int>(_bondType) << std::endl;
            exit(-1);
        }
    }
    return bond;
}

CircleBond::CircleBond() {
    name = "Circle";
}

void CircleBond::mulK(float kx, float ky) {
    center *= kx;
    r *= (kx + ky) / 2.0;
    isLatest = false;
}

void CircleBond::resizeTo(float w, float h, bool keepRatio) {
    float oldW = r * 2;
    float oldH = r * 2;
    float kx = w / oldW;
    float ky = h / oldH;
    if (keepRatio) {
        kx = ky = std::min(kx, ky);
    }
    r *= (kx + ky) / 2;
    isLatest = false;
}

void CircleBond::moveLeftTopTo(const Point &leftTop) {
    Point oldLeftTop = center - Point(r, r);
    auto offset = leftTop - oldLeftTop;
    move(offset);
    isLatest = false;
}

void CircleBond::moveCenterTo(const Point &newCenter) {
    auto offset = newCenter - center;
    move(offset);
    isLatest = false;
}

void CircleBond::move(const Point &offset) {
    center += offset;
    isLatest = false;
}

void CircleBond::rotate(float angle) {
// DO NOTHING FOR A CIRCLE
    isLatest = false;
}

void CircleBond::rotateBy(float angle, const Point &cent) {
    float theta = -M_PI * angle / 180;
    float x, y;
    x = (center.x - cent.x) * std::cos(theta) + (center.y - cent.y) * std::sin(theta) + cent.x;
    y = (center.y - cent.y) * std::cos(theta) - (center.x - cent.x) * std::sin(theta) + cent.y;
    center.x = x;
    center.y = y;
    isLatest = false;
}

void CircleBond::setVertices(const vector<Point> &pts) {
    if (pts.size() < 2) {
        std::cerr << "圆键的pts约定为多边形顶点,but "
                  << pts.size() << " pts got." << std::endl;
        exit(-1);
    }
    center.x = center.y = 0;
    for (auto &pt:pts)
        center += pt;
    center.x /= pts.size();
    center.y /= pts.size();
    r = 0;
    for (auto &pt:pts)
        r += getDistance2D(center, pt);
    r /= pts.size();
    // TODO: 分离随即要素
    const float minLen = 0.2;
    r *= (minLen + 0.5 * belowProb(std::sqrt(3) / 2 - minLen));
    isLatest = false;
}

void CircleBond::paintTo(cv::Mat &canvas) {
    if (!isLatest) {
        updateShapes();
    }
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::circle(canvas, center, r,
                   RC::get_shape_color(),
                   RC::get_shape_thickness(),
                   RC::get_shape_lineType(),
                   RC::get_shape_shift());
    }
}

const cv::Rect2f CircleBond::getBoundingBox() const {
    if (isLatest && mUseHWChar) { return ShapeGroup::getBoundingBox(); }
    float x, y;
    x = center.x - r;
    y = center.y - r;
    return cv::Rect2f(x - 1, y - 1, 2 * r + 2, 2 * r + 2);
}

void CircleBond::updateShapes() {
    BondItem::updateShapes();
    if (mUseHWChar) {
        auto circle = CouchLoader::GetShape("circle");
        circle.castToCircle(center, r, r);
        shapes.push_back(std::move(circle));
    }
}

SingleBond::SingleBond() {
    name = "Single";
}

const cv::Rect2f SingleBond::getBoundingBox() const {
    if (!isLatest) {
        const_cast<SingleBond *>(this)->updateShapes();
    }
    return ShapeGroup::getBoundingBox();
    // FIXME: 键图元的包围盒是用关键点算的，和加载手写数据以后的包围盒不一致
//    if (isLatest) { return ShapeGroup::getBoundingBox(); }
//    float x, y, w, h;
//    x = std::min(from.x, to.x);
//    y = std::min(from.y, to.y);
//    w = std::max(from.x, to.x) - x;
//    h = std::max(from.y, to.y) - y;
//    return cv::Rect2f(x - 1, y - 1, w + 2, h + 2);
}

void SingleBond::updateShapes() {
    BondItem::updateShapes();
    if (mUseHWChar) {
        auto line = CouchLoader::GetShape("line");
        line.castToLine(from, to);
        shapes.push_back(std::move(line));
    }
}

void SingleBond::paintTo(cv::Mat &canvas) {
    if (!isLatest) {
        updateShapes();
    }
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::line(canvas, from, to,
                 RC::get_shape_color(),
                 RC::get_shape_thickness(),
                 RC::get_shape_lineType(),
                 RC::get_shape_shift());
    }
}

void SingleBond::mulK(float kx, float ky) {
    from *= kx;
    to *= ky;
    isLatest = false;
}

void SingleBond::resizeTo(float w, float h, bool keepRatio) {
    float oldW = std::fabs(from.x - to.x) + 0.0001;
    float oldH = std::fabs(from.y - to.y) + 0.0001;
    float kx = w / oldW;
    float ky = h / oldH;
    if (keepRatio) {
        kx = ky = std::min(kx, ky);
    }
    std::cout << kx << "," << ky << std::endl;
    auto cent = (from + to) / 2;
    auto offset = from - cent;
    from.x += offset.x * kx;
    from.y += offset.y * ky;
    offset = to - cent;
    to.x += offset.x * kx;
    to.y += offset.y * ky;
    isLatest = false;
}

void SingleBond::moveLeftTopTo(const Point &leftTop) {
    Point oldLeftTop(std::min(from.x, to.x), std::min(from.y, to.y));
    auto offset = leftTop - oldLeftTop;
    move(offset);
    isLatest = false;
}

void SingleBond::moveCenterTo(const Point &newCenter) {
    auto oldCenter = (from + to) / 2.0;
    auto offset = newCenter - oldCenter;
    move(offset);
    isLatest = false;
}

void SingleBond::move(const Point &offset) {
    from += offset;
    to += offset;
    isLatest = false;
}

void SingleBond::rotate(float angle) {
    auto oldCenter = (from + to) / 2.0;
    rotateBy(angle, oldCenter);
    isLatest = false;
}

void SingleBond::rotateBy(float angle, const Point &cent) {
    float theta = -M_PI * angle / 180;
    float x, y;
    x = (from.x - cent.x) * std::cos(theta) + (from.y - cent.y) * std::sin(theta) + cent.x;
    y = (from.y - cent.y) * std::cos(theta) - (from.x - cent.x) * std::sin(theta) + cent.y;
    from.x = x;
    from.y = y;
    x = (to.x - cent.x) * std::cos(theta) + (to.y - cent.y) * std::sin(theta) + cent.x;
    y = (to.y - cent.y) * std::cos(theta) - (to.x - cent.x) * std::sin(theta) + cent.y;
    to.x = x;
    to.y = y;
    isLatest = false;
}

void SingleBond::setVertices(const vector<Point> &pts) {
    if (pts.size() != 2) {
        std::cerr << "单双三楔波浪线的pts约定为起点和终点,but "
                  << pts.size() << " pts got." << std::endl;
        exit(-1);
    }
    from = pts[0];
    to = pts[1];
    isLatest = false;
}

void DoubleBond::paintTo(cv::Mat &canvas) {
    if (!isLatest) {
        updateShapes();
    }
    const float intervalK = 0.2;
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::Point2f vec = from - to;
        auto length = getDistance2D(from, to);
        cv::Point2f vecT(0, 1);
        if (vec.y != 0) {
            vecT = cv::Point2f(-1, vec.x / vec.y);
            vecT /= cv::norm(vecT);
        }
        auto offset = vecT * length * intervalK;
        cv::line(canvas, from + offset / 2, to + offset / 2,
                 RC::get_shape_color(),
                 RC::get_shape_thickness(),
                 RC::get_shape_lineType(),
                 RC::get_shape_shift());
        cv::line(canvas, from - offset / 2, to - offset / 2,
                 RC::get_shape_color(),
                 RC::get_shape_thickness(),
                 RC::get_shape_lineType(),
                 RC::get_shape_shift());
    }
}

DoubleBond::DoubleBond() {
    name = "Double";
}

void DoubleBond::updateShapes() {
    BondItem::updateShapes();
    //FIXME: 设置间隔，这里和 MolItem::reloadHWData 有冲突
    const float intervalK = 0.2;
    cv::Point2f vec = from - to;
    cv::Point2f vecT(0, 1);
    if (vec.y != 0) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    if (mUseHWChar) {
        auto length = getDistance2D(from, to);
        auto offset = vecT * length * intervalK;
        auto line1 = CouchLoader::GetShape("line");
        line1.castToLine(from + offset / 2, to + offset / 2);
        auto line2 = CouchLoader::GetShape("line");
        line2.castToLine(from - offset / 2, to - offset / 2);
        shapes.push_back(std::move(line1));
        shapes.push_back(std::move(line2));
    }
}

void TripleBond::paintTo(cv::Mat &canvas) {
    if (!isLatest) {
        updateShapes();
    }
    const float intervalK = 0.4;
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::Point2f vec = from - to;
        auto length = getDistance2D(from, to);
        cv::Point2f vecT(0, 1);
        if (vec.y != 0) {
            vecT = cv::Point2f(-1, vec.x / vec.y);
            vecT /= cv::norm(vecT);
        }
        auto offset = vecT * length * intervalK;
        cv::line(canvas, from + offset / 2, to + offset / 2,
                 RC::get_shape_color(),
                 RC::get_shape_thickness(),
                 RC::get_shape_lineType(),
                 RC::get_shape_shift());
        cv::line(canvas, from, to, RC::get_shape_color(),
                 RC::get_shape_thickness(),
                 RC::get_shape_lineType(),
                 RC::get_shape_shift());
        cv::line(canvas, from - offset / 2, to - offset / 2,
                 RC::get_shape_color(),
                 RC::get_shape_thickness(),
                 RC::get_shape_lineType(),
                 RC::get_shape_shift());
    }
}

TripleBond::TripleBond() {
    name = "Triple";
}

void TripleBond::updateShapes() {
    BondItem::updateShapes();
    const float intervalK = 0.4;
    cv::Point2f vec = from - to;
    cv::Point2f vecT(0, 1);
    if (vec.y != 0) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    if (mUseHWChar) {
        auto length = getDistance2D(from, to);
        auto offset = vecT * length * intervalK;
        auto line1 = CouchLoader::GetShape("line");
        line1.castToLine(from + offset / 2, to + offset / 2);
        auto line2 = CouchLoader::GetShape("line");
        line2.castToLine(from, to);
        auto line3 = CouchLoader::GetShape("line");
        line3.castToLine(from - offset / 2, to - offset / 2);
        shapes.push_back(std::move(line1));
        shapes.push_back(std::move(line2));
        shapes.push_back(std::move(line3));
    }
}

void SolidWedgeBond::paintTo(cv::Mat &canvas) {
    if (!isLatest) {
        updateShapes();
    }
    const float intervalK = 0.5;
    const int numOfSplit = 10;
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::Point2f vec = from - to;
        auto length = getDistance2D(from, to);
        cv::Point2f vecT(0, 1);
        if (vec.y != 0) {
            vecT = cv::Point2f(-1, vec.x / vec.y);
            vecT /= cv::norm(vecT);
        }
        auto offset = vecT * length * intervalK;
        vector<cv::Point> polygon = {from, to + offset / 2, to - offset / 2};
        cv::fillConvexPoly(canvas, polygon, RC::get_shape_color(),
                           RC::get_shape_lineType(),
                           RC::get_shape_shift());
    }
}

SolidWedgeBond::SolidWedgeBond() {
    name = "Solid";
}

void SolidWedgeBond::updateShapes() {
    BondItem::updateShapes();
    const float intervalK = 0.5;
    const int numOfSplit = 10;
    if (mUseHWChar) {
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
        for (float i = 0; i <= 1.0; i += 1.0 / numOfSplit) {
            auto line = CouchLoader::GetShape("line");
            line.castToLine(from, cv::Point2f(x, y));
            shapes.push_back(std::move(line));
            x += dx;
            y += dy;
        }
    }
}

void DashWedgeBond::paintTo(cv::Mat &canvas) {
    if (!isLatest) {
        updateShapes();
    }
    const float intervalK = 0.5;
    const int numOfSplit = 5;
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::Point2f vec = from - to;
        auto length = getDistance2D(from, to);
        cv::Point2f vecT(0, 1);
        if (vec.y != 0) {
            vecT = cv::Point2f(-1, vec.x / vec.y);
            vecT /= cv::norm(vecT);
        }
        auto offset = vecT * length * intervalK;
        auto to1 = to + offset / 2, to2 = to - offset / 2;
        //         _ to1
        // from--<          from-to1,from-to2采样
        //        \__to2
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
        for (float i = 0; i <= 1.0; i += 1.0 / numOfSplit) {
            cv::line(canvas, Point(x1, y1), Point(x2, y2),
                     RC::get_shape_color(),
                     RC::get_shape_thickness(),
                     RC::get_shape_lineType(),
                     RC::get_shape_shift());
            x1 += d[0];
            y1 += d[1];
            x2 += d[2];
            y2 += d[3];
        }
    }
}

DashWedgeBond::DashWedgeBond() {
    name = "Dash";
}

void DashWedgeBond::updateShapes() {
    BondItem::updateShapes();
    const float intervalK = 0.5;
    const int numOfSplit = 4 + rand() % 3; // default: 5
    if (mUseHWChar) {
        cv::Point2f vec = from - to;
        auto length = getDistance2D(from, to);
        cv::Point2f vecT(0, 1);
        if (vec.y != 0) {
            vecT = cv::Point2f(-1, vec.x / vec.y);
            vecT /= cv::norm(vecT);
        }
        auto offset = vecT * length * intervalK;
        auto to1 = to + offset / 2, to2 = to - offset / 2;
        //         _ to1
        // from--<          from-to1,from-to2采样
        //        \__to2
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
        for (float i = 0; i <= 1.0; i += 1.0 / numOfSplit) {
            auto line = CouchLoader::GetShape("line");
            line.castToLine(cv::Point2f(x1, y1), cv::Point2f(x2, y2));
            line.rotate(angle);
            shapes.push_back(std::move(line));
            x1 += d[0];
            y1 += d[1];
            x2 += d[2];
            y2 += d[3];
        }
    }
}

void WaveBond::paintTo(cv::Mat &canvas) {
    if (!isLatest) {
        updateShapes();
    }
    for (auto &s:shapes) {
        s.paintTo(canvas);
    }
}

WaveBond::WaveBond() {
    name = "Wave";
}

void WaveBond::updateShapes() {
    BondItem::updateShapes();
    float cyclesOfSin = (rand() % 500) / 100.0 + 4;
    float end = M_PI * 2 * cyclesOfSin;
    Stroke stroke;
    // TODO: 这里存在一个随机要素
    for (float start = rand() % 100 / 100; start < end; start += 0.0520603) {
        stroke.push_back(cv::Point2f(start, 2 * std::sin(start)));
    }
    ShapeItem shape;
    auto &script = shape.getData();
    script.push_back(std::move(stroke));
    shape.castToLine(from, to, 1);
    shapes.push_back(std::move(shape));
}

RC::ShapeAttr::ShapeAttr() : color(0, 0, 0),
                             thickness(2), lineType(cv::LINE_AA), shift(0) {
}
