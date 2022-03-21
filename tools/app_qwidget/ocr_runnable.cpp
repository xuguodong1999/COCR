#include "ocr_runnable.hpp"
#include "ocr/text_corrector.hpp"
#include "ocr/graph_composer.hpp"
#include "ocr/text_recognizer.hpp"
#include "ocr/object_detector.hpp"

#include "opencv_util/opencv_util.hpp"
#include "util.h"

#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv_modules.hpp>

#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

namespace cocr {
    class OCRManager {
        cv::Mat image;
        inline static const int MAX_WIDTH = 960;
        inline static const auto BLACK = cv::Scalar(0, 0, 0), WHITE = cv::Scalar(255, 255, 255);
        TextRecognizer &recognizer;
        ObjectDetector &detector;
        GraphComposer &composer;
        TextCorrector &corrector;

        void display(const std::vector<OCRItem> &_items, const cv::Mat &_input);

        std::vector<OCRItem> convert(const std::vector<DetectorObject> &_objects, const cv::Mat &_input);

    public:
        OCRManager(ObjectDetector &_detector, TextRecognizer &_recognizer, TextCorrector &_corrector,
                   GraphComposer &_composer);

        std::shared_ptr<JMol> ocr(cv::Mat &_originInput, bool _debug);

        cv::Mat &getImage() {
            return image;
        }

        void setImage(const QList<QList<QPointF>> &_script) {
            if (_script.empty()) {
                image = cv::Mat(32, 32, CV_8UC1, WHITE);
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
            float scale = QApplication::desktop()->width() / 720.f;
            if (scale < 1)scale = 1;
            if (scale > 2)scale = 2;
            int width = (std::min)(static_cast<int>(maxx - minx), MAX_WIDTH) / scale,
                    height = (std::min)(static_cast<int>(maxy - miny ), MAX_WIDTH) / scale;
            float kx = static_cast<float>(width) / (maxx - minx);
            float ky = static_cast<float>(height) / (maxy - miny);
            width += padding * 2;
            height += padding * 2;
            qDebug() << "kx=" << kx << ",ky=" << ky << ",scale=" << scale;
            std::vector<std::vector<cv::Point2f>> ptsVec(_script.size());
            for (size_t i = 0; i < ptsVec.size(); i++) {
                ptsVec[i].resize(_script[i].size());
                for (size_t j = 0; j < ptsVec[i].size(); j++) {
                    ptsVec[i][j].x = padding + kx * (_script[i][j].x() - minx);
                    ptsVec[i][j].y = padding + ky * (_script[i][j].y() - miny);
                }
            }
            image = cv::Mat(height, width, CV_8UC1, WHITE);
            for (auto &pts: ptsVec) {
                if (pts.size() == 1) {
                    cv::line(image, pts[0], pts[0], BLACK, 2, cv::LINE_AA);
                } else if (pts.size() > 1) {
                    for (size_t i = 1; i < pts.size(); i++) {
                        cv::line(image, pts[i], pts[i - 1], BLACK, 2, cv::LINE_AA);
                    }
                }
            }
        }

        void setImage(const QImage &_image) {
            cv::Mat cvMat = cocr::convertQImageToMat(_image);
            setImage(cvMat);
        }

        void setImage(const QPixmap &_pixmap) {
            cv::Mat cvMat = cocr::convertQPixmapToMat(_pixmap);
            setImage(cvMat);
        }

        void setImage(cv::Mat &_cvMat) {
//        cv::erode(image,image, cv::Mat());
//        cv::dilate(image,image, cv::Mat());
            image = _cvMat;
            copyMakeBorder(image, image, 32, 32, 32, 32, cv::BORDER_CONSTANT, 255);
            image = rotateCvMat(image, 2);
        }

        void clearImage() {
            image = cv::Mat();
        }
    };
}

inline static cocr::BondType toBondType(const cocr::DetectorObjectType &_objType) {
    using namespace cocr;
    switch (_objType) {
        case DetectorObjectType::SingleLine :
            return cocr::BondType::SingleBond;
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

cocr::OCRManager::OCRManager(cocr::ObjectDetector &_detector, cocr::TextRecognizer &_recognizer,
                             TextCorrector &_corrector, GraphComposer &_composer)
        : detector(_detector), recognizer(_recognizer), corrector(_corrector), composer(_composer) {
    qDebug() << "OCRManager::OCRManager";
}

std::shared_ptr<cocr::JMol> cocr::OCRManager::ocr(cv::Mat &_originInput, bool _debug) {
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

std::vector<cocr::OCRItem>
cocr::OCRManager::convert(const std::vector<DetectorObject> &_objects, const cv::Mat &_input) {
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

void cocr::OCRManager::display(const std::vector<OCRItem> &_items, const cv::Mat &_input) {
    cv::Mat canvas;
    cv::cvtColor(_input, canvas, cv::COLOR_GRAY2BGR);
    auto move_text_box = [&](const cv::Point2f &_pts) -> cv::Point {
        const int offset = 10;
        int x = _pts.x - offset, y = _pts.y;
        if (x < 0)x = 0;
        if (y < offset) { y += offset; }
        return cv::Point2f(x, y);
    };
    for (auto &item: _items) {
        const auto &rect = item.getRect();
        cv::Scalar color;
        std::string info;
        switch (item.type) {
            case OCRItemType::Element: {
                color = getScalar(ColorName::rgbRed);
                break;
            }
            case OCRItemType::Group: {
                color = getScalar(ColorName::rgbBlue);
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
                color = getScalar(mm[item.getBondType()]);
                cross_line(canvas, item.getFrom(), 5, getScalar(ColorName::rgbPink), 2, true);
                cross_line(canvas, item.getTo(), 5, getScalar(ColorName::rgbDeepPink), 2, true);
                break;
            }
            case OCRItemType::Circle: {
                color = getScalar(ColorName::rgbBlue);
                cross_line(canvas, item.getCenter(), 5, getScalar(ColorName::rgbRed), 2, true);
                break;
            }
            default: {
                throw std::runtime_error("cocr::OCRManager::display: unknown item.type");
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
    cv::imshow("cocr::OCRManager::display", canvas);
}

OCRThread::OCRThread(QObject *_parent)
        : QThread(_parent) {
    qDebug() << "OCRThread::OCRThread";
    static auto detector = cocr::ObjectDetector::MakeInstance();
    static auto recognizer = cocr::TextRecognizer::MakeInstance();
    static cocr::TextCorrector corrector;
    static cocr::GraphComposer composer;
    if (detector && recognizer) {
        ocrManager = std::make_shared<cocr::OCRManager>(
                *detector, *recognizer, corrector, composer);
    }
}

void OCRThread::run() {
    try {
        mol = ocrManager->ocr(ocrManager->getImage(), false);
    } catch (std::exception &e) {
        qDebug() << __FUNCTION__ << "catch" << e.what();
        mol = nullptr;
    }
    if (mol) {
        emit sig_mol_ready();
    }
}

std::shared_ptr<cocr::JMol> OCRThread::getMol() {
    return mol;
}

void OCRThread::bindData(const QList<QList<QPointF>> &_script) {
    ocrManager->setImage(_script);
}

void OCRThread::bindData(const QImage &_image) {
    ocrManager->setImage(_image);
}

void OCRThread::bindData(const QPixmap &_pixmap) {
    ocrManager->setImage(_pixmap);
}

void OCRThread::bindData(cv::Mat &_cvMat) {
    ocrManager->setImage(_cvMat);
}
