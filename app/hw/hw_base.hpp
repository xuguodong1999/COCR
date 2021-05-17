#ifndef _HW_BASE_HPP_
#define _HW_BASE_HPP_

#include "cocr_types.hpp"

#include <opencv2/core/mat.hpp>
#include <optional>

class HwController {
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

class HwBase {
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

#endif//_HW_BASE_HPP_
