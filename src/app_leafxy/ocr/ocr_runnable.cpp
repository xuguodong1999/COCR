#include "ocr_runnable.hpp"
#include <opencv2/opencv_modules.hpp>

#if defined(HAVE_OPENCV_DNN) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)// && !defined(WITH_MODEL_QRC)

#include "opencv_dnn_impl/object_detector_opencv_impl.hpp"
//#include "opencv_dnn_impl/text_recognizer_opencv_impl.hpp"// unused, only for performance test

#else

#include "ncnn_impl/object_detector_ncnn_impl.hpp"

#endif

#include "ncnn_impl/text_recognizer_ncnn_impl.hpp"
#include "ocr_manager.hpp"
#include "opencv_util.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <opencv2/highgui.hpp>

class OCRRunnablePrivate {
    cv::Mat image;
    inline static const int MAX_WIDTH = 960;
    inline static const auto BLACK = cv::Scalar(0, 0, 0), WHITE = cv::Scalar(255, 255, 255);
public:
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
        cv::Mat cvMat = xgd::convertQImageToMat(_image);
        setImage(cvMat);
    }

    void setImage(const QPixmap &_pixmap) {
        cv::Mat cvMat = xgd::convertQPixmapToMat(_pixmap);
        setImage(cvMat);
    }

    void setImage(cv::Mat &_cvMat) {
//        cv::erode(image,image, cv::Mat());
//        cv::dilate(image,image, cv::Mat());
        image = _cvMat;
        copyMakeBorder(image, image, 32, 32, 32, 32, cv::BORDER_CONSTANT, 255);
        image = xgd::rotateCvMat(image, 2);
    }

    void clearImage() {
        image = cv::Mat();
    }
};

OCRThread::OCRThread(QObject *_parent, const QString &_dir)
        : QThread(_parent), _p(std::make_shared<OCRRunnablePrivate>()) {
    const char *ncnnTextModel = ":/models/ncnn/text_57/vgg_lstm_57_fp16_mixFont.bin";
    const char *ncnnTextModelCfg = ":/models/ncnn/text_57/vgg_lstm_57_fp16.param";
    const char *ncnnDetModel = ":/models/ncnn/det_8/yolo_3l_c8.bin";
    const char *ncnnDetModelCfg = ":/models/ncnn/det_8/yolo_3l_c8.param";
    const char *ocvDetModel = ":/models/darknet/det_8/yolo-3l-c8.weights";
    const char *ocvDetModelCfg = ":/models/darknet/det_8/yolo-3l-c8.cfg";
/// detector
#if defined(HAVE_OPENCV_DNN) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)// && !defined(WITH_MODEL_QRC)
    static xgd::ObjectDetectorOpenCVImpl detector;
    if (!detector.initModel(ocvDetModelCfg, ocvDetModel)) {
        qDebug() << "fail to init opencv detector";
    }
    detector.setConfThresh(0.25);
    detector.setIouThresh(0.45);
#else
    static xgd::ObjectDetectorNcnnImpl detector;
    detector.setNumThread(4);
    if (!detector.initModel(ncnnDetModel, ncnnTextModelCfg, 1280)) {
        qDebug() << "fail to init ncnn detector";
    }
#endif
    /// recognizer
    static xgd::TextRecognizerNcnnImpl recognizer;
    if (!recognizer.initModel(ncnnTextModel, ncnnTextModelCfg,
                              xgd::TextCorrector::GetAlphabet(), 3200)) {
        qDebug() << "fail to init ncnn recognizer";
    }
//    /// recognizer by opencv_dnn
//    static xgd::TextRecognizerOpenCVImpl recognizer;
//    if (!recognizer.initModel((modelDir.absolutePath()+ +"/crnn_192_mix_sim.onnx").toStdString(),
//                              xgd::TextCorrector::GetAlphabet(), 192)) {
//        qDebug() << "fail to init opencv recognizer";
//    }
    /// corrector
    static xgd::TextCorrector corrector;
    corrector.InitData();
    /// composer
    static xgd::GraphComposer composer;
    /// manager
    ocrManager = std::make_shared<xgd::OCRManager>(detector, recognizer, corrector, composer);
}

OCRThread::~OCRThread() {

}

void OCRThread::run() {
    try {
//        QThread::msleep(2000);
#ifdef Q_OS_WIN
        //        bool debug = true;
                bool debug = false;
#else
        bool debug = false;
#endif
        mol = ocrManager->ocr(_p->getImage(), debug);
    } catch (std::exception &e) {
        qDebug() << __FUNCTION__ << "catch" << e.what();
        mol = nullptr;
    }
    if (mol) {
        emit sig_mol_ready();
    }
}

std::shared_ptr<xgd::JMol> OCRThread::getMol() {
    return mol;
}

void OCRThread::bindData(const QList<QList<QPointF>> &_script) {
    _p->setImage(_script);
}

void OCRThread::bindData(const QImage &_image) {
    _p->setImage(_image);
}

void OCRThread::bindData(const QPixmap &_pixmap) {
    _p->setImage(_pixmap);
}

void OCRThread::bindData(cv::Mat &_cvMat) {
    _p->setImage(_cvMat);
}
