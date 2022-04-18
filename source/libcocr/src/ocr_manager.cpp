#include "cocr/ocr_manager.h"
#include "ocv/algorithm.h"
#include <QDebug>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

OCRManager::OCRManager(ObjectDetector &_detector, TextRecognizer &_recognizer,
                       TextCorrector &_corrector, GraphComposer &_composer)
        : detector(_detector), recognizer(_recognizer), corrector(_corrector), composer(_composer),
          image(MatChannel::GRAY, DataType::UINT8, 1, 1) {
    qDebug() << "OCRManager::OCRManager";
}

std::shared_ptr<GuiMol> OCRManager::ocr(Mat &_originInput, bool _debug) {
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

std::vector<OCRItem> OCRManager::convert(
        const std::vector<DetectorObject> &_objects, const Mat &_input) {
    int width = _input.getWidth(), height = _input.getHeight();
    auto round_scale = [&](const float &_x, const float &_y, const float &_w, const float &_h) -> recti {
        int x, y, w, h;
        int sx = 4, sy = 2;
        x = std::max(0, (int) std::round(_x - sx));
        y = std::max(0, (int) std::round(_y - sy));
        w = std::min(width - 1, (int) std::round(_x + _w + 2 * sx)) - x;
        h = std::min(height - 1, (int) std::round(_y + _h + 2 * sy)) - y;
        return {point2i{x, y}, point2i{x + w, y + h}};
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
                item.setAsLineBond(DetectorUtil::toBondType(obj.label), obj.asRect(), _input);
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

void OCRManager::display(const std::vector<OCRItem> &_items, const Mat &_input) {
//    Mat canvas;
//    cv::cvtColor(_input, canvas, cv::COLOR_GRAY2BGR);
//    auto move_text_box = [&](const cv::Point2f &_pts) -> cv::Point {
//        const int offset = 10;
//        int x = _pts.x - offset, y = _pts.y;
//        if (x < 0)x = 0;
//        if (y < offset) { y += offset; }
//        return cv::Point2f(x, y);
//    };
//    for (auto &item: _items) {
//        const auto &rect = item.getRect();
//        cv::Scalar color;
//        std::string info;
//        switch (item.type) {
//            case OCRItemType::Element: {
//                color = getScalar(ColorName::rgbRed);
//                break;
//            }
//            case OCRItemType::Group: {
//                color = getScalar(ColorName::rgbBlue);
//                break;
//            }
//            case OCRItemType::Line: {
//                static std::unordered_map<BondType, ColorName> mm = {
//                        {BondType::SingleBond,     ColorName::rgbDarkRed},
//                        {BondType::DoubleBond,     ColorName::rgbDarkGreen},
//                        {BondType::TripleBond,     ColorName::rgbDarkGrey},
//                        {BondType::WaveBond,       ColorName::rgbDarkGoldenrod},
//                        {BondType::SolidWedgeBond, ColorName::rgbDarkOliveGreen},
//                        {BondType::DashWedgeBond,  ColorName::rgbDarkKhaki}
//                };
//                color = getScalar(mm[item.getBondType()]);
//                cross_line(canvas, item.getFrom(), 5, getScalar(ColorName::rgbPink), 2, true);
//                cross_line(canvas, item.getTo(), 5, getScalar(ColorName::rgbDeepPink), 2, true);
//                break;
//            }
//            case OCRItemType::Circle: {
//                color = getScalar(ColorName::rgbBlue);
//                cross_line(canvas, item.getCenter(), 5, getScalar(ColorName::rgbRed), 2, true);
//                break;
//            }
//            default: {
//                throw std::runtime_error("OCRManager::display: unknown item.type");
//            }
//        }
//        info = item.getText();
////        if (item.type == OCRItemType::Element || item.type == OCRItemType::Group) {
////            info = "string";
////        }
//        cv::putText(canvas, info, move_text_box(item.getRect().tl()),
//                    1, 1.5, color,
//                    2, cv::LINE_AA, false);
//        cv::rectangle(canvas, rect, color, 1, cv::LINE_AA);
//    }
//    cv::imshow("OCRManager::display", canvas);
}


Mat &OCRManager::getImage() {
    return image;
}

void OCRManager::setImage(const QList<QList<QPointF>> &_script, const int &screenWidth) {
    if (_script.empty()) {
        image = Mat(MatChannel::GRAY, DataType::UINT8, 32, 32);
        return;
    }
    qreal minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<qreal>::max();
    maxx = maxy = std::numeric_limits<qreal>::lowest();

    for (auto &pts: _script) {
        for (auto &pt: pts) {
            minx = (std::min)(minx, pt.x());
            miny = (std::min)(miny, pt.y());
            maxx = (std::max)(maxx, pt.x());
            maxy = (std::max)(maxy, pt.y());
        }
    }
    const int padding = 16;
    float scale = screenWidth / 720.f;
    if (scale < 1)scale = 1;
    if (scale > 2)scale = 2;
    int width = (std::min)(static_cast<int>(maxx - minx), MAX_WIDTH) / scale,
            height = (std::min)(static_cast<int>(maxy - miny ), MAX_WIDTH) / scale;
    float kx = static_cast<float>(width) / (maxx - minx);
    float ky = static_cast<float>(height) / (maxy - miny);
    width += padding * 2;
    height += padding * 2;
    qDebug() << "kx=" << kx << ",ky=" << ky << ",scale=" << scale;
    std::vector<std::vector<point2f>> ptsVec(_script.size());
    for (size_t i = 0; i < ptsVec.size(); i++) {
        ptsVec[i].resize(_script[i].size());
        for (size_t j = 0; j < ptsVec[i].size(); j++) {
            ptsVec[i][j].first = padding + kx * (_script[i][j].x() - minx);
            ptsVec[i][j].second = padding + ky * (_script[i][j].y() - miny);
        }
    }
    image = Mat(MatChannel::GRAY, DataType::UINT8, width, height);
    for (auto &pts: ptsVec) {
        if (pts.size() == 1) {
            image.drawLine(pts[0], pts[0], ColorUtil::GetRGB(ColorName::rgbBlack), 2);
        } else if (pts.size() > 1) {
            for (size_t i = 1; i < pts.size(); i++) {
                image.drawLine(pts[i], pts[i - 1], ColorUtil::GetRGB(ColorName::rgbBlack), 2);
            }
        }
    }
}

void OCRManager::setImage(const QImage &_image) {
    Mat mat(_image, MatChannel::GRAY, DataType::UINT8);
    setImage(mat);
}

void OCRManager::setImage(const QPixmap &_pixmap) {
    Mat mat(_pixmap.toImage(), MatChannel::GRAY, DataType::UINT8);
    setImage(mat);
}

void OCRManager::setImage(const Mat &_cvMat) {
    image = _cvMat;
    CvUtil::Rotate(_cvMat, 2);
}

void OCRManager::clearImage() {
    image.clear();
}