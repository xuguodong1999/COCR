#include "opencv_util.hpp"
#include <opencv2/imgproc.hpp>

std::pair<cv::Mat, std::tuple<float, float, float>>
resizeCvMatTo(const cv::Mat &_img, const int &_newWidth, const int &_newHeight,
              const cv::Scalar &_padColor) {
    int w = _img.cols;
    int h = _img.rows;
    float kw = (float) _newWidth / w, kh = (float) _newHeight / h;
    float k = std::min(kw, kh);
    int newWidth = k * w, newHeight = k * h;
    cv::Mat ret;
    cv::resize(_img, ret, cv::Size(newWidth, newHeight),
               0, 0, cv::INTER_CUBIC);
    int dw = std::max(0, _newWidth - newWidth), dh = std::max(0, _newHeight - newHeight);
    cv::copyMakeBorder(ret, ret,
                       dh / 2, dh - dh / 2, dw / 2, dw - dw / 2,
                       cv::BORDER_CONSTANT, _padColor);
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
