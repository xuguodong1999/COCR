#ifndef _XGD_OPENCV_UTIL_HPP_
#define _XGD_OPENCV_UTIL_HPP_

#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include <QImage>

namespace xgd {
    template<typename _Tp>
    inline _Tp getDistance2D(const cv::Point_<_Tp> &p1, const cv::Point_<_Tp> &p2) {
        return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
    }

    template<typename _Tp>
    inline cv::Point_<_Tp> getRectCenter2D(const cv::Rect_<_Tp> &_rect) {
        return cv::Point_<_Tp>(_rect.x + _rect.width / 2, _rect.y + _rect.height / 2);
    }

    template<typename _Tp>
    inline _Tp getScalarSum(const cv::Scalar_<_Tp> &_scalar) {
        const auto val = _scalar.val;
        _Tp sum = 0;
        for (size_t i = 0; i < 4; i++) {
            sum += val[i];
        }
        return sum;
    }

    void cross_line(cv::InputOutputArray &_canvas, const cv::Point &_center, const int &_length,
                    const cv::Scalar &_color, const int &_thickness = 1, bool _rotate = true);

    cv::Mat rotateCvMat(const cv::Mat &srcImage, double angle);

    cv::Mat convertQImageToMat(const QImage &_img);

    cv::Mat convertQPixmapToMat(const QPixmap &_img);
}
#endif//_XGD_OPENCV_UTIL_HPP_
