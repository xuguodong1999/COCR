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
    try {
        float avgSize = script.getAvgMaxSize();
        auto rect = script.getBoundingBox();
        float ow = rect->width, oh = rect->height;
        float k = idealItemSize / avgSize;
        auto round32 = [&](const float &_num) -> int {
            int num2 = std::round(_num * k);
            num2 += (64 - num2 % 32);
            return (std::max)(minSize, (std::min)(maxSize, num2));
        };
        int nw = round32(ow);
        int nh = round32(oh);
        auto image=cv::Mat(nh, nw, CV_8UC3, getScalar(
                ColorName::rgbWhite));
        script.resizeTo(nw - 16, nh - 16);
        script.moveCenterTo(cv::Point2f(nw / 2.0, nh / 2.0));
        script.paintTo(image);
        SOSO17Converter converter;
        auto objs=yoloDetector->detect(image);
        k= idealItemSize /getAvgObjectSize(objs);
        nw = round32(nw);
        nh = round32(nh);
        image=cv::Mat(nh, nw, CV_8UC3, getScalar(
                ColorName::rgbWhite));
        script.resizeTo(nw - 16, nh - 16);
        script.moveCenterTo(cv::Point2f(nw / 2.0, nh / 2.0));
        script.paintTo(image);
        converter.accept(image, yoloDetector->detect(image));
        auto mol = converter.getMol();
        if (mol && mol->atomsNum() > 0) {
            mols = molUtil->split(*mol);
        } else {
            mols.clear();
        }
//        yoloDetector->detectAndDisplay(image,CLASSES);
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

static HwController hw(1);

void OCRThread::setHwScript(HwScript _hwScript) {
    script = std::move(_hwScript);
    script.setHwController(hw);
    script.rotate(3);
}
