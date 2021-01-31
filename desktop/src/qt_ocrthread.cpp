#include "qt_ocrthread.hpp"

#include "yolo.hpp"
#include "colors.hpp"
#include "mol_util.hpp"
#include "soso17_converter.hpp"
#include <QDebug>
#include <opencv2/highgui.hpp>

extern std::shared_ptr<YoloDetector> yoloDetector;
extern std::shared_ptr<MolUtil> molUtil;

void OCRThread::run() {
    QThread::currentThread()->msleep(2000);
    try {
        float avgSize = script.getAvgMaxSize();
        auto rect = script.getBoundingBox();
        float ow = rect->width, oh = rect->height;
        float k = idealItemSize / avgSize;
        auto round32 = [&](const float &_num) -> int {
            int num2 = std::round(_num * k);
            num2 += (32 - num2 % 32);
            return (std::max)(minSize, (std::min)(maxSize, num2));
        };
        int nw = round32(ow);
        int nh = round32(oh);
        cv::Mat image(nh, nw, CV_8UC3, getScalar(ColorName::rgbWhite));
        script.resizeTo(nw - 8, nh - 8);
        script.moveCenterTo(cv::Point2f(nw / 2.0, nh / 2.0));
        script.paintTo(image);
        script.clear();
        qDebug() << "OCRThread::run, size=" << image.cols << "x" << image.rows;
//        cv::imshow("fuck",image);
        SOSO17Converter converter;
        converter.accept(image, yoloDetector->detect(image));
        auto mol = converter.getMol();
        if (mol && mol->atomsNum() > 0) {
            mols = molUtil->split(*mol);
        } else {
            mols.clear();
        }
    } catch (std::exception &e) {
        mols.clear();
        script.clear();
        qDebug() << "exception in OCRThread::run: " << e.what();
    }
}

std::vector<std::shared_ptr<JMol>> &OCRThread::getMols() {
    return mols;
}

OCRThread::OCRThread(QObject *_parent) : QThread(_parent) {

}

void OCRThread::setHwScript(HwScript _hwScript) {
    script = std::move(_hwScript);
}
