//
// Created by xgd on 2020/9/18.
//

#include "symbol.hpp"
#include "hwchar.hpp"


void Text::setSNormSizeK(float sNormSizeK) {
    RC::sNormSizeK = sNormSizeK;
}

void Text::setSSubSizeK(float sSubSizeK) {
    RC::sSubSizeK = sSubSizeK;
}

void Text::setSNormOffsetKx(float sNormOffsetKx) {
    RC::sNormOffsetKx = sNormOffsetKx;
}

void Text::setSNormOffsetKy(float sNormOffsetKy) {
    RC::sNormOffsetKy = sNormOffsetKy;
}

void Text::setSSubOffsetKx(float sSubOffsetKx) {
    RC::sSubOffsetKx = sSubOffsetKx;
}

void Text::setSSubOffsetKy(float sSubOffsetKy) {
    RC::sSubOffsetKy = sSubOffsetKy;
}

void Text::setSAngleK(float sAngleK) {
    RC::sAngleK = sAngleK;
}

std::shared_ptr<Text> Text::GetSymbol(const std::string &_symbolType) {
    std::shared_ptr<Text> symbol;
    if (_symbolType == "positive") {
        auto sym = HWChar::GetByStrLabel("⊕");
        symbol = std::make_shared<Text>();
        symbol->shapes.emplace_back(std::move(sym));
    } else if (_symbolType == "negative") {
        auto sym1 = HWChar::GetShape("circle");
        sym1.resizeTo(100, 100);
        sym1.moveCenterTo(cv::Point2f(50, 50));
        auto sym2 = HWChar::GetByStrLabel("-");
        // TODO: 分离随机要素
        sym2.resizeTo(80, 80);
        sym2.moveCenterTo(cv::Point2f(50, 50));
        symbol = std::make_shared<Text>();
        symbol->shapes.emplace_back(std::move(sym1));
        symbol->shapes.emplace_back(std::move(sym2));
    } else {
        symbol = std::make_shared<Text>(makeNotedStringByChar(_symbolType));
    }
    return symbol;
}

void Text::append(const NChar &c) {
    auto sym = HWChar::GetByStrLabel(c.second);
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

void Text::resizeTo(float w, float h, bool keepRatio) {
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

void Text::moveLeftTopTo(const cv::Point2f &leftTop) {
    cv::Rect2f bBox = getBoundingBox();
    cv::Point2f offset = -bBox.tl() + leftTop;
    move(offset);
}

void Text::moveCenterTo(const cv::Point2f &newCenter) {
//    cv::Rect2f bBox = getBoundingBox();
//    cv::Point2f oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    auto oldCenter = getCenter();
//    std::cout<<"in Symbol::moveCenterTo: old cent="<<oldCenter<<std::endl;
    cv::Point2f offset = newCenter - oldCenter;
    move(offset);
}

void Text::move(const cv::Point2f &offset) {
    for (auto &s:shapes) {
        s.move(offset);
    }
}

void Text::rotate(float angle) {
    float theta = -M_PI * angle / 180.0;
    auto oldCenter = getCenter();
//    std::cout<<"in Symbol::rotate: old cent="<<oldCenter<<std::endl;
//    moveCenterTo(cv::Point2f(0, 0));
    for (auto &s:shapes) {
//        s.rotateTheta(theta);
        s.rotateThetaBy(theta, oldCenter);
    }
//    moveCenterTo(oldCenter);
}

void Text::rotateBy(float angle, const cv::Point2f &cent) {
    float theta = -M_PI * angle / 180;
    for (auto &s:shapes) {
        s.rotateThetaBy(theta, cent);
    }
}

const cv::Rect2f Text::getBoundingBox() const {
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
    return cv::Rect2f(minx, miny, maxx - minx, maxy - miny);
}

void Text::paintTo(cv::Mat &canvas) {
    for (auto &shape:shapes) {
        shape.paintTo(canvas);
    }
}