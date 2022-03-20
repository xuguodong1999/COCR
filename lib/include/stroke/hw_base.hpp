#pragma once
#include <cocr_stroke_export.h>
#include "base/cocr_types.hpp"

#include <opencv2/core/mat.hpp>
#include <optional>

template<typename _Tp>
inline _Tp getDistance2D(const cv::Point_<_Tp> &p1, const cv::Point_<_Tp> &p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

template<typename _Tp>
inline cv::Point_<_Tp> getRectCenter2D(const cv::Rect_<_Tp> &_rect) {
    return cv::Point_<_Tp>(_rect.x + _rect.width / 2, _rect.y + _rect.height / 2);
}

class COCR_STROKE_EXPORT HwController {
    cv::Scalar color;
public:
    const cv::Scalar &getColor() const;

    int getThickness() const;

    int getLineType() const;

    int getShift() const;

    float getRevertColorProb() const;

private:
    int thickness, lineType, shift;
    float revertColorProb;
public:
    HwController(const int &_thickness = 1);
};

class COCR_STROKE_EXPORT HwBase {
protected:
    bool keepDirection;
public:
    HwBase();

    virtual std::shared_ptr<HwBase> clone() const = 0;

    /**
     * 在子类重写这个函数，使得子类获得一个标签
     * @return
     */
    virtual DetectorClasses getItemType() const;

    /**
     * 在关键点旋转的时候，是否要保持现有点的方向
     * @return
     */
    virtual bool isDirectionKept() const;

    /**
     * 设置方向保持属性
     * @param _keepDirection
     */
    void setKeepDirection(bool _keepDirection = true);

    /**
     * 全局绘图属性控制器
     */
    inline static HwController baseController;

    /**
     * 设置风格控制器
     * @param hwController
     */
    virtual void setHwController(HwController &_hwController) = 0;

    /**
     * 将当前数据画入 canvas
     * @param canvas 目标画布
     */
    virtual void paintTo(cv::Mat &_canvas) const = 0;

    /**
     * 获取平行于二维坐标轴的最小边框
     * @return
     */
    virtual std::optional<cv::Rect2f> getBoundingBox() const = 0;

    /**
     * 顺时针旋转，绕图元中心
     * @param angle 角度制
     */
    virtual void rotate(float _angle);

    /**
     * 顺时针旋转，绕点 cent
     * @param angle 角度制
     */
    virtual void rotateBy(float _angle, const cv::Point2f &_cent) = 0;

    virtual std::optional<cv::Point2f> getCenter();

    /**
     * 将数据点整体移动 offset
     * @param offset
     */
    virtual void moveBy(const cv::Point2f &_offset) = 0;

    /**
     * 将数据点的中心移动到 newCenter
     * @param newCenter
     */
    virtual void moveCenterTo(const cv::Point2f &_newCenter);

    /**
     * 将数据点的边框左上角移动到 leftTop
     * @param leftTop
     */
    virtual void moveLeftTopTo(const cv::Point2f &_leftTop);

    /**
     * 将数据点整体缩放到 (w,h)
     * @param w 目标宽度
     * @param h 目标长度
     * @param keepRatio 是否保持比例，为 true 时撑满长边
     */
    virtual void resizeTo(float _w, float _h, bool _keepRatio = true);

    /**
     * 为所有点数乘 (kx,ky)
     * @param kx
     * @param ky
     */
    virtual void mulK(float _kx, float _ky) = 0;
};