#include "bond.hpp"
#include "handwritting.hpp"
#include "statistic.hpp"
#include <opencv2/opencv.hpp>

Bond::Bond(const s &_bondType)
        : mUseHWChar(true), isLatest(false) {
    isRotateAllowed = true;
    name = _bondType;
}

void Bond::updateShapes() {
    if(mUseHWChar) {
        isLatest = true;
    }
    shapes.clear();
}

void Bond::setUseHandWrittenWChar(bool useHandWrittenChar) {
    mUseHWChar = useHandWrittenChar;
}

p<Bond> Bond::GetBond(const s &_bondType) {
    p<Bond> bond;
    if (_bondType == "Single") {
        bond = std::make_shared<SingleBond>(_bondType);
    } else if (_bondType == "Double") {
        bond = std::make_shared<DoubleBond>(_bondType);
    } else if (_bondType == "Triple") {
        bond = std::make_shared<TripleBond>(_bondType);
    } else if (_bondType == "Circle") {
        bond = std::make_shared<CircleBond>(_bondType);
    } else if (_bondType == "SolidWedge") {
        bond = std::make_shared<SolidWedgeBond>(_bondType);
    } else if (_bondType == "DashWedge") {
        bond = std::make_shared<DashWedgeBond>(_bondType);
    } else if (_bondType == "Wave") {
        bond = std::make_shared<WaveBond>(_bondType);
    } else {
        std::cerr << "Bond::GetBond: unknown bond type "
                  << _bondType << std::endl;
        exit(-1);
    }
    return bond;
}

CircleBond::CircleBond(const s &_bondType) : Bond(_bondType) {
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

void CircleBond::setVertices(const v<Point> &pts) {
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
        r += distance(center, pt);
    r /= pts.size();
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
    Bond::updateShapes();
    if (mUseHWChar) {
        auto circle = HandWritting::GetShape("circle");
        circle.castToCircle(center, r, r);
        shapes.push_back(std::move(circle));
    }
}

SingleBond::SingleBond(const s &_bondType) : Bond(_bondType) {
}

const cv::Rect2f SingleBond::getBoundingBox() const {
    if (isLatest) { return ShapeGroup::getBoundingBox(); }
    float x, y, w, h;
    x = std::min(from.x, to.x);
    y = std::min(from.y, to.y);
    w = std::max(from.x, to.x) - x;
    h = std::max(from.y, to.y) - y;
    return cv::Rect2f(x - 1, y - 1, w + 2, h + 2);
}

void SingleBond::updateShapes() {
    Bond::updateShapes();
    if (mUseHWChar) {
        auto line = HandWritting::GetShape("line");
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

void SingleBond::setVertices(const v<Point> &pts) {
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
    const float intervalK = 0.1;
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::Point2f vec = from - to;
        auto length = distance(from, to);
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

DoubleBond::DoubleBond(const s &_bondType) : SingleBond(_bondType) {
}

void DoubleBond::updateShapes() {
    Bond::updateShapes();
    const float intervalK = 0.2;
    cv::Point2f vec = from - to;
    cv::Point2f vecT(0, 1);
    if (vec.y != 0) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    if (mUseHWChar) {
        auto length = distance(from, to);
        auto offset = vecT * length * intervalK;
        auto line1 = HandWritting::GetShape("line");
        line1.castToLine(from + offset / 2, to + offset / 2);
        auto line2 = HandWritting::GetShape("line");
        line2.castToLine(from - offset / 2, to - offset / 2);
        shapes.push_back(std::move(line1));
        shapes.push_back(std::move(line2));
    }
}

void TripleBond::paintTo(cv::Mat &canvas) {
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
        auto length = distance(from, to);
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

TripleBond::TripleBond(const s &_bondType) : SingleBond(_bondType) {
}

void TripleBond::updateShapes() {
    Bond::updateShapes();
    const float intervalK = 0.3;
    cv::Point2f vec = from - to;
    cv::Point2f vecT(0, 1);
    if (vec.y != 0) {
        vecT = cv::Point2f(-1, vec.x / vec.y);
        vecT /= cv::norm(vecT);
    }
    if (mUseHWChar) {
        auto length = distance(from, to);
        auto offset = vecT * length * intervalK;
        auto line1 = HandWritting::GetShape("line");
        line1.castToLine(from + offset / 2, to + offset / 2);
        auto line2 = HandWritting::GetShape("line");
        line2.castToLine(from, to);
        auto line3 = HandWritting::GetShape("line");
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
        auto length = distance(from, to);
        cv::Point2f vecT(0, 1);
        if (vec.y != 0) {
            vecT = cv::Point2f(-1, vec.x / vec.y);
            vecT /= cv::norm(vecT);
        }
        auto offset = vecT * length * intervalK;
        v<cv::Point> polygon = {from, to + offset / 2, to - offset / 2};
        cv::fillConvexPoly(canvas, polygon, RC::get_shape_color(),
                           RC::get_shape_lineType(),
                           RC::get_shape_shift());
    }
}

SolidWedgeBond::SolidWedgeBond(const s &_bondType) : SingleBond(_bondType) {
}

void SolidWedgeBond::updateShapes() {
    Bond::updateShapes();
    const float intervalK = 0.5;
    const int numOfSplit = 10;
    if (mUseHWChar) {
        cv::Point2f vec = from - to;
        auto length = distance(from, to);
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
            auto line = HandWritting::GetShape("line");
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
    const int numOfSplit = 10;
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::Point2f vec = from - to;
        auto length = distance(from, to);
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
            cv::line(canvas, Point (x1,y1),Point (x2,y2),
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

DashWedgeBond::DashWedgeBond(const s &_bondType) : SingleBond(_bondType) {
}

void DashWedgeBond::updateShapes() {
    Bond::updateShapes();
    const float intervalK = 0.5;
    const int numOfSplit = 10;
    if (mUseHWChar) {
        cv::Point2f vec = from - to;
        auto length = distance(from, to);
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
            auto line = HandWritting::GetShape("line");
            line.castToLine(cv::Point2f(x1, y1), cv::Point2f(x2, y2));
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

WaveBond::WaveBond(const s &_bondType) : SingleBond(_bondType) {
}

void WaveBond::updateShapes() {
    Bond::updateShapes();
    float cyclesOfSin = (rand() % 500) / 100.0 + 4;
    float end = M_PI * 2 * cyclesOfSin;
    Stroke stroke;
    // TODO: 这里存在一个随机要素
    for (float start = rand() % 100 / 100; start < end; start += 0.0520603) {
        stroke.push_back(cv::Point2f(start, 2 * std::sin(start)));
    }
    Shape shape;
    auto &script = shape.getData();
    script.push_back(std::move(stroke));
    shape.castToLine(from, to, 1);
    shapes.push_back(std::move(shape));
}
