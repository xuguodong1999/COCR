#include "opencv_util.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
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
