#include "shape.hpp"
#include "handwritting.hpp"

#include <opencv2/opencv.hpp>

Shape::Shape() : mLabel(-1) {
}

/**
 * 将当前数据画入 canvas
 * @param canvas 目标画布
 */
void Shape::paintTo(cv::Mat &canvas) {
    for (auto &stroke:mData) {
        if (stroke.size() == 0)continue;
        cv::line(canvas, stroke[0], stroke[0],
                 color, thickness, lineType, shift);
        for (size_t i = 1; i < stroke.size(); i++) {
            cv::line(canvas, stroke[i], stroke[i - 1],
                     color, thickness, lineType, shift);
        }
    }
}

/**
 * 获取平行于二维坐标轴的最小边框
 * @return
 */
const cv::Rect2f Shape::getBoundingBox() const {
    return getRect(mData);
}

/**
 * 顺时针旋转
 * @param angle 角度制
 */
void Shape::rotate(float angle) {
    float theta = -M_PI * angle / 180;
    cv::Rect2f bBox = getBoundingBox();
    Point oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    moveCenterTo(Point(0, 0));
    rotateTheta(theta);
    moveCenterTo(oldCenter);
}

void Shape::rotateBy(float angle, const Point &cent) {
    float theta = -M_PI * angle / 180;
    rotateThetaBy(theta, cent);
}

/**
 * 将数据点整体移动 offset
 * @param offset
 */
void Shape::move(const Point &offset) {
    for (auto &stroke:mData) {
        for (auto &p:stroke)
            p += offset;
    }
}

/**
 * 将数据点的中心移动到 newCenter
 * @param newCenter
 */
void Shape::moveCenterTo(const Point &newCenter) {
    cv::Rect2f bBox = getBoundingBox();
    Point oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    Point offset = newCenter - oldCenter;
    move(offset);
}

/**
 * 将数据点的边框左上角移动到 leftTop
 * @param leftTop
 */
void Shape::moveLeftTopTo(const Point &leftTop) {
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
void Shape::resizeTo(float w, float h, bool keepRatio) {
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
void Shape::castToCircle(const Point &center, float rx, float ry) {
    v<Point> from(4), to = {// just a square box
            Point(0, 0), Point(0, 100),
            Point(100, 100), Point(100, 0)
    };
    v<Point> input;
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
void Shape::castToLine(const Point &pp1, const Point &pp2, const float lThresh) {
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
    v<Point> from(4), to;
    v<Point> input;
    for (auto &stroke:mData) {
        std::copy(stroke.begin(), stroke.end(),
                  std::inserter(input, input.end()));
    }
    cv::minAreaRect(input).points(from.data());
    auto s03 = distance(from[0], from[3]);
    auto s01 = distance(from[0], from[1]);
    Point vec = p1 - p2;
    Point vecT(0, 1);
    if (vec.y != 0) {
        vecT = Point(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    float kk = s03 / s01;
    while (lThresh < kk && kk < 1 / lThresh) {
        mData.clear();
        mData = std::move(HandWritting::GetShape("line").mData);
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
void Shape::castBy(const v<Point> &from, const v<Point> &to) {
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

Script &Shape::getData() {
    return mData;
}

cv::Rect2f getRect(const Stroke &stroke) {
    if (stroke.empty()) {
//        exit(-1);
        return cv::Rect2f(0, 0, 1, 1);
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
//        exit(-1);
        return cv::Rect2f(0, 0, 1, 1);
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
