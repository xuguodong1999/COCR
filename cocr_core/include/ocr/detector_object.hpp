#pragma once

#include <cocr_ocr_export.h>

#include <opencv2/core/types.hpp>

namespace cocr {
    enum class DetectorObjectType {
        SingleLine = 0,
        DoubleLine = 1,
        TripleLine = 2,
        SolidWedge = 3,
        DashWedge = 4,
        WaveLine = 5,
        Circle = 6,
        Text = 7
    };

    struct COCR_OCR_EXPORT DetectorObject {
        inline static int maxLabel = 7;
        inline static int minLabel = 0;

        static bool isValidLabel(const int &_label);

        const cv::Rect2f rect;
        float prob;
        DetectorObjectType label;

        template<class RectFloatType, class ProbFloatType>
        DetectorObject(const RectFloatType &_x, const RectFloatType &_y,
                       const RectFloatType &_w, const RectFloatType &_h,
                       const int &_label, const ProbFloatType &_prob = 1):
                rect(_x, _y, _w, _h), label(static_cast<DetectorObjectType>(_label)), prob(_prob) {}

        const cv::Rect2f &asRect() const;

        const float &x() const;

        const float &y() const;

        const float &w() const;

        const float &h() const;
    };
}