#pragma once

#include "els_ocv_export.h"
#include "base/point2.h"
#include "mat.h"
#include "base/rect.h"
#include <vector>

class ELS_OCV_EXPORT AffineHelper {
    std::shared_ptr<cv::Mat> affine;
public:
    void calc(const std::vector<point2f> &from, const std::vector<point2f> &to);

    point2f transform(const point2f &p);
};

class ELS_OCV_EXPORT CvUtil {

public:
    static std::vector<point2f> GetMinAreaRect(const std::vector<point2f> &input);

    static std::pair<point2f, point2f> GetLineFromTo(const Mat &mat, const rectf &box);

    static std::pair<point2f, point2f> GetWedgeFromTo(const Mat &mat, const rectf &box);

    static Mat Resize(const Mat &mat, const Size<int> &dstSize);

    static Mat ResizeWithBlock(const Mat &mat, const Size<int> &dstSize, const Size<int> &dstBlock);

    static float GetInterArea(const rectf &r0, const rectf &r1);

    static Mat Rotate(const Mat &mat,const float&angle);
};
