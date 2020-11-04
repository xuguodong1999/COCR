#include "shapeitem.hpp"
#include "couch.hpp"
#include "statistic.hpp"
#include <opencv2/opencv.hpp>

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
    L:;
    float kLen1 = (rand() % 10) / 100 - 0.05;
    float kLen2 = (rand() % 10) / 100 - 0.05;
    Point p1 = pp1 + (pp1 - pp2) * kLen1, p2 = pp2 + (pp2 - pp1) * kLen2;
//    Point p1=pp1,p2=pp2;
    float lenPP = distance(p1, p2);
    p1.x += lenPP * (rand() % 10) / 100 - 0.05;
    p2.x += lenPP * (rand() % 10) / 100 - 0.05;
    p1.y += lenPP * (rand() % 10) / 100 - 0.05;
    p2.y += lenPP * (rand() % 10) / 100 - 0.05;
    vector<Point> from(4), to;
    vector<Point> input;
    for (auto &stroke:mData) {
        std::copy(stroke.begin(), stroke.end(),
                  std::inserter(input, input.end()));
    }
    cv::minAreaRect(input).points(from.data());
    auto s03 = distance(from[0], from[3]);
    auto s01 = distance(from[0], from[1]);
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
