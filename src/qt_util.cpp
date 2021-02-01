#include "qt_util.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace cv;
cv::Mat convertQImageToMat(const QImage &_img) {
    cv::Mat mat;
    if (_img.isNull()) {
        std::cerr << "you are converting an empty QImage to cv::Mat" << std::endl;
        exit(-1);
    }
    void *dataPtr = const_cast<uchar *>(_img.constBits());
    int step = _img.bytesPerLine();
    switch (_img.format()) {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC4, dataPtr, step);
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC3, dataPtr, step);
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC1, dataPtr, step);
            break;
        default: {
            std::cerr << "unImpl image format:" << _img.format() << std::endl;
            exit(-1);
        }
    }
//    std::cout << mat.cols << "x" << mat.rows << std::endl;
    return std::move(mat);
}

cv::Mat convertQPixmapToMat(const QPixmap &_img) {
    return convertQImageToMat(_img.toImage());
}

