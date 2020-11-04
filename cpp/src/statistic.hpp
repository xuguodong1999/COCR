#ifndef _STATISTIC_HPP_
#define _STATISTIC_HPP_

#include "config.hpp"
#include "shapeitem.hpp"
#include <opencv2/core/types.hpp>

#include <set>

/**
 * randomization controller
 * 意图：1、插桩执行数据分布统计；2、控制随机性
 */
class RC {
public:
    static float sNormSizeK;    // (-1,1)*100
    static float sSubSizeK;     // (-1,1)*50
    static float sNormOffsetKx; // (-1,1)*100
    static float sNormOffsetKy; // (-1,1)*100
    static float sSubOffsetKx;  // (-1,1)*50
    static float sSubOffsetKy;  // (-1,1)*50
    static float sAngleK;       // [0,1)*30
    static std::set<string> bsSet, aeSet, btSet;
    static std::set<int> acSet;


    struct ShapeAttr {
        //color(cvBlack), thickness(3), lineType(cv::LINE_AA), shift(0)
        int thickness;
        int lineType;
        int shift;
        cv::Scalar color;

        ShapeAttr() : color(cvBlack), thickness(2), lineType(cv::LINE_AA), shift(0) {}
    };

    static void update_shape_attr() {

    }

    static ShapeAttr shapeAttr;

    static const int get_shape_shift() {
        return shapeAttr.shift;
    }

    static const int get_shape_thickness() {
        return shapeAttr.thickness;
    }

    static const int get_shape_lineType() {
        return shapeAttr.lineType;
    }

    static const cv::Scalar get_shape_color() {
        return shapeAttr.color;
    }
};

#endif//_STATISTIC_HPP_