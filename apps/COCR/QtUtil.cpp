#include "QtUtil.h"
#include "XGD/Chem/Bond.h"
//#include <opencv2/core/mat.hpp>
//#include <opencv2/imgproc.hpp>
#include <iostream>
#include <QObject>

//cv::Mat convertQImageToMat(const QImage &_img) {
//    cv::Mat mat;
//    if (_img.isNull()) {
//        std::cerr << "you are converting an empty QImage to cv::Mat" << std::endl;
//        exit(-1);
//    }
//    void *dataPtr = const_cast<uchar *>(_img.constBits());
//    int step = _img.bytesPerLine();
//    switch (_img.format()) {
//        case QImage::Format_RGB32:
//        case QImage::Format_ARGB32:
//        case QImage::Format_ARGB32_Premultiplied:
//            mat = cv::Mat(_img.height(), _img.width(), CV_8UC4, dataPtr, step);
//            break;
//        case QImage::Format_RGB888:
//            mat = cv::Mat(_img.height(), _img.width(), CV_8UC3, dataPtr, step);
//            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
//            break;
//        case QImage::Format_Indexed8:
//        case QImage::Format_Grayscale8:
//            mat = cv::Mat(_img.height(), _img.width(), CV_8UC1, dataPtr, step);
//            break;
//        default: {
//            std::cerr << "unImpl image format:" << _img.format() << std::endl;
//            exit(-1);
//        }
//    }
////    std::cout << mat.cols << "x" << mat.rows << std::endl;
//    return std::move(mat);
//}

//cv::Mat convertQPixmapToMat(const QPixmap &_img) {
//    return convertQImageToMat(_img.toImage());
//}

QString getBondName(const Bond &bond) {
    switch (bond.getType()) {
        case BondType::SingleBond:
            return QObject::tr("Single");
        case BondType::DoubleBond:
            return QObject::tr("Double");
        case BondType::TripleBond:
            return QObject::tr("Triple");
        case BondType::DelocalizedBond:
            return QObject::tr("Delocalized");
        case BondType::WaveBond:
            return QObject::tr("Implicit");
        case BondType::SolidWedgeBond:
            return QObject::tr("UpBond");
        case BondType::DashWedgeBond:
            return QObject::tr("Down");
        default:
            return QObject::tr("Unknown");
    }
}

//QImage binaryAlphaImage(cv::Mat &src) {
//    cv::Mat result;
//    cv::cvtColor(src, result, cv::COLOR_RGBA2GRAY);
//    cv::adaptiveThreshold(result, result, 255.0,
//                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
//                          cv::THRESH_BINARY, 21, 10);
//    cv::cvtColor(result, result, cv::COLOR_GRAY2BGRA);
//    const uchar *pSrc = (const uchar *) result.data;
//    // Create QImage with same dimensions as input Mat
//    QImage image(pSrc, result.cols, result.rows, result.step, QImage::Format_ARGB32);
//    return image.copy();
//}

QColor getColor(const ColorName &color) {
    const auto &[r, g, b] = ColorUtil::GetRGB(color);
    return {r, g, b};
}
