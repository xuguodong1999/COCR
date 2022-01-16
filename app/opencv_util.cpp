#include "opencv_util.hpp"
#include <opencv2/imgproc.hpp>
#include <QPixmap>

void xgd::cross_line(cv::InputOutputArray &_canvas, const cv::Point &_center, const int &_length,
                     const cv::Scalar &_color, const int &_thickness, bool _rotate) {
    int x = _center.x, y = _center.y, xx[4], yy[4];
    if (_rotate) {
        xx[0] = xx[3] = x - _length;
        xx[1] = xx[2] = x + _length;
        yy[0] = yy[2] = y - _length;
        yy[1] = yy[3] = y + _length;
    } else {
        xx[0] = xx[1] = x;
        xx[2] = x - _length;
        xx[3] = x + _length;
        yy[2] = yy[3] = y;
        yy[0] = y - _length;
        yy[1] = y + _length;
    }
    cv::line(_canvas, cv::Point(xx[0], yy[0]), cv::Point(xx[1], yy[1]),
             _color, _thickness, cv::LINE_AA);
    cv::line(_canvas, cv::Point(xx[2], yy[2]), cv::Point(xx[3], yy[3]),
             _color, _thickness, cv::LINE_AA);
}

cv::Mat xgd::rotateCvMat(const cv::Mat &srcImage, double angle) {
    cv::Mat destImage;
    cv::Point2f center(srcImage.cols / 2, srcImage.rows / 2);
    cv::Mat M = cv::getRotationMatrix2D(center, angle, 1);
    cv::warpAffine(srcImage, destImage, M, cv::Size(srcImage.cols, srcImage.rows),
                   cv::INTER_CUBIC, cv::BORDER_CONSTANT,
                   cv::Scalar(255, 255, 255));
    return destImage;
}


cv::Mat xgd::convertQImageToMat(const QImage &_img) {
    cv::Mat mat;
    if (_img.isNull()) {
        throw std::runtime_error("you are converting an empty QImage to cv::Mat");
    }
    auto dataPtr = const_cast<uchar *>(_img.constBits());
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
        case QImage::Format_Grayscale8:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC1, dataPtr, step);
            break;
        default: {
            throw std::runtime_error("you are converting an umImpl QImage to cv::Mat");
        }
    }
    return mat;
}

cv::Mat xgd::convertQPixmapToMat(const QPixmap &_img) {
    return xgd::convertQImageToMat(_img.toImage());
}

#include <opencv2/highgui.hpp>

QImage xgd::binaryAlphaImage(cv::Mat &src) {
    cv::Mat result;
    cv::cvtColor(src, result, cv::COLOR_RGBA2GRAY);
    cv::adaptiveThreshold(result, result, 255.0,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY, 21, 10);
    cv::cvtColor(result, result, cv::COLOR_GRAY2BGRA);
    const uchar *pSrc = (const uchar *) result.data;
    // Create QImage with same dimensions as input Mat
    QImage image(pSrc, result.cols, result.rows, result.step, QImage::Format_ARGB32);
    return image.copy();
}

