#include "ocr_manager.hpp"
#include "color_types.hpp"

#include "detector_object.hpp"
#include "../chem/jbond.hpp"

#include "opencv_util.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>

inline static xgd::BondType toBondType(const xgd::DetectorObjectType &_objType) {
    using namespace xgd;
    switch (_objType) {
        case DetectorObjectType::SingleLine :
            return xgd::BondType::SingleBond;
        case DetectorObjectType::DoubleLine  :
            return BondType::DoubleBond;
        case DetectorObjectType::TripleLine  :
            return BondType::TripleBond;
        case DetectorObjectType::SolidWedge  :
            return BondType::UpBond;
        case DetectorObjectType::DashWedge  :
            return BondType::DownBond;
        case DetectorObjectType::WaveLine  :
            return BondType::ImplicitBond;
        default: {
            throw std::runtime_error("toBondType: unknown bondType");
        }
    }
}

xgd::OCRManager::OCRManager(xgd::ObjectDetector &_detector, xgd::TextRecognizer &_recognizer,
                            TextCorrector &_corrector, GraphComposer &_composer)
        : detector(_detector), recognizer(_recognizer), corrector(_corrector), composer(_composer) {

}

std::shared_ptr<xgd::JMol> xgd::OCRManager::ocr(cv::Mat &_originInput, bool _debug) {
    std::vector<OCRItem> items;
    try {
        auto[input, objects]=detector.detect(_originInput);
        items = convert(objects, input);
        if (_debug) {
            display(items, input);
        }
    } catch (std::exception &e) {
        qDebug() << __FUNCTION__ << "detector and convert catch" << e.what();
        return nullptr;
    }
    try {
        auto mol = composer.compose(items);
        mol->set2DInfoLatest(false);
        return mol;
    } catch (std::exception &e) {
        qDebug() << __FUNCTION__ << "compose catch" << e.what();
        return nullptr;
    }
}

std::vector<xgd::OCRItem> xgd::OCRManager::convert(const std::vector<DetectorObject> &_objects, const cv::Mat &_input) {
    int width = _input.cols, height = _input.rows;
    auto round_scale = [&](const float &_x, const float &_y, const float &_w, const float &_h) -> cv::Rect {
        int x, y, w, h;
        int sx = 4, sy = 2;
        x = std::max(0, (int) std::round(_x - sx));
        y = std::max(0, (int) std::round(_y - sy));
        w = std::min(width - 1, (int) std::round(_x + _w + 2 * sx)) - x;
        h = std::min(height - 1, (int) std::round(_y + _h + 2 * sy)) - y;
        return cv::Rect(x, y, w, h);
    };
    std::vector<OCRItem> items(_objects.size());
    for (size_t i = 0; i < _objects.size(); i++) {
        const auto &obj = _objects[i];
        auto &item = items[i];
        item.setUId(i);
        switch (obj.label) {
            case DetectorObjectType::SingleLine :
            case DetectorObjectType::DoubleLine :
            case DetectorObjectType::TripleLine:
            case DetectorObjectType::WaveLine :
            case DetectorObjectType::SolidWedge :
            case DetectorObjectType::DashWedge : {
                item.setAsLineBond(toBondType(obj.label), obj.asRect(), _input);
                break;
            }
            case DetectorObjectType::Circle : {
                item.setAsCircleBond(obj.asRect());
                break;
            }
            case DetectorObjectType::Text : {
                auto[text, scores]=recognizer.recognize(_input(round_scale(obj.x(), obj.y(), obj.w(), obj.h())));
//                qDebug() << "text=" << text.c_str();
                text = corrector.correct(text);
                item.setAsText(text, obj.asRect());
                break;
            }
            default: {
                throw std::runtime_error("obj.label: unknown DetectorObjectType");
            }
        }
    }
    return items;
}

void xgd::OCRManager::display(const std::vector<OCRItem> &_items, const cv::Mat &_input) {
    cv::Mat canvas;
    cv::cvtColor(_input, canvas, cv::COLOR_GRAY2BGR);
    auto move_text_box = [&](const cv::Point2f &_pts) -> cv::Point {
        const int offset = 10;
        int x = _pts.x - offset, y = _pts.y;
        if (x < 0)x = 0;
        if (y < offset) { y += offset; }
        return cv::Point2f(x, y);
    };
    for (auto &item:_items) {
        const auto &rect = item.getRect();
        cv::Scalar color;
        std::string info;
        switch (item.type) {
            case OCRItemType::Element: {
                color = cvColor(ColorName::rgbRed);
                break;
            }
            case OCRItemType::Group: {
                color = cvColor(ColorName::rgbBlue);
                break;
            }
            case OCRItemType::Line: {
                static std::unordered_map<BondType, ColorName> mm = {
                        {BondType::SingleBond,   ColorName::rgbDarkRed},
                        {BondType::DoubleBond,   ColorName::rgbDarkGreen},
                        {BondType::TripleBond,   ColorName::rgbDarkGrey},
                        {BondType::ImplicitBond, ColorName::rgbDarkGoldenrod},
                        {BondType::UpBond,       ColorName::rgbDarkOliveGreen},
                        {BondType::DownBond,     ColorName::rgbDarkKhaki}
                };
                color = cvColor(mm[item.getBondType()]);
                xgd::cross_line(canvas, item.getFrom(), 5, cvColor(ColorName::rgbPink), 2, true);
                xgd::cross_line(canvas, item.getTo(), 5, cvColor(ColorName::rgbDeepPink), 2, true);
                break;
            }
            case OCRItemType::Circle: {
                color = cvColor(ColorName::rgbBlue);
                xgd::cross_line(canvas, item.getCenter(), 5, cvColor(ColorName::rgbRed), 2, true);
                break;
            }
            default: {
                throw std::runtime_error("xgd::OCRManager::display: unknown item.type");
            }
        }
        info = item.getText();
//        if (item.type == OCRItemType::Element || item.type == OCRItemType::Group) {
//            info = "string";
//        }
        cv::putText(canvas, info, move_text_box(item.getRect().tl()),
                    1, 1.5, color,
                    2, cv::LINE_AA, false);
        cv::rectangle(canvas, rect, color, 1, cv::LINE_AA);
    }
    cv::imshow("xgd::OCRManager::display", canvas);
}
