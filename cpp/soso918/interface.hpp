//
// Created by xgd on 2020/9/18.
//

#ifndef PLAY_OPENCV_INTERFACE_HPP
#define PLAY_OPENCV_INTERFACE_HPP
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif // !_USE_MATH_DEFINES

#include <cmath>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <set>

#define BLACK cv::Scalar(0, 0, 0)
#define WHITE cv::Scalar(255, 255, 255)

enum StringNote {
    Normal,
    RightButtom,
    RightTop,
    Up,
    Down,
    UpRand,
    DownRand
};
using BInt = std::pair<bool, bool>;

class BVector {
    BInt *data;
public:
    BVector() {

    }
};

inline bool operator!=(const BInt &a, const BInt &b) {
    return (a.first ^ b.first) | (a.second ^ b.second);
}

inline bool operator==(const BInt &a, const BInt &b) {
    return !(a != b);
}


inline int toInt(const BInt &num) {
    // 0 1 2 3
    if (num.first) {
        if (num.second) {
            return 3;
        } else {
            return 2;
        }
    } else {
        if (num.second) {
            return 1;
        } else {
            return 0;
        }
    }
}

enum BIntEnum {
    BIntCarbon,
    BIntLeft,
    BIntRight,
    BIntEnd
};

inline BInt toBInt(BIntEnum num) {
    switch (num) {
        case BIntCarbon:
            return std::make_pair(false, false);
        case BIntLeft:
            return std::make_pair(false, true);
        case BIntRight:
            return std::make_pair(true, false);
        default:
            return std::make_pair(true, true);
    }
}

using NChar = std::pair<StringNote, std::string>;
using NString = std::vector<NChar>;

inline NString makeNotedStringByChar(const std::string &text, StringNote note = Normal) {
    NString ns;
    for (auto c:text) {
        ns.push_back(std::make_pair(note, std::string(1, c)));
    }
    return ns;
}

inline NString makeNotedString(
        const std::string &text, const std::vector<StringNote> &note) {
    NString ns;
    std::vector<StringNote> _note = note;
    if (_note.size() == 0) {
        _note.resize(text.size(), Normal);
    } else {
        _note.resize(text.size(), _note.back());
    }
    for (size_t i = 0; i < text.size(); i++) {
        ns.push_back(std::make_pair(_note[i], std::string(1, text[i])));
    }
    return ns;
}

using Stroke = std::vector<cv::Point2f>;
using PointArr = std::vector<cv::Point2f>;
using Script = std::vector<Stroke>;

class ShapeInterface {
public:

    /**
     * 将当前数据画入 canvas
     * @param canvas 目标画布
     */
    virtual void paintTo(cv::Mat &canvas) = 0;

    /**
     * 获取平行于二维坐标轴的最小边框
     * @return
     */
    virtual const cv::Rect2f getBoundingBox() const = 0;

    /**
     * 顺时针旋转，绕图元中心
     * @param angle 角度制
     */
    virtual void rotate(float angle) = 0;

    /**
     * 顺时针旋转，绕点 cent
     * @param angle 角度制
     */
    virtual void rotateBy(float angle, const cv::Point2f &cent) = 0;

    virtual const cv::Point2f getCenter() {
        auto rect = getBoundingBox();
//        std::cout<<rect<<std::endl;
        return cv::Point2f(rect.x + rect.width / 2.0, rect.y + rect.height / 2.0);
//        return (rect.tl() + rect.br()) / 2.;
    }

    /**
     * 将数据点整体移动 offset
     * @param offset
     */
    virtual void move(const cv::Point2f &offset) = 0;

    /**
     * 将数据点的中心移动到 newCenter
     * @param newCenter
     */
    virtual void moveCenterTo(const cv::Point2f &newCenter) = 0;

    /**
     * 将数据点的边框左上角移动到 leftTop
     * @param leftTop
     */
    virtual void moveLeftTopTo(const cv::Point2f &leftTop) = 0;

    /**
     * 将数据点整体缩放到 (w,h)
     * @param w 目标宽度
     * @param h 目标长度
     * @param keepRatio 是否保持比例，为 true 时撑满长边
     */
    virtual void resizeTo(float w, float h, bool keepRatio = true) = 0;


    virtual void setColor(const cv::Scalar &color) {
        ShapeInterface::color = color;
    }

    virtual void setThickness(int thickness) {
        ShapeInterface::thickness = thickness;
    }

    virtual void setLineType(int lineType) {
        ShapeInterface::lineType = lineType;
    }

    virtual void setShift(int shift) {
        ShapeInterface::shift = shift;
    }

    bool IsRotateAllowed() const { return isRotateAllowed; }

protected:
    int thickness;
    int lineType;
    int shift;
    cv::Scalar color;
    bool isRotateAllowed;

    ShapeInterface() :
            isRotateAllowed(true),
            color(BLACK), thickness(3), lineType(cv::LINE_AA), shift(0) {
    }
};

const char *get_couch_data_path();

const char *get_drugbank_json_path();

const char *get_drugbank_h_json_path();

const char *get_chonps_json_path();

class R {
public:
    static float sNormSizeK;    // (-1,1)*100
    static float sSubSizeK;     // (-1,1)*50
    static float sNormOffsetKx; // (-1,1)*100
    static float sNormOffsetKy; // (-1,1)*100
    static float sSubOffsetKx;  // (-1,1)*50
    static float sSubOffsetKy;  // (-1,1)*50
    static float sAngleK;       // [0,1)*30
    static std::vector<std::string> sElementData;
    static std::set<std::string> bsSet, aeSet, btSet;
    static std::set<int> acSet;
};

#endif //PLAY_OPENCV_INTERFACE_HPP
