#include "opencv_util/opencv_util.hpp"

#include "base/std_util.hpp"
#include "base/cocr_types.hpp"

#include <opencv2/opencv.hpp>

cv::Scalar_<unsigned char> getScalar(const ColorName &_colorName) {
    const auto&[r, g, b]=
    predefinedColors[(size_t) _colorName];
    return {b, g, r};
}

void cross_line(cv::InputOutputArray &_canvas, const cv::Point &_center, const int &_length,
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

cv::Mat rotateCvMat(const cv::Mat &srcImage, double angle) {
    cv::Mat destImage;
    cv::Point2f center(srcImage.cols / 2, srcImage.rows / 2);
    cv::Mat M = cv::getRotationMatrix2D(center, angle, 1);
    cv::warpAffine(srcImage, destImage, M, cv::Size(srcImage.cols, srcImage.rows),
                   cv::INTER_CUBIC, cv::BORDER_CONSTANT,
                   cv::Scalar(255, 255, 255));
    return destImage;
}

//QImage  binaryAlphaImage(cv::Mat &src) {
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

std::pair<cv::Mat, std::tuple<float, float, float>>
resizeCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight,
              const cv::Scalar &_padColor) {
    int w = _img.cols;
    int h = _img.rows;
    float kw = (float) _newWidth / w, kh = (float) _newHeight / h;
    float k = std::min(kw, kh);
    int newWidth = k * w, newHeight = k * h;
    cv::Mat ret;
//    std::cout << w << "," << h << "," << k << "," << std::endl;
    cv::resize(_img, ret, cv::Size(newWidth, newHeight),
               0, 0, cv::INTER_CUBIC);
    int dw = std::max(0, _newWidth - newWidth), dh = std::max(0, _newHeight - newHeight);
    cv::copyMakeBorder(ret, ret,
                       dh / 2, dh - dh / 2, dw / 2, dw - dw / 2,
                       cv::BORDER_CONSTANT, _padColor);
//    std::cout<<dw<<","<<dh<<","<<ret.cols<<","<<ret.rows<<std::endl;
    return {std::move(ret), {k, dw / 2.f, dh / 2.f}};
}

std::pair<cv::Mat, std::tuple<float, float, float>>
padCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight,
           const cv::Scalar &_padColor) {
    int w = _img.cols;
    int h = _img.rows;
    cv::Mat ret;
    int dw = std::max(0, _newWidth - w), dh = std::max(0, _newHeight - h);
    cv::copyMakeBorder(_img, ret,
                       dh / 2, dh - dh / 2, dw / 2, dw - dw / 2,
                       cv::BORDER_CONSTANT, _padColor);
//    std::cout<<dw<<","<<dh<<","<<ret.cols<<","<<ret.rows<<std::endl;
    return {std::move(ret), {1.0f, dw / 2.f, dh / 2.f}};
}

std::optional<cv::Rect2i> getBoundBoxForBWFont(const cv::Mat &_uMat, const uchar &_bgPixel) {
    int xmin;
    for (int x = 0; x < _uMat.cols; x++) {
        for (int y = 0; y < _uMat.rows; y++) {
            if (_bgPixel != _uMat.at<uchar>(y, x)) {
                xmin = x;
                goto L1;
            }
        }
    }
    return std::nullopt;
    L1:;
    int xmax;
    for (int x = _uMat.cols - 1; x >= xmin; x--) {
        for (int y = _uMat.rows - 1; y >= 0; y--) {
            if (_bgPixel != _uMat.at<uchar>(y, x)) {
                xmax = x;
                goto L2;
            }
        }
    }
    xmax = xmin;
    L2:;
    int ymin;
    for (int y = 0; y < _uMat.rows; y++) {
        for (int x = xmin; x <= xmax; x++) {
            if (_bgPixel != _uMat.at<uchar>(y, x)) {
                ymin = y;
                goto L3;
            }
        }
    }
    ymin = _uMat.rows - 1;
    L3:;
    int ymax;
    for (int y = _uMat.rows - 1; y >= ymin; y--) {
        for (int x = xmin; x <= xmax; x++) {
            if (_bgPixel != _uMat.at<uchar>(y, x)) {
                ymax = y;
                goto L4;
            }
        }
    }
    ymax = ymin;
    L4:;
    return cv::Rect2i(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1);
}

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

/**
 *  版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
    本文链接：https://blog.csdn.net/iracer/article/details/49383491
 */

void salt_pepper(cv::Mat image, int n) {
    int i, j;
    static std::vector<int> noise{0, 255};
    for (int k = 0; k < n / 2; k++) {
        // rand() is the random number generator
        i = randInt() % image.cols; // % 整除取余数运算符,rand=1022,cols=1000,rand%cols=22
        j = randInt() % image.rows;
        if (image.type() == CV_8UC1) { // gray-level image
            image.at<uchar>(j, i) = randSelect(noise); //at方法需要指定Mat变量返回值类型,如uchar等
        } else if (image.type() == CV_8UC3) { // color image
            image.at<cv::Vec3b>(j, i)[0] = randSelect(noise); //cv::Vec3b为opencv定义的一个3个值的向量类型
            image.at<cv::Vec3b>(j, i)[1] = randSelect(noise); //[]指定通道，B:0，G:1，R:2
            image.at<cv::Vec3b>(j, i)[2] = randSelect(noise);
        }
    }
}
