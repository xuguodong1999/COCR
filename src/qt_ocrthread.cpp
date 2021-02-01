#include "qt_ocrthread.hpp"
#include "yolo.hpp"
#include "colors.hpp"
#include "mol_util.hpp"
#include "qt_util.hpp"
#include "opencv_util.hpp"
#include "soso17_converter.hpp"
#include <QDebug>
extern std::shared_ptr<YoloDetector> yoloDetector;
extern std::shared_ptr<MolUtil> molUtil;

void OCRThread::run() {
    mols.clear();
    cv::Mat image;
    if(!qImage.width()||!qImage.height()){
        return;
    }
    try {
        image = convertQImageToMat(qImage);
        auto round32_no_k = [&](const float &_num) -> int {
            int num2 = std::round(_num);
            num2 += (64 - num2 % 32);
            return (std::max)(minSize, (std::min)(maxSize, num2));
        };
        auto [image2,offset] = resizeCvMatTo(image,round32_no_k(image.cols),round32_no_k(image.rows));
        auto&[kSize,offsetx,offsety]=offset;
        image.release();
        std::swap(image,image2);
        // pre detection
//        yoloDetector->detectAndDisplay(image,CLASSES);
        auto objs = yoloDetector->detect(image);
        auto avgSize = getAvgObjectSize(objs);
        if(avgSize < 0){
            avgSize = 120;
        }
        float k = kSize * idealItemSize / avgSize;

        auto rect = script.getBoundingBox();
        float ow = rect->width;
        float oh = rect->height;
        auto round32 = [&](const float &_num) -> int {
            int num2 = std::round(_num * k);
            num2 += (64 - num2 % 32);
            return (std::max)(minSize, (std::min)(maxSize, num2));
        };
        int nw = round32(ow);
        int nh = round32(oh);
        image = cv::Mat(nh, nw, CV_8UC3, getScalar(
                ColorName::rgbWhite));
        script.resizeTo(nw - 16, nh - 16);
        script.moveCenterTo(cv::Point2f(nw / 2.0, nh / 2.0));
        script.paintTo(image);

        SOSO17Converter converter;
        // final detection
        converter.accept(image, yoloDetector->detect(image));
        auto mol = converter.getMol();
        if (mol && mol->atomsNum() > 0) {
            mols = molUtil->split(*mol);
        }
//        yoloDetector->detectAndDisplay(image,CLASSES);
    } catch (std::exception &e) {
        qDebug() << "exception in OCRThread::run: " << e.what();
    }
    image.release();
    script.clear();
}

std::vector<std::shared_ptr<JMol>> &OCRThread::getMols() {
    return mols;
}

OCRThread::OCRThread(QObject *_parent) : QThread(_parent) {

}

static HwController hw(2);

void OCRThread::setHwScript(HwScript _hwScript) {
    script = std::move(_hwScript);
    script.setHwController(hw);
}

void OCRThread::setQImage(QImage&&_qImage) {
    qImage = _qImage;
}
