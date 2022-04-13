#include <opencv2/imgproc.hpp>
#include "ocv_wrapper/algorithm.h"

static void sync(std::vector<cv::Point2f> &dst, const std::vector<point2f> &src) {
    dst.resize(src.size());
    for (size_t i = 0; i < src.size(); i++) {
        const auto&[x, y]=src[i];
        dst[i] = {x, y};
    }
}

void AffineHelper::calc(const std::vector<point2f> &from, const std::vector<point2f> &to) {
    std::vector<cv::Point2f> _from, _to;
    sync(_from, from);
    sync(_to, to);
    // 2x3 仿射变换矩阵
    auto matrix = cv::getAffineTransform(_from.data(), _to.data());
    affine = std::make_shared<cv::Mat>();
    std::swap(*affine, matrix);
}

point2f AffineHelper::transform(const point2f &p) {
    const auto&[x, y]=p;
    cv::Mat pm = (*affine) * cv::Mat({(double) x, (double) y, 1.});
    return {pm.at<double>(0, 0), pm.at<double>(1, 0)};
}

std::vector<point2f> CvUtil::GetMinAreaRect(const std::vector<point2f> &input) {
    std::vector<cv::Point2f> dst(4), _input;
    sync(_input, input);
    cv::minAreaRect(_input).points(dst.data());
    std::vector<point2f> result(dst.size());
    for (size_t i = 0; i < dst.size(); i++) {
        const auto &p = dst[i];
        result[i] = {p.x, p.y};
    }
    return result;
}
