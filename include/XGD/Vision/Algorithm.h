#pragma once

#include "xgd_vision_export.h"
#include "XGD/Base/Util.h"
#include "XGD/Base/Rect.h"
#include "XGD/Base/Vec3.h"
#include "XGD/Vision/Mat.h"

#include <vector>

class XGD_VISION_EXPORT AffineHelper {
    std::shared_ptr<cv::Mat> affine;
public:
    void calc(const std::vector<point2f> &from, const std::vector<point2f> &to);

    point2f transform(const point2f &p);
};

class XGD_VISION_EXPORT CvUtil {

public:
    static std::vector<point2f> GetMinAreaRect(const std::vector<point2f> &input);

    static std::pair<point2f, point2f> GetLineFromTo(const Mat &mat, const rectf &box);

    static std::pair<point2f, point2f> GetWedgeFromTo(const Mat &mat, const rectf &box);

    static Mat Resize(const Mat &mat, const Size<int> &dstSize);

    // fill with resize
    static std::pair<Mat, point3f> ResizeKeepRatio(
            const Mat &mat, const Size<int> &dstSize, const ColorName &color = ColorName::rgbWhite);

    // fill without resize
    static std::pair<Mat, point3f> PadTo(
            const Mat &mat, const Size<int> &dstSize, const rgb &color = {0, 0, 0});

    static Mat ResizeWithBlock(const Mat &mat, const Size<int> &dstSize, const Size<int> &dstBlock);

    static float GetInterArea(const rectf &r0, const rectf &r1);

    static Mat Rotate(const Mat &mat, const float &angle);

    static Mat GetFont(const std::string &_text, const std::string &_fontFamily = "Arial");

    static Mat AddGaussianNoise(const Mat &mat);

    static Mat AddSaltPepperNoise(const Mat &mat, const int &n);

    static Mat RevertColor(const Mat &mat);

    static Mat BufferToGrayMat(std::vector<unsigned char> &buffer);

    static Mat HConcat(const Mat &m1, const Mat &m2);
};
