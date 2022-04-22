#pragma once

#include "els_ocv_export.h"
#include "base/point2.h"
#include "mat.h"
#include "base/rect.h"

#include <vector>
#include "base/point3.h"

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

    // 将图像填充到指定尺寸，先进行缩放
    static std::pair<Mat, point3f> ResizeKeepRatio(
            const Mat &mat, const Size<int> &dstSize, const ColorName &color = ColorName::rgbWhite);

    // 将图像填充到更大尺寸，不进行缩放
    static std::pair<Mat, point3f> PadTo(
            const Mat &mat, const Size<int> &dstSize, const rgb &color = {0, 0, 0});

    static Mat ResizeWithBlock(const Mat &mat, const Size<int> &dstSize, const Size<int> &dstBlock);

    static float GetInterArea(const rectf &r0, const rectf &r1);

    static Mat Rotate(const Mat &mat, const float &angle);

    static Mat GetFont(const std::string &_text, const std::string &_fontFamily = "Arial");

    static Mat AddGaussianNoise(const Mat &mat);

    static Mat AddSaltPepperNoise(const Mat &mat, const int &n);

    static Mat RevertColor(const Mat &mat);
#ifndef Q_OS_WASM
    static Mat BufferToGrayMat(std::vector<unsigned char> &buffer);
#endif
    static Mat HConcat(const Mat &m1, const Mat &m2);
};
