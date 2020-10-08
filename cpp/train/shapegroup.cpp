#include "shapegroup.hpp"
#include "statistic.hpp"
#include "handwritting.hpp"


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

p<ShapeGroup> ShapeGroup::GetShapeGroup(const s &_textType) {
    p<ShapeGroup> text;
    if (_textType == "positive") {
        auto sym = HandWritting::GetByStrLabel("⊕");
        text = std::make_shared<ShapeGroup>();
        text->shapes.emplace_back(std::move(sym));
    } else if (_textType == "negative") {
        auto sym1 = HandWritting::GetShape("circle");
        sym1.resizeTo(100, 100);
        sym1.moveCenterTo(cv::Point2f(50, 50));
        auto sym2 = HandWritting::GetByStrLabel("-");
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
    auto sym = HandWritting::GetByStrLabel(c.second);
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