#ifndef _DETECTOR_OBJECT_HPP_
#define _DETECTOR_OBJECT_HPP_

#include <opencv2/core/types.hpp>

namespace xgd {
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

    struct DetectorObject {
        inline static int maxLabel = 7, minLabel = 0;

        static bool isValidLabel(const int &_label);

        const cv::Rect2f rect;
        float prob;
        DetectorObjectType label;

        DetectorObject(const float &_x, const float &_y, const float &_w, const float &_h,
                       const int &_label, const float &_prob = 1);

        const cv::Rect2f &asRect() const;

        const float &x() const;

        const float &y() const;

        const float &w() const;

        const float &h() const;
    };
}
#endif//_DETECTOR_OBJECT_HPP_
