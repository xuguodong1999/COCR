#include "bond.hpp"
#include "handwritting.hpp"
#include <opencv2/opencv.hpp>

std::shared_ptr<Bond> Bond::GetBond(const std::string &_bondType) {
    std::shared_ptr<Bond> bond;
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

void CircleBond::paintTo(cv::Mat &canvas) {
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::circle(canvas, center, r, color, thickness, lineType, shift);
    }
}

void SingleBond::paintTo(cv::Mat &canvas) {
    if (mUseHWChar) {
        for (auto &s:shapes) {
            s.paintTo(canvas);
        }
    } else {
        cv::line(canvas, from, to, color, thickness, lineType, shift);
    }
}

void DoubleBond::paintTo(cv::Mat &canvas) {
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
                 color, thickness, lineType, shift);
        cv::line(canvas, from - offset / 2, to - offset / 2,
                 color, thickness, lineType, shift);
    }
}

void TripleBond::paintTo(cv::Mat &canvas) {
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
                 color, thickness, lineType, shift);
        cv::line(canvas, from, to, color, thickness, lineType, shift);
        cv::line(canvas, from - offset / 2, to - offset / 2,
                 color, thickness, lineType, shift);
    }
}

void SolidWedgeBond::paintTo(cv::Mat &canvas) {
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
        std::vector<cv::Point> polygon = {from, to + offset / 2, to - offset / 2};
        cv::fillConvexPoly(canvas, polygon, color, lineType, shift);
    }
}

void DashWedgeBond::paintTo(cv::Mat &canvas) {
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
//        std::vector<cv::Point> polygon = {from, to + offset / 2, to - offset / 2};
//        cv::fillConvexPoly(canvas, polygon, color, lineType, shift);
    }
}

void WaveBond::paintTo(cv::Mat &canvas) {
    for (auto &s:shapes) {
        s.paintTo(canvas);
    }
}


CircleBond::CircleBond(const std::string &_bondType) : Bond(_bondType) {


}

SingleBond::SingleBond(const std::string &_bondType) : Bond(_bondType) {


}

DoubleBond::DoubleBond(const std::string &_bondType) : SingleBond(_bondType) {

}

TripleBond::TripleBond(const std::string &_bondType) : SingleBond(_bondType) {

}

SolidWedgeBond::SolidWedgeBond(const std::string &_bondType) : SingleBond(_bondType) {


}

DashWedgeBond::DashWedgeBond(const std::string &_bondType) : SingleBond(_bondType) {

}

WaveBond::WaveBond(const std::string &_bondType) : SingleBond(_bondType) {

}

void WaveBond::updateShapes() {
    shapes.clear();
    float cyclesOfSin = (rand() % 500) / 100.0 + 4;
    float end = M_PI * 2 * cyclesOfSin;
    Stroke stroke;
    // TODO: 这里存在一个随机要素
    for (float start = rand() % 100 / 100; start < end; start += 0.0520603) {
        stroke.push_back(cv::Point2f(start, 2 * std::sin(start)));
    }
    Shape shape;
    auto &script = shape.getData();
    script.emplace_back(std::move(stroke));
    shape.castToLine(from, to, 1);
    shapes.emplace_back(std::move(shape));
}

void DashWedgeBond::updateShapes() {
    const float intervalK = 0.5;
    const int numOfSplit = 10;

    shapes.clear();
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
            shapes.emplace_back(std::move(line));
            x1 += d[0];
            y1 += d[1];
            x2 += d[2];
            y2 += d[3];
        }
    }
}

void SolidWedgeBond::updateShapes() {
    const float intervalK = 0.5;
    const int numOfSplit = 10;

    shapes.clear();
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
            shapes.emplace_back(std::move(line));
            x += dx;
            y += dy;
        }
    }
}

void TripleBond::updateShapes() {
    const float intervalK = 0.3;
    shapes.clear();
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
        shapes.emplace_back(std::move(line1));
        shapes.emplace_back(std::move(line2));
        shapes.emplace_back(std::move(line3));
    }
}

void DoubleBond::updateShapes() {
    const float intervalK = 0.2;

    shapes.clear();
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
        shapes.emplace_back(std::move(line1));
        shapes.emplace_back(std::move(line2));
    }
}

void SingleBond::updateShapes() {
    shapes.clear();
    if (mUseHWChar) {
        auto line = HandWritting::GetShape("line");
        line.castToLine(from, to);
        shapes.emplace_back(std::move(line));
    }
}

void CircleBond::updateShapes() {
    shapes.clear();
    if (mUseHWChar) {
        auto circle = HandWritting::GetShape("circle");
        circle.castToCircle(center, r, r);
        shapes.emplace_back(std::move(circle));
    }
}